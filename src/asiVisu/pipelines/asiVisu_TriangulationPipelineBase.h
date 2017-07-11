//-----------------------------------------------------------------------------
// Created on: 11 November 2017
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

#ifndef asiVisu_TriangulationPipelineBase_h
#define asiVisu_TriangulationPipelineBase_h

// asiVisu includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>
#include <asiVisu_Selection.h>
#include <asiVisu_TriangulationDisplayModeFilter.h>
#include <asiVisu_TriangulationSource.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

//! Base class for all surface triangulation pipelines.
class asiVisu_TriangulationPipelineBase : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_TriangulationPipelineBase, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_TriangulationPipelineBase(const vtkSmartPointer<asiVisu_TriangulationSource>& source);

public:

  asiVisu_EXPORT void
    SetDiagnosticTools(ActAPI_ProgressEntry progress,
                       ActAPI_PlotterEntry  plotter);

  asiVisu_EXPORT void
    SetPickedElements(const TColStd_PackedMapOfInteger& elementIds,
                      const asiVisu_SelectionNature     selNature);

  asiVisu_EXPORT void
    ResetPickedElements(const asiVisu_SelectionNature selNature);

public:

  //! \return data source.
  const vtkSmartPointer<asiVisu_TriangulationSource>& GetSource() const
  {
    return m_source;
  }

  //! \return display mode filter.
  const vtkSmartPointer<asiVisu_TriangulationDisplayModeFilter>& GetDisplayModeFilter() const
  {
    return m_dmFilter;
  }

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_TriangulationPipelineBase(const asiVisu_TriangulationPipelineBase&);

  //! Assignment prohibited.
  asiVisu_TriangulationPipelineBase& operator=(const asiVisu_TriangulationPipelineBase&);

protected:

  //! Clears internal cache data maps.
  void clearCache()
  {
    m_detectedCells.Clear();
    m_selectedCells.Clear();
  }

protected:

  //! Technical flag indicating whether a GL-mapper is initialized with the
  //! custom color scheme or not.
  bool m_bMapperColorsSet;

  //! Data source.
  vtkSmartPointer<asiVisu_TriangulationSource> m_source;

  //! Display mode filter.
  vtkSmartPointer<asiVisu_TriangulationDisplayModeFilter> m_dmFilter;

  //! Progress notifier.
  ActAPI_ProgressEntry m_progress;

  //! Imperative plotter.
  ActAPI_PlotterEntry m_plotter;

  //! Data map of currently DETECTED cell IDs (for fast access). The value
  //! is the original scalar.
  NCollection_DataMap<vtkIdType, int> m_detectedCells;

  //! Data map of currently SELECTED cell IDs (for fast access). The value
  //! is the original scalar.
  NCollection_DataMap<vtkIdType, int> m_selectedCells;

};

#endif
