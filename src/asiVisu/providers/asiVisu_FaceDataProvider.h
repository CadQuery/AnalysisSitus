//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_FaceDataProvider_h
#define asiVisu_FaceDataProvider_h

// A-Situs visualization includes
#include <asiVisu_DataProvider.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_FaceDataProvider, asiVisu_DataProvider)

//! Data provider for face geometry.
class asiVisu_FaceDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FaceDataProvider, asiVisu_DataProvider)

public:

  asiVisu_FaceDataProvider(const ActAPI_DataObjectId&           theNodeId,
                           const Handle(ActAPI_HParameterList)& theParamList);

public:

  ActAPI_DataObjectId GetNodeID                  () const;
  int                 GetFaceIndexAmongSubshapes () const;
  int                 GetFaceIndexAmongFaces     () const;
  TopoDS_Face         ExtractFace                () const;

public:

  Handle(asiVisu_FaceDataProvider)
    Clone() const;

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  //! Source Node ID.
  ActAPI_DataObjectId m_nodeID;

  //! Source Parameters.
  Handle(ActAPI_HParameterList) m_params;

  //! Map of sub-shapes.
  TopTools_IndexedMapOfShape m_subShapes;

  //! Map of faces.
  TopTools_IndexedMapOfShape m_faces;

};

#endif
