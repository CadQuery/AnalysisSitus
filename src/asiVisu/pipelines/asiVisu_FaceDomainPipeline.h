//-----------------------------------------------------------------------------
// Created on: 02 December 2015
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

#ifndef asiVisu_FaceDomainPipeline_h
#define asiVisu_FaceDomainPipeline_h

// asiVisu includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>

// VTK includes
#include <vtkExtractSelection.h>
#include <vtkGeometryFilter.h>
#include <vtkIdTypeArray.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>

//-----------------------------------------------------------------------------

//! Visualization pipeline for face domain.
class asiVisu_FaceDomainPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FaceDomainPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_FaceDomainPipeline(const bool isDefaultColorScheme = true);

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& DP);

public:

  void ForceExecution() { m_bForced = true; }

public:

  asiVisu_EXPORT void
    SetSelectedCells(const TColStd_PackedMapOfInteger& mask);

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_FaceDomainPipeline(const asiVisu_FaceDomainPipeline&);

  //! Assignment prohibited.
  asiVisu_FaceDomainPipeline& operator=(const asiVisu_FaceDomainPipeline&);

private:

  double computeTipSize(const TopoDS_Face& F) const;

private:

  bool                                 m_bDefaultColorScheme; //!< Indicates whether to use a default color scheme.
  bool                                 m_bMapperColorsSet;    //!< Boolean flag indicating whether lookup table is set.
  bool                                 m_bForced;             //!< Forced update.
  vtkSmartPointer<vtkIdTypeArray>      m_selected;            //!< Poles selected for visualization.
  vtkSmartPointer<vtkSelectionNode>    m_selectionNode;       //!< VTK selection node.
  vtkSmartPointer<vtkSelection>        m_selection;           //!< VTK selection.
  vtkSmartPointer<vtkExtractSelection> m_extractSelection;    //!< VTK selection extractor.
  vtkSmartPointer<vtkGeometryFilter>   m_toPolyData;          //!< VTK geometry filter.

};

#endif
