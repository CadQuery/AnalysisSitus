//-----------------------------------------------------------------------------
// Created on: 22 June 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiTest_SuppressBlends.h>

// asiAlgo includes
#include <asiAlgo_RecognizeBlends.h>
#include <asiAlgo_SuppressBlendChain.h>

//-----------------------------------------------------------------------------

outcome asiTest_SuppressBlends::runTestScript(const int   funcID,
                                              const char* filename)
{
  // Get filename of script to execute.
  TCollection_AsciiString fullFilename = GetFilename(filename);

  // Execute test script.
  outcome res = evaluate(fullFilename);

  // Set description variables.
  SetVarDescr("filename", fullFilename,       ID(), funcID);
  SetVarDescr("time",     res.elapsedTimeSec, ID(), funcID);

  // Return status.
  return res;
}

//-----------------------------------------------------------------------------

outcome asiTest_SuppressBlends::runTest(const int   funcID,
                                        const char* shortFilename,
                                        const int   seedFaceId)
{
  outcome res(DescriptionFn(), funcID);

  // Get common facilities.
  Handle(asiTest_CommonFacilities) cf = asiTest_CommonFacilities::Instance();

  // Prepare filename.
  std::string
    filename = asiAlgo_Utils::Str::Slashed( asiAlgo_Utils::Env::AsiTestData() )
             + shortFilename;

  // Read shape.
  TopoDS_Shape shape;
  if ( !asiAlgo_Utils::ReadBRep(filename.c_str(), shape) )
  {
    cf->Progress.SendLogMessage( LogErr(Normal) << "Cannot read file %1."
                                                << filename.c_str() );
    return res.failure();
  }

  // Perform recognition starting from the guess face.
  asiAlgo_RecognizeBlends recognizer(shape, cf->Progress);
  //
  if ( !recognizer.Perform(seedFaceId) )
  {
    cf->Progress.SendLogMessage(LogErr(Normal) << "Recognition failed.");
    return res.failure();
  }

  // Perform suppression.
  asiAlgo_SuppressBlendChain suppressor(recognizer.GetAAG(), cf->Progress);
  //
  if ( !suppressor.Perform(seedFaceId) )
  {
    cf->Progress.SendLogMessage(LogErr(Normal) << "Suppression failed.");
    return res.failure();
  }

  return res.success();
}

//-----------------------------------------------------------------------------

//! Test scenario 001.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend001(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_001.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 002.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend002(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_002.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 003.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend003(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_003.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 004.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend004(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_004.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 005.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend005(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_005.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 006.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend006(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_006.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 007.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend007(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_007.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 008.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend008(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_008.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 009.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend009(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_009.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 010.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend010(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_010.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 011.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend011(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_011.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 012.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend012(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_012.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 013.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend013(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_013.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 014.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend014(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_014.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 015.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend015(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_015.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 016.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend016(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_016.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 017.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend017(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_017.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 018.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend018(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_018.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 019.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend019(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_019.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 020.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend020(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_020.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 021. In this test case, skipping frozen vertices at the
//! edge normalization stage allows to avoid inaccuracy.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend021(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_021.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 022.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend022(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_022.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 023.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend023(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_023.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 024.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend024(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_024.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 025.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend025(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_025.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 026.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend026(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_026.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 027.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend027(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_027.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 028.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend028(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_028.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 029.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend029(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_029.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 030.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend030(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_030.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 031.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend031(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_031.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 032.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend032(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_032.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 033.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend033(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_033.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 034.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend034(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_034.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 035.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend035(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_035.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 036.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend036(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_036.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 037.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend037(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_037.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 038.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend038(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_038.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 039.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend039(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_039.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 040.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend040(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_040.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 041.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend041(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_041.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 042.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend042(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_042.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 043.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend043(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_043.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 044.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend044(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_044.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 045.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend045(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_045.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 046.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend046(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_046.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 047.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend047(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_047_metadata.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 048.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend048(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_048_metadata.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 049.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend049(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_049_metadata.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 050.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend050(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_050_metadata.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 051.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend051(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_051.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 052.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend052(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_052.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 053.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend053(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_053.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 054.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend054(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_054.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 055.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend055(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_055.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 056.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend056(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_056.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 057.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend057(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_057.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 058.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend058(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_058.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 059.
//!
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testSuppressBlend059(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_059.tcl");
}
