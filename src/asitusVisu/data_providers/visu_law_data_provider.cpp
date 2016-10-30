//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_law_data_provider.h>

//! Creates data provider for the given design parameter's law.
//! \param N [in] law Node.
visu_law_data_provider::visu_law_data_provider(const Handle(calculus_design_law_node)& N)
: visu_data_provider(),
  m_node(N)
{}

//! Extracts X or Y evolution law from a persistent object.
//! \return X or Y evolution law.
Handle(calculus_design_law) visu_law_data_provider::GetLaw() const
{
  // Let's take all persistent properties and create a transient law
  return m_node->AsLaw();
}

//! \return persistent Node ID.
ActAPI_DataObjectId visu_law_data_provider::GetNodeID() const
{
  return m_node->GetId();
}

//! \return sensitive Parameters,
Handle(ActAPI_HParameterList) visu_law_data_provider::translationSources() const
{
  return ActParamStream() << m_node->Parameter(calculus_design_law_node::PID_Degree)
                          << m_node->Parameter(calculus_design_law_node::PID_Knots)
                          << m_node->Parameter(calculus_design_law_node::PID_Mults)
                          << m_node->Parameter(calculus_design_law_node::PID_Poles);
}

