//-----------------------------------------------------------------------------
// Created on: 13 October 2018
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

#ifndef exe_Keywords_h
#define exe_Keywords_h

//-----------------------------------------------------------------------------

#define ASITUS_KW_runscript "runscript"

//-----------------------------------------------------------------------------

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Standard includes
#include <string>

//-----------------------------------------------------------------------------

class asiExe
{
public:

  static bool IsKeyword(const std::string& opt,
                        const std::string& key)
  {
    std::string slashedKey = "/"; slashedKey += key;
    size_t      found      = opt.find(slashedKey);
    //
    if ( found == std::string::npos )
      return false;

    return true;
  }

  static bool HasKeyword(const int          argc,
                         const char**       argv,
                         const std::string& key)
  {
    for ( int k = 1; k < argc; ++k )
    {
      if ( IsKeyword(argv[k], key) )
        return true;
    }
    return false;
  }

  static bool GetKeyValue(const int          argc,
                          char**             argv,
                          const std::string& key,
                          std::string&       value)
  {
    for ( int k = 1; k < argc; ++k )
    {
      if ( IsKeyword(argv[k], key) )
      {
        std::vector<std::string> chunks;
        asiAlgo_Utils::Str::Split(argv[k], "=", chunks);

        if ( chunks.size() != 2 )
          return false;

        value = chunks[1];
        return true;
      }
    }
    return false;
  }

};

#endif
