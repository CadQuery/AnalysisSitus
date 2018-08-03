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

#ifndef asiTest_EdgeVexity_HeaderFile
#define asiTest_EdgeVexity_HeaderFile

// asiTest includes
#include <asiTest_CaseIDs.h>
#include <asiTest_TclTestCase.h>

//! Test functions for checking vexity of edges (convex, concave, etc.).
class asiTest_EdgeVexity : public asiTest_TclTestCase
{
public:

  //! Returns Test Case ID.
  //! \return ID of the Test Case.
  static int ID()
  {
    return CaseID_EdgeVexity;
  }

  //! Returns filename for the description.
  //! \return filename for the description of the Test Case.
  static std::string DescriptionFn()
  {
    return "asiTest_EdgeVexity";
  }

  //! Returns Test Case description directory.
  //! \return description directory for the Test Case.
  static std::string DescriptionDir()
  {
    return "inspection";
  }

  //! Returns pointers to the Test Functions to launch.
  //! \param[out] functions output collection of pointers.
  static void Functions(AsiTestFunctions& functions)
  {
    functions << &test001
              << &test002
              << &test003
              << &test004
              << &test005
              << &test006
              << &test007
              << &test008
              << &test009
              << &test010
              << &test011
              << &test012
              << &test013
              << &test014
              << &test015
              << &test016
              << &test017
              << &test018
              << &test019
              << &test020
              << &test021
              << &test022
              << &test023
              << &test024
              << &test025
              << &test026
              << &test027
              << &test028
              << &test029
              << &test030
    ; // Put semicolon here for convenient adding new functions above ;)
  }

private:

  static outcome runtest(const int   funcID,
                         const char* filename);

private:

  static outcome test001(const int funcID);
  static outcome test002(const int funcID);
  static outcome test003(const int funcID);
  static outcome test004(const int funcID);
  static outcome test005(const int funcID);
  static outcome test006(const int funcID);
  static outcome test007(const int funcID);
  static outcome test008(const int funcID);
  static outcome test009(const int funcID);
  static outcome test010(const int funcID);
  static outcome test011(const int funcID);
  static outcome test012(const int funcID);
  static outcome test013(const int funcID);
  static outcome test014(const int funcID);
  static outcome test015(const int funcID);
  static outcome test016(const int funcID);
  static outcome test017(const int funcID);
  static outcome test018(const int funcID);
  static outcome test019(const int funcID);
  static outcome test020(const int funcID);
  static outcome test021(const int funcID);
  static outcome test022(const int funcID);
  static outcome test023(const int funcID);
  static outcome test024(const int funcID);
  static outcome test025(const int funcID);
  static outcome test026(const int funcID);
  static outcome test027(const int funcID);
  static outcome test028(const int funcID);
  static outcome test029(const int funcID);
  static outcome test030(const int funcID);

};

#endif
