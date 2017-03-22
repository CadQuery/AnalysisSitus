//-----------------------------------------------------------------------------
// Created on: 20 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShapeData_h
#define asiVisu_ShapeData_h

// asiVisu includes
#include <asiVisu_ShapeCellType.h>

// OCCT includes
#include <Standard_Type.hxx>

// VTK includes
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

//! Wrapper for vtkPolyData representing B-Rep shape.
class asiVisu_ShapeData : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ShapeData, Standard_Transient)

public:

  asiVisu_EXPORT
    asiVisu_ShapeData();

public:

  asiVisu_EXPORT virtual vtkIdType
    InsertCoordinate(const double x,
                     const double y,
                     const double z);

  asiVisu_EXPORT virtual void
    InsertVertex(vtkIdType             shapeID,
                 vtkIdType             pid,
                 asiVisu_ShapeCellType scType);

  asiVisu_EXPORT virtual void
    InsertLine(vtkIdType             shapeID,
               vtkIdType             pid1,
               vtkIdType             pid2,
               asiVisu_ShapeCellType scType);

  asiVisu_EXPORT virtual void
    InsertPolyline(vtkIdType                         shapeID,
                   const vtkSmartPointer<vtkIdList>& pids,
                   asiVisu_ShapeCellType             scType);

  asiVisu_EXPORT virtual void
    InsertTriangle(vtkIdType             shapeID,
                   vtkIdType             pid1,
                   vtkIdType             pid2,
                   vtkIdType             pid3,
                   asiVisu_ShapeCellType scType);


public:

  //! \return instance of vtkPolyData representing the shape.
  const vtkSmartPointer<vtkPolyData>& GetPolyData() const
  {
    return m_polyData;
  }

private:

  vtkSmartPointer<vtkPolyData>    m_polyData;       //!< Polygonal data.
  vtkSmartPointer<vtkIdTypeArray> m_subShapeIDs;    //!< Array of sub-shapes IDs.
  vtkSmartPointer<vtkIdTypeArray> m_shapeCellTypes; //!< Array of cell types.

};

#endif
