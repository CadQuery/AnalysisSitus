//-----------------------------------------------------------------------------
// Created on: 25 September 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_DataProvider.h>

// Active Data includes
#include <ActData_UserParameter.h>

//! Returns TRUE if DOMAIN -> VTK translation process must perform, FALSE --
//! otherwise.
//! \param Against [in] MTime of the corresponding pipeline.
//! \return true/false.
bool asiVisu_DataProvider::MustExecute(const Handle(ActAux_TimeStamp)& Against) const
{
  // Access source Parameters
  Handle(ActAPI_HParameterList) aSources = this->translationSources();

  if ( aSources.IsNull() )
    return true; // Let the data pass idle pipelining

  // Iterate over the source Parameters checking their Modification Time.
  ActAPI_ParameterList::Iterator aParamIt( *aSources.operator->() );
  for ( ; aParamIt.More(); aParamIt.Next() )
  {
    Handle(ActData_UserParameter)
      aNextParam = Handle(ActData_UserParameter)::DownCast( aParamIt.Value() );

    if ( aNextParam.IsNull() )
      continue;

    if ( aNextParam->GetMTime()->IsOrigin() || // Never modified
         aNextParam->GetMTime()->IsGreater(Against) ) // Or more fresh than pipeline
      return true;
  }

  return false;
}
