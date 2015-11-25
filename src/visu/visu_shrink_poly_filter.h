//-----------------------------------------------------------------------------
// Created on: 20 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef visu_shrink_poly_filter_h
#define visu_shrink_poly_filter_h

// Visualization includes
#include <visu_common.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//! Filter performing shrink operation on polygonal data set and having
//! a polygonal data set as an output as well. Unlike vtkShrinkPolyData
//! filter shipped with VTK, this one is more stable (the native one leads
//! to memory corruption crashes on VTK 5.8.0).
class visu_shrink_poly_filter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(visu_shrink_poly_filter, vtkPolyDataAlgorithm);
  static visu_shrink_poly_filter* New();

  vtkSetClampMacro(ShrinkFactor, double, 0.0, 1.0);
  vtkGetMacro(ShrinkFactor, double);

protected:

  virtual int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*);

protected:

  visu_shrink_poly_filter();
  ~visu_shrink_poly_filter();

private:

  double ShrinkFactor; //!< Shrink factor to apply.

};

#endif
