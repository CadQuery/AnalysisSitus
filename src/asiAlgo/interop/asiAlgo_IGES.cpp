//-----------------------------------------------------------------------------
// Created on: 17 February 2016
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
