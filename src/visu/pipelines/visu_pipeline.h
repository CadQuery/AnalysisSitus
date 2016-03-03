//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_pipeline_h
#define visu_pipeline_h

// A-Situs includes
#include <analysis_situs.h>

// Visualization includes
#include <visu_data_provider.h>

// Active Data (auxiliary) includes
#include <ActAux_TimeStamp.h>

// VTK includes
#include <vtkAlgorithm.h>
#include <vtkPassThroughFilter.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

DEFINE_STANDARD_HANDLE(visu_pipeline, Standard_Transient)

//! Interface for any visualization pipeline in Analysis Situs. Each pipeline
//! represents a set of filters for the input dataset along with the
//! corresponding actor and mapper. The one-to-one correspondence between
//! the actor and the pipeline reflects the idea that a single actor is
//! responsible for a dedicated presentation rather than for a distinct object.
class visu_pipeline : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(visu_pipeline, Standard_Transient)

public:

  virtual void
    SetInput(const Handle(visu_data_provider)& theDataProvider) = 0;

public:

  void
    SetInputData(const vtkSmartPointer<vtkDataSet>& data);

  void
    SetInputConnection(vtkAlgorithmOutput* out);

public:

  virtual void
    Update();

  virtual void
    Build();

public:

  vtkActor*
    Actor();

  vtkMapper*
    Mapper();

  void
    AddToRenderer(vtkRenderer* renderer);

  void
    RemoveFromRenderer(vtkRenderer* renderer);

public:

  void
    Modified();

  Handle(ActAux_TimeStamp)
    GetMTime() const;

protected:

  visu_pipeline(const vtkSmartPointer<vtkMapper>& mapper,
                const vtkSmartPointer<vtkActor>&  actor);

// Pipeline construction routines to be used by derived classes only:
protected:

  void
    disable(const int index);

  void
    enable(const int index);

  void
    append(const vtkSmartPointer<vtkAlgorithm>& filter);

  vtkSmartPointer<vtkAlgorithm>
    filter(const int index) const;

  int
    numFilters() const;

private:

  //! Callback routine which gives the derived classes a chance to do
  //! something just after the pipeline's actor was added to the renderer.
  //! \param renderer [in] active renderer.
  virtual void
    callback_add_to_renderer(vtkRenderer* renderer) = 0;

  //! Callback routine which gives the derived classes a chance to do
  //! something just after the pipeline's actor was removed from the renderer.
  //! \param renderer [in] active renderer.
  virtual void
    callback_remove_from_renderer(vtkRenderer* renderer) = 0;

  //! Callback routine which gives the derived classes a chance to do
  //! something just before the pipeline will be updated.
  //! \param theRenderer [in] active renderer.
  virtual void
    callback_update() = 0;

private:

  //! Copying prohibited.
  visu_pipeline(const visu_pipeline&);

  //! Assignment prohibited.
  visu_pipeline& operator=(const visu_pipeline&);

// Few internal type definitions:
protected:

  //! Type shortcut for primitive collection of filters.
  typedef NCollection_Sequence< vtkSmartPointer<vtkAlgorithm> > t_algo_sequence;

// Basic members:
protected:

  //! Indicates whether the input of the pipeline is initialized.
  bool m_bInitialized;

  //! Indicates whether the pipeline has been already built or not.
  bool m_bBuilt;

  //! Collection of filters composing the pipeline. This is the main property
  //! which is changed between different types of pipelines.
  t_algo_sequence m_filters;

  //! Data source mapper. Normally created automatically, however, user is free
  //! to pass a custom mapper into the pipeline.
  vtkSmartPointer<vtkMapper> m_mapper;

  //! Pipeline's actor. Normally created automatically, however, user is free
  //! to pass a custom actor into the pipeline.
  vtkSmartPointer<vtkActor> m_actor;

  //! Filter used to standardize building procedure for pipelines. Each
  //! pipeline is not forced to start from vtkDataSet as it can be connected
  //! to the outputs of other external filtering procedures (e.g. some kinds
  //! of "data selectors"). This dummy filter performs simple shallow copying
  //! of input data to the output.
  vtkSmartPointer<vtkPassThroughFilter> m_dummyFilter;

  //! Modification timestamp for the Pipeline.
  Handle(ActAux_TimeStamp) m_MTime;

};

//! Shortcuts for list of Pipelines.
typedef NCollection_Sequence<Handle(visu_pipeline)> visu_pipeline_list;
typedef NCollection_Shared<visu_pipeline_list>      h_visu_pipeline_list;

#endif
