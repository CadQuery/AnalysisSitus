//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_FuncUnivariateSource_h
#define asiVisu_FuncUnivariateSource_h

// asiVisu includes
#include <asiVisu_CurveSource.h>

// asiAlgo includes
#include <asiAlgo_FuncUnivariate.h>

//! Source of polygonal data representing a univariate explicit function.
class asiVisu_FuncUnivariateSource : public asiVisu_CurveSource
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_FuncUnivariateSource, vtkPolyDataAlgorithm);
  static asiVisu_FuncUnivariateSource* New();

// Kernel methods:
public:

  void SetFunc(const Handle(asiAlgo_FuncUnivariate)& func);

protected:

  asiVisu_FuncUnivariateSource();
  ~asiVisu_FuncUnivariateSource();

private:

  asiVisu_FuncUnivariateSource(const asiVisu_FuncUnivariateSource&);
  asiVisu_FuncUnivariateSource& operator=(const asiVisu_FuncUnivariateSource&);

private:

  Handle(asiAlgo_FuncUnivariate) m_func; //!< Function to convert to polygonal form.

};

#endif
