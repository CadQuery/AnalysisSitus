//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_law_func_source_h
#define visu_law_func_source_h

// Visualization includes
#include <visu_curve_source.h>

// Calculation includes
#include <calculus_design_law.h>

//! Source of polygonal data representing a univariate explicit law function.
class visu_law_source : public visu_curve_source
{
// RTTI and construction:
public:

  vtkTypeMacro(visu_law_source, vtkPolyDataAlgorithm);
  static visu_law_source* New();

// Kernel methods:
public:

  void SetLaw(const Handle(calculus_design_law)& law);

protected:

  visu_law_source();
  ~visu_law_source();

private:

  visu_law_source(const visu_law_source&);
  visu_law_source& operator=(const visu_law_source&);

private:

  Handle(calculus_design_law) m_law; //!< Design law to convert to polygonal form.

};

#endif
