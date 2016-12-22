//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_LawDataProvider.h>

//! Creates data provider for the given design parameter's law.
//! \param N [in] law Node.
asiVisu_LawDataProvider::asiVisu_LawDataProvider(const Handle(asiData_DesignLawNode)& N)
: asiVisu_DataProvider(),
  m_node(N)
{}

//! Extracts X or Y evolution law from a persistent object.
//! \return X or Y evolution law.
Handle(asiAlgo_DesignLaw) asiVisu_LawDataProvider::GetLaw() const
{
  // Let's take all persistent properties and create a transient law
  return m_node->AsLaw();
}

//! \return persistent Node ID.
ActAPI_DataObjectId asiVisu_LawDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//! \return sensitive Parameters,
Handle(ActAPI_HParameterList) asiVisu_LawDataProvider::translationSources() const
{
  return ActParamStream() << m_node->Parameter(asiData_DesignLawNode::PID_Degree)
                          << m_node->Parameter(asiData_DesignLawNode::PID_Knots)
                          << m_node->Parameter(asiData_DesignLawNode::PID_Mults)
                          << m_node->Parameter(asiData_DesignLawNode::PID_Poles);
}

