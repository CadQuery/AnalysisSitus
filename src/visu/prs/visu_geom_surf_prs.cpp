//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_geom_surf_prs.h>

// A-Situs (visualization) includes
#include <visu_face_data_provider.h>
#include <visu_face_surface_pipeline.h>
#include <visu_mesh_result_utils.h>
#include <visu_shaded_surface_pipeline.h>
#include <visu_utils.h>

// VTK includes
#include <vtkCoordinate.h>
#include <vtkImageData.h>
#include <vtkMapper.h>
#include <vtkProperty.h>
#include <vtkTextActor.h>
#include <vtkTexturedButtonRepresentation2D.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

//-----------------------------------------------------------------------------

void PlaceButton(vtkButtonWidget* pButton,
                 vtkRenderer*     pRenderer)
{
  // Place the widget. Must be done after a render so that the viewport is
  // defined. Here the widget placement is in normalized display coordinates
  vtkSmartPointer<vtkCoordinate> upperRight = vtkSmartPointer<vtkCoordinate>::New();
  upperRight->SetCoordinateSystemToNormalizedDisplay();
  upperRight->SetValue(1.0, 1.0);

  const double displaySize[2] = { upperRight->GetComputedDisplayValue(pRenderer)[0],
                                  upperRight->GetComputedDisplayValue(pRenderer)[1] };

  double bds[6];
  const double size = 25.0;
  bds[0] = 3;
  bds[1] = bds[0] + size;
  bds[2] = displaySize[1] - size - 3;
  bds[3] = bds[2] + size;
  bds[4] = bds[5] = 0.0;

  // Scale to 1, default is .5
  pButton->GetRepresentation()->SetPlaceFactor(1);
  pButton->GetRepresentation()->PlaceWidget(bds);
}

//-----------------------------------------------------------------------------

void CreateImage(vtkSmartPointer<vtkImageData> image,
                 unsigned char*                color1,
                 unsigned char*                color2)
{
  // Specify the size of the image data
  image->SetDimensions(10, 10, 1);
  image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
  int* dims = image->GetDimensions();

  // Fill the image
  for ( int y = 0; y < dims[1]; ++y )
  {
    for ( int x = 0; x < dims[0]; ++x )
    {
      unsigned char*
        pixel = static_cast<unsigned char*>( image->GetScalarPointer(x, y, 0) );
      //
      if ( x < 5 )
      {
        pixel[0] = color1[0];
        pixel[1] = color1[1];
        pixel[2] = color1[2];
      }
      else
      {
        pixel[0] = color2[0];
        pixel[1] = color2[1];
        pixel[2] = color2[2];
      }
    }
  }
}

//-----------------------------------------------------------------------------

//! Creates a Presentation object for the passed Geometry Surface Node.
//! \param theNode [in] Geometry Surface Node to create a Presentation for.
visu_geom_surf_prs::visu_geom_surf_prs(const Handle(ActAPI_INode)& theNode)
: visu_prs(theNode)
{
  // Create Data Provider
  Handle(visu_face_data_provider) DP =
    new visu_face_data_provider( theNode->GetId(),
                                 ActParamStream() << theNode->Parameter(geom_surf_node::PID_SelectedFace) );

  //---------------------------------------------------------------------------
  // Surface isolines
  //---------------------------------------------------------------------------

  // Pipeline for surface isolines
  this->addPipeline        ( Pipeline_Main, new visu_face_surface_pipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  //---------------------------------------------------------------------------
  // Surface in shaded view
  //---------------------------------------------------------------------------

  // Pipeline for surface in manually crafted shading
  Handle(visu_shaded_surface_pipeline) shaded_pl = new visu_shaded_surface_pipeline;
  //
  this->addPipeline        ( Pipeline_Shaded, shaded_pl );
  this->assignDataProvider ( Pipeline_Shaded, DP );
  //
  shaded_pl->Actor()->SetVisibility(0);
  shaded_pl->SetStepsNumber(200);

  //---------------------------------------------------------------------------
  // Additional widgets
  //---------------------------------------------------------------------------

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_textWidget);

  // Initialize scalar bar
  m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
  visu_mesh_result_utils::InitScalarBarWidget(m_scalarBarWidget);

  // Button to switch between visualization modes
  m_modeButton = vtkSmartPointer<vtkButtonWidget>::New();
  m_modeCallback = vtkSmartPointer<visu_surf_btn_callback>::New();
  //
  m_modeCallback->SetPresentation(this);
  m_modeButton->AddObserver(vtkCommand::StateChangedEvent, m_modeCallback);
}

//! Factory method for Presentation.
//! \param theNode [in] Surface Node to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_prs) visu_geom_surf_prs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new visu_geom_surf_prs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool visu_geom_surf_prs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Enables wireframe pipeline.
void visu_geom_surf_prs::DoWireframe()
{
  // Pipeline for wireframe surface
  Handle(visu_face_surface_pipeline)
    wire_pl = Handle(visu_face_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  //
  wire_pl->Actor()->SetVisibility(1);

  // Pipeline for shaded surface
  Handle(visu_shaded_surface_pipeline)
    shaded_pl = Handle(visu_shaded_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  shaded_pl->Actor()->SetVisibility(0);
}

//! Enables shaded pipeline.
void visu_geom_surf_prs::DoShaded()
{
  // Pipeline for wireframe surface
  Handle(visu_face_surface_pipeline)
    wire_pl = Handle(visu_face_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  //
  wire_pl->Actor()->SetVisibility(0);

  // Pipeline for shaded surface
  Handle(visu_shaded_surface_pipeline)
    shaded_pl = Handle(visu_shaded_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  shaded_pl->CurvatureOff();
  shaded_pl->ForceUpdate();
  shaded_pl->Actor()->SetVisibility(1);
}

//! Enables shaded pipeline with Gaussian curvature scalars.
void visu_geom_surf_prs::DoShadedAndGaussian()
{
  // Pipeline for wireframe surface
  Handle(visu_face_surface_pipeline)
    wire_pl = Handle(visu_face_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  //
  wire_pl->Actor()->SetVisibility(0);

  // Pipeline for shaded surface
  Handle(visu_shaded_surface_pipeline)
    shaded_pl = Handle(visu_shaded_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  shaded_pl->GaussianCurvatureOn();
  shaded_pl->ForceUpdate();
  shaded_pl->Actor()->SetVisibility(1);
}

//! Enables shaded pipeline with mean curvature scalars.
void visu_geom_surf_prs::DoShadedAndMean()
{
  // Pipeline for wireframe surface
  Handle(visu_face_surface_pipeline)
    wire_pl = Handle(visu_face_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  //
  wire_pl->Actor()->SetVisibility(0);

  // Pipeline for shaded surface
  Handle(visu_shaded_surface_pipeline)
    shaded_pl = Handle(visu_shaded_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  shaded_pl->MeanCurvatureOn();
  shaded_pl->ForceUpdate();
  shaded_pl->Actor()->SetVisibility(1);
}

//! Checks whether the shaded pipeline is enabled or not.
//! \return true/false.
bool visu_geom_surf_prs::IsShaded() const
{
  // Pipeline for shaded surface
  Handle(visu_shaded_surface_pipeline)
    shaded_pl = Handle(visu_shaded_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  return shaded_pl->Actor()->GetVisibility() > 0;
}

//! Checks whether the shaded pipeline is enabled or not.
//! \return true/false.
bool visu_geom_surf_prs::IsShadedAndScalars() const
{
  // Pipeline for shaded surface
  Handle(visu_shaded_surface_pipeline)
    shaded_pl = Handle(visu_shaded_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  return ( shaded_pl->Actor()->GetVisibility() > 0 ) &&
         ( shaded_pl->IsGaussianCurvature() || shaded_pl->IsMeanCurvature() );
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void visu_geom_surf_prs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void visu_geom_surf_prs::afterInitPipelines()
{
  Handle(visu_face_data_provider)
    DP = Handle(visu_face_data_provider)::DownCast( this->dataProvider(Pipeline_Main) );

  // Get working face and its host surface
  const int            F_idx = DP->GetFaceIndexAmongFaces();
  TopoDS_Face          F     = DP->ExtractFace();
  Handle(Geom_Surface) S     = F.IsNull() ? NULL : BRep_Tool::Surface(F);

  // Prepare main title
  TCollection_AsciiString TITLE("Face (#");
  TITLE += F_idx;
  TITLE += "): ";
  TITLE += ( S.IsNull() ? "NONE" : S->DynamicType()->Name() );

  // Rectangular Trimmed Surface
  if ( !S.IsNull() && S->IsInstance( STANDARD_TYPE(Geom_RectangularTrimmedSurface) ) )
  {
    Handle(Geom_RectangularTrimmedSurface)
      RS = Handle(Geom_RectangularTrimmedSurface)::DownCast(S);

    // Let's check basis surface
    Handle(Geom_Surface) BS = RS->BasisSurface();
    //
    TITLE += "\nBasis surface: ";
    TITLE += ( BS.IsNull() ? "NONE" : BS->DynamicType()->Name() );
  }

  // B-surface
  if ( !S.IsNull() && S->IsInstance( STANDARD_TYPE(Geom_BSplineSurface) ) )
  {
    Handle(Geom_BSplineSurface) BS = Handle(Geom_BSplineSurface)::DownCast(S);

    // Get surface properties
    TITLE += "\nU degree: "; TITLE += BS->UDegree();
    TITLE += "\nV degree: "; TITLE += BS->VDegree();
    TITLE += "\nContinuity: ";
    //
    const GeomAbs_Shape cont = BS->Continuity();
    switch ( cont )
    {
      case GeomAbs_C0 : TITLE += "C0"; break;
      case GeomAbs_C1 : TITLE += "C1"; break;
      case GeomAbs_C2 : TITLE += "C2"; break;
      case GeomAbs_C3 : TITLE += "C3"; break;
      case GeomAbs_CN : TITLE += "CN"; break;
      case GeomAbs_G1 : TITLE += "G1"; break;
      case GeomAbs_G2 : TITLE += "G2"; break;
      default: break;
    }

    if ( BS->IsUPeriodic() )
      TITLE += "\nU-periodic";
    else
      TITLE += "\nNot U-periodic";

    if ( BS->IsVPeriodic() )
      TITLE += "\nV-periodic";
    else
      TITLE += "\nNot V-periodic";
  }

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void visu_geom_surf_prs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void visu_geom_surf_prs::afterUpdatePipelines() const
{
  // Scalar bar
  if ( this->IsShadedAndScalars() )
    m_scalarBarWidget->On();
  else
    m_scalarBarWidget->Off();

  // Actualize scalar bar
  if ( this->IsShadedAndScalars() )
  {
    // Pipeline for shaded surface
    Handle(visu_shaded_surface_pipeline)
      shaded_pl = Handle(visu_shaded_surface_pipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );

    // Initialize scalar bar Actor
    m_scalarBarWidget->GetScalarBarActor()->SetLookupTable( shaded_pl->Mapper()->GetLookupTable() );

    // Set suitable title
    vtkTexturedButtonRepresentation2D*
      pRepr = vtkTexturedButtonRepresentation2D::SafeDownCast( m_modeButton->GetRepresentation() );
    //
    const int state = pRepr->GetState();
    //
    if ( state == 0 )
      m_scalarBarWidget->GetScalarBarActor()->SetTitle("Isoparametric lines");
    else if ( state == 1 )
      m_scalarBarWidget->GetScalarBarActor()->SetTitle("Flat shading");
    else if ( state == 2 )
      m_scalarBarWidget->GetScalarBarActor()->SetTitle("Gaussian curvature");
    else if ( state == 3 )
      m_scalarBarWidget->GetScalarBarActor()->SetTitle("Mean curvature");
  }
}

//! Callback for highlighting.
//! \param theRenderer  [in] renderer.
//! \param thePickRes   [in] picking results.
//! \param theSelNature [in] selection nature (picking or detecting).
void visu_geom_surf_prs::highlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                   const visu_pick_result&      ASitus_NotUsed(thePickRes),
                                   const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void visu_geom_surf_prs::unHighlight(vtkRenderer*                 ASitus_NotUsed(theRenderer),
                                     const visu_selection_nature& ASitus_NotUsed(theSelNature)) const
{
  // Do nothing...
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void visu_geom_surf_prs::renderPipelines(vtkRenderer* theRenderer) const
{
  // Annotation
  if ( !m_textWidget->GetCurrentRenderer() )
  {
    m_textWidget->SetInteractor      ( theRenderer->GetRenderWindow()->GetInteractor() );
    m_textWidget->SetDefaultRenderer ( theRenderer );
    m_textWidget->SetCurrentRenderer ( theRenderer );
    m_textWidget->On                 ( );
  }

  // Scalar bar
  if ( !m_scalarBarWidget->GetCurrentRenderer() )
  {
    m_scalarBarWidget->SetInteractor( theRenderer->GetRenderWindow()->GetInteractor() );
    m_scalarBarWidget->SetDefaultRenderer(theRenderer);
    m_scalarBarWidget->SetCurrentRenderer(theRenderer);
  }

  /* ==============================================
   *  Button to switch between visualization modes
   * ============================================== */

  if ( !m_modeButton->GetEnabled() )
  {
    // Create images for textures
    vtkSmartPointer<vtkImageData> image1 = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkImageData> image2 = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkImageData> image3 = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkImageData> image4 = vtkSmartPointer<vtkImageData>::New();
    //
    unsigned char color1[3] = {255, 0,   0};
    unsigned char color2[3] = {0,   255, 0};
    unsigned char color3[3] = {0,   0,   255};
    unsigned char color4[3] = {0,   255, 255};
    //
    CreateImage(image1, color1, color1);
    CreateImage(image2, color2, color2);
    CreateImage(image3, color3, color3);
    CreateImage(image4, color4, color4);

    // Create the widget and its representation
    vtkSmartPointer<vtkTexturedButtonRepresentation2D>
      buttonRepresentation = vtkSmartPointer<vtkTexturedButtonRepresentation2D>::New();
    buttonRepresentation->SetNumberOfStates(4);
    buttonRepresentation->SetButtonTexture(0, image1);
    buttonRepresentation->SetButtonTexture(1, image2);
    buttonRepresentation->SetButtonTexture(2, image3);
    buttonRepresentation->SetButtonTexture(3, image4);

    m_modeButton->SetInteractor( theRenderer->GetRenderWindow()->GetInteractor() );
    m_modeButton->SetRepresentation(buttonRepresentation);
    //
    m_modeButton->On();
    //
    PlaceButton(m_modeButton, theRenderer);
  }
}

//! Callback for de-rendering.
//! \param theRenderer [in] renderer.
void visu_geom_surf_prs::deRenderPipelines(vtkRenderer* ASitus_NotUsed(theRenderer)) const
{
  m_textWidget->Off();
  //
  if ( this->IsShadedAndScalars() )
    m_scalarBarWidget->Off();
  //
  m_modeButton->Off();
}

//-----------------------------------------------------------------------------

//! Instantiation routine.
//! \return instance of the callback class.
visu_surf_btn_callback* visu_surf_btn_callback::New()
{
  return new visu_surf_btn_callback();
}

//! Callback for button clicking.
void visu_surf_btn_callback::Execute(vtkObject* theCaller,
                                     unsigned long,
                                     void*)
{
  if ( m_prs.IsNull() )
    return;

  vtkButtonWidget*                   pBtn  = vtkButtonWidget::SafeDownCast(theCaller);
  vtkTexturedButtonRepresentation2D* pRepr = vtkTexturedButtonRepresentation2D::SafeDownCast( pBtn->GetRepresentation() );
  //
  const int state = pRepr->GetState();

  if ( state == 1 )
    m_prs->DoShaded();
  else if ( state == 2 )
    m_prs->DoShadedAndGaussian();
  else if ( state == 3 )
    m_prs->DoShadedAndMean();
  else if ( state == 0 )
    m_prs->DoWireframe();

  // Update pipelines
  m_prs->InitPipelines();
  m_prs->UpdatePipelines();
}
