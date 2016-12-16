//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiAlgo_DeleteEdges_h
#define asiAlgo_DeleteEdges_h

// A-Situs includes
#include <asiAlgo.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to delete edges from the model.
class asiAlgo_DeleteEdges
{
public:

  asiAlgo_EXPORT
    asiAlgo_DeleteEdges(const TopoDS_Shape& masterCAD);

public:

  asiAlgo_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& edges,
            const bool                        edgeOnly);

public:

  //! \return result shape.
  const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  TopoDS_Shape m_master; //!< Master model.
  TopoDS_Shape m_result; //!< Result.

};

#endif
