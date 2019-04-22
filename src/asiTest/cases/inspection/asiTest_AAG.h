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

#ifndef asiTest_AAG_HeaderFile
#define asiTest_AAG_HeaderFile

// asiTest includes
#include <asiTest_CaseIDs.h>

// asiTestEngine includes
#include <asiTestEngine_TestCase.h>

//! Test functions for attributed adjacency graphs (AAG).
class asiTest_AAG : public asiTestEngine_TestCase
{
public:

  //! Returns Test Case ID.
  //! \return ID of the Test Case.
  static int ID()
  {
    return CaseID_AAG;
  }

  //! Returns filename for the description.
  //! \return filename for the description of the Test Case.
  static std::string DescriptionFn()
  {
    return "asiTest_AAG";
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
    functions << &testNeighborsIterator001
              << &testNeighborsIterator002
              << &testNeighborsIterator003
              << &testNeighborsIterator004
              << &testNeighborsIterator005
              << &testNeighborsIterator006
              << &testNeighborsIterator007
              << &testNeighborsIterator008
              << &testNeighborsIterator009
              << &testJSON01
              << &testJSON02
              << &testNaming01
              << &testNaming02
              << &testNaming03
              << &testNaming04
    ; // Put semicolon here for convenient adding new functions above ;)
  }

private:

  static TopoDS_Shape
    readBRep(const char* shortFilename);

  static bool
    prepareAAGFromFile(const char*          shortFilename,
                       Handle(asiAlgo_AAG)& aag);

  static outcome
    testAllNeighborsIterator(const int               funcID,
                             const char*             filename,
                             const int               seedFaceId,
                             const std::vector<int>& refFaceIds);

  static outcome
    testAAG2JSON(const int   funcID,
                 const char* shortFilename,
                 const char* shortFilenameRef);

  static outcome
    testAAGIndices(const int           funcID,
                   const TopoDS_Shape& shape);

  static bool
    checkNamesVersusIds(const Handle(asiAlgo_AAG)& aag,
                        const TopAbs_ShapeEnum     shapeType,
                        ActAPI_ProgressEntry       progress);

private:

  static outcome testNeighborsIterator001 (const int funcID);
  static outcome testNeighborsIterator002 (const int funcID);
  static outcome testNeighborsIterator003 (const int funcID);
  static outcome testNeighborsIterator004 (const int funcID);
  static outcome testNeighborsIterator005 (const int funcID);
  static outcome testNeighborsIterator006 (const int funcID);
  static outcome testNeighborsIterator007 (const int funcID);
  static outcome testNeighborsIterator008 (const int funcID);
  static outcome testNeighborsIterator009 (const int funcID);
  static outcome testJSON01               (const int funcID);
  static outcome testJSON02               (const int funcID);
  static outcome testNaming01             (const int funcID);
  static outcome testNaming02             (const int funcID);
  static outcome testNaming03             (const int funcID);
  static outcome testNaming04             (const int funcID);

};

#endif
