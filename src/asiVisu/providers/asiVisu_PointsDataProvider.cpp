//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PointsDataProvider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param N [in] source Node.
asiVisu_PointsDataProvider::asiVisu_PointsDataProvider(const Handle(ActAPI_INode)& N)
: asiVisu_DataProvider(),
  m_node(N)
{
}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_PointsDataProvider::GetNodeID() const
{
  return m_node->GetId();
}
