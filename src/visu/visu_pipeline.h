//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su
//-----------------------------------------------------------------------------

#ifndef visu_pipeline_h
#define visu_pipeline_h

// A-Situs includes
#include <analysis_situs.h>

// SBM Algorithmic layer includes
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
  DEFINE_STANDARD_RTTI(visu_pipeline, Standard_Transient)

public:

  ASITUS_EXPORTS
    visu_pipeline(const vtkSmartPointer<vtkMapper>& mapper,
                  const vtkSmartPointer<vtkActor>&  actor);

public:

  ASITUS_EXPORTS void
    set_input_data(const vtkSmartPointer<vtkDataSet>& data);

  ASITUS_EXPORTS void
    set_input_connection(vtkAlgorithmOutput* out);

public:

  ASITUS_EXPORTS virtual void
    update();

  ASITUS_EXPORTS virtual void
    build();

public:

  ASITUS_EXPORTS vtkActor*
    actor();

  ASITUS_EXPORTS vtkMapper*
    mapper();

  ASITUS_EXPORTS void
    add_to_renderer(vtkRenderer* renderer);

  ASITUS_EXPORTS void
    remove_from_renderer(vtkRenderer* renderer);

public:

  ASITUS_EXPORTS void
    modified();

  ASITUS_EXPORTS Handle(ActAux_TimeStamp)
    get_MTime() const;

// Pipeline construction routines to be used by derived classes only:
protected:

  ASITUS_EXPORTS void
    _disable(const int index);

  ASITUS_EXPORTS void
    _enable(const int index);

  ASITUS_EXPORTS void
    _append(const vtkSmartPointer<vtkAlgorithm>& filter);

  ASITUS_EXPORTS vtkSmartPointer<vtkAlgorithm>
    _filter(const int index) const;

  ASITUS_EXPORTS int
    _num_filters() const;

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
  vtkSmartPointer<vtkPassThroughFilter> m_dummy_filter;

  //! Modification timestamp for the Pipeline.
  Handle(ActAux_TimeStamp) m_MTime;

};

#endif
