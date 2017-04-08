//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_MeshPrs.h>

// Visualization includes
#include <asiVisu_DisplayMode.h>
#include <asiVisu_MeshContourPipeline.h>
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Convert integer value to a color.
//! \param theColor [in] integer value.
//! \return converted value
static QColor IntToColor(const int theColor)
{
  unsigned char aRed   = ( theColor >> 16 ) & 0xFF;
  unsigned char aGreen = ( theColor >>  8 ) & 0xFF;
  unsigned char aBlue  =  theColor          & 0xFF;
  return QColor(aRed, aGreen, aBlue);
}

//! Creates a Presentation object for the passed Mesh Node.
//! \param theNode [in] Mesh Node to create a Presentation for.
asiVisu_MeshPrs::asiVisu_MeshPrs(const Handle(ActAPI_INode)& theNode) : asiVisu_Prs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_MeshDataProvider)
    DP = new asiVisu_MeshDataProvider( theNode->GetId(),
                                       ActAPI_ParameterStream() << theNode->Parameter(asiData_TessNode::PID_Mesh) );

  // Pipeline for mesh
  this->addPipeline(Pipeline_Mesh, new asiVisu_MeshPipeline);
  this->assignDataProvider(Pipeline_Mesh, DP);

  // Pipeline for mesh contour
  this->addPipeline(Pipeline_MeshContour, new asiVisu_MeshContourPipeline);
  this->assignDataProvider(Pipeline_MeshContour, DP);
  //
  this->GetPipeline(Pipeline_MeshContour)->Actor()->GetProperty()->SetOpacity(0.5);
  this->GetPipeline(Pipeline_MeshContour)->Actor()->SetPickable(0);

  // We use CONTOUR mesh pipeline along with an ordinary one. Thus it is
  // really necessary to resolve coincident primitives to avoid blinking
  // on mesh edges
  vtkMapper::SetResolveCoincidentTopology(1);

  /* =====================================
   *  Prepare a pipeline for highlighting
   * ===================================== */

  Handle(asiVisu_MeshContourPipeline) aHiliPipeline = new asiVisu_MeshContourPipeline();

  // Set color, opacity and line width
  double aHiliColor[3];
  asiVisu_Utils::DefaultDetectionColor(aHiliColor[0], aHiliColor[1], aHiliColor[2]);
  aHiliPipeline->Actor()->GetProperty()->SetColor(aHiliColor[0], aHiliColor[1], aHiliColor[2]);
  aHiliPipeline->Actor()->GetProperty()->SetOpacity(0.75);
  aHiliPipeline->Actor()->GetProperty()->SetLineWidth( asiVisu_Utils::DefaultDetectionLineWidth() );

  // Set picking pipeline
  this->installDetectPipeline( aHiliPipeline, DP->Clone() );
}

//! Factory method for Node's Presentation.
//! \param theNode [in] Mesh Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_MeshPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_MeshPrs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_MeshPrs::IsVisible() const
{
  return true; // TODO: make visibility controllable
}

//! Sets SHADING visualization mode.
void asiVisu_MeshPrs::doShading() const
{
  Handle(asiVisu_MeshPipeline)
    aMeshPL = Handle(asiVisu_MeshPipeline)::DownCast( this->GetPipeline(Pipeline_Mesh) );
  //
  Handle(asiVisu_MeshPipeline)
    aMeshContourPL = Handle(asiVisu_MeshPipeline)::DownCast( this->GetPipeline(Pipeline_MeshContour) );

  aMeshPL->Actor()->GetProperty()->EdgeVisibilityOff();
  aMeshPL->Actor()->GetProperty()->SetRepresentationToSurface();
  aMeshContourPL->Actor()->VisibilityOn();
}

//! Sets WIREFRAME visualization mode.
void asiVisu_MeshPrs::doWireframe() const
{
  Handle(asiVisu_MeshPipeline) aMeshPL =
    Handle(asiVisu_MeshPipeline)::DownCast( this->GetPipeline(Pipeline_Mesh) );
  Handle(asiVisu_MeshPipeline) aMeshContourPL =
    Handle(asiVisu_MeshPipeline)::DownCast( this->GetPipeline(Pipeline_MeshContour) );

  aMeshPL->Actor()->GetProperty()->EdgeVisibilityOn();
  aMeshPL->Actor()->GetProperty()->SetRepresentationToWireframe();
  aMeshContourPL->Actor()->VisibilityOff();
}

//! Sets custom color for the Mesh.
//! \param theColor [in] color to set.
void asiVisu_MeshPrs::doColor(const QColor& theColor) const
{
  if ( !theColor.isValid() )
    return;

  Handle(asiVisu_MeshPipeline) aMeshPL =
    Handle(asiVisu_MeshPipeline)::DownCast( this->GetPipeline(Pipeline_Mesh) );

  aMeshPL->Mapper()->ScalarVisibilityOff();
  aMeshPL->Actor()->GetProperty()->SetColor( theColor.redF(),
                                             theColor.greenF(),
                                             theColor.blueF() );
}

//! Unsets custom color for the Mesh.
void asiVisu_MeshPrs::doUnColor() const
{
  Handle(asiVisu_MeshPipeline) aMeshPL =
    Handle(asiVisu_MeshPipeline)::DownCast( this->GetPipeline(Pipeline_Mesh) );

  aMeshPL->Mapper()->ScalarVisibilityOn();
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_MeshPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_MeshPrs::afterInitPipelines()
{
  // Access a dedicated pipeline for highlighting
  const Handle(asiVisu_MeshContourPipeline)& detect_pl =
    Handle(asiVisu_MeshContourPipeline)::DownCast( this->GetDetectPipeline() );

  // Initialize the pipeline's input
  detect_pl->SetInput( this->dataProvider(Pipeline_Mesh) );
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_MeshPrs::beforeUpdatePipelines() const
{
  Handle(asiData_TessNode) Mesh_Node = Handle(asiData_TessNode)::DownCast( this->GetNode() );

  asiVisu_DisplayMode aDMode = (asiVisu_DisplayMode) Mesh_Node->GetDisplayMode();
  if ( aDMode == DisplayMode_Undefined || aDMode == DisplayMode_Shaded )
    this->doShading();
  else if ( aDMode == DisplayMode_Wireframe )
    this->doWireframe();
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_MeshPrs::afterUpdatePipelines() const
{
  /* ======================================
   *  Update highlighting pipeline as well
   * ====================================== */

  // Access a dedicated pipeline for highlighting
  const Handle(asiVisu_MeshContourPipeline)& detect_pl =
    Handle(asiVisu_MeshContourPipeline)::DownCast( this->GetDetectPipeline() );

  // IMPORTANT: We update our highlighting pipeline here just to make things
  // faster. The better place to do that is "highlight" method, because
  // we do not really need to build highlighting pipeline just after
  // the Nodal Presentation is created. Logically, we would better to prepare
  // this pipeline only on actual pick request from user. However, in the
  // latter case the reactivity of application might significantly slow down
  detect_pl->Update();

  /* =================
   *  Actualize color
   * ================= */

  Handle(asiData_TessNode) Mesh_Node = Handle(asiData_TessNode)::DownCast( this->GetNode() );
  if ( Mesh_Node->HasColor() )
  {
    QColor aColor = ::IntToColor( Mesh_Node->GetColor() );
    this->doColor(aColor);
  }
  else
    this->doUnColor();
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection kind.
void asiVisu_MeshPrs::highlight(vtkRenderer*                  theRenderer,
                                const asiVisu_PickResult&     thePickRes,
                                const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(thePickRes);

  //---------------------------------------------------------------------------
  // Update highlighting pipelines
  //---------------------------------------------------------------------------

  // Access pipeline for highlighting
  Handle(asiVisu_MeshContourPipeline) hili_pl;
  //
  if ( theSelNature == SelectionNature_Pick )
    hili_pl = Handle(asiVisu_MeshContourPipeline)::DownCast( this->GetPickPipeline() );
  else
    hili_pl = Handle(asiVisu_MeshContourPipeline)::DownCast( this->GetDetectPipeline() );

  if ( !hili_pl )
    return;

  // Set selection mask...
  //hili_pl->SetSelectedNode(cellIds);
  //hili_pl->ForceExecution();
  hili_pl->SetInput( this->dataProviderDetect() );

  // ... and visibility
  //hili_pl->Actor()->SetVisibility( !cellIds.IsEmpty() );
}

//! Callback for un-highlighting.
//! \param theRenderer  [in] renderer.
//! \param theSelNature [in] selection kind.
void asiVisu_MeshPrs::unHighlight(vtkRenderer*                  theRenderer,
                                  const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);

  // Access pipeline for highlighting
  Handle(asiVisu_MeshContourPipeline) hili_pl;
  //
  if ( theSelNature == SelectionNature_Pick )
    hili_pl = Handle(asiVisu_MeshContourPipeline)::DownCast( this->GetPickPipeline() );
  else
    hili_pl = Handle(asiVisu_MeshContourPipeline)::DownCast( this->GetDetectPipeline() );

  if ( !hili_pl )
    return;

  // ... and visibility
  hili_pl->Actor()->SetVisibility(0);
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_MeshPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  //---------------------------------------------------------------------------
  // Highlighting
  //---------------------------------------------------------------------------

  const Handle(asiVisu_MeshContourPipeline)&
    detect_pl = Handle(asiVisu_MeshContourPipeline)::DownCast( this->GetDetectPipeline() );

  // Picking pipeline must be added to renderer the LAST (!). Otherwise
  // we experience some strange coloring bug because of their coincidence
  /* (1) */ detect_pl->AddToRenderer(theRenderer);
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void asiVisu_MeshPrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  //---------------------------------------------------------------------------
  // Highlighting
  //---------------------------------------------------------------------------

  Handle(asiVisu_MeshContourPipeline)
    detect_pl = Handle(asiVisu_MeshContourPipeline)::DownCast( this->GetDetectPipeline() );
  //
  detect_pl->RemoveFromRenderer(theRenderer);
}
