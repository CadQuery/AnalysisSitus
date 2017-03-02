//-----------------------------------------------------------------------------
// Created on: 09 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_VectorsDataProvider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param N [in] source Node.
asiVisu_VectorsDataProvider::asiVisu_VectorsDataProvider(const Handle(ActAPI_INode)& N)
: asiVisu_DataProvider (),
  m_source             (N)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_VectorsDataProvider::GetNodeID() const
{
  return m_source->GetId();
}
