//-----------------------------------------------------------------------------
// Created on: 15 February 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
#include <asiAlgo_MeshGen.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <OSD.hxx>
#include <Poly_Triangulation.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <Standard_ErrorHandler.hxx>

#undef FILE_DEBUG
#if defined FILE_DEBUG
  #pragma message("===== warning: FILE_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

double asiAlgo_MeshGen::AutoSelectLinearDeflection(const TopoDS_Shape& shape)
{
  double xMin, yMix, zMin, xMax, yMax, zMax;
  if ( !asiAlgo_Utils::Bounds(shape, xMin, yMix, zMin, xMax, yMax, zMax, 0.0001) )
  {
    xMin = yMix = zMin = 0.0;
    xMax = yMax = zMax = 1.0;
  }

  // Use a fraction of a bounding diagonal.
  const double diag = ( gp_XYZ(xMin, yMix, zMin) - gp_XYZ(xMax, yMax, zMax) ).Modulus();
  return 0.001*diag;
}

//-----------------------------------------------------------------------------

double asiAlgo_MeshGen::AutoSelectAngularDeflection(const TopoDS_Shape& shape)
{
  asiAlgo_NotUsed(shape);

  return 0.5; // In degrees.
}

//-----------------------------------------------------------------------------

bool asiAlgo_MeshGen::DoNative(const TopoDS_Shape& shape,
                               const double        linearDeflection,
                               const double        angularDeflection_deg,
                               asiAlgo_MeshInfo&   info)
{
  // Clean up polygonal data.
  BRepTools::Clean(shape);

  // Tessellate.
  try
  {
    OCC_CATCH_SIGNALS

#if defined FILE_DEBUG
    static int callId = 0;
    callId++;

    std::cout << "Running b-rep mesh: " << callId << std::endl;
    TCollection_AsciiString filename("C:/users/ssv/desktop/tmp/");
    filename += callId;
    filename += ".brep";
    //
    BRepTools::Write(shape, filename.ToCString());
#endif

    // Notice that parallel mode is enabled.
    IMeshTools_Parameters params;
    params.Deflection               = linearDeflection;
    params.Angle                    = angularDeflection_deg;
    //params.MinSize                  = 0.05;
    //params.ControlSurfaceDeflection = false;
    params.InParallel               = true;
    //
    BRepMesh_IncrementalMesh MeshGen(shape, params);
    
  }
  catch ( ... )
  {
    std::cout << "B-Rep mesh crash" << std::endl;
    return false;
  }

  //---------------------------------------------------------------------------
  // Accumulate summary info
  //---------------------------------------------------------------------------

  double maxDeflection = 0.0;
  int    nTriangles    = 0;
  int    nNodes        = 0;
  //
  for ( TopExp_Explorer ex(shape, TopAbs_FACE); ex.More(); ex.Next() )
  {
    const TopoDS_Face&                F = TopoDS::Face( ex.Current() );
    TopLoc_Location                   L;
    const Handle(Poly_Triangulation)& T = BRep_Tool::Triangulation(F, L);
    //
    if ( T.IsNull() )
      continue;

    nTriangles += T->NbTriangles();
    nNodes     += T->NbNodes();
    //
    if ( T->Deflection() > maxDeflection )
      maxDeflection = T->Deflection();
  }

  // Store results
  info.maxDeflection = maxDeflection;
  info.nFacets       = nTriangles;
  info.nNodes        = nNodes;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_MeshGen::DoNative(const TopoDS_Shape& shape)
{
  const double linDefl = AutoSelectLinearDeflection(shape);
  const double angDefl = AutoSelectAngularDeflection(shape);
  //
  asiAlgo_MeshInfo info;

  return DoNative(shape, linDefl, angDefl, info);
}
