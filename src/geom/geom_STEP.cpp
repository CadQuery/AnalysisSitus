//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <geom_STEP.h>

// OCCT includes
#include <STEPControl_Reader.hxx>
#include <Interface_Static.hxx>
#include <Message_ProgressSentry.hxx>
#include <ShapeFix_Shape.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

//! Performs STEP import.
//! \param filename  [in]  file to read.
//! \param doHealing [in]  indicates whether to run shape healing after import.
//! \param result    [out] retrieved shape.
bool geom_STEP::Read(const TCollection_AsciiString& filename,
                     const bool                     doHealing,
                     TopoDS_Shape&                  result)
{
  /* ==================================
   *  Prepare OCCT translation toolkit
   * ================================== */

  STEPControl_Reader aReader;
  Handle(XSControl_WorkSession) aWS = aReader.WS();
  Interface_Static::SetCVal("xstep.cascade.unit", "M");

  /* ================
   *  Read STEP file
   * ================ */

  if ( aReader.ReadFile( filename.ToCString() ) != IFSelect_RetDone )
  {
#ifdef COUT_DEBUG
    cout << "Failed to read the file " << filename.ToCString() << endl;
#endif
    return false;
  }

  /* ==========================
   *  Translate STEP into BREP
   * ========================== */

  // Transfer all roots into one shape or into several shapes
  try
  {
    aReader.TransferRoots();
  }
  catch ( Standard_Failure )
  {
#ifdef COUT_DEBUG
    cout << "Warning: exception occurred during translation" << endl;
#endif
  }
  if ( aReader.NbShapes() <= 0 )
  {
#ifdef COUT_DEBUG
    cout << "Error: transferring STEP to BREP failed" << endl;
#endif
    return false;
  }

  TopoDS_Shape aPreResult = aReader.OneShape();

  // Release memory after translation
  aWS->NewModel();
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
      aFixRes = aShapeHealer->Perform() > 0;
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
