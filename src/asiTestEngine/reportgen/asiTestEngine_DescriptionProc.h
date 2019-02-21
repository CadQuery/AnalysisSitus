//-----------------------------------------------------------------------------
// Created on: 30 November 2013
//-----------------------------------------------------------------------------
// Copyright (c) 2013-2018, Sergey Slyadnev
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

#ifndef asiTestEngine_DescriptionProc_HeaderFile
#define asiTestEngine_DescriptionProc_HeaderFile

// asiTestEngine includes
#include <asiTestEngine_Macro.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// STD includes
#include <map>
#include <vector>

//! Utility class for processing of description files corresponding to Test
//! Cases.
class asiTestEngine_DescriptionProc
{
public:

  //! shortcut for variable expansion maps.
  typedef std::map<std::string, std::string> StrStrMap;

  //! shortcut for string-string pairs.
  typedef std::pair<std::string, std::string> StrStrPair;

  //! Shortcut for ordered collection of strings.
  typedef std::vector<std::string> SeqStr;

public:

  //! Performs processing of description template substituting variable
  //! placeholders with actual values.
  //!
  //! \param[in]  dir            directory to search the description file in.
  //! \param[in]  filename       filename pointing to description template.
  //! \param[in]  vars           variables expansion map.
  //! \param[in]  caseID         ID of the Test Case.
  //! \param[in]  numFunctions   number of test functions in the Test Case.
  //! \param[out] title          title of the Test Case.
  //! \param[out] overviewBlocks overview sections.
  //! \param[out] detailsBlocks  details sections.
  //!
  //! \return true in case of success, false -- otherwise.
  asiTestEngine_EXPORT static bool
    Process(const std::string& dir,
            const std::string& filename,
            const StrStrMap&   vars,
            const int          caseID,
            const int          numFunctions,
            std::string&       title,
            SeqStr&            overviewBlocks,
            SeqStr&            detailsBlocks);

private:

  //! Extracts semantic sections from the given text.
  //!
  //! \param[in]  numFunctions   number of test functions in the processed Test Case.
  //! \param[in]  text           input monolith text block to parse.
  //! \param[out] title          extracted title of the Test Case.
  //! \param[out] overviewBlocks extracted overview sections.
  //! \param[out] detailsBlocks  extracted details sections.
  //!
  //! \return text from [OVERVIEW] and [DETAILS] sections.
  static void
    extractBlocks(const int          numFunctions,
                  const std::string& text,
                  std::string&       title,
                  SeqStr&            overviewBlocks,
                  SeqStr&            detailsBlocks);

  //! Extracts numerical indices from the passed tag token.
  //! \param[in]  tag         tag token.
  //! \param[out] startIdx    first index.
  //! \param[out] endIdx      last index.
  //! \param[out] isUnbounded indicates whether the range is unbounded.
  static void
    extractIndicesFromTag(const std::string& tag,
                          int&               startIdx,
                          int&               endIdx,
                          bool&              isUnbounded);

  //! Expands all variables referenced by the given text against the
  //! passed expansion map.
  //!
  //! \param[in] text      text to substitute variables into.
  //! \param[in] vars      expansion map.
  //! \param[in] varsScope string representation for scope of variables.
  //!
  //! \return copy of input text enriched with variable values.
  static std::string
    expandVariables(const std::string& text,
                    const StrStrMap&   vars,
                    const std::string& varsScope);

  //! Checks whether the passed line contains whitespaces only.
  //! \param[in] line line to check.
  //! \return true/false.
  static bool
    isLineOfNulls(const std::string& line);

  //! Returns true if the passed string token is recognized as a placeholder
  //! for a variable.
  //! \param[in]  token    string to check.
  //! \param[out] varStart 0-based index of character where the variable
  //!                      marker starts.
  //! \param[out] varEnd   0-based index of character where the variable
  //!                      marker ends.
  //! \return true/false.
  static bool
    isVar(const std::string& token,
          int&               varStart,
          int&               varEnd);

  //! Returns true if the passed string token is recognized as pre-formatting
  //! tag.
  //! \param[in] token  string to check.
  //! \param[in] isOpen indicates whether we want to recognize opening or
  //!                   closing pre-formatting tag.
  //! \return true/false.
  static bool
    isPre(const std::string& token,
          const bool         isOpen);

  //! Prepares a string to be used as a global scope for resolving variables.
  //! \param[in] caseID global ID of the Test Case.
  //! \param[in] funcID local ID of the Test Function.
  //! \return string representing variable's scope in format {caseID}::{funcID}.
  static std::string
    varScope(const int caseID,
             const int funcID);

private:

  asiTestEngine_DescriptionProc() = delete;
  asiTestEngine_DescriptionProc(const asiTestEngine_DescriptionProc&) = delete;
  void operator=(const asiTestEngine_DescriptionProc&) = delete;

};

#endif
