//-----------------------------------------------------------------------------
// Created on: 16 October 2018
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
#include <asiTest_RecognizeBlends.h>

// asiAlgo includes
#include <asiAlgo_RecognizeBlends.h>

#undef FILE_DEBUG
#if defined FILE_DEBUG
  #pragma message("===== warning: FILE_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

// Filenames are specified relatively to ASI_TEST_DATA environment variable.
#define filename_boxblend_01        "cad/blends/boxblend_01.brep"
#define filename_boxblend_01_f3_ref "reference/aag/test_boxblend_01_f3_ref.json"
//
#define filename_boxblend_02        "cad/blends/boxblend_02.brep"
#define filename_boxblend_02_f3_ref "reference/aag/test_boxblend_02_f3_ref.json"
//
#define filename_bb_boxblend_03         "cad/blends/bb_boxblend_03.brep"
#define filename_bb_boxblend_03_f29_ref "reference/aag/test_bb_boxblend_03_f29_ref.json"

//-----------------------------------------------------------------------------

outcome asiTest_RecognizeBlends::runtest(const int   funcID,
                                         const char* shortFilename,
                                         const char* shortFilenameRef,
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

  // Prepare recognizer.
  asiAlgo_RecognizeBlends recognizer(shape, cf->ProgressNotifier);

  // Perform recognition starting from the guess face.
  if ( !recognizer.Perform(seedFaceId) )
  {
    cf->ProgressNotifier->SendLogMessage(LogErr(Normal) << "Recognition failed.");
    return res.failure();
  }

  // Get AAG after recognition.
  const Handle(asiAlgo_AAG)& aag = recognizer.GetAAG();

  // Dump AAG to JSON.
  std::stringstream ss;
  aag->DumpJSON(ss);

#if defined FILE_DEBUG
  std::ofstream filestream("aagrecognized.json");
  //
  if ( !filestream.is_open() )
  {
    cf->ProgressNotifier->SendLogMessage(LogErr(Normal) << "FILE_DEBUG: file cannot be opened.");
    return res.failure();
  }
  //
  aag->DumpJSON(filestream);
  filestream.close();
#endif

  // Read JSON from file.
  std::string
    refFilename = asiAlgo_Utils::Str::Slashed( asiAlgo_Utils::Env::AsiTestData() )
                + shortFilenameRef;
  //
  std::ifstream refFile(refFilename);
  std::stringstream refBuffer;
  refBuffer << refFile.rdbuf();
  //
  std::string refJson = refBuffer.str();
  std::string json    = ss.str();

  // Verify.
  if ( json != refJson )
  {
    cf->ProgressNotifier->SendLogMessage(LogErr(Normal) << "Dumped JSON is different from the expected one.");
    return res.failure();
  }

  return res.success();
}

//-----------------------------------------------------------------------------

outcome asiTest_RecognizeBlends::test_boxblend_01_f3(const int funcID)
{
  return runtest(funcID,
                 filename_boxblend_01,
                 filename_boxblend_01_f3_ref,
                 3);
}

//-----------------------------------------------------------------------------

outcome asiTest_RecognizeBlends::test_boxblend_02_f3(const int funcID)
{
  return runtest(funcID,
                 filename_boxblend_02,
                 filename_boxblend_02_f3_ref,
                 3);
}

//-----------------------------------------------------------------------------

outcome asiTest_RecognizeBlends::test_bb_boxblend_03_f29(const int funcID)
{
  return runtest(funcID,
                 filename_bb_boxblend_03,
                 filename_bb_boxblend_03_f29_ref,
                 29);
}
