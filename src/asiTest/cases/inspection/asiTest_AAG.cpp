//-----------------------------------------------------------------------------
// Created on: 01 October 2018
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
#include <asiTest_AAG.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_AAGIterator.h>

// OCCT includes
#include <BRepPrimAPI_MakeBox.hxx>

#undef FILE_DEBUG
#if defined FILE_DEBUG
  #pragma message("===== warning: FILE_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

// Filenames are specified relatively to ASI_TEST_DATA environment variable.
#define filename_brep_001 "cad/box.brep"
#define filename_brep_002 "cad/ANC101_isolated_components.brep"
#define filename_brep_003 "cad/ANC101.brep"
#define filename_brep_004 "cad/blends/3boxesblend_06.brep"
#define filename_brep_005 "cad/blends/customblend_04.brep"
#define filename_brep_006 "cad/blends/nist_ctc_01_asme1_ap242.brep"
//
#define filename_json_001 "reference/aag/testJSON01.json"
#define filename_json_002 "reference/aag/testJSON02.json"

//-----------------------------------------------------------------------------

TopoDS_Shape asiTest_AAG::readBRep(const char* shortFilename)
{
  // Get common facilities.
  Handle(asiTest_CommonFacilities) cf = asiTest_CommonFacilities::Instance();

  // Prepare filename.
  std::string
    filename = asiAlgo_Utils::Str::Slashed( asiAlgo_Utils::Env::AsiTestData() )
             + shortFilename;

  // Read shape.
  TopoDS_Shape shape;
  //
  if ( !asiAlgo_Utils::ReadBRep(filename.c_str(), shape) )
  {
    cf->Progress.SendLogMessage( LogErr(Normal) << "Cannot read file %1."
                                                << filename.c_str() );
    return TopoDS_Shape();
  }

  return shape;
}

//-----------------------------------------------------------------------------

bool asiTest_AAG::prepareAAGFromFile(const char*          shortFilename,
                                     Handle(asiAlgo_AAG)& aag)
{
  TopoDS_Shape shape = readBRep(shortFilename);
  if ( shape.IsNull() )
    return false;

  // Prepare AAG allowing smooth dihedral edges for better performance.
  aag = new asiAlgo_AAG(shape, true);
  return true;
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testAllNeighborsIterator(const int               funcID,
                                              const char*             shortFilename,
                                              const int               seedFaceId,
                                              const std::vector<int>& refFaceIds)
{
  // Prepare outcome.
  outcome res(DescriptionFn(), funcID);

  // Get common facilities.
  Handle(asiTest_CommonFacilities) cf = asiTest_CommonFacilities::Instance();

  // Prepare AAG.
  Handle(asiAlgo_AAG) aag;
  //
  if ( !prepareAAGFromFile(shortFilename, aag) )
    return res.failure();

  // Use AAG iterator for neighbor faces.
  size_t refIdx = 0;
  //
  asiAlgo_AAGNeighborsIterator<asiAlgo_AAGIterationRule::AllowAny>
    nit( aag, seedFaceId, new asiAlgo_AAGIterationRule::AllowAny() );
  //
  for ( ; nit.More(); nit.Next(), refIdx++ )
  {
    const int iNext = nit.GetFaceId();

    if ( refIdx >= refFaceIds.size() )
    {
      cf->Progress.SendLogMessage( LogErr(Normal) << "Not enough reference data (out of range error)." );
      return res.failure();
    }

    if ( iNext != refFaceIds[refIdx] )
    {
      cf->Progress.SendLogMessage( LogErr(Normal) << "Unexpected next ID (expected %1, while current is %2)."
                                                           << refFaceIds[refIdx] << iNext );
      return res.failure();
    }
  }

  // Check the number of iterations.
  if ( refFaceIds.size() != refIdx )
  {
    cf->Progress.SendLogMessage( LogErr(Normal) << "Unexpected number of iterations (expected %1, while realized %2)."
                                                         << int( refFaceIds.size() ) << int(refIdx) );
    return res.failure();
  }

  // Set description variables.
  SetVarDescr("time", res.elapsedTimeSec, ID(), funcID);

  // Return success.
  return res.success();
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testAAG2JSON(const int   funcID,
                                  const char* shortFilename,
                                  const char* shortFilenameRef)
{
  // Prepare outcome.
  outcome res(DescriptionFn(), funcID);

  // Get common facilities.
  Handle(asiTest_CommonFacilities) cf = asiTest_CommonFacilities::Instance();

  // Prepare AAG.
  Handle(asiAlgo_AAG) aag;
  //
  if ( !prepareAAGFromFile(shortFilename, aag) )
    return res.failure();

  // Dump AAG to JSON.
  std::stringstream ss;
  aag->DumpJSON(ss);

#if defined FILE_DEBUG
  std::ofstream filestream("aagdump.json");
  //
  if ( !filestream.is_open() )
  {
    cf->Progress.SendLogMessage(LogErr(Normal) << "FILE_DEBUG: file cannot be opened.");
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
    cf->Progress.SendLogMessage(LogErr(Normal) << "Dumped JSON is different from the expected one.");
    return res.failure();
  }

  return res.success();
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testAAGIndices(const int           funcID,
                                    const TopoDS_Shape& shape)
{
  // Get common facilities.
  Handle(asiTest_CommonFacilities) cf = asiTest_CommonFacilities::Instance();

  // Prepare outcome.
  outcome res(DescriptionFn(), funcID);

  // Prepare AAG (indexation follows right at the same time).
  Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(shape, true);

  // Create naming service to give all sub-shapes their persistent names.
  Handle(asiAlgo_Naming) naming = new asiAlgo_Naming(shape);

  /* Iterate sub-shapes indexed in AAG and check their persistent names. Those
     names should normally be derived from the indices. */

  if ( !checkNamesVersusIds(aag, naming, TopAbs_FACE, cf->Progress) )
    return res.failure();

  if ( !checkNamesVersusIds(aag, naming, TopAbs_EDGE, cf->Progress) )
    return res.failure();

  if ( !checkNamesVersusIds(aag, naming, TopAbs_VERTEX, cf->Progress) )
    return res.failure();

  return res.success();
}

//-----------------------------------------------------------------------------

bool asiTest_AAG::checkNamesVersusIds(const Handle(asiAlgo_AAG)&    aag,
                                      const Handle(asiAlgo_Naming)& naming,
                                      const TopAbs_ShapeEnum        shapeType,
                                      ActAPI_ProgressEntry          progress)
{
  asiTest_NotUsed(naming);

  TopTools_IndexedMapOfShape mapOfShapes;
  aag->RequestMapOf(shapeType, mapOfShapes);

  // Iterate over the map indices and compare those indices with the suffixes
  // of persistent names.
  for ( int k = 1; k <= mapOfShapes.Extent(); ++k )
  {
    TCollection_AsciiString
      shapeName = asiAlgo_Naming::PrepareName(shapeType, k);

    // The name is composed of two parts where the second one is the index.
    // This latter index should be equal to the index in AAG.
    std::vector<TCollection_AsciiString> nameChunks;
    asiAlgo_Utils::Str::Split(shapeName, "_", nameChunks);
    //
    if ( nameChunks.size() != 2 )
    {
      progress.SendLogMessage(LogErr(Normal) << "Unexpected number of name chunks.");
      return false;
    }
    //
    if ( !nameChunks[1].IsIntegerValue() )
    {
      progress.SendLogMessage(LogErr(Normal) << "The second name chunk is not an integer.");
      return false;
    }

    // Get ID from the name and verify.
    const int idOfName = nameChunks[1].IntegerValue();
    //
    if ( idOfName != k )
    {
      progress.SendLogMessage(LogErr(Normal) << "ID of name '%1' does not correspond to serial ID %2."
                                             << shapeName << k);
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNeighborsIterator001(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_001,
                                   1,
                                  {1, 6, 2, 5, 4, 3} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNeighborsIterator002(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   1,
                                  {1, 6, 9, 3, 2} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNeighborsIterator003(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   2,
                                  {2, 9, 6, 3, 1} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNeighborsIterator004(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   39,
                                  {39, 38} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNeighborsIterator005(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   38,
                                  {38, 39} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNeighborsIterator006(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   5,
                                  {5, 14, 34, 37, 40, 41} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNeighborsIterator007(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   7,
                                  {7, 31, 8} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNeighborsIterator008(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   35,
                                  {35, 22, 21, 20, 19, 18, 17, 16, 15} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNeighborsIterator009(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   4,
                                  {4, 13, 33, 32, 12, 11, 10} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testJSON01(const int funcID)
{
  return testAAG2JSON(funcID, filename_brep_001, filename_json_001);
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testJSON02(const int funcID)
{
  return testAAG2JSON(funcID, filename_brep_003, filename_json_002);
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNaming01(const int funcID)
{
  // Make a unit box as a working body.
  TopoDS_Shape box = BRepPrimAPI_MakeBox(1, 1, 1);

  return testAAGIndices(funcID, box);
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNaming02(const int funcID)
{
  return testAAGIndices( funcID, readBRep(filename_brep_004) );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNaming03(const int funcID)
{
  return testAAGIndices( funcID, readBRep(filename_brep_005) );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::testNaming04(const int funcID)
{
  return testAAGIndices( funcID, readBRep(filename_brep_006) );
}
