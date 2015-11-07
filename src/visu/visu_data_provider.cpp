//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su
//-----------------------------------------------------------------------------

// Own include
#include <visu_data_provider.h>

// SBM Data Framework includes
#include <ActData_BaseParameter.h>

//! Returns TRUE if DOMAIN -> VTK translation process must perform, FALSE --
//! otherwise.
//! \param Against [in] MTime of the corresponding pipeline.
//! \return true/false.
Standard_Boolean
  SbmVisu_BasePipelineDataProvider::MustExecute(const Handle(ActAux_TimeStamp)& Against) const
{
  // Access source Parameters
  Handle(ActAPI_ParameterList) aSources = this->translationSources();

  if ( aSources.IsNull() )
    return Standard_True; // Let the data pass idle pipelining

  // Iterate over the source Parameters checking their Modification Time.
  ActAPI_ParameterList::Iterator aParamIt( *aSources.operator->() );
  for ( ; aParamIt.More(); aParamIt.Next() )
  {
    Handle(ActData_BaseParameter)
      aNextParam = Handle(ActData_BaseParameter)::DownCast( aParamIt.Value() );

    if ( aNextParam.IsNull() )
      continue;

    if ( aNextParam->GetMTime()->IsOrigin() || // Never modified
         aNextParam->GetMTime()->IsGreater(Against) ) // Or more fresh than pipeline
      return Standard_True;
  }

  return Standard_False;
}
