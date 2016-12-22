//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_PCurveSource_h
#define asiVisu_PCurveSource_h

// Visualization includes
#include <asiVisu_CurveSource.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//! Source of polygonal data representing a curve on surface.
class asiVisu_PCurveSource : public asiVisu_CurveSource
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_PCurveSource, vtkPolyDataAlgorithm);
  static asiVisu_PCurveSource* New();

// Kernel methods:
public:

  void
    SetEdgeOnFace(const TopoDS_Edge& edge,
                  const TopoDS_Face& face);

protected:

  asiVisu_PCurveSource();
  ~asiVisu_PCurveSource();

private:

  asiVisu_PCurveSource(const asiVisu_PCurveSource&);
  asiVisu_PCurveSource& operator=(const asiVisu_PCurveSource&);

private:

  TopoDS_Edge m_edge; //!< Edge.
  TopoDS_Face m_face; //!< Face.

};

#endif
