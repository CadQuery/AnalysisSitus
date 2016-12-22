//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_MeshMerge_h
#define asiAlgo_MeshMerge_h

// A-Situs includes
#include <asiAlgo.h>

// Active Data includes
#include <Mesh.h>

// OCCT includes
#include <Poly_CoherentTriangulation.hxx>
#include <TopoDS_Shape.hxx>

//! Given a shape with initialized tessellations inside, assembles all
//! triangles in a monolithic structure where entire topology is discarded.
//! Use this tool to assemble a single tessellation from series of
//! tessellations distributed by several faces.
class asiAlgo_MeshMerge
{
public:

  //! Conversion mode.
  enum Mode
  {
    Mode_PolyCoherentTriangulation,
    Mode_Mesh
  };

public:

  asiAlgo_EXPORT
    asiAlgo_MeshMerge(const TopoDS_Shape& body,
                      const Mode          mode);

public:

  //! \return result.
  const Handle(Poly_CoherentTriangulation)& GetResultPoly() const
  {
    return m_resultPoly;
  }

  //! \return result.
  const Handle(Mesh)& GetResultMesh() const
  {
    return m_resultMesh;
  }

protected:

  void build();

protected:

  TopoDS_Shape                       m_body;       //!< Working body.
  Mode                               m_mode;       //!< Conversion mode.
  Handle(Poly_CoherentTriangulation) m_resultPoly; //!< Result tessellation.
  Handle(Mesh)                       m_resultMesh; //!< Result mesh.

};

#endif
