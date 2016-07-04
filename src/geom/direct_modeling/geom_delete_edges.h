//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_delete_edges_h
#define geom_delete_edges_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to delete edges from the model.
class geom_delete_edges
{
public:

  ASitus_EXPORT
    geom_delete_edges(const TopoDS_Shape& masterCAD);

public:

  ASitus_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& edges,
            const bool                        edgeOnly);

public:

  //! \return result shape.
  inline const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  TopoDS_Shape m_master; //!< Master model.
  TopoDS_Shape m_result; //!< Result.

};

#endif
