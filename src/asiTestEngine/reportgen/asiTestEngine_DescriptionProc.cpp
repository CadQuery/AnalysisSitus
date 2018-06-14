//-----------------------------------------------------------------------------
// Created on: November 2013
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

#include <asiTestEngine_DescriptionProc.h>

// STD includes
#include <fstream>

//! Performs processing of description template substituting variable
//! placeholders with actual values.
//! \param dir [in] directory to search the description file in.
//! \param filename [in] filename pointing to description template.
//! \param vars [in] variables expansion map.
//! \param caseID [in] ID of the Test Case.
//! \param title [out] title of the Test Case.
//! \param overviewBlocks [out] overview sections.
//! \param detailsBlocks [out] details sections.
//! \return true in case of success, false -- otherwise.
bool asiTestEngine_DescriptionProc::Process(const std::string& dir,
                                            const std::string& filename,
                                            const StrStrMap&   vars,
                                            const int          caseID,
                                            std::string&       title,
                                            SeqStr&            overviewBlocks,
                                            SeqStr&            detailsBlocks)
{
  /* ==================
   *  Prepare filename
   * ================== */

  std::string fullFilename, dirCopy(dir);
  char dir_last = dir.at(dir.size() - 1);
  if ( dir_last != asiTestEngine_Macro_SLASH && dir_last != asiTestEngine_Macro_RSLASH )
  {
    dirCopy += asiTestEngine_Macro_RSLASH;
  }
  fullFilename = dirCopy + filename;

  /* =======================================
   *  Read file contents to a single string
   * ======================================= */

  std::ifstream FILE( fullFilename.c_str() );
  if ( !FILE.is_open() )
    return false;

  std::string text;
  while ( !FILE.eof() )
  {
    char str[256];
    FILE.getline(str, 256);
    text += str;
    text += asiTestEngine_Macro_NL_STR;
  }

  FILE.close();

  /* ==================
   *  Extract sections
   * ================== */

  std::string title_tmp;
  SeqStr overviewBlocks_tmp, detailsBlocks_tmp;

  // Extract sections from monolith text block
  extractBlocks(text, title_tmp, overviewBlocks_tmp, detailsBlocks_tmp);

  /* ==================
   *  Expand variables
   * ================== */

  // Expand title
  title = expandVariables(title_tmp, vars, ""); // Null string means "no scope"

  // Expand overview blocks
  for ( int block = 0; block < (int) overviewBlocks_tmp.size(); ++block )
    overviewBlocks.push_back( expandVariables( overviewBlocks_tmp[block], vars,
                                               varScope(caseID, block + 1) ) );

  // Expand details blocks
  for ( int block = 0; block < (int) detailsBlocks_tmp.size(); ++block )
    detailsBlocks.push_back( expandVariables( detailsBlocks_tmp[block], vars,
                                              varScope(caseID, block + 1) ) );

  return true;
}

//! Extracts semantic sections from the given text.
//! \param text [in] input monolith text block to parse.
//! \param title [out] extracted title of the Test Case.
//! \param overviewBlocks [out] extracted overview sections.
//! \param detailsBlocks [out] extracted details sections.
//! \return text from [OVERVIEW] section.
void asiTestEngine_DescriptionProc::extractBlocks(const std::string& text,
                                                  std::string&       title,
                                                  SeqStr&            overviewBlocks,
                                                  SeqStr&            detailsBlocks)
{
  enum ReadMode
  {
    Read_Undefined, // We do not know what is being read currently
    Read_Title,     // Well, the currently read line is related to [TITLE]
    Read_Overview,  // Now we read [OVERVIEW] section
    Read_Details    // And [DETAILS] section goes now
  }
  readMode = Read_Undefined;

  // Split text by newline characters
  std::vector<std::string> textLines;
  asiAlgo_Utils::Str::Split(text, std::string(1, asiTestEngine_Macro_NL), textLines);

  // Iterate over the lines
  int idx_F = 0, idx_L = 0;
  for ( int l = 0; l < (int) textLines.size(); ++l )
  {
    const std::string& lineToken = textLines[l];

    // Skip empty lines
    if ( isLineOfNulls(lineToken) )
      continue;

    // Choose reading mode
    if ( lineToken.find(asiTestEngine_Macro_SEC_TITLE) != std::string::npos )
    {
      readMode = Read_Title;
      continue;
    }
    else if ( lineToken.find(asiTestEngine_Macro_SEC_OVERVIEW) != std::string::npos )
    {
      readMode = Read_Overview;
      extractIndicesFromTag(lineToken, idx_F, idx_L);
      continue;
    }
    else if ( lineToken.find(asiTestEngine_Macro_SEC_DETAILS) != std::string::npos )
    {
      readMode = Read_Details;
      extractIndicesFromTag(lineToken, idx_F, idx_L);
      continue;
    }

    if ( readMode == Read_Undefined )
      continue; // We are completely lost in what we are reading...

    // Accumulate title
    if ( readMode == Read_Title )
      title += (lineToken + asiTestEngine_Macro_NL);

    // Accumulate overview
    if ( readMode == Read_Overview )
    {
      // Enrich collection with empty items
      while ( (int) overviewBlocks.size() < idx_L )
        overviewBlocks.push_back( std::string() );

      // Fill items
      for ( int idx = idx_F; idx <= idx_L; ++idx )
        overviewBlocks[idx - 1] += (lineToken + asiTestEngine_Macro_NL);
    }

    if ( readMode == Read_Details )
    {
      // Enrich collection with empty items
      while ( (int) detailsBlocks.size() < idx_L )
        detailsBlocks.push_back( std::string() );

      // Fill items
      for ( int idx = idx_F; idx <= idx_L; ++idx )
       detailsBlocks[idx - 1] += (lineToken + asiTestEngine_Macro_NL);
    }
  }
}

//! Extracts numerical indices from the passed tag token.
//! \param tag [in] tag token.
//! \param startIdx [out] first index.
//! \param endIdx [out] last index.
void asiTestEngine_DescriptionProc::extractIndicesFromTag(const std::string& tag,
                                                          int&               startIdx,
                                                          int&               endIdx)
{
  // Remove brackets
  std::string tagBase = asiAlgo_Utils::Str::SubStr(tag, 1, (int) tag.length() - 2);

  // Extract tokens delimited with ":"
  std::vector<std::string> tagTokens;
  asiAlgo_Utils::Str::Split(tagBase, std::string(1, asiTestEngine_Macro_COLON), tagTokens);
  std::string leftToken = tagTokens[0]; // Left part in "XX:TAG" is interesting

  // Just simple integer value
  if ( leftToken.find(asiTestEngine_Macro_MINUS) == std::string::npos )
  {
    startIdx = endIdx = atoi( leftToken.c_str() );
    return;
  }

  // Looks like a range of indices, so we split it by minus character
  std::vector<std::string> idxTokens;
  asiAlgo_Utils::Str::Split(leftToken, std::string(1, asiTestEngine_Macro_MINUS), idxTokens);
  std::string numStrFirst = idxTokens[0];
  std::string numStrLast = idxTokens[1];

  // Store range in output
  startIdx = atoi( numStrFirst.c_str() );
  endIdx = atoi( numStrLast.c_str() );
}

//! Expands all variables referenced by the given piece of text against the
//! passed expansion map.
//! \param text [in] text to substitute variables into.
//! \param vars [in] expansion map.
//! \param varsScope [in] string representation for scope of variables.
//! \return copy of input text block enriched with variable values.
std::string asiTestEngine_DescriptionProc::expandVariables(const std::string& text,
                                                           const StrStrMap&   vars,
                                                           const std::string& varsScope)
{
  std::string textExpanded;

  // Split text by newline characters
  std::vector<std::string> textLines;
  asiAlgo_Utils::Str::Split(text, std::string(1, asiTestEngine_Macro_NL), textLines);

  // Iterate over lines
  bool isPreOngoing = false;
  for ( int l = 0; l < (int) textLines.size(); ++l )
  {
    const std::string& lineToken = textLines[l];

    if ( !isPreOngoing && isPre(lineToken, true) )
      isPreOngoing = true;

    if ( isPreOngoing && isPre(lineToken, false) )
      isPreOngoing = false;

    std::string lineAdjusted;
    if ( !isPreOngoing )
    {
      // Split line by words
      std::vector<std::string> lineWords;
      asiAlgo_Utils::Str::Split(lineToken, std::string(1, asiTestEngine_Macro_WHITESPACE), lineWords);

      // Iterate over thewords
      for ( int w = 0; w < (int) lineWords.size(); ++w )
      {
        std::string wordToken = lineWords[w];

        // Check if word token is a variable placeholder
        int varStart, varEnd;
        if ( isVar(wordToken, varStart, varEnd) )
        {
          std::string left, right;

          // Extract leading and trailing characters
          if ( varStart > 0 )
            left = asiAlgo_Utils::Str::SubStr(wordToken, 0, varStart);
          if ( varEnd + 2 < (int) wordToken.size() )
            right = asiAlgo_Utils::Str::SubStr(wordToken, varEnd + 2, (int) wordToken.size() - varEnd);

          // Extract variable name
          wordToken = asiAlgo_Utils::Str::SubStr(wordToken, varStart + 2, (int) varEnd - varStart - 2);

          // Add namespace (scope) for variable name
          if ( varsScope.length() )
            wordToken = varsScope + asiTestEngine_Macro_NAMESPACE + wordToken;

          // Replace variable with its value
          if ( vars.find(wordToken) != vars.end() )
          {
            wordToken = vars.find(wordToken)->second;
            wordToken = left + wordToken + right;
          }
        }
        lineAdjusted += (wordToken + asiTestEngine_Macro_WHITESPACE);
      }
    }
    else // Contents of <pre> tag are used as-is
      lineAdjusted = lineToken;

    textExpanded += (lineAdjusted + asiTestEngine_Macro_NL);
  }

  return textExpanded;
}

//! Checks whether the passed line contains whitespaces only.
//! \param line [in] line to check.
//! \return true/false.
bool asiTestEngine_DescriptionProc::isLineOfNulls(const std::string& line)
{
  for ( int s = 0; s < (int) line.length(); ++s )
  {
    char c = line.at(s);
    if ( c != asiTestEngine_Macro_WHITESPACE && c != asiTestEngine_Macro_NL )
      return false;
  }
  return true;
}

//! Returns true if the passed string token is recognized as a placeholder
//! for variable.
//! \param token [in] string to check.
//! \param varStart [out] 0-based index of character where the variable
//!        marker starts.
//! \param varEnd [out] 0-based index of character where the variable
//!        marker ends.
//! \return true/false.
bool asiTestEngine_DescriptionProc::isVar(const std::string& token,
                                          int&               varStart,
                                          int&               varEnd)
{
  if ( token.length() <= 4 )
    return false;

  varStart = (int) token.find(asiTestEngine_Macro_VAR_MARKER);
  varEnd = (int) token.rfind(asiTestEngine_Macro_VAR_MARKER);

  if ( varStart == (int) std::string::npos || varEnd == (int) std::string::npos || varStart >= varEnd )
    return false;

  return true;
}

//! Returns true if the passed string token is recognized as pre-formatting
//! tag.
//! \param token [in] string to check.
//! \param isOpen [in] indicates whether we want to recognize opening or
//!        closing pre-formatting tag.
//! \return true/false.
bool asiTestEngine_DescriptionProc::isPre(const std::string& token,
                                          const bool         isOpen)
{
  if ( token.length() <= 6 )
    return false;

  if ( isOpen && token.find(asiTestEngine_Macro_SUBSEC_PRE_O) != std::string::npos )
    return true;

  if ( !isOpen && token.find(asiTestEngine_Macro_SUBSEC_PRE_C) != std::string::npos )
    return true;

  return false;
}

//! Prepares a string to be used as a global scope for resolving variables.
//! \param caseID [in] global ID of the Test Case.
//! \param funcID [in] local ID of the Test Function.
//! \return string representing variable's scope in format {caseID}::{funcID}.
std::string asiTestEngine_DescriptionProc::varScope(const int caseID,
                                                    const int funcID)
{
  std::string vScope = asiAlgo_Utils::Str::ToString(caseID) +
                       asiTestEngine_Macro_NAMESPACE +
                       asiAlgo_Utils::Str::ToString(funcID);
  return vScope;
}
