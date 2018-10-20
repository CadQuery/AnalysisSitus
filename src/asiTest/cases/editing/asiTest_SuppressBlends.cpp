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
    cf->ProgressNotifier->SendLogMessage( LogErr(Normal) << "Cannot read file %1."
                                                         << filename.c_str() );
    return res.failure();
  }

  // Perform recognition starting from the guess face.
  asiAlgo_RecognizeBlends recognizer(shape, cf->ProgressNotifier);
  //
  if ( !recognizer.Perform(seedFaceId) )
  {
    cf->ProgressNotifier->SendLogMessage(LogErr(Normal) << "Recognition failed.");
    return res.failure();
  }

  // Perform suppression.
  asiAlgo_SuppressBlendChain suppressor(recognizer.GetAAG(), cf->ProgressNotifier);
  //
  if ( !suppressor.Perform(seedFaceId) )
  {
    cf->ProgressNotifier->SendLogMessage(LogErr(Normal) << "Suppression failed.");
    return res.failure();
  }

  return res.success();
}

//-----------------------------------------------------------------------------

//! Test scenario 001.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler001(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_001.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 002.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler002(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_002.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 003.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler003(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_003.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 004.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler004(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_004.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 005.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler005(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_005.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 006.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler006(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_006.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 007.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler007(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_007.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 008.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler008(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_008.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 009.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler009(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_009.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 010.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler010(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_010.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 011.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler011(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_011.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 012.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testEuler012(const int funcID)
{
  return runTestScript(funcID, "editing/kill-blend/kill-blend_012.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario for automatic recognition and suppression.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::testRecognizeAndSuppress001(const int funcID)
{
  return outcome().failure();
}
