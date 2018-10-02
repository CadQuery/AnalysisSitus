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
#include <asiTest_AAG.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>
#include <asiAlgo_AAGIterator.h>

//-----------------------------------------------------------------------------

// Filenames are specified relatively to ASI_TEST_DATA environment variable.
#define filename_brep_001 "cad/box.brep"
#define filename_brep_002 "cad/ANC101_isolated_components.brep"

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

  // Prepare AAG allowing smooth dihedral edges for better performance.
  Handle(asiAlgo_AAG) aag = new asiAlgo_AAG(shape, true);

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
      cf->ProgressNotifier->SendLogMessage( LogErr(Normal) << "Not enough reference data (out of range error)." );
      return res.failure();
    }

    if ( iNext != refFaceIds[refIdx] )
    {
      cf->ProgressNotifier->SendLogMessage( LogErr(Normal) << "Unexpected next ID (expected %1, while current is %2)."
                                                           << refFaceIds[refIdx] << iNext );
      return res.failure();
    }
  }

  // Check the number of iterations.
  if ( refFaceIds.size() != refIdx )
  {
    cf->ProgressNotifier->SendLogMessage( LogErr(Normal) << "Unexpected number of iterations (expected %1, while realized %2)."
                                                         << int( refFaceIds.size() ) << int(refIdx) );
    return res.failure();
  }

  // Set description variables.
  SetVarDescr("time", res.elapsedTimeSec, ID(), funcID);

  // Return success.
  return res.success();
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::test001(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_001,
                                   1,
                                  {1, 6, 2, 5, 4, 3} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::test002(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   1,
                                  {1, 6, 9, 3, 2} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::test003(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   2,
                                  {2, 9, 6, 3, 1} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::test004(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   39,
                                  {39, 38} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::test005(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   38,
                                  {38, 39} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::test006(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   5,
                                  {5, 14, 34, 37, 40, 41} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::test007(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   7,
                                  {7, 31, 8} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::test008(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   35,
                                  {35, 22, 21, 20, 19, 18, 17, 16, 15} );
}

//-----------------------------------------------------------------------------

outcome asiTest_AAG::test009(const int funcID)
{
  return testAllNeighborsIterator( funcID,
                                   filename_brep_002,
                                   4,
                                  {4, 13, 33, 32, 12, 11, 10} );
}
