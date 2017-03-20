//-----------------------------------------------------------------------------
// Created on: 20 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShapeData_h
#define asiVisu_ShapeData_h

// asiVisu includes
#include <asiVisu.h>

// OCCT includes
#include <Standard_Transient.hxx>

// VTK includes
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

// Wrapper for vtkPolyData representing B-Rep shape.
class asiVisu_ShapeData : public Standard_Transient
{
public:

  IVtkVTK_EXPORT static const char* const ARRNAME_SUBSHAPE_IDS;
  IVtkVTK_EXPORT static const char* const ARRNAME_MESH_TYPES;

  //! Constructor
  Standard_EXPORT IVtkVTK_ShapeData();
  //! Destructor
  Standard_EXPORT ~IVtkVTK_ShapeData();

  DEFINE_STANDARD_RTTIEXT(IVtkVTK_ShapeData,IVtk_IShapeData)

  //! Insert a coordinate
  //! @param [in] theX X coordinate
  //! @param [in] theY Y coordinate
  //! @param [in] theZ Z coordinate
  //! @return id of added point
  Standard_EXPORT virtual IVtk_PointId InsertCoordinate (double theX, double theY, double theZ) Standard_OVERRIDE;

  //! Insert a vertex.
  //! @param [in] theShapeID id of the subshape to which the vertex belongs.
  //! @param [in] thePointId id of the point that defines the coordinates of the vertex
  //! @param [in] theMeshType mesh type of the subshape (MT_Undefined by default)
  Standard_EXPORT virtual void InsertVertex (const IVtk_IdType   theShapeID,
                                             const IVtk_PointId  thePointId,
                                             const IVtk_MeshType theMeshType) Standard_OVERRIDE;

  //! Insert a line.
  //! @param [in] theShapeID id of the subshape to which the line belongs.
  //! @param [in] thePointId1 id of the first point
  //! @param [in] thePointId2 id of the second point
  //! @param [in] theMeshType mesh type of the subshape (MT_Undefined by default)
  Standard_EXPORT virtual void InsertLine (const IVtk_IdType   theShapeID,
                                           const IVtk_PointId  thePointId1,
                                           const IVtk_PointId  thePointId2,
                                           const IVtk_MeshType theMeshType) Standard_OVERRIDE;

  //! Insert a poly-line.
  //! @param [in] theShapeID id of the subshape to which the polyline belongs.
  //! @param [in] thePointIds vector of point ids
  //! @param [in] theMeshType mesh type of the subshape (MT_Undefined by default)
  Standard_EXPORT virtual void InsertLine (const IVtk_IdType       theShapeID, 
                                           const IVtk_PointIdList* thePointIds,
                                           const IVtk_MeshType     theMeshType) Standard_OVERRIDE;
  //! Insert a triangle
  //! @param [in] theShapeID id of the subshape to which the triangle belongs.
  //! @param [in] thePointId1 id of the first point
  //! @param [in] thePointId2 id of the second point
  //! @param [in] thePointId3 id of the third point
  //! @param [in] theMeshType mesh type of the subshape (MT_Undefined by default)
  Standard_EXPORT virtual void InsertTriangle (const IVtk_IdType   theShapeID,
                                               const IVtk_PointId  thePointId1,
                                               const IVtk_PointId  thePointId2,
                                               const IVtk_PointId  thePointId3,
                                               const IVtk_MeshType theMeshType) Standard_OVERRIDE;


public: //! @name Specific methods

  //! Get VTK PolyData.
  //! @return VTK PolyData
  vtkPolyData* getVtkPolyData() const
  { return myPolyData; }

private:
  vtkSmartPointer< vtkPolyData >    myPolyData;    //!< Shape geometry as vtkPolyData
  vtkSmartPointer< vtkIdTypeArray > mySubShapeIDs; //!< Array of sub-shapes ids
  vtkSmartPointer< vtkIdTypeArray > myMeshTypes;   //!< Array of type codes of mesh parts
};

#endif // __IVTKVTK_SHAPEDATA_H__
