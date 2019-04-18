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

#ifndef asiTest_SuppressBlends_HeaderFile
#define asiTest_SuppressBlends_HeaderFile

// asiTest includes
#include <asiTest_CaseIDs.h>
#include <asiTest_TclTestCase.h>

//! Test functions for suppressing blend chains.
class asiTest_SuppressBlends : public asiTest_TclTestCase
{
public:

  //! Returns Test Case ID.
  //! \return ID of the Test Case.
  static int ID()
  {
    return CaseID_SuppressBlends;
  }

  //! Returns filename for the description.
  //! \return filename for the description of the Test Case.
  static std::string DescriptionFn()
  {
    return "asiTest_SuppressBlends";
  }

  //! Returns Test Case description directory.
  //! \return description directory for the Test Case.
  static std::string DescriptionDir()
  {
    return "editing";
  }

  //! Returns pointers to the Test Functions to launch.
  //! \param[out] functions output collection of pointers.
  static void Functions(AsiTestFunctions& functions)
  {
    functions << &testSuppressBlend001
              << &testSuppressBlend002
              << &testSuppressBlend003
              << &testSuppressBlend004
              << &testSuppressBlend005
              << &testSuppressBlend006
              << &testSuppressBlend007
              << &testSuppressBlend008
              << &testSuppressBlend009
              << &testSuppressBlend010
              << &testSuppressBlend011
              /*<< &testSuppressBlend012*/
              << &testSuppressBlend013
              << &testSuppressBlend014
              << &testSuppressBlend015
              << &testSuppressBlend016
              << &testSuppressBlend017
              << &testSuppressBlend018
              << &testSuppressBlend019
              << &testSuppressBlend020
              << &testSuppressBlend021
              << &testSuppressBlend022
              << &testSuppressBlend023
              << &testSuppressBlend024
              << &testSuppressBlend025
              << &testSuppressBlend026
              << &testSuppressBlend027
              << &testSuppressBlend028
              << &testSuppressBlend029
              //<< &testSuppressBlend030
              << &testSuppressBlend031
              << &testSuppressBlend032
              << &testSuppressBlend033
              << &testSuppressBlend034
              << &testSuppressBlend035
              << &testSuppressBlend036
              << &testSuppressBlend037
              << &testSuppressBlend038
              << &testSuppressBlend039
              << &testSuppressBlend040
              << &testSuppressBlend041
              << &testSuppressBlend042
              << &testSuppressBlend043
    ; // Put semicolon here for convenient adding new functions above ;)
  }

private:

  static outcome runTestScript(const int   funcID,
                               const char* filename);

  static outcome runTest(const int   funcID,
                         const char* shortFilename,
                         const int   seedFaceId);

private:

  static outcome testSuppressBlend001(const int funcID);
  static outcome testSuppressBlend002(const int funcID);
  static outcome testSuppressBlend003(const int funcID);
  static outcome testSuppressBlend004(const int funcID);
  static outcome testSuppressBlend005(const int funcID);
  static outcome testSuppressBlend006(const int funcID);
  static outcome testSuppressBlend007(const int funcID);
  static outcome testSuppressBlend008(const int funcID);
  static outcome testSuppressBlend009(const int funcID);
  static outcome testSuppressBlend010(const int funcID);
  static outcome testSuppressBlend011(const int funcID);
  static outcome testSuppressBlend012(const int funcID);
  static outcome testSuppressBlend013(const int funcID);
  static outcome testSuppressBlend014(const int funcID);
  static outcome testSuppressBlend015(const int funcID);
  static outcome testSuppressBlend016(const int funcID);
  static outcome testSuppressBlend017(const int funcID);
  static outcome testSuppressBlend018(const int funcID);
  static outcome testSuppressBlend019(const int funcID);
  static outcome testSuppressBlend020(const int funcID);
  static outcome testSuppressBlend021(const int funcID);
  static outcome testSuppressBlend022(const int funcID);
  static outcome testSuppressBlend023(const int funcID);
  static outcome testSuppressBlend024(const int funcID);
  static outcome testSuppressBlend025(const int funcID);
  static outcome testSuppressBlend026(const int funcID);
  static outcome testSuppressBlend027(const int funcID);
  static outcome testSuppressBlend028(const int funcID);
  static outcome testSuppressBlend029(const int funcID);
  static outcome testSuppressBlend030(const int funcID);
  static outcome testSuppressBlend031(const int funcID);
  static outcome testSuppressBlend032(const int funcID);
  static outcome testSuppressBlend033(const int funcID);
  static outcome testSuppressBlend034(const int funcID);
  static outcome testSuppressBlend035(const int funcID);
  static outcome testSuppressBlend036(const int funcID);
  static outcome testSuppressBlend037(const int funcID);
  static outcome testSuppressBlend038(const int funcID);
  static outcome testSuppressBlend039(const int funcID);
  static outcome testSuppressBlend040(const int funcID);
  static outcome testSuppressBlend041(const int funcID);
  static outcome testSuppressBlend042(const int funcID);
  static outcome testSuppressBlend043(const int funcID);

};

#endif
