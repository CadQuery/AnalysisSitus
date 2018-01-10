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

#ifndef asiAlgo_FileFormat_h
#define asiAlgo_FileFormat_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <NCollection_IndexedMap.hxx>
#include <TCollection_AsciiString.hxx>

//! Supported file types.
enum asiAlgo_FileFormat
{
  FileFormat_Unknown = 0, //!< Unknown file format.
  //---------------------------------------------------------------------------
  // Native
  //---------------------------------------------------------------------------
  FileFormat_NATIVE,      //!< Native project format.
  //---------------------------------------------------------------------------
  // CAD
  //---------------------------------------------------------------------------
  FileFormat_BREP,        //!< BREP.
  FileFormat_STEP,        //!< STEP.
  FileFormat_IGES,        //!< IGES.
  //---------------------------------------------------------------------------
  // Mesh
  //---------------------------------------------------------------------------
  FileFormat_STL,         //!< (Polygonal) STL.
  //---------------------------------------------------------------------------
  // Last item
  //---------------------------------------------------------------------------
  FileFormat_Last         //!< Last item for convenient iteration over enum.
};

//! Tools to work with file formats.
namespace asiAlgo_FileFormatTools
{
  //! The list of names for all defined formats.
  asiAlgo_EXPORT extern const char* const NAMED_FORMATS[FileFormat_Last];

  //! Get format name
  inline const char* NameFromFormat(const asiAlgo_FileFormat format)
  {
    return ( format >= 0 && format < FileFormat_Last ) ? NAMED_FORMATS[format]
                                                       : NAMED_FORMATS[FileFormat_Unknown];
  }

  //! Get format type.
  inline asiAlgo_FileFormat FormatFromName(const char* name)
  {
    for ( int it = 0; it < FileFormat_Last; ++it )
    {
      if ( std::strcmp(name, NAMED_FORMATS[it]) == 0 )
        return (asiAlgo_FileFormat) it;
    }
    return FileFormat_Unknown;
  }

  //! Returns file extension from the its name in lower case.
  asiAlgo_EXPORT TCollection_AsciiString
    GetFileExtension(const TCollection_AsciiString& path);

  //! Recognizes the file format by its extension.
  asiAlgo_EXPORT asiAlgo_FileFormat
    FormatFromFileExtension(const TCollection_AsciiString& path);

  //! Recognizes the file format by its contents.
  asiAlgo_EXPORT asiAlgo_FileFormat
    FormatFromFileContent(const TCollection_AsciiString& path);

  //! Returns true if export to the passed format is supported.
  asiAlgo_EXPORT bool
    IsExportSupported(const asiAlgo_FileFormat& format);

};

#endif
