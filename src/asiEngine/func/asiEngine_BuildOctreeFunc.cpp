//-----------------------------------------------------------------------------
// Created on: 04 March 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiEngine_BuildOctreeFunc.h>

// asiData includes
#include <asiData_BVHParameter.h>
#include <asiData_OctreeParameter.h>

// asiAlgo includes
#if defined USE_MOBIUS
  #include <asiAlgo_MeshDistanceFunc.h>
#endif
#include <asiAlgo_Timer.h>

// Active Data includes
#include <ActData_ParameterFactory.h>
#include <ActData_UserExtParameter.h>

#ifdef USE_MOBIUS
  #include <mobius/poly_DistanceField.h>
  #include <mobius/poly_SVO.h>

  using namespace mobius;
#endif

// OpenCascade includes
#include <Precision.hxx>

//-----------------------------------------------------------------------------

Handle(asiEngine_BuildOctreeFunc) asiEngine_BuildOctreeFunc::Instance()
{
  return new asiEngine_BuildOctreeFunc();
}

//-----------------------------------------------------------------------------

const char* asiEngine_BuildOctreeFunc::GUID()
{
  return "ADC09C32-68B1-4DBD-A4C4-540B3D6C36A8";
}

//-----------------------------------------------------------------------------

const char* asiEngine_BuildOctreeFunc::GetGUID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------

int asiEngine_BuildOctreeFunc::execute(const Handle(ActAPI_HParameterList)& inputs,
                                       const Handle(ActAPI_HParameterList)& outputs,
                                       const Handle(Standard_Transient)&) const
{
  ActAPI_ProgressEntry progress = this->GetProgressNotifier();

  /* ============================
   *  Interpret input Parameters.
   * ============================ */

  // Get BVH facets in the external Parameter.
  Handle(ActData_UserExtParameter)
    bvhExtParam = Handle(ActData_UserExtParameter)::DownCast( inputs->Value(1) );
  //
  Handle(ActAPI_INode) ownerNode = bvhExtParam->GetNode();

  // Get the custom BVH Parameter.
  Handle(asiData_BVHParameter)
    bvhParam = Handle(asiData_BVHParameter)::DownCast( ownerNode->Parameter( bvhExtParam->GetParamId() ) );

  // Get the stored BVH.
  Handle(asiAlgo_BVHFacets) bvh = bvhParam->GetBVH();

  // Get domain definition.
  Handle(ActData_RealParameter) xMinParam = ActParamTool::AsReal( inputs->Value(2) );
  Handle(ActData_RealParameter) yMinParam = ActParamTool::AsReal( inputs->Value(3) );
  Handle(ActData_RealParameter) zMinParam = ActParamTool::AsReal( inputs->Value(4) );
  Handle(ActData_RealParameter) xMaxParam = ActParamTool::AsReal( inputs->Value(5) );
  Handle(ActData_RealParameter) yMaxParam = ActParamTool::AsReal( inputs->Value(6) );
  Handle(ActData_RealParameter) zMaxParam = ActParamTool::AsReal( inputs->Value(7) );
  //
  const double xMin = xMinParam->GetValue();
  const double yMin = yMinParam->GetValue();
  const double zMin = zMinParam->GetValue();
  const double xMax = xMaxParam->GetValue();
  const double yMax = yMaxParam->GetValue();
  const double zMax = zMaxParam->GetValue();
  //
  gp_XYZ domainMin(xMin, yMin, zMin);
  gp_XYZ domainMax(xMax, yMax, zMax);
  //
  const bool isCustomDomain = ( (domainMax - domainMin).Modulus() > Precision::Confusion() );

  // Whether the domain is a cube.
  const bool isCube = ActParamTool::AsBool( inputs->Value(8) )->GetValue();

  // Get cell sizes and precision.
  const double minSize = ActParamTool::AsReal( inputs->Value(9)  )->GetValue();
  const double maxSize = ActParamTool::AsReal( inputs->Value(10) )->GetValue();
  const double prec    = ActParamTool::AsReal( inputs->Value(11) )->GetValue();

  /* ==============
   *  Build octree.
   * ============== */

  /* Construct distance function. */

  TIMER_NEW
  TIMER_GO

  asiAlgo_MeshDistanceFunc*
    pDistFunc = isCustomDomain ? new asiAlgo_MeshDistanceFunc(bvh, domainMin, domainMax, poly_DistanceFunc::Mode_Signed, isCube)
                               : new asiAlgo_MeshDistanceFunc(bvh, poly_DistanceFunc::Mode_Signed, isCube);

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(progress, "Construct implicit distance function")

  /* Construct distance field. */

  TIMER_RESET
  TIMER_GO

  poly_DistanceField* pDDF = new poly_DistanceField();
  //
  pDDF->SetMaxCellSize(maxSize);
  //
  if ( !pDDF->Build(minSize, prec, pDistFunc) )
  {
    delete pDDF;

    progress.SendLogMessage(LogErr(Normal) << "Failed to build distance field.");
    return 1;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(progress, "Build SVO for distance field")

  poly_SVO* pRoot = pDDF->GetRoot();

  // Measure SVO.
  int                      numSVONodes = 0;
  const unsigned long long memBytes    = pRoot->GetMemoryInBytes(numSVONodes);
  const double             memMBytes   = memBytes / (1024.*1024.);
  //
  progress.SendLogMessage( LogInfo(Normal) << "SVO contains %1 nodes and occupies %2 bytes (%3 MiB) of memory."
                                           << numSVONodes << int(memBytes) << memMBytes );

  /* =======================
   *  Set output Parameters.
   * ======================= */

  Handle(ActData_UserExtParameter)
    octreeExtParam = Handle(ActData_UserExtParameter)::DownCast( outputs->Value(1) );
  //
  Handle(ActAPI_INode) octreeNode = octreeExtParam->GetNode();

  // Get the custom Octree Parameter.
  Handle(asiData_OctreeParameter)
    octreeParam = Handle(asiData_OctreeParameter)::DownCast( octreeNode->Parameter( octreeExtParam->GetParamId() ) );

  // Store the octree.
  octreeParam->SetOctree(pRoot);

  return 0; // Success.
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream
  asiEngine_BuildOctreeFunc::inputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_BVH
                                      << Parameter_Real  // Domain min X.
                                      << Parameter_Real  // Domain min Y.
                                      << Parameter_Real  // Domain min Z.
                                      << Parameter_Real  // Domain max X.
                                      << Parameter_Real  // Domain max Y.
                                      << Parameter_Real  // Domain max Z.
                                      << Parameter_Bool  // Whether the domain is a cube.
                                      << Parameter_Real  // Min cell size.
                                      << Parameter_Real  // Max cell size.
                                      << Parameter_Real; // Precision.
}

//-----------------------------------------------------------------------------

ActAPI_ParameterTypeStream
  asiEngine_BuildOctreeFunc::outputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Octree
                                      << Parameter_Int; // Num. of elements.
}
