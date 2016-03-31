//-----------------------------------------------------------------------------
// Created on: 25 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_surface_source_h
#define visu_surface_source_h

// Visualization includes
#include <visu_utils.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// OCCT includes
#include <Geom_Surface.hxx>

//! Source of polygonal data representing a parametric surface.
class visu_surface_source : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_surface_source, vtkPolyDataAlgorithm);
  static visu_surface_source* New();

// Kernel methods:
public:

  bool SetInputSurface(const Handle(Geom_Surface)& surface);

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  vtkIdType
    registerGridPoint(const gp_Pnt& point,
                      vtkPolyData*  polyData);

  vtkIdType
    registerLine(const gp_Pnt& pointStart,
                 const gp_Pnt& pointEnd,
                 vtkPolyData*  polyData);

protected:

  visu_surface_source();
  ~visu_surface_source();

private:

  visu_surface_source(const visu_surface_source&);
  visu_surface_source& operator=(const visu_surface_source&);

private:

  Handle(Geom_Surface) m_surf; //!< Parametric surface to render.

};

#endif
