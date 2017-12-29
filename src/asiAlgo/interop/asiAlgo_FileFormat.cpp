//-----------------------------------------------------------------------------
// Created on: 29 December 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
#include <asiAlgo_FileFormat.h>

// OCCT includes
#include <OSD_OpenFile.hxx>

//! The list of names for all defined formats.
const char* const asiAlgo_FileFormatTools::NAMED_FORMATS[] =
{
  "Unknown", // FileFormat_Unknown
  "SITU",    // FileFormat_NATIVE
  "BREP",    // FileFormat_BREP
  "STEP",    // FileFormat_STEP
  "IGES",    // FileFormat_IGES
  "STL"      // FileFormat_STL
};

//-----------------------------------------------------------------------------

TCollection_AsciiString
  asiAlgo_FileFormatTools::GetFileExtension(const TCollection_AsciiString& path)
{
  const int THE_EXT_MAX_LEN = 20;
  const int aLen = path.Length();
  for ( int anExtLen = 1; anExtLen < aLen && anExtLen < THE_EXT_MAX_LEN; ++anExtLen )
  {
    if ( path.Value (aLen - anExtLen) == '.' )
    {
      TCollection_AsciiString anExtension = path.SubString (aLen - anExtLen + 1, aLen);
      anExtension.LowerCase();
      return anExtension;
    }
  }
  return TCollection_AsciiString();
}

//-----------------------------------------------------------------------------

asiAlgo_FileFormat
  asiAlgo_FileFormatTools::FormatFromFileExtension(const TCollection_AsciiString& path)
{
  TCollection_AsciiString ext = GetFileExtension(path);

  if ( ext == "brep" || ext == "rle" )
  {
    return FileFormat_BREP;
  }
  else if ( ext == "stl" )
  {
    return FileFormat_STL;
  }
  else if ( ext == "step" || ext == "stp" )
  {
    return FileFormat_STEP;
  }
  else if ( ext == "situ" )
  {
    return FileFormat_NATIVE;
  }
  else if ( ext == "igs" || ext == "iges" )
  {
    return FileFormat_IGES;
  }

  return FileFormat_Unknown;
}

//-----------------------------------------------------------------------------

asiAlgo_FileFormat
  asiAlgo_FileFormatTools::FormatFromFileContent(const TCollection_AsciiString& path)
{
  TCollection_AsciiString ext = GetFileExtension(path);

  std::ifstream aFile;
  OSD_OpenStream( aFile, path.ToCString(), std::ios::in | std::ios::binary );
  char aBytes[2048] = {};
  if ( !aFile.is_open() || !aFile.good() )
  {
    return FileFormat_Unknown;
  }

  // Read first 2048 bytes
  aFile.read( aBytes, 2048 );
  aFile.close();

  // Suspect STEP if string "ISO-10303-21" is present
  if ( ::strstr( aBytes, "ISO-10303-21" ) )
  {
    // Double-check by presence of "FILE_SHEMA" statement
    const char* aPtr = ::strstr( aBytes, "FILE_SCHEMA" );
    if ( aPtr )
    {
      return FileFormat_STEP;
    }
  }

  // IGES is recognized by 'S' in position 72 of the first line,
  // followed by number 1 (first line of the IGES start section).
  // Naturally this holds if first line is at least 80 symbols length.
  if ( aBytes[72] == 'S' )
  {
    const char *aPtr = aBytes + 73;
    while ( aPtr < aBytes + 80 && (*aPtr == ' ' || *aPtr == '0') )
    {
      aPtr++;
    }
    if ( *aPtr == '1' && !::isalnum( (unsigned char )*++aPtr ) )
    {
      return FileFormat_IGES;
    }
  }

  // text STL starts with "solid"
  else if ( !(::strncmp( aBytes, "solid", 5 ) || ::strncmp( aBytes, "SOLID", 5 )) && isspace( aBytes[5] ) )
  {
    return FileFormat_STL;
  }

  // binary STL has no header for identification - format can be detected only by file extension
  else if ( ext == "stl" )
  {
    return FileFormat_STL;
  }

  // detect BREP by "CASCADE Topology V1" or "DBRep_DrawableShape"
  else if ( ::strstr( aBytes, "DBRep_DrawableShape" ) || ::strstr( aBytes, "CASCADE Topology V1" ) )
  {
    return FileFormat_BREP;
  }

  return FileFormat_Unknown;
}

//-----------------------------------------------------------------------------

bool asiAlgo_FileFormatTools::IsExportSupported(const asiAlgo_FileFormat& format)
{
  if ( format == FileFormat_NATIVE
    || format == FileFormat_BREP
    || format == FileFormat_STEP )
  {
    return true;
  }

  return false;
}
