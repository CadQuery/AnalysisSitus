//-----------------------------------------------------------------------------
// Created on: 14 March 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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

#ifndef asiAlgo_CSG_h
#define asiAlgo_CSG_h

// asiAlgo includes
#include <asiAlgo.h>

//-----------------------------------------------------------------------------

//! Type of CSG operation.
enum asiAlgo_CSG
{
  CSG_Undefined = 0,
  CSG_Primitive,   //!< Construction of a primitive.
  CSG_Union,       //!< Boolean union.
  CSG_Difference,  //!< Boolean difference.
  CSG_Intersection //!< Boolean intersection.
};

//-----------------------------------------------------------------------------

//! Auxiliary functions.
namespace asiAlgo_CSGUtils
{
  //! Returns operation name.
  //! \param[in] operation type of the CSG operation in question.
  //! \return const char pointer to the operation name.
  inline const char* GetOperationName(const asiAlgo_CSG operation)
  {
    switch ( operation )
    {
      case CSG_Primitive:    return "Primitive";
      case CSG_Union:        return "Union";
      case CSG_Difference:   return "Difference";
      case CSG_Intersection: return "Intersection";
      default: break;
    }

    return "Undefined";
  }
}


#endif
