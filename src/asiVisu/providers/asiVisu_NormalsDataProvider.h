//-----------------------------------------------------------------------------
// Created on: 09 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_NormalsDataProvider_h
#define asiVisu_NormalsDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiAlgo includes
#include <asiAlgo_PointCloud.h>

DEFINE_STANDARD_HANDLE(asiVisu_NormalsDataProvider, asiVisu_DataProvider)

//! Data provider for a normal field.
class asiVisu_NormalsDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_NormalsDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_NormalsDataProvider(const Handle(ActAPI_INode)& N);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  virtual Handle(asiAlgo_PointCloud<float>)
    GetNormalsf() const = 0;

protected:

  Handle(ActAPI_INode) m_node; //!< Source Node.

};

#endif
