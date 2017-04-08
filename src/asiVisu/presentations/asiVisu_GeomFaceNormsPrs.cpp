//-----------------------------------------------------------------------------
// Created on: 02 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_GeomFaceNormsPrs.h>

// asiVisu includes
#include <asiVisu_FaceNormalsDataProvider.h>
#include <asiVisu_Utils.h>
#include <asiVisu_VectorsPipeline.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Node.
//! \param N [in] Node to create a Presentation for.
asiVisu_GeomFaceNormsPrs::asiVisu_GeomFaceNormsPrs(const Handle(ActAPI_INode)& N)
: asiVisu_Prs(N)
{
  Handle(asiData_FaceNormsNode) face_n = Handle(asiData_FaceNormsNode)::DownCast(N);

  // Create Data Provider
  Handle(asiVisu_FaceNormalsDataProvider) DP = new asiVisu_FaceNormalsDataProvider(face_n);

  // Pipeline for normals
  this->addPipeline        ( Pipeline_Main, new asiVisu_VectorsPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Disable scalar visibility as colors will correspond to orientation
  Handle(asiVisu_VectorsPipeline)
    PL = Handle(asiVisu_VectorsPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  //
  PL->Mapper()->ScalarVisibilityOff();
}

//! Factory method for Presentation.
//! \param N [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_GeomFaceNormsPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_GeomFaceNormsPrs(N);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_GeomFaceNormsPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomFaceNormsPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomFaceNormsPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_GeomFaceNormsPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_GeomFaceNormsPrs::afterUpdatePipelines() const
{
  Handle(asiVisu_FaceNormalsDataProvider)
    DP = Handle(asiVisu_FaceNormalsDataProvider)::DownCast( this->dataProvider(Pipeline_Main) );
  //
  Handle(asiVisu_VectorsPipeline)
    PL = Handle(asiVisu_VectorsPipeline)::DownCast( this->GetPipeline(Pipeline_Main) );

  TopoDS_Face face = DP->GetFace();

  // Set color according to orientation
  if ( face.Orientation() == TopAbs_FORWARD )
  {
    PL->Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
  }
  else if ( face.Orientation() == TopAbs_REVERSED )
  {
    PL->Actor()->GetProperty()->SetColor(0.0, 0.0, 1.0);
  }
  else
  {
    PL->Actor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
  }
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void asiVisu_GeomFaceNormsPrs::highlight(vtkRenderer*                  theRenderer,
                                         const asiVisu_PickResult&     thePickRes,
                                         const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(thePickRes);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_GeomFaceNormsPrs::unHighlight(vtkRenderer*                  theRenderer,
                                           const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomFaceNormsPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomFaceNormsPrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}
