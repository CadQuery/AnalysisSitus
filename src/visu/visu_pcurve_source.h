//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_pcurve_source_h
#define visu_pcurve_source_h

// Visualization includes
#include <visu_curve_source.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//! Source of polygonal data representing a curve on surface.
class visu_pcurve_source : public visu_curve_source
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_pcurve_source, vtkPolyDataAlgorithm);
  ASitus_EXPORT static visu_pcurve_source* New();

// Kernel methods:
public:

  ASitus_EXPORT void
    SetEdgeOnFace(const TopoDS_Edge& edge,
                  const TopoDS_Face& face);

protected:

  visu_pcurve_source();
  ~visu_pcurve_source();

private:

  visu_pcurve_source(const visu_pcurve_source&);
  visu_pcurve_source& operator=(const visu_pcurve_source&);

private:

  TopoDS_Edge m_edge; //!< Edge.
  TopoDS_Face m_face; //!< Face.

};

#endif
