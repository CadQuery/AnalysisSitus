//-----------------------------------------------------------------------------
// Created on: 02 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_SuppressFaces_h
#define asiAlgo_SuppressFaces_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <BRepTools_ReShape.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Utility to delete faces.
class asiAlgo_SuppressFaces
{
public:

  asiAlgo_EXPORT
    asiAlgo_SuppressFaces(const TopoDS_Shape& masterCAD);

public:

  asiAlgo_EXPORT virtual bool
    Perform(const TColStd_PackedMapOfInteger& faceIndices,
            const bool                        facesOnly);

public:

  //! \return result shape.
  const TopoDS_Shape& GetResult() const
  {
    return m_result;
  }

  //! \return instance of Re-Shape utility used for topological reduction.
  const Handle(BRepTools_ReShape)& GetReShape() const
  {
    return m_reShape;
  }

protected:

  TopoDS_Shape              m_master;  //!< Master model.
  TopoDS_Shape              m_result;  //!< Result.
  Handle(BRepTools_ReShape) m_reShape; //!< Re-Shape tool.

};

#endif
