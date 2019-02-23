//-----------------------------------------------------------------------------
// Created on: November 2013
//-----------------------------------------------------------------------------
// Copyright (c) 2013-pesent, Sergey Slyadnev
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
#include <asiTestEngine_DescriptionProc.h>

// STD includes
#include <fstream>

//-----------------------------------------------------------------------------

bool asiTestEngine_DescriptionProc::Process(const std::string& dir,
                                            const std::string& filename,
                                            const StrStrMap&   vars,
                                            const int          caseID,
                                            const int          numFunctions,
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
  extractBlocks(numFunctions,
                text,
                title_tmp,
                overviewBlocks_tmp,
                detailsBlocks_tmp);

  /* ==================
   *  Expand variables
   * ================== */

  // Expand title
  title = expandVariables(title_tmp, vars, ""); // Null string means "no scope"

  // Expand overview blocks
  for ( int block = 0; block < (int) overviewBlocks_tmp.size(); ++block )
    overviewBlocks.push_back( expandVariables( overviewBlocks_tmp[block],
                                               vars,
                                               varScope(caseID, block + 1) ) );

  // Expand details blocks
  for ( int block = 0; block < (int) detailsBlocks_tmp.size(); ++block )
    detailsBlocks.push_back( expandVariables( detailsBlocks_tmp[block],
                                              vars,
                                              varScope(caseID, block + 1) ) );

  return true;
}

//-----------------------------------------------------------------------------

void asiTestEngine_DescriptionProc::extractBlocks(const int          numFunctions,
                                                  const std::string& text,
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
  bool isUnbounded = false;
  //
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
      extractIndicesFromTag(lineToken, idx_F, idx_L, isUnbounded);
      continue;
    }
    else if ( lineToken.find(asiTestEngine_Macro_SEC_DETAILS) != std::string::npos )
    {
      readMode = Read_Details;
      extractIndicesFromTag(lineToken, idx_F, idx_L, isUnbounded);
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
      int tillIdx = (isUnbounded ? numFunctions : idx_L);

      // Add empty blocks
      while ( (int) overviewBlocks.size() < tillIdx )
        overviewBlocks.push_back( std::string() );

      // Fill blocks
      for ( int idx = idx_F; idx <= tillIdx; ++idx )
        overviewBlocks[idx-1] += (lineToken + asiTestEngine_Macro_NL);
    }

    if ( readMode == Read_Details )
    {
      int tillIdx = (isUnbounded ? numFunctions : idx_L);

      // Enrich collection with empty items
      while ( (int) detailsBlocks.size() < tillIdx )
        detailsBlocks.push_back( std::string() );

      // Fill items
      for ( int idx = idx_F; idx <= tillIdx; ++idx )
       detailsBlocks[idx-1] += (lineToken + asiTestEngine_Macro_NL);
    }
  }
}

//-----------------------------------------------------------------------------

void asiTestEngine_DescriptionProc::extractIndicesFromTag(const std::string& tag,
                                                          int&               startIdx,
                                                          int&               endIdx,
                                                          bool&              isUnbounded)
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

  if ( numStrLast == asiTestEngine_Macro_ASTERISK_STR )
    isUnbounded = true;
  else
    isUnbounded = false;

  // Store range in output
  startIdx = atoi( numStrFirst.c_str() );
  endIdx   = (isUnbounded ? 1 : atoi( numStrLast.c_str() ) );
}

//-----------------------------------------------------------------------------

std::string
  asiTestEngine_DescriptionProc::expandVariables(const std::string& text,
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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

bool asiTestEngine_DescriptionProc::isVar(const std::string& token,
                                          int&               varStart,
                                          int&               varEnd)
{
  if ( token.length() <= 4 )
    return false;

  varStart = (int) token.find(asiTestEngine_Macro_VAR_MARKER);
  varEnd   = (int) token.rfind(asiTestEngine_Macro_VAR_MARKER);

  if ( varStart == (int) std::string::npos || varEnd == (int) std::string::npos || varStart >= varEnd )
    return false;

  return true;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

std::string asiTestEngine_DescriptionProc::varScope(const int caseID,
                                                    const int funcID)
{
  std::string vScope = asiAlgo_Utils::Str::ToString(caseID) +
                       asiTestEngine_Macro_NAMESPACE +
                       asiAlgo_Utils::Str::ToString(funcID);
  return vScope;
}
