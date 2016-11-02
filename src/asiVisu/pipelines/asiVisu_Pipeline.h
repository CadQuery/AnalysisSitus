//-----------------------------------------------------------------------------
// Created on: 25 September 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiVisu_Pipeline_h
#define asiVisu_Pipeline_h

// A-Situs includes
#include <asiVisu.h>

// Visualization includes
#include <asiVisu_DataProvider.h>

// Active Data (auxiliary) includes
#include <ActAux_TimeStamp.h>

// VTK includes
#include <vtkAlgorithm.h>
#include <vtkPassThroughFilter.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_Pipeline, Standard_Transient)

//! Interface for any visualization pipeline in Analysis Situs. Each pipeline
//! represents a set of filters for the input dataset along with the
//! corresponding actor and mapper. The one-to-one correspondence between
//! the actor and the pipeline reflects the idea that a single actor is
//! responsible for a dedicated presentation rather than for a distinct object.
class asiVisu_Pipeline : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_Pipeline, Standard_Transient)

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& theDataProvider) = 0;

public:

  asiVisu_EXPORT void
    SetInputData(const vtkSmartPointer<vtkDataSet>& data);

  asiVisu_EXPORT void
    SetInputConnection(vtkAlgorithmOutput* out);

public:

  asiVisu_EXPORT virtual void
    Update();

  asiVisu_EXPORT virtual void
    Build();

public:

  asiVisu_EXPORT vtkActor*
    Actor();

  asiVisu_EXPORT vtkMapper*
    Mapper();

  asiVisu_EXPORT void
    AddToRenderer(vtkRenderer* renderer);

  asiVisu_EXPORT void
    RemoveFromRenderer(vtkRenderer* renderer);

public:

  asiVisu_EXPORT void
    Modified();

  asiVisu_EXPORT Handle(ActAux_TimeStamp)
    GetMTime() const;

protected:

  asiVisu_EXPORT
    asiVisu_Pipeline(const vtkSmartPointer<vtkMapper>& mapper,
                     const vtkSmartPointer<vtkActor>&  actor);

// Pipeline construction routines to be used by derived classes only:
protected:

  asiVisu_EXPORT void
    disable(const int index);

  asiVisu_EXPORT void
    enable(const int index);

  asiVisu_EXPORT void
    append(const vtkSmartPointer<vtkAlgorithm>& filter);

  asiVisu_EXPORT vtkSmartPointer<vtkAlgorithm>
    filter(const int index) const;

  asiVisu_EXPORT int
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
  asiVisu_Pipeline(const asiVisu_Pipeline&);

  //! Assignment prohibited.
  asiVisu_Pipeline& operator=(const asiVisu_Pipeline&);

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
typedef NCollection_Sequence<Handle(asiVisu_Pipeline)> asiVisu_PipelineList;
typedef NCollection_Shared<asiVisu_PipelineList>       asiVisu_HPipelineList;

#endif
