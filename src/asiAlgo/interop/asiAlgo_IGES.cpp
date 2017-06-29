//-----------------------------------------------------------------------------
// Created on: 17 February 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_IGES.h>

// OCCT includes
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <Interface_Static.hxx>
#include <Message_ProgressSentry.hxx>
#include <ShapeFix_Shape.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

//! Performs IGES import.
//! \param filename  [in]  file to read.
//! \param doHealing [in]  indicates whether to run shape healing after import.
//! \param result    [out] retrieved shape.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_IGES::Read(const TCollection_AsciiString& filename,
                        const bool                     doHealing,
                        TopoDS_Shape&                  result)
{
  /* ==================================
   *  Prepare OCCT translation toolkit
   * ================================== */

  IGESControl_Reader aReader;
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
