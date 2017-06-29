//-----------------------------------------------------------------------------
// Created on: 15 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_BCurvePolesPipeline_h
#define asiVisu_BCurvePolesPipeline_h

// A-Situs includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

// VTK includes
#include <vtkExtractSelection.h>
#include <vtkGeometryFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>

// OCCT includes
#include <TColStd_PackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_BCurvePolesPipeline, asiVisu_Pipeline)

//! Visualization pipeline for b-curve poles.
class asiVisu_BCurvePolesPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_BCurvePolesPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_BCurvePolesPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

public:

  asiVisu_EXPORT void
    SetSelectedPoles(const TColStd_PackedMapOfInteger& mask);

public:

  void ForceExecution() { m_bForced = true; }

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_BCurvePolesPipeline(const asiVisu_BCurvePolesPipeline&);

  //! Assignment prohibited.
  asiVisu_BCurvePolesPipeline& operator=(const asiVisu_BCurvePolesPipeline&);

private:

  bool                                 m_bForced;          //!< Forced update.
  bool                                 m_bMapperColorsSet; //!< Boolean flag indicating whether lookup table is set.
  vtkSmartPointer<vtkIdTypeArray>      m_selected;         //!< Poles selected for visualization.
  vtkSmartPointer<vtkSelectionNode>    m_selectionNode;    //!< VTK selection node.
  vtkSmartPointer<vtkSelection>        m_selection;        //!< VTK selection.
  vtkSmartPointer<vtkExtractSelection> m_extractSelection; //!< VTK selection extractor.
  vtkSmartPointer<vtkGeometryFilter>   m_toPolyData;       //!< VTK geometry filter.

};

#endif
