//-----------------------------------------------------------------------------
// Created on: 04 November 2013
//-----------------------------------------------------------------------------
// Copyright (c) 2013-present, Sergey Slyadnev
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

#ifndef asiAlgo_FileDumper_HeaderFile
#define asiAlgo_FileDumper_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// STD includes
#include <fstream>

//-----------------------------------------------------------------------------

//! Utility class providing functionality for dumping of algorithmic data
//! to ASCII files in different manners.
class asiAlgo_FileDumper
{
public:

  asiAlgo_EXPORT
    asiAlgo_FileDumper();

  asiAlgo_EXPORT
    asiAlgo_FileDumper(const std::string& filename);

  asiAlgo_EXPORT
    ~asiAlgo_FileDumper();

public:

  asiAlgo_EXPORT bool
    Open(const std::string& filename);

  asiAlgo_EXPORT void
    Dump(const std::string& msg);

  asiAlgo_EXPORT void
    Dump(const int          val,
         const std::string& msg = "");

  asiAlgo_EXPORT void
    Dump(const double       val,
         const std::string& msg = "");

  asiAlgo_EXPORT void
    Dump(const bool         val,
         const std::string& msg = "");

  asiAlgo_EXPORT void
    Dump(const double*      arr,
         const int          numElems,
         const std::string& msg = "");

  asiAlgo_EXPORT void
    Dump(const double*      mx,
         const int          numRows,
         const int          numCols,
         const std::string& msg = "");

private:

  std::ofstream m_FILE; //!< File stream.

};

#endif
