//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_STEP.h>

// OCCT includes
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
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
//! \return true in case of success, false -- otherwise.
bool asiAlgo_STEP::Read(const TCollection_AsciiString& filename,
                        const bool                     doHealing,
                        TopoDS_Shape&                  result)
{
  /* ==================================
   *  Prepare OCCT translation toolkit
   * ================================== */

  STEPControl_Reader aReader;
  Handle(XSControl_WorkSession) aWS = aReader.WS();

  /* ================
   *  Read STEP file
   * ================ */

  if ( aReader.ReadFile( filename.ToCString() ) != IFSelect_RetDone )
  {
    std::cout << "Failed to read the file " << filename.ToCString() << std::endl;
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
    std::cout << "Warning: exception occurred during translation" << std::endl;
  }
  if ( aReader.NbShapes() <= 0 )
  {
    std::cout << "Error: transferring STEP to BREP failed" << std::endl;
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

//! Save the passed CAD model to STEP file.
//! \param shape    [in] shape to store.
//! \param filename [in] file to save.
//! \return true in case of success, false -- otherwise.
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
