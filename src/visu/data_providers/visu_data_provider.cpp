//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_data_provider.h>

// Active Data includes
#include <ActData_BaseParameter.h>

//! Returns TRUE if DOMAIN -> VTK translation process must perform, FALSE --
//! otherwise.
//! \param Against [in] MTime of the corresponding pipeline.
//! \return true/false.
bool visu_data_provider::MustExecute(const Handle(ActAux_TimeStamp)& Against) const
{
  // Access source Parameters
  Handle(ActAPI_HParameterList) aSources = this->translationSources();

  if ( aSources.IsNull() )
    return true; // Let the data pass idle pipelining

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
      return true;
  }

  return false;
}
