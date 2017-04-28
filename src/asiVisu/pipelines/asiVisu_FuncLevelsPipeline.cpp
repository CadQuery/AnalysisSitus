//-----------------------------------------------------------------------------
// Created on: 28 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_FuncLevelsPipeline.h>

// asiVisu includes
#include <asiVisu_FuncDataProvider.h>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates new Pipeline initialized by default VTK mapper and actor.
template <typename T_VARIABLE>
asiVisu_FuncLevelsPipeline<T_VARIABLE>::asiVisu_FuncLevelsPipeline()
: asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                    vtkSmartPointer<vtkActor>::New() )
{
  this->Actor()->GetProperty()->SetPointSize(3.0);
  this->Mapper()->ScalarVisibilityOn();
}

//-----------------------------------------------------------------------------

//! Sets input data for the pipeline.
//! \param DP [in] Data Provider.
template <typename T_VARIABLE>
void asiVisu_FuncLevelsPipeline<T_VARIABLE>::SetInput(const Handle(asiVisu_DataProvider)& DP)
{
  const Handle(asiVisu_FuncDataProvider<T_VARIABLE>)&
    provider = Handle(asiVisu_FuncDataProvider<T_VARIABLE>)::DownCast(DP);
  //
  if ( provider.IsNull() )
    return;

  /* ===========================
   *  Validate input Parameters
   * =========================== */

  Handle(asiAlgo_Function<T_VARIABLE>) func = provider->GetFunction();
  //
  if ( func.IsNull() )
  {
    // Pass empty data set in order to have valid pipeline
    vtkSmartPointer<vtkPolyData> aDummyDS = vtkSmartPointer<vtkPolyData>::New();
    this->SetInputData(aDummyDS);
    this->Modified(); // Update modification timestamp
    return; // Do nothing
  }

  /* ============================
   *  Prepare polygonal data set
   * ============================ */

  if ( provider->MustExecute( this->GetMTime() ) )
  {
    // Source for points
    vtkSmartPointer< asiVisu_FuncLevelsSource<T_VARIABLE> >
      src = vtkSmartPointer< asiVisu_FuncLevelsSource<T_VARIABLE> >::New();
    //
    src->SetInputFunction(func);

    // Initialize pipeline
    this->SetInputConnection( src->GetOutputPort() );
  }

  // Update modification timestamp
  this->Modified();
}

//-----------------------------------------------------------------------------

//! Callback for AddToRenderer() routine. Good place to adjust visualization
//! properties of the pipeline's actor.
template <typename T_VARIABLE>
void asiVisu_FuncLevelsPipeline<T_VARIABLE>::callback_add_to_renderer(vtkRenderer*)
{}

//! Callback for RemoveFromRenderer() routine.
template <typename T_VARIABLE>
void asiVisu_FuncLevelsPipeline<T_VARIABLE>::callback_remove_from_renderer(vtkRenderer*)
{}

//! Callback for Update() routine.
template <typename T_VARIABLE>
void asiVisu_FuncLevelsPipeline<T_VARIABLE>::callback_update()
{}
