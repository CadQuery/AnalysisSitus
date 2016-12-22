//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef visu_law_func_source_h
#define visu_law_func_source_h

// Visualization includes
#include <asiVisu_CurveSource.h>

// Calculation includes
#include <asiAlgo_DesignLaw.h>

//! Source of polygonal data representing a univariate explicit law function.
class asiVisu_LawSource : public asiVisu_CurveSource
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_LawSource, vtkPolyDataAlgorithm);
  static asiVisu_LawSource* New();

// Kernel methods:
public:

  void SetLaw(const Handle(asiAlgo_DesignLaw)& law);

protected:

  asiVisu_LawSource();
  ~asiVisu_LawSource();

private:

  asiVisu_LawSource(const asiVisu_LawSource&);
  asiVisu_LawSource& operator=(const asiVisu_LawSource&);

private:

  Handle(asiAlgo_DesignLaw) m_law; //!< Design law to convert to polygonal form.

};

#endif
