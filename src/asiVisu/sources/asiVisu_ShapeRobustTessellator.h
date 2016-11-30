//-----------------------------------------------------------------------------
// Created on: 30 November 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShapeRobustTessellator_h
#define asiVisu_ShapeRobustTessellator_h

// asiVisu includes
#include <asiVisu.h>

// OCCT includes
#include <IVtk_IShapeMesher.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//! Utility which is designed to offer a high level of sustainability of
//! visualization faceting. Why would you want to have a non-standard
//! faceter for your B-Rep model? Such need arises in two major contexts:
//!
//! - Your CAD model is broken. Local topology anomalies may cause failures
//!   in standard triangulation algorithms, which will lead to a situation
//!   when you see just nothing on your screen. However, you may want to
//!   observe the topology anomalies and have at least an idea on how
//!   shape looks like.
//!
//! - Your CAD model is in temporary "not-so-valid" state. This usually happens
//!   when you apply Euler Operators which keep the topological integrity
//!   of your model but generally break its geometric consistency. A broken
//!   geometric integrity is a severe issue for faceters, so you end up,
//!   again, with a "black screen". However, the inability to see the
//!   intermediate results of Euler Ops contradicts to our intuition as
//!   the result of every single operator can be crafted by hand (well,
//!   involving some abstract shapes due to unknown geometries).
//!
//! This tessellation tool attempts to build a detailed polygonal
//! representation of a shape regardless of any invalidities. Of course,
//! such representation cannot be unambiguous since PMC is not defined for
//! "not-so-valid" models. However, the advantage of such visualization over
//! the standard one is its capacity to give you "at least something" to
//! look at. The latter enables you to debug your geometric stuff while
//! having some visual feedback from your prototyping system.
class asiVisu_ShapeRobustTessellator : public IVtk_IShapeMesher
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ShapeRobustTessellator, IVtk_IShapeMesher)

public:

  asiVisu_EXPORT
    asiVisu_ShapeRobustTessellator();

public:

  asiVisu_EXPORT TopoDS_Shape
    GetShape() const;

protected:

  asiVisu_EXPORT virtual void
    internalBuild();

protected:

  asiVisu_EXPORT void
    addVertex(const TopoDS_Vertex& vertex,
              const IVtk_IdType    shapeId,
              const IVtk_MeshType  meshType);

  asiVisu_EXPORT void
    addEdge(const TopoDS_Edge&  edge,
            const IVtk_IdType   shapeId,
            const IVtk_MeshType meshType);

protected:

  TopTools_IndexedMapOfShape m_all;      //!< All topological elements.
  TopTools_IndexedMapOfShape m_vertices; //!< Vertices.
  TopTools_IndexedMapOfShape m_edges;    //!< Edges.
  TopTools_IndexedMapOfShape m_faces;    //!< Faces.

};

#endif
