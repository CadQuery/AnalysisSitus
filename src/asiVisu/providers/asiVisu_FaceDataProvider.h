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

// asiData includes
#include <asiData_PartNode.h>

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

  asiVisu_EXPORT
    asiVisu_FaceDataProvider(const Handle(asiData_FaceNode)& face_n);

  asiVisu_EXPORT
    asiVisu_FaceDataProvider(const Handle(asiData_SurfNode)& surf_n);

public:

  asiVisu_EXPORT ActAPI_DataObjectId
    GetNodeID() const;

  asiVisu_EXPORT int
    GetFaceIndexAmongSubshapes() const;

  asiVisu_EXPORT int
    GetFaceIndexAmongFaces();

  asiVisu_EXPORT TopoDS_Face
    ExtractFace();

public:

  asiVisu_EXPORT Handle(asiVisu_FaceDataProvider)
    Clone() const;

protected:

  asiVisu_EXPORT void
    init(const Handle(ActAPI_INode)& subNode);

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  //! Source Node.
  Handle(ActAPI_INode) m_node;

  //! Working part Node.
  Handle(asiData_PartNode) m_partNode;

};

#endif
