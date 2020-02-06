//-----------------------------------------------------------------------------
// Created on: 15 April 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_PartPipelineBase_h
#define asiVisu_PartPipelineBase_h

// asiVisu includes
#include <asiVisu_Pipeline.h>
#include <asiVisu_Selection.h>
#include <asiVisu_ShapeDisplayModeFilter.h>
#include <asiVisu_ShapeRobustSource.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// VTK includes
#include <vtkPolyDataNormals.h>

//-----------------------------------------------------------------------------

//! Base class for all pipelines associated with Part Node.
class asiVisu_PartPipelineBase : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PartPipelineBase, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_PartPipelineBase(const vtkSmartPointer<asiVisu_ShapeRobustSource>& source);

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
  const vtkSmartPointer<asiVisu_ShapeRobustSource>& GetSource() const
  {
    return m_source;
  }

  //! \return normals calculation filter.
  const vtkSmartPointer<vtkPolyDataNormals>& GetNormalsFilter() const
  {
    return m_normalsFilter;
  }

  //! \return display mode filter.
  const vtkSmartPointer<asiVisu_ShapeDisplayModeFilter>& GetDisplayModeFilter() const
  {
    return m_dmFilter;
  }

  //! Enables/disables scalars for the mapper.
  //! \param[in] on Boolean value to set.
  void SetScalarsAllowed(const bool on)
  {
    m_bScalarsAllowed = on;
  }

  //! \return Boolean flag indicating whether scalars are enabled for mapper.
  bool GetScalarsAllowed()
  {
    return m_bScalarsAllowed;
  }

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_PartPipelineBase(const asiVisu_PartPipelineBase&);

  //! Assignment prohibited.
  asiVisu_PartPipelineBase& operator=(const asiVisu_PartPipelineBase&);

protected:

  //! Clears internal cache data maps.
  void clearCache()
  {
    m_detectedCells.Clear();
    m_selectedCells.Clear();
  }

protected:

  //! Indicates whether scalars are allowed or not.
  bool m_bScalarsAllowed;

  //! Technical flag indicating whether a GL-mapper is initialized with the
  //! custom color scheme or not.
  bool m_bMapperColorsSet;

  //! Data source.
  vtkSmartPointer<asiVisu_ShapeRobustSource> m_source;

  //! Filter for normals.
  vtkSmartPointer<vtkPolyDataNormals> m_normalsFilter;

  //! Display mode filter.
  vtkSmartPointer<asiVisu_ShapeDisplayModeFilter> m_dmFilter;

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
