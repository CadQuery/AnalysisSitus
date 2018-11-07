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
    functions << &testEuler001
              << &testEuler002
              << &testEuler003
              << &testEuler004
              << &testEuler005
              << &testEuler006
              << &testEuler007
              << &testEuler008
              << &testEuler009
              << &testEuler010
              << &testEuler011
              << &testEuler012
              /*<< &testRecognizeAndSuppress001*/
    ; // Put semicolon here for convenient adding new functions above ;)
  }

private:

  static outcome runTestScript(const int   funcID,
                               const char* filename);

  static outcome runTest(const int   funcID,
                         const char* shortFilename,
                         const int   seedFaceId);

private:

  static outcome testEuler001(const int funcID);
  static outcome testEuler002(const int funcID);
  static outcome testEuler003(const int funcID);
  static outcome testEuler004(const int funcID);
  static outcome testEuler005(const int funcID);
  static outcome testEuler006(const int funcID);
  static outcome testEuler007(const int funcID);
  static outcome testEuler008(const int funcID);
  static outcome testEuler009(const int funcID);
  static outcome testEuler010(const int funcID);
  static outcome testEuler011(const int funcID);
  static outcome testEuler012(const int funcID);

  static outcome testRecognizeAndSuppress001(const int funcID);

};

#endif
