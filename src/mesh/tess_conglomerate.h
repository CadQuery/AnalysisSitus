//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef tess_conglomerate_h
#define tess_conglomerate_h

// A-Situs includes
#include <analysis_situs.h>

// Active Data includes
#include <Mesh.h>

// OCCT includes
#include <Poly_CoherentTriangulation.hxx>
#include <TopoDS_Shape.hxx>

//! Given a shape with initialized tessellations inside, assembles all
//! triangles in a monolithic structure where entire topology is discarded.
//! Use this tool to assemble a single tessellation from series of
//! tessellations distributed by several faces.
class tess_conglomerate
{
public:

  //! Conversion mode.
  enum Mode
  {
    Mode_PolyCoherentTriangulation,
    Mode_Mesh
  };

public:

  ASitus_EXPORT
    tess_conglomerate(const TopoDS_Shape& body,
                      const Mode          mode);

public:

  //! \return result.
  inline const Handle(Poly_CoherentTriangulation)& GetResultPoly() const
  {
    return m_resultPoly;
  }

  //! \return result.
  inline const Handle(Mesh)& GetResultMesh() const
  {
    return m_resultMesh;
  }

protected:

  void build();

protected:

  TopoDS_Shape                       m_body;       //!< Working body.
  Mode                               m_mode;       //!< Conversion mode.
  Handle(Poly_CoherentTriangulation) m_resultPoly; //!< Result tessellation.
  Handle(Mesh)                 m_resultMesh; //!< Result mesh.

};

#endif
