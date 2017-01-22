//-----------------------------------------------------------------------------
// Created on: 09 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_NormalsDataProvider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param pointsNode  [in] source points Node.
//! \param normalsNode [in] source normals Node.
asiVisu_NormalsDataProvider::asiVisu_NormalsDataProvider(const Handle(ActAPI_INode)& pointsNode,
                                                         const Handle(ActAPI_INode)& normalsNode)
: asiVisu_DataProvider (),
  m_points             (pointsNode),
  m_normals            (normalsNode)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_NormalsDataProvider::GetNodeID() const
{
  return m_normals->GetId();
}
