//-----------------------------------------------------------------------------
// Created on: 28 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_FuncLevelsPipeline_h
#define asiVisu_FuncLevelsPipeline_h

// asiVisu includes
#include <asiVisu_FuncDataProvider.h>
#include <asiVisu_FuncLevelsSource.h>
#include <asiVisu_Pipeline.h>

// VTK includes
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Visualization pipeline for function level sets.
template <typename T_VARIABLE>
class asiVisu_FuncLevelsPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_FuncLevelsPipeline, asiVisu_Pipeline)

public:

  asiVisu_FuncLevelsPipeline() : asiVisu_Pipeline( vtkSmartPointer<vtkPolyDataMapper>::New(),
                                                   vtkSmartPointer<vtkActor>::New() )
  {
    this->Actor()->GetProperty()->SetPointSize(3.0);
    this->Mapper()->ScalarVisibilityOn();
  }

public:

  virtual void SetInput(const Handle(asiVisu_DataProvider)& DP)
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
      src->SetInputFunction( func,
                             provider->GetMinCorner(),
                             provider->GetMaxCorner() );

      // Initialize pipeline
      this->SetInputConnection( src->GetOutputPort() );
    }

    // Update modification timestamp
    this->Modified();
  }

private:

  virtual void callback_add_to_renderer      (vtkRenderer*) {}
  virtual void callback_remove_from_renderer (vtkRenderer*) {}
  virtual void callback_update               ()             {}

private:

  //! Copying prohibited.
  asiVisu_FuncLevelsPipeline(const asiVisu_FuncLevelsPipeline&);

  //! Assignment prohibited.
  asiVisu_FuncLevelsPipeline& operator=(const asiVisu_FuncLevelsPipeline&);

};

#endif
