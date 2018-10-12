//-----------------------------------------------------------------------------
// Created on: 25 November 2015
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
//    * Neither the name of Sergey Slyadnev nor the
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

// Visualization includes
#include <asiVisu.h>
#include <asiVisu_DataProvider.h>
#include <asiVisu_Pipeline.h>
#include <asiVisu_Selection.h>

// Active Data (API) includes
#include <ActAPI_INode.h>
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>

// VTK includes
#include <vtkCellPicker.h>
#include <vtkRenderer.h>

// Qt includes
#pragma warning(push, 0)
#include <QColor>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Base class for all kinds of Node Presentations which can be visualized
//! in 3D viewer.
class asiVisu_Prs : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_Prs, Standard_Transient)

public:

  //! Pipeline groups.
  enum PipelineGroup
  {
    Group_Prs,   //!< Group of pipelines dedicated to presentation.
    Group_Pick,  //!< Group of pipelines dedicated to picking.
    Group_Detect //!< Group of pipelines dedicated to detection.
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
    GetPipeline(const int theId) const;

  asiVisu_EXPORT virtual Handle(asiVisu_HPipelineList)
    GetPipelineList() const;

  asiVisu_EXPORT virtual Handle(asiVisu_Pipeline)
    GetPickPipeline(const int theIdx = 1) const;

  asiVisu_EXPORT virtual Handle(asiVisu_HPipelineList)
    GetPickPipelineList() const;

  asiVisu_EXPORT virtual Handle(asiVisu_Pipeline)
    GetDetectPipeline(const int theIdx = 1) const;

  asiVisu_EXPORT virtual Handle(asiVisu_HPipelineList)
    GetDetectPipelineList() const;

  asiVisu_EXPORT virtual Handle(ActAPI_INode)
    GetNode() const;

public:

  asiVisu_EXPORT void
    RenderPipelines(vtkRenderer* theRenderer) const;

  asiVisu_EXPORT void
    DeRenderPipelines(vtkRenderer* theRenderer) const;

  asiVisu_EXPORT void
    Highlight(vtkRenderer*                  theRenderer,
              const asiVisu_PickResult&     thePickRes,
              const asiVisu_SelectionNature theSelNature) const;

  asiVisu_EXPORT void
    UnHighlight(vtkRenderer*                  theRenderer,
                const asiVisu_SelectionNature theSelNature) const;

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
    addPipeline(const int                       theId,
                const Handle(asiVisu_Pipeline)& thePipeline);

  asiVisu_EXPORT void
    assignDataProvider(const int                           theId,
                       const Handle(asiVisu_DataProvider)& theDataProvider);

  asiVisu_EXPORT Handle(asiVisu_DataProvider)
    dataProvider(const int theId) const;

  asiVisu_EXPORT Handle(asiVisu_DataProvider)
    dataProvider(const Handle(asiVisu_Pipeline)& thePipeline,
                 const PipelineGroup             theGroup) const;

  asiVisu_EXPORT Handle(asiVisu_DataProvider)
    dataProviderPick(const int theIdx = 1) const;

  asiVisu_EXPORT Handle(asiVisu_DataProvider)
    dataProviderDetect(const int theIdx = 1) const;

  asiVisu_EXPORT void
    installPickPipeline(const Handle(asiVisu_Pipeline)&     thePipeline,
                        const Handle(asiVisu_DataProvider)& theDataProvider,
                        const int                           theIdx = 1);

  asiVisu_EXPORT void
    installDetectPipeline(const Handle(asiVisu_Pipeline)&     thePipeline,
                          const Handle(asiVisu_DataProvider)& theDataProvider,
                          const int                           theIdx = 1);

protected:

  asiVisu_EXPORT
    asiVisu_Prs(const Handle(ActAPI_INode)& theNode);

private:

  //! Copying prohibited.
  asiVisu_Prs(const asiVisu_Prs&);

  //! Assignment prohibited.
  asiVisu_Prs& operator=(const asiVisu_Prs&);

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
                         const asiVisu_PickResult&,
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

  //! Data Node the Presentation is bound to.
  Handle(ActAPI_INode) m_node;

  //! Progress notifier.
  ActAPI_ProgressEntry m_progress;

  //! Imperative plotter.
  ActAPI_PlotterEntry m_plotter;

};

//-----------------------------------------------------------------------------

//! This class represents current selection.
class asiVisu_ActualSelection
{
public:

  typedef NCollection_Sequence<Handle(asiVisu_Prs)>                                      PrsSeq;
  typedef NCollection_DataMap<asiVisu_SelectionNature, PrsSeq>                           PrsMap;
  typedef NCollection_DataMap<asiVisu_SelectionNature, Handle(asiVisu_CellPickerResult)> PickerResultMap;

public:

  //! Default ctor.
  asiVisu_ActualSelection()
  {
    m_prsSet.Bind( SelectionNature_Pick,      PrsSeq() );
    m_prsSet.Bind( SelectionNature_Detection, PrsSeq() );

    // Picker results.
    m_pickerResultSet.Bind( SelectionNature_Pick,      NULL );
    m_pickerResultSet.Bind( SelectionNature_Detection, NULL );
  }

  //! Cleans up the internal collections.
  //! \param[in] renderer  renderer to remove the Presentations from.
  //! \param[in] selNature selection kind.
  void PopAll(vtkRenderer*                  renderer,
              const asiVisu_SelectionNature selNature)
  {
    PrsSeq::Iterator anIt( m_prsSet.Find(selNature) );
    for ( ; anIt.More(); anIt.Next() )
    {
      const Handle(asiVisu_Prs)& aPrs = anIt.Value();
      if ( aPrs.IsNull() )
        continue;

      aPrs->UnHighlight(renderer, selNature);
    }

    m_prsSet          .ChangeFind(selNature).Clear();
    m_pickerResultSet .ChangeFind(selNature).Nullify();
  }

  //! Adds the passed Presentation instance to the internal collection.
  //! \param[in] prs       Presentation to add.
  //! \param[in] renderer  renderer to add the Presentation to.
  //! \param[in] selNature selection kind.
  void PushToRender(const Handle(asiVisu_Prs)&    prs,
                    vtkRenderer*                  renderer,
                    const asiVisu_SelectionNature selNature)
  {
    const asiVisu_CellPickerResult& cellPickRes = m_pickerResultSet(selNature);
    m_prsSet.ChangeFind(selNature).Append(prs);

    prs->Highlight(renderer, cellPickRes, selNature);
  }

  //! Accessor for picking result structure by selection nature.
  //! \param theSelNature [in] selection nature.
  //! \return picking result structure.
  asiVisu_PickResult& ChangePickResult(const asiVisu_SelectionNature theSelNature)
  {
    return m_pickResultSet.ChangeFind(theSelNature);
  }

  //! Accessor for picking result structure by selection nature.
  //! \param theSelNature [in] selection nature.
  //! \return picking result structure.
  const asiVisu_PickResult& PickResult(const asiVisu_SelectionNature theSelNature) const
  {
    return m_pickResultSet.Find(theSelNature);
  }

  //! Get sequence of rendered presentations.
  //! \param theSelNature [in] selection kind.
  //! \return sequence of rendered presentations.
  const PrsSeq&
    RenderedPresentations(const asiVisu_SelectionNature theSelNature) const
  {
    return m_prsSet.Find(theSelNature);
  }

private:

  //! Internal collection of Presentations in OCCT form.
  PrsMap m_prsSet;

  //! Picker results.
  PickerResultMap m_pickerResultSet;

};

#endif
