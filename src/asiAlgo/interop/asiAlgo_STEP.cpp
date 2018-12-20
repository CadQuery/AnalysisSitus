//-----------------------------------------------------------------------------
// Created on: 28 November 2015
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiAlgo_STEP.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_BSplineCurve.hxx>
#include <NCollection_CellFilter.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_BSplineCurveWithKnots.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Static.hxx>
#include <Message_ProgressSentry.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif


//-----------------------------------------------------------------------------

bool asiAlgo_STEP::Read(const TCollection_AsciiString& filename,
                        const bool                     doHealing,
                        TopoDS_Shape&                  result)
{
  if ( filename.IsEmpty() )
    return false;

  /* ==================================
   *  Prepare OCCT translation toolkit
   * ================================== */

  STEPControl_Reader reader;

  /* ================
   *  Read STEP file
   * ================ */

  if ( reader.ReadFile( filename.ToCString() ) != IFSelect_RetDone )
  {
    std::cout << "Failed to read the file " << filename.ToCString() << std::endl;
    return false;
  }

  // Get STEP model.
  Handle(StepData_StepModel) stepModel = reader.StepModel();
  Interface_EntityIterator entIt = stepModel->Entities();

  m_progress.SendLogMessage( LogInfo(Normal) << "Read %1 entities from STEP file."
                                             << entIt.NbEntities() );

#if defined COUT_DEBUG
  // Prepare a map for counting entities.
  NCollection_DataMap<Handle(Standard_Type), int> entCountMap;

  // Iterate all entities.
  for ( ; entIt.More(); entIt.Next() )
  {
    const Handle(Standard_Transient)& ent     = entIt.Value();
    const Handle(Standard_Type)&      entType = ent->DynamicType();

    // Count in a map.
    if ( entCountMap.IsBound(entType) )
      entCountMap(entType)++;
    else
      entCountMap.Bind(entType, 1);
  }

  // Dump counts.
  for ( NCollection_DataMap<Handle(Standard_Type), int>::Iterator it(entCountMap); it.More(); it.Next() )
  {
    const Handle(Standard_Type)& entType = it.Key();
    const int                    count   = it.Value();

    m_progress.SendLogMessage(LogInfo(Normal) << "\t%1 : %2" << entType->Name() << count);
  }
#endif

  /* ==========================
   *  Translate STEP into BREP
   * ========================== */

  // Transfer all roots into one shape or into several shapes.
  try
  {
    reader.TransferRoots();
  }
  catch ( Standard_Failure )
  {
    std::cout << "Warning: exception occurred during translation." << std::endl;
  }
  if ( reader.NbShapes() <= 0 )
  {
    std::cout << "Error: transferring STEP to BREP failed." << std::endl;
    return false;
  }

  TopoDS_Shape preResult = reader.OneShape();

  // Release memory after translation.
  reader.WS()->NewModel();
  Standard::Purge();

  /* =================
   *  Post-processing
   * ================= */

  result = preResult;

  // Apply shape healing.
  if ( doHealing )
  {
    Handle(ShapeFix_Shape) aShapeHealer = new ShapeFix_Shape(result);
    bool aFixRes = false;
    try
    {
      aFixRes = aShapeHealer->Perform();
    }
    catch ( Standard_Failure )
    {
      aFixRes = false;
    }
    if ( aFixRes )
    {
      result = aShapeHealer->Shape();
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_STEP::Write(const TopoDS_Shape&            shape,
                         const TCollection_AsciiString& filename)
{
  /* ==================================
   *  Prepare OCCT translation toolkit
   * ================================== */

  STEPControl_Writer aWriter;
  Handle(XSControl_WorkSession) aWS = aWriter.WS();

  /* ========================
   *  Transfer shape to STEP
   * ======================== */

  if ( aWriter.Transfer( shape, STEPControl_AsIs ) != IFSelect_RetDone )
  {
    std::cout << "Failed to transfer shape to STEP" << std::endl;
    return false;
  }

  if ( aWriter.Write( filename.ToCString() ) != IFSelect_RetDone )
  {
    std::cout << "Failed to write shape into the file " << filename.ToCString() << std::endl;
    return false;
  }

  // Success
  return true;
}
