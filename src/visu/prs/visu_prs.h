//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_prs_h
#define visu_prs_h

// Visualization includes
#include <visu_common.h>
#include <visu_data_provider.h>
#include <visu_pipeline.h>
#include <visu_selection.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <NCollection_Sequence.hxx>

// VTK includes
#include <vtkRenderer.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_prs, Standard_Transient)

//! Base class for all kinds of Node presentations which can be visualized
//! in 3D viewer.
class visu_prs : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_prs, Standard_Transient)

public:

  //! Pipeline groups.
  enum PipelineGroup
  {
    Group_Prs,   //!< Group of pipelines dedicated to presentation.
    Group_Pick,  //!< Group of pipelines dedicated to picking.
    Group_Detect //!< Group of pipelines dedicated to detection.
  };

public:

  virtual bool
    IsVisible() const = 0;

// Interface methods:
public:

  virtual void
    InitPipelines();

  virtual void
    UpdatePipelines() const;

  virtual const Handle(visu_pipeline)&
    GetPipeline(const int theId) const;

  virtual const Handle(h_visu_pipeline_list)
    GetPipelineList() const;

  virtual Handle(visu_pipeline)
    GetPickPipeline(const int theIdx = 1) const;

  virtual const Handle(h_visu_pipeline_list)
    GetPickPipelineList() const;

  virtual const Handle(visu_pipeline)&
    GetDetectPipeline(const int theIdx = 1) const;

  virtual const Handle(h_visu_pipeline_list)
    GetDetectPipelineList() const;

  virtual Handle(ActAPI_INode)
    GetNode() const;

public:

  void
    RenderPipelines(vtkRenderer* theRenderer) const;

  void
    DeRenderPipelines(vtkRenderer* theRenderer) const;

  void
    Highlight(vtkRenderer*                 theRenderer,
              const visu_pick_result&      thePickRes,
              const visu_selection_nature& theSelNature) const;

  void
    UnHighlight(vtkRenderer*                 theRenderer,
                const visu_selection_nature& theSelNature) const;

// Presentation construction methods:
protected:

  void
    addPipeline(const int                    theId,
                const Handle(visu_pipeline)& thePipeline);

  void
    assignDataProvider(const int                         theId,
                       const Handle(visu_data_provider)& theDataProvider);

  Handle(visu_data_provider)
    dataProvider(const int theId) const;

  Handle(visu_data_provider)
    dataProvider(const Handle(visu_pipeline)& thePipeline,
                 const PipelineGroup          theGroup) const;

  Handle(visu_data_provider)
    dataProviderPick(const int theIdx = 1) const;

  Handle(visu_data_provider)
    dataProviderDetect(const int theIdx = 1) const;

  void
    installPickPipeline(const Handle(visu_pipeline)&      thePipeline,
                        const Handle(visu_data_provider)& theDataProvider,
                        const int                         theIdx = 1);

  void
    installDetectPipeline(const Handle(visu_pipeline)&      thePipeline,
                          const Handle(visu_data_provider)& theDataProvider,
                          const int                         theIdx = 1);

protected:

  visu_prs(const Handle(ActAPI_INode)& theNode);

private:

  //! Copying prohibited.
  visu_prs(const visu_prs&);

  //! Assignment prohibited.
  visu_prs& operator=(const visu_prs&);

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
                         const visu_pick_result&,
                         const visu_selection_nature&) const = 0;

  //! To be provided by descendants.
  virtual void unHighlight(vtkRenderer*,
                           const visu_selection_nature&) const = 0;

  //! To be provided by descendants.
  virtual void renderPipelines(vtkRenderer*) const = 0;

  //! To be provided by descendants.
  virtual void deRenderPipelines(vtkRenderer*) const = 0;

private:

  //! Type definition for a collection of visualization pipelines comprising
  //! the presentation object. Notice that each pipeline is associated with
  //! some unique ID to facilitate its manipulation.
  typedef NCollection_DataMap<int,
                              Handle(visu_pipeline)> PipelineMap;

  //! Type definition for pipeline Data Providers.
  typedef NCollection_DataMap<int,
                              Handle(visu_data_provider)> DataProviderMap;

  //! Repository of pipelines distributed by groups.
  typedef NCollection_DataMap<PipelineGroup, PipelineMap> PipelineRepo;

  //! Repository of Data Providers distributed by groups and corresponding
  //! to pipelines.
  typedef NCollection_DataMap<PipelineGroup, DataProviderMap> DataProviderRepo;

private:

  //! Pipelines.
  PipelineRepo m_pipelineRepo;

  //! Data Providers.
  DataProviderRepo m_dataPrvRepo;

  //! Data Node the Presentation is bound to.
  Handle(ActAPI_INode) m_node;

};

//-----------------------------------------------------------------------------

//! Collection of Presentation instances issued for highlighting. This class
//! represents the currently active highlighting.
class visu_actual_selection
{
public:

  typedef NCollection_Sequence<Handle(visu_prs)>                       PrsSeq;
  typedef NCollection_DataMap<visu_selection_nature, PrsSeq>           PrsMap;
  typedef NCollection_DataMap<visu_selection_nature, visu_pick_result> PickResultMap;

public:

  //! Default constructor.
  visu_actual_selection()
  {
    m_prsSet.Bind( SelectionNature_Pick,      PrsSeq() );
    m_prsSet.Bind( SelectionNature_Detection, PrsSeq() );

    m_pickResultSet.Bind( SelectionNature_Pick,      visu_pick_result() );
    m_pickResultSet.Bind( SelectionNature_Detection, visu_pick_result() );
  }

  //! Cleans up the internal collection.
  //! \param theRenderer  [in] renderer to remove the Presentations from.
  //! \param theSelNature [in] selection kind.
  void PopAll(vtkRenderer*                 theRenderer,
              const visu_selection_nature& theSelNature)
  {
    PrsSeq::Iterator anIt( m_prsSet.Find(theSelNature) );
    for ( ; anIt.More(); anIt.Next() )
    {
      const Handle(visu_prs)& aPrs = anIt.Value();
      if ( aPrs.IsNull() )
        continue;

      aPrs->UnHighlight(theRenderer, theSelNature);
    }

    m_prsSet.ChangeFind(theSelNature).Clear();
    m_pickResultSet.ChangeFind(theSelNature).Clear();
  }

  //! Adds the passed Presentation instance to the internal collection.
  //! \param thePrs       [in] Presentation to add.
  //! \param theRenderer  [in] renderer to add the Presentation to.
  //! \param theSelNature [in] selection kind.
  void PushToRender(const Handle(visu_prs)&      thePrs,
                    vtkRenderer*                 theRenderer,
                    const visu_selection_nature& theSelNature)
  {
    visu_pick_result aPickRes = m_pickResultSet.Find(theSelNature);
    m_prsSet.ChangeFind(theSelNature).Append(thePrs);

    thePrs->Highlight(theRenderer, aPickRes, theSelNature);
  }

  //! Accessor for picking result structure by selection nature.
  //! \param theSelNature [in] selection nature.
  //! \return picking result structure.
  visu_pick_result& ChangePickResult(const visu_selection_nature& theSelNature)
  {
    return m_pickResultSet.ChangeFind(theSelNature);
  }

  //! Accessor for picking result structure by selection nature.
  //! \param theSelNature [in] selection nature.
  //! \return picking result structure.
  const visu_pick_result& PickResult(const visu_selection_nature& theSelNature) const
  {
    return m_pickResultSet.Find(theSelNature);
  }

  //! Get sequence of rendered presentations.
  //! \param theSelNature [in] selection kind.
  //! \return sequence of rendered presentations.
  const PrsSeq&
    RenderedPresentations(const visu_selection_nature theSelNature) const
  {
    return m_prsSet.Find(theSelNature);
  }

private:

  //! Internal collection of Presentations in OCCT form.
  PrsMap m_prsSet;

  //! Picking results data structure.
  PickResultMap m_pickResultSet;

};

#endif
