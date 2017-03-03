//-----------------------------------------------------------------------------
// Created on: 09 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_VectorsDataProvider_h
#define asiVisu_VectorsDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>

// asiAlgo includes
#include <asiAlgo_BaseCloud.h>

DEFINE_STANDARD_HANDLE(asiVisu_VectorsDataProvider, asiVisu_DataProvider)

//! Data provider for a vector field.
class asiVisu_VectorsDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_VectorsDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_VectorsDataProvider(const Handle(ActAPI_INode)& N);

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  virtual Handle(asiAlgo_BaseCloud<float>)
    GetPointsf() = 0;

  virtual Handle(asiAlgo_BaseCloud<float>)
    GetVectorsf() = 0;

  virtual double
    GetMaxVectorModulus() const = 0;

protected:

  Handle(ActAPI_INode) m_source; //!< Source Node.

};

#endif
