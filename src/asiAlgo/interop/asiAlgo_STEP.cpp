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
#include <asiAlgo_BaseCloud.h>
#include <asiAlgo_STEP.h>

// OCCT includes
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
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

//-----------------------------------------------------------------------------

//! Auxiliary class to search for coincident spatial points.
class InspectXYZ : public NCollection_CellFilter_InspectorXYZ
{
public:

  typedef gp_XYZ Target;

  //! Constructor accepting resolution distance and point.
  InspectXYZ(const double tol, const gp_XYZ& P) : m_fTol(tol), m_bFound(false), m_P(P) {}

  //! \return true/false depending on whether the node was found or not.
  bool IsFound() const { return m_bFound; }

  //! Implementation of inspection method.
  NCollection_CellFilter_Action Inspect(const gp_XYZ& Target)
  {
    m_bFound = ( (m_P - Target).SquareModulus() <= Square(m_fTol) );
    return CellFilter_Keep;
  }

  //! \return cell center.
  gp_XYZ GetTarget() const { return m_P; }

private:

  gp_XYZ m_P;      //!< Source point.
  bool   m_bFound; //!< Whether two points are coincident or not.
  double m_fTol;   //!< Resolution to check for coincidence.

};

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

  // Prepare a map for counting entities.
  NCollection_DataMap<Handle(Standard_Type), int> entCountMap;

  // Cell filter for Cartesian points.
  const double conf = Precision::Confusion();
  //
  NCollection_CellFilter<InspectXYZ> NodeFilter(conf);

  // Iterate all entities.
  Handle(asiAlgo_BaseCloud<double>) pts = new asiAlgo_BaseCloud<double>;
  //
  for ( ; entIt.More(); entIt.Next() )
  {
    const Handle(Standard_Transient)& ent     = entIt.Value();
    const Handle(Standard_Type)&      entType = ent->DynamicType();

    /* Check Cartesian points */

    if ( ent->IsKind( STANDARD_TYPE(StepGeom_CartesianPoint) ) )
    {
      Handle(StepGeom_CartesianPoint)
        cpEnt = Handle(StepGeom_CartesianPoint)::DownCast(ent);

      Handle(TColStd_HArray1OfReal) coords = cpEnt->Coordinates();
      //
      if ( !coords.IsNull() && coords->Size() == 3 )
      {
        gp_XYZ xyz( coords->Value( coords->Lower() ),
                    coords->Value( coords->Lower() + 1 ),
                    coords->Value( coords->Lower() + 2) );

        InspectXYZ Inspect(conf, xyz);
        gp_XYZ XYZ_min = Inspect.Shift( xyz, -Precision::Confusion() );
        gp_XYZ XYZ_max = Inspect.Shift( xyz,  Precision::Confusion() );

        // Coincidence test
        NodeFilter.Inspect(XYZ_min, XYZ_max, Inspect);
        const bool isFound = Inspect.IsFound();
        //
        if ( !isFound )
        {
          pts->AddElement( xyz.X(), xyz.Y(), xyz.Z() );
        }

        NodeFilter.Add(xyz, xyz);
      }
    }

    /* Check B-spline curves with knots */

    if ( ent->IsKind( STANDARD_TYPE(StepGeom_BSplineCurveWithKnots) ) )
    {
      Handle(StepGeom_BSplineCurveWithKnots)
        bcuEnt = Handle(StepGeom_BSplineCurveWithKnots)::DownCast(ent);

      const int nPoles = bcuEnt->NbControlPointsList();

    }

    // Count in a map.
    if ( entCountMap.IsBound(entType) )
      entCountMap(entType)++;
    else
      entCountMap.Bind(entType, 1);
  }
  //
  m_plotter.REDRAW_POINTS("StepGeom_CartesianPoint", pts->GetCoordsArray(), Color_White);

  return false;

  // Dump counts.
  for ( NCollection_DataMap<Handle(Standard_Type), int>::Iterator it(entCountMap); it.More(); it.Next() )
  {
    const Handle(Standard_Type)& entType = it.Key();
    const int                    count   = it.Value();

    m_progress.SendLogMessage(LogInfo(Normal) << "\t%1 : %2" << entType->Name() << count);
  }

  /* ==========================
   *  Translate STEP into BREP
   * ========================== */

  // Transfer all roots into one shape or into several shapes
  try
  {
    reader.TransferRoots();
  }
  catch ( Standard_Failure )
  {
    std::cout << "Warning: exception occurred during translation" << std::endl;
  }
  if ( reader.NbShapes() <= 0 )
  {
    std::cout << "Error: transferring STEP to BREP failed" << std::endl;
    return false;
  }

  TopoDS_Shape aPreResult = reader.OneShape();

  // Release memory after translation
  reader.WS()->NewModel();
  Standard::Purge();

  /* =================
   *  Post-processing
   * ================= */

  result = aPreResult;

  // Apply shape healing
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
