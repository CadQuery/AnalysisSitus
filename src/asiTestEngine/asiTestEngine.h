//-----------------------------------------------------------------------------
// Created on: 23 November 2013
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

#ifndef asiTestEngine_HeaderFile
#define asiTestEngine_HeaderFile

#if defined _WIN32
  #if defined asiTestEngine_EXPORTS
    #define asiTestEngine_EXPORT __declspec(dllexport)
  #else
    #define asiTestEngine_EXPORT __declspec(dllimport)
  #endif
#else
  #define asiTestEngine_EXPORT
#endif

#define asiTestEngine_NotUsed(x)

#include <outcome.h>

//! Common functions for unit testing.
class asiTestEngine
{
public:

  //! Compares two passed floating-point values.
  //! \param[in] val1         first value.
  //! \param[in] val2         second value.
  //! \param[in] diff_percent allowed difference in percentage regarding to
  //!                         average value.
  //! \param[in] resolution   resolution for very small numbers (which are
  //!                         impractical to compare: e.g. 0.0001 is 100%
  //!                         different from 0.0002, but both values may be
  //!                         too small to consider this difference as
  //!                         significant in certain use cases).
  //! \return true in case of such fuzzy equality, false -- otherwise.
  static bool FuzzyCompare(const double val1,
                           const double val2,
                           const double diff_percent,
                           const double resolution)
  {
    if ( Abs(val1 - val2) < resolution )
      return true;

    const double avg     = (val1 + val2)/2.0;
    const double actDev = Abs( (val1 - val2)/avg )*100.0;

    return actDev < diff_percent;
  }

};

#endif
