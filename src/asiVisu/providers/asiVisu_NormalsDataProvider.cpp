//-----------------------------------------------------------------------------
// Created on: 09 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_NormalsDataProvider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param N [in] source Node.
asiVisu_NormalsDataProvider::asiVisu_NormalsDataProvider(const Handle(ActAPI_INode)& N)
: asiVisu_DataProvider(),
  m_node(N)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_NormalsDataProvider::GetNodeID() const
{
  return m_node->GetId();
}
