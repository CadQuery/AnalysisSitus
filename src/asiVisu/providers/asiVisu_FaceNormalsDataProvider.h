//-----------------------------------------------------------------------------
// Created on: 02 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_FaceNormalsDataProvider_h
#define asiVisu_FaceNormalsDataProvider_h

// asiVisu includes
#include <asiVisu_VectorsDataProvider.h>

// asiData includes
#include <asiData_PartNode.h>

DEFINE_STANDARD_HANDLE(asiVisu_FaceNormalsDataProvider, asiVisu_VectorsDataProvider)

//! Data provider for b-rep face normals.
class asiVisu_FaceNormalsDataProvider : public asiVisu_VectorsDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FaceNormalsDataProvider, asiVisu_VectorsDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_FaceNormalsDataProvider(const Handle(asiData_FaceNormsNode)& faceNode);

public:

  asiVisu_EXPORT virtual Handle(asiAlgo_PointCloud<float>)
    GetPointsf();

  asiVisu_EXPORT virtual Handle(asiAlgo_PointCloud<float>)
    GetVectorsf();

  asiVisu_EXPORT virtual double
    GetMaxVectorModulus() const;

public:

  asiVisu_EXPORT TopoDS_Face
    GetFace() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(asiData_PartNode)          m_partNode; //!< Owning part Node.
  Handle(asiAlgo_PointCloud<float>) m_points;   //!< Cached points.
  Handle(asiAlgo_PointCloud<float>) m_vectors;  //!< Cached vectors.

};

#endif
