//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_ShrinkPolyFilter_h
#define asiVisu_ShrinkPolyFilter_h

// Visualization includes
#include <asiVisu.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//! Filter performing shrink operation on polygonal data set and having
//! a polygonal data set as an output as well. Unlike vtkShrinkPolyData
//! filter shipped with VTK, this one is more stable (the native one leads
//! to memory corruption crashes on VTK 5.8.0).
class asiVisu_ShrinkPolyFilter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_ShrinkPolyFilter, vtkPolyDataAlgorithm);
  static asiVisu_ShrinkPolyFilter* New();

  vtkSetClampMacro(ShrinkFactor, double, 0.0, 1.0);
  vtkGetMacro(ShrinkFactor, double);

protected:

  virtual int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);

protected:

  asiVisu_ShrinkPolyFilter();
  ~asiVisu_ShrinkPolyFilter();

private:

  double ShrinkFactor; //!< Shrink factor to apply.

};

#endif
