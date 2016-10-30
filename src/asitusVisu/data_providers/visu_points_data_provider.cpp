//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_points_data_provider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param N [in] source Node.
visu_points_data_provider::visu_points_data_provider(const Handle(ActAPI_INode)& N)
: visu_data_provider(),
  m_node(N)
{
}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId visu_points_data_provider::GetNodeID() const
{
  return m_node->GetId();
}
