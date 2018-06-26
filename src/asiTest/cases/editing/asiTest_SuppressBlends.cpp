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

//-----------------------------------------------------------------------------

// Filenames are specified relatively to ASI_TEST_SCRIPTS environment variable.
#define filename_test001 "editing/suppress_blend_001.tcl"
#define filename_test002 "editing/suppress_blend_002.tcl"
#define filename_test003 "editing/suppress_blend_003.tcl"
#define filename_test004 "editing/suppress_blend_004.tcl"
#define filename_test005 "editing/suppress_blend_005.tcl"
#define filename_test006 "editing/suppress_blend_006.tcl"
#define filename_test007 "editing/suppress_blend_007.tcl"
#define filename_test008 "editing/suppress_blend_008.tcl"
#define filename_test009 "editing/suppress_blend_009.tcl"

//-----------------------------------------------------------------------------

//! Test scenario 001.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::test001(const int funcID)
{
  // Get filename of script to execute.
  TCollection_AsciiString filename = GetFilename(filename_test001);

  // Execute test script.
  outcome res = evaluate(filename);

  // Set description variables.
  SetVarDescr("filename", filename,           ID(), funcID);
  SetVarDescr("time",     res.elapsedTimeSec, ID(), funcID);

  // Return status.
  return res;
}

//-----------------------------------------------------------------------------

//! Test scenario 002.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::test002(const int funcID)
{
  // Get filename of script to execute.
  TCollection_AsciiString filename = GetFilename(filename_test002);

  // Execute test script.
  outcome res = evaluate(filename);

  // Set description variables.
  SetVarDescr("filename", filename,           ID(), funcID);
  SetVarDescr("time",     res.elapsedTimeSec, ID(), funcID);

  // Return status.
  return res;
}

//-----------------------------------------------------------------------------

//! Test scenario 003.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::test003(const int funcID)
{
  // Get filename of script to execute.
  TCollection_AsciiString filename = GetFilename(filename_test003);

  // Execute test script.
  outcome res = evaluate(filename);

  // Set description variables.
  SetVarDescr("filename", filename,           ID(), funcID);
  SetVarDescr("time",     res.elapsedTimeSec, ID(), funcID);

  // Return status.
  return res;
}

//-----------------------------------------------------------------------------

//! Test scenario 004.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::test004(const int funcID)
{
  // Get filename of script to execute.
  TCollection_AsciiString filename = GetFilename(filename_test004);

  // Execute test script.
  outcome res = evaluate(filename);

  // Set description variables.
  SetVarDescr("filename", filename,           ID(), funcID);
  SetVarDescr("time",     res.elapsedTimeSec, ID(), funcID);

  // Return status.
  return res;
}

//-----------------------------------------------------------------------------

//! Test scenario 005.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::test005(const int funcID)
{
  // Get filename of script to execute.
  TCollection_AsciiString filename = GetFilename(filename_test005);

  // Execute test script.
  outcome res = evaluate(filename);

  // Set description variables.
  SetVarDescr("filename", filename,           ID(), funcID);
  SetVarDescr("time",     res.elapsedTimeSec, ID(), funcID);

  // Return status.
  return res;
}

//-----------------------------------------------------------------------------

//! Test scenario 006.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::test006(const int funcID)
{
  // Get filename of script to execute.
  TCollection_AsciiString filename = GetFilename(filename_test006);

  // Execute test script.
  outcome res = evaluate(filename);

  // Set description variables.
  SetVarDescr("filename", filename,           ID(), funcID);
  SetVarDescr("time",     res.elapsedTimeSec, ID(), funcID);

  // Return status.
  return res;
}

//-----------------------------------------------------------------------------

//! Test scenario 007.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::test007(const int funcID)
{
  // Get filename of script to execute.
  TCollection_AsciiString filename = GetFilename(filename_test007);

  // Execute test script.
  outcome res = evaluate(filename);

  // Set description variables.
  SetVarDescr("filename", filename,           ID(), funcID);
  SetVarDescr("time",     res.elapsedTimeSec, ID(), funcID);

  // Return status.
  return res;
}

//-----------------------------------------------------------------------------

//! Test scenario 008.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::test008(const int funcID)
{
  // Get filename of script to execute.
  TCollection_AsciiString filename = GetFilename(filename_test008);

  // Execute test script.
  outcome res = evaluate(filename);

  // Set description variables.
  SetVarDescr("filename", filename,           ID(), funcID);
  SetVarDescr("time",     res.elapsedTimeSec, ID(), funcID);

  // Return status.
  return res;
}

//-----------------------------------------------------------------------------

//! Test scenario 009.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_SuppressBlends::test009(const int funcID)
{
  // Get filename of script to execute.
  TCollection_AsciiString filename = GetFilename(filename_test009);

  // Execute test script.
  outcome res = evaluate(filename);

  // Set description variables.
  SetVarDescr("filename", filename,           ID(), funcID);
  SetVarDescr("time",     res.elapsedTimeSec, ID(), funcID);

  // Return status.
  return res;
}
