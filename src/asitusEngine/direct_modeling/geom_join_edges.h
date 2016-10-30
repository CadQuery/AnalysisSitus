//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_join_edges_h
#define geom_join_edges_h

// A-Situs includes
#include <asitusGeom.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Utility to join edge chains.
class geom_join_edges
{
public:

  ASitus_EXPORT
    geom_join_edges(const TopoDS_Shape& masterCAD);

public:

  ASitus_EXPORT bool
    Perform(const TopTools_IndexedMapOfShape& edges,
            const TopoDS_Face&                face);

public:

  //! \return result shape.
  inline const TopoDS_Shape& Result() const
  {
    return m_result;
  }

protected:

  void chooseOrder(const TopTools_IndexedMapOfShape& edges,
                   TopoDS_Edge&                      eFirst,
                   TopoDS_Edge&                      eSecond) const;

  bool joinEdges(const TopoDS_Edge& eFirst,
                 const TopoDS_Edge& eSecond,
                 const TopoDS_Face& face,
                 TopoDS_Edge&       eResult) const;

protected:

  TopoDS_Shape m_master; //!< Master model.
  TopoDS_Shape m_result; //!< Result.

};

#endif
