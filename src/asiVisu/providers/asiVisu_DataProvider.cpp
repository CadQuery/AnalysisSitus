//-----------------------------------------------------------------------------
// Created on: 25 September 2015
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
#include <asiVisu_DataProvider.h>

// Active Data includes
#include <ActData_UserParameter.h>

//! Returns TRUE if DOMAIN -> VTK translation process must perform, FALSE --
//! otherwise.
//! \param[in] Against MTime of the corresponding pipeline.
//! \return true/false.
bool asiVisu_DataProvider::MustExecute(const Handle(ActAux_TimeStamp)& Against) const
{
  // Access source Parameters
  Handle(ActAPI_HParameterList) sources = this->translationSources();

  if ( sources.IsNull() )
    return true; // Let the data pass idle pipelining.

  // Iterate over the source Parameters checking their Modification Time.
  for ( ActAPI_ParameterList::Iterator pit(*sources); pit.More(); pit.Next() )
  {
    Handle(ActData_UserParameter)
      nextParam = Handle(ActData_UserParameter)::DownCast( pit.Value() );

    if ( nextParam.IsNull() )
      continue;

    if ( nextParam->GetMTime()->IsOrigin() || // Never modified.
         nextParam->GetMTime()->IsGreater(Against) ) // Or more fresh than pipeline.
      return true;
  }

  return false;
}
