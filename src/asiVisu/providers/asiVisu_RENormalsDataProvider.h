//-----------------------------------------------------------------------------
// Created on: 08 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_RENormalsDataProvider_h
#define asiVisu_RENormalsDataProvider_h

// asiVisu includes
#include <asiVisu_VectorsDataProvider.h>

// asiData includes
#include <asiData_RENormalsNode.h>
#include <asiData_REPointsNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_RENormalsDataProvider, asiVisu_VectorsDataProvider)

//! Data provider for re-engineering normals.
class asiVisu_RENormalsDataProvider : public asiVisu_VectorsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_RENormalsDataProvider, asiVisu_VectorsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_RENormalsDataProvider(const Handle(asiData_REPointsNode)&  pointsNode,
                                  const Handle(asiData_RENormalsNode)& normalsNode);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_BaseCloud<float>)
    GetPointsf();

  asiVisu_EXPORT virtual Handle(asiAlgo_BaseCloud<float>)
    GetVectorsf();

  asiVisu_EXPORT virtual double
    GetMaxVectorModulus() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(ActAPI_INode) m_vectors; //!< Vectors Node.

};

#endif
