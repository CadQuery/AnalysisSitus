//-----------------------------------------------------------------------------
// Created on: 02 August 2018
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
#include <asiTest_EdgeVexity.h>

//-----------------------------------------------------------------------------

outcome asiTest_EdgeVexity::runtest(const int   funcID,
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

//! Test scenario 001.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test001(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_001.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 002.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test002(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_002.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 003.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test003(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_003.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 004.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test004(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_004.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 005.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test005(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_005.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 006.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test006(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_006.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 007.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test007(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_007.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 008.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test008(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_008.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 009.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test009(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_009.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 010.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test010(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_010.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 011.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test011(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_011.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 012.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test012(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_012.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 013.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test013(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_013.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 014.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test014(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_014.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 015.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test015(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_015.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 016.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test016(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_016.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 017.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test017(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_017.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 018.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test018(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_018.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 019.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test019(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_019.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 020.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test020(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_020.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 021.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test021(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_021.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 022.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test022(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_022.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 023.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test023(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_023.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 024.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test024(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_024.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 025.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test025(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_025.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 026.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test026(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_026.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 027.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test027(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_027.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 028.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test028(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_028.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 029.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test029(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_029.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 030.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test030(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_030.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 031.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test031(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_031.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 032.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test032(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_032.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 033.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test033(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_033.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 034.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test034(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_034.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 035.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test035(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_035.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 036.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test036(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_036.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 037.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test037(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_037.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 038.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test038(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_038.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 039.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test039(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_039.tcl");
}

//-----------------------------------------------------------------------------

//! Test scenario 040.
//! \param[in] funcID ID of the Test Function.
//! \return true in case of success, false -- otherwise.
outcome asiTest_EdgeVexity::test040(const int funcID)
{
  return runtest(funcID, "inspection/edge-vexity_040.tcl");
}
