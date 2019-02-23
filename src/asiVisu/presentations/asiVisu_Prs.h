//-----------------------------------------------------------------------------
// Created on: 25 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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

#ifndef asiVisu_Prs_h
#define asiVisu_Prs_h

// asiVisu includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>
#include <asiVisu_Selection.h>

// Active Data (API) includes
#include <ActAPI_IModel.h>
#include <ActAPI_INode.h>
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>

// VTK includes
#pragma warning(push, 0)
#include <vtkCellPicker.h>
#include <vtkRenderer.h>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QColor>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Base class for all kinds of Node Presentations which can be visualized
//! in a 3D viewer.
class asiVisu_Prs : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_Prs, Standard_Transient)

public:

  //! Pipeline groups.
  enum PipelineGroup
  {
    Group_Prs,       //!< Group of pipelines dedicated to presentation.
    Group_Selection, //!< Group of pipelines dedicated to persistent selection.
    Group_Detection  //!< Group of pipelines dedicated to transient detection.
  };

public:

  asiVisu_EXPORT virtual bool
    IsVisible() const = 0;

// Interface methods:
public:

  asiVisu_EXPORT virtual bool
    IsColorizable() const;

  asiVisu_EXPORT virtual void
    SetColor(const QColor& color) const;

  asiVisu_EXPORT virtual void
    InitPipelines();

  asiVisu_EXPORT virtual void
    UpdatePipelines() const;

  asiVisu_EXPORT virtual Handle(asiVisu_Pipeline)
    GetPipeline(const int id) const;

  asiVisu_EXPORT virtual Handle(asiVisu_HPipelineList)
    GetPipelineList() const;

  asiVisu_EXPORT virtual Handle(asiVisu_Pipeline)
    GetSelectionPipeline(const int idx = 1) const;

  asiVisu_EXPORT virtual Handle(asiVisu_HPipelineList)
    GetSelectionPipelineList() const;

  asiVisu_EXPORT virtual Handle(asiVisu_Pipeline)
    GetDetectionPipeline(const int idx = 1) const;

  asiVisu_EXPORT virtual Handle(asiVisu_HPipelineList)
    GetDetectionPipelineList() const;

  asiVisu_EXPORT virtual const Handle(ActAPI_INode)&
    GetNode() const;

  asiVisu_EXPORT virtual void
    SetModel(const Handle(ActAPI_IModel)& model);

  asiVisu_EXPORT virtual const Handle(ActAPI_IModel)&
    GetModel() const;

public:

  asiVisu_EXPORT void
    RenderPipelines(vtkRenderer* renderer) const;

  asiVisu_EXPORT void
    DeRenderPipelines(vtkRenderer* renderer) const;

  asiVisu_EXPORT void
    Highlight(vtkRenderer*                        renderer,
              const Handle(asiVisu_PickerResult)& pickRes,
              const asiVisu_SelectionNature       selNature) const;

  asiVisu_EXPORT void
    UnHighlight(vtkRenderer*                  renderer,
                const asiVisu_SelectionNature selNature) const;

public:

  asiVisu_EXPORT virtual void
    InitializePicker(const vtkSmartPointer<vtkCellPicker>& picker) const;

public:

  asiVisu_EXPORT virtual void
    SetDiagnosticTools(ActAPI_ProgressEntry progress,
                       ActAPI_PlotterEntry  plotter);

// Presentation construction methods:
protected:

  asiVisu_EXPORT void
    addPipeline(const int                       id,
                const Handle(asiVisu_Pipeline)& pipeline);

  asiVisu_EXPORT void
    assignDataProvider(const int                           id,
                       const Handle(asiVisu_DataProvider)& dataProvider);

  asiVisu_EXPORT Handle(asiVisu_DataProvider)
    dataProvider(const int id) const;

  asiVisu_EXPORT Handle(asiVisu_DataProvider)
    dataProvider(const Handle(asiVisu_Pipeline)& pipeline,
                 const PipelineGroup             group) const;

  asiVisu_EXPORT Handle(asiVisu_DataProvider)
    dataProviderSelection(const int idx = 1) const;

  asiVisu_EXPORT Handle(asiVisu_DataProvider)
    dataProviderDetection(const int idx = 1) const;

  asiVisu_EXPORT void
    installSelectionPipeline(const Handle(asiVisu_Pipeline)&     pipeline,
                             const Handle(asiVisu_DataProvider)& dataProvider,
                             const int                           idx = 1);

  asiVisu_EXPORT void
    installDetectionPipeline(const Handle(asiVisu_Pipeline)&     pipeline,
                             const Handle(asiVisu_DataProvider)& dataProvider,
                             const int                           idx = 1);

protected:

  asiVisu_EXPORT
    asiVisu_Prs(const Handle(ActAPI_INode)& N);

private:

  //! Copying prohibited.
  asiVisu_Prs(const asiVisu_Prs&) = delete;

  //! Assignment prohibited.
  asiVisu_Prs& operator=(const asiVisu_Prs&) = delete;

protected:

  //! To be provided by descendants.
  virtual void beforeInitPipelines() = 0;

  //! To be provided by descendants.
  virtual void afterInitPipelines() = 0;

  //! To be provided by descendants.
  virtual void beforeUpdatePipelines() const = 0;

  //! To be provided by descendants.
  virtual void afterUpdatePipelines() const = 0;

  //! To be provided by descendants.
  virtual void highlight(vtkRenderer*,
                         const Handle(asiVisu_PickerResult)&,
                         const asiVisu_SelectionNature) const = 0;

  //! To be provided by descendants.
  virtual void unHighlight(vtkRenderer*,
                           const asiVisu_SelectionNature) const = 0;

  //! To be provided by descendants.
  virtual void renderPipelines(vtkRenderer*) const = 0;

  //! To be provided by descendants.
  virtual void deRenderPipelines(vtkRenderer*) const = 0;

private:

  //! Type definition for a collection of visualization pipelines comprising
  //! the presentation object. Notice that each pipeline is associated with
  //! some unique ID to facilitate its manipulation.
  typedef NCollection_DataMap<int,
                              Handle(asiVisu_Pipeline)> PipelineMap;

  //! Type definition for pipeline Data Providers.
  typedef NCollection_DataMap<int,
                              Handle(asiVisu_DataProvider)> DataProviderMap;

  //! Repository of pipelines distributed by groups.
  typedef NCollection_DataMap<PipelineGroup, PipelineMap> PipelineRepo;

  //! Repository of Data Providers distributed by groups and corresponding
  //! to pipelines.
  typedef NCollection_DataMap<PipelineGroup, DataProviderMap> DataProviderRepo;

protected:

  //! Pipelines.
  PipelineRepo m_pipelineRepo;

  //! Data Providers.
  DataProviderRepo m_dataPrvRepo;

  //! Data Model instance.
  Handle(ActAPI_IModel) m_model;

  //! Data Node the Presentation is bound to.
  Handle(ActAPI_INode) m_node;

  //! Progress notifier.
  ActAPI_ProgressEntry m_progress;

  //! Imperative plotter.
  ActAPI_PlotterEntry m_plotter;

};

//-----------------------------------------------------------------------------

//! This class represents current selection state. The selection state is
//! two-fold: it contains information on the selected entities for both
//! "detection" and "picking" modes (what we call a "selection nature").
//! For each mode, the following information is available:
//!
//! - Result of cell picker.
//! - Result of point picker.
//! - Result of world picker.
//! - Tentative presentations.
//!
//! The list of tentative presentations is used for highlighting and
//! unhighlighting. The logic of how to highlight and unhighlight
//! a Presentation is implemented directly in the presentation classes, so
//! the visualization framework asks presentations to highlight/unhighlight
//! themselves.
class asiVisu_ActualSelection
{
public:

  //! Asset of structures used to represent the current selection.
  struct t_asset
  {
    std::vector<Handle(asiVisu_Prs)>  Presentations;     //!< Picked presentations.
    ActAPI_DataObjectIdList           NodeIds;           //!< Picked Data Nodes.
    Handle(asiVisu_CellPickerResult)  CellPickerResult;  //!< Last result of cell picker.
    Handle(asiVisu_PointPickerResult) PointPickerResult; //!< Last result of point picker.
    Handle(asiVisu_WorldPickerResult) WorldPickerResult; //!< Last result of world picker.

    //! Default ctor.
    t_asset()
    {
      CellPickerResult  = new asiVisu_CellPickerResult;
      PointPickerResult = new asiVisu_PointPickerResult;
      WorldPickerResult = new asiVisu_WorldPickerResult;
    }
  };

public:

  //! Default ctor.
  asiVisu_ActualSelection()
  {
    m_detection = new t_asset;
    m_picking   = new t_asset;
  }

  //! Destructor.
  ~asiVisu_ActualSelection()
  {
    delete m_detection;
    delete m_picking;
  }

public:

  //! Cleans up the internal collections.
  //! \param[in] renderer  renderer to remove the Presentations from.
  //! \param[in] selNature selection kind.
  void PopAll(vtkRenderer*                  renderer,
              const asiVisu_SelectionNature selNature)
  {
    t_asset* pAsset = this->getAsset(selNature);
    //
    for ( size_t p = 0; p < pAsset->Presentations.size(); ++p )
    {
      const Handle(asiVisu_Prs)& prs = pAsset->Presentations[p];
      //
      if ( prs.IsNull() )
        continue;

      prs->UnHighlight(renderer, selNature);
    }

    // Clean up the internally stored collection of presentations and picking
    // results for all kinds of pickers.
    pAsset->Presentations.clear();
    pAsset->NodeIds.Clear();
    //
    pAsset->CellPickerResult  ->Clear();
    pAsset->PointPickerResult ->Clear();
    pAsset->WorldPickerResult ->Clear();
  }

  //! Adds the passed Node ID to the list of stored IDs.
  //! \param[in] selNature selection nature in question.
  //! \param[in] id        object ID to add.
  void AddNodeId(const asiVisu_SelectionNature selNature,
                 const ActAPI_DataObjectId&    id)
  {
    this->getAsset(selNature)->NodeIds.Append(id);
  }

  //! Adds the passed Presentation instance to the internal collection.
  //! \param[in] prs       Presentation to add.
  //! \param[in] renderer  renderer to add the Presentation to.
  //! \param[in] selNature selection kind.
  void PushToRender(const Handle(asiVisu_Prs)&    prs,
                    vtkRenderer*                  renderer,
                    const asiVisu_SelectionNature selNature)
  {
    if ( prs.IsNull() )
      return;

    t_asset* pAsset = this->getAsset(selNature);

    // Add presentation (if not yet exists).
    bool prsExists = false;
    //
    for ( size_t p = 0; p < pAsset->Presentations.size(); ++p )
      if ( pAsset->Presentations[p] == prs )
      {
        prsExists = true;
        break;
      }
    //
    if ( !prsExists )
      pAsset->Presentations.push_back(prs);

    // Ask presentation to highlight itself passing non-empty picker results.
    if ( !pAsset->CellPickerResult->IsEmpty() )
      prs->Highlight(renderer, pAsset->CellPickerResult, selNature);
    //
    if ( !pAsset->PointPickerResult->IsEmpty() )
      prs->Highlight(renderer, pAsset->PointPickerResult, selNature);
    //
    if ( !pAsset->WorldPickerResult->IsEmpty() )
      prs->Highlight(renderer, pAsset->WorldPickerResult, selNature);
  }

  //! Accessor for the cell picking result in the given selection nature.
  //! \param[in] selNature selection nature.
  //! \return picking result structure.
  const Handle(asiVisu_CellPickerResult)&
    GetCellPickerResult(const asiVisu_SelectionNature selNature) const
  {
    return this->getAsset(selNature)->CellPickerResult;
  }

  //! Accessor for the point picking result in the given selection nature.
  //! \param[in] selNature selection nature.
  //! \return picking result structure.
  const Handle(asiVisu_PointPickerResult)&
    GetPointPickerResult(const asiVisu_SelectionNature selNature) const
  {
    return this->getAsset(selNature)->PointPickerResult;
  }

  //! Accessor for the world picking result in the given selection nature.
  //! \param[in] selNature selection nature.
  //! \return picking result structure.
  const Handle(asiVisu_WorldPickerResult)&
    GetWorldPickerResult(const asiVisu_SelectionNature selNature) const
  {
    return this->getAsset(selNature)->WorldPickerResult;
  }

  //! Get the collection of the rendered presentations for the given selection
  //! nature.
  //! \param[in] selNature selection kind.
  //! \return sequence of rendered presentations.
  const std::vector<Handle(asiVisu_Prs)>&
    GetRenderedPresentations(const asiVisu_SelectionNature selNature) const
  {
    return this->getAsset(selNature)->Presentations;
  }

  //! \return selection modes.
  int GetSelectionModes() const
  {
    return m_iSelectionModes;
  }

  //! Set active selection modes.
  //! \param[in] selModes selection modes to set.
  void SetSelectionModes(const int selModes)
  {
    m_iSelectionModes = selModes;

    m_picking->CellPickerResult->SetSelectionModes(selModes);
    m_picking->PointPickerResult->SetSelectionModes(selModes);
    m_picking->WorldPickerResult->SetSelectionModes(selModes);

    m_detection->CellPickerResult->SetSelectionModes(selModes);
    m_detection->PointPickerResult->SetSelectionModes(selModes);
    m_detection->WorldPickerResult->SetSelectionModes(selModes);
  }

protected:

  //! Returns the selection asset structure for the given selection nature.
  //! \param[in] selNature selection nature of interest.
  //! \return pointer to the corresponding asset.
  t_asset* getAsset(const asiVisu_SelectionNature selNature) const
  {
    if ( selNature == SelectionNature_Persistent )
      return m_picking;

    if ( selNature == SelectionNature_Detection )
      return m_detection;

    return NULL;
  }

private:

  t_asset* m_detection; //!< For "detection" selection nature.
  t_asset* m_picking;   //!< For "picking" selection nature.

  //! Selection modes.
  int m_iSelectionModes;

};

#endif
