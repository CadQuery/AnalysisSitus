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

// asiTest includes
#include <asiTest_AAG.h>
#include <asiTest_CommonFacilities.h>
#include <asiTest_EdgeVexity.h>
#include <asiTest_InvertShells.h>
#include <asiTest_IsContourClosed.h>
#include <asiTest_KEV.h>
#include <asiTest_RecognizeBlends.h>
#include <asiTest_SuppressBlends.h>

// asiTestEngine includes
#include <asiTestEngine_Launcher.h>

// asiTcl includes
#include <asiTcl_Plugin.h>

#define PAUSE \
  system("pause");

#define RET_OK \
  PAUSE \
  return 0;

#define RET_FAILURE \
  PAUSE \
  return 1;

#define PRINT_DECOR \
  std::cout << "------------------------------------------------------------" << std::endl;

//-----------------------------------------------------------------------------

DEFINE_TEST_VARIABLES

//-----------------------------------------------------------------------------

#define TEST_LOAD_MODULE(name) \
{ \
  Handle(asiTest_CommonFacilities) cf = asiTest_CommonFacilities::Instance();\
  \
  if ( !asiTcl_Plugin::Load(cf->Interp, cf, name) ) \
    cf->ProgressNotifier->SendLogMessage(LogErr(Normal) << "Cannot load %1 commands." << name); \
  else \
    cf->ProgressNotifier->SendLogMessage(LogInfo(Normal) << "Loaded %1 commands." << name); \
}

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  asiTest_NotUsed(argc);
  asiTest_NotUsed(argv);

  // Load commands.
  TEST_LOAD_MODULE("cmdMisc")
  TEST_LOAD_MODULE("cmdEngine")

  // Populate launchers.
  std::cout << "asiTest : main()" << std::endl;
  std::vector< Handle(asiTestEngine_CaseLauncherAPI) > CaseLaunchers;
  //
  CaseLaunchers.push_back( new asiTestEngine_CaseLauncher<asiTest_InvertShells>    );
  CaseLaunchers.push_back( new asiTestEngine_CaseLauncher<asiTest_KEV>             );
  CaseLaunchers.push_back( new asiTestEngine_CaseLauncher<asiTest_RecognizeBlends> );
  CaseLaunchers.push_back( new asiTestEngine_CaseLauncher<asiTest_SuppressBlends>  );
  CaseLaunchers.push_back( new asiTestEngine_CaseLauncher<asiTest_AAG>             );
  CaseLaunchers.push_back( new asiTestEngine_CaseLauncher<asiTest_EdgeVexity>      );
  CaseLaunchers.push_back( new asiTestEngine_CaseLauncher<asiTest_IsContourClosed> );

  // Launcher of entire test suite
  asiTestEngine_Launcher Launcher;
  for ( int c = 0; c < (int) CaseLaunchers.size(); ++c )
    Launcher << CaseLaunchers[c];

  PRINT_DECOR
  if ( !Launcher.Launch(&std::cout) ) // Launch test cases.
  {
    std::cout << "\t***\n\tTests FAILED" << std::endl;
    PRINT_DECOR
    return 1;
  }

  std::cout << "\t***\n\tTests SUCCEEDED" << std::endl;
  PRINT_DECOR
  return 0;
}
