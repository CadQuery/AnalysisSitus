//-----------------------------------------------------------------------------
// Created on: 04 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_GeomSurfPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_FaceDataProvider.h>
#include <asiVisu_FaceSurfacePipeline.h>
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_ShadedSurfacePipeline.h>
#include <asiVisu_Utils.h>

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
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>

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
asiVisu_GeomSurfPrs::asiVisu_GeomSurfPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_Prs(theNode)
{
  Handle(asiData_SurfNode)
    face_n = Handle(asiData_SurfNode)::DownCast(theNode);

  // Create Data Provider
  Handle(asiVisu_FaceDataProvider) DP = new asiVisu_FaceDataProvider(face_n);

  //---------------------------------------------------------------------------
  // Surface isolines
  //---------------------------------------------------------------------------

  // Pipeline for surface isolines
  this->addPipeline        ( Pipeline_Main, new asiVisu_FaceSurfacePipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  //---------------------------------------------------------------------------
  // Surface in shaded view
  //---------------------------------------------------------------------------

  // Pipeline for surface in manually crafted shading
  Handle(asiVisu_ShadedSurfacePipeline) shaded_pl = new asiVisu_ShadedSurfacePipeline;
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
  asiVisu_Utils::InitTextWidget(m_textWidget);

  // Initialize scalar bar
  m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
  asiVisu_MeshResultUtils::InitScalarBarWidget(m_scalarBarWidget);

  // Button to switch between visualization modes
  m_modeButton   = vtkSmartPointer<vtkButtonWidget>::New();
  m_modeCallback = vtkSmartPointer<asiVisu_SurfBtnCallback>::New();
  //
  m_modeCallback->SetPresentation(this);
  m_modeButton->AddObserver(vtkCommand::StateChangedEvent, m_modeCallback);
}

//! Factory method for Presentation.
//! \param theNode [in] Surface Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_GeomSurfPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_GeomSurfPrs(theNode);
}

//! Returns true if the Presentation is visible, false -- otherwise.
//! \return true/false.
bool asiVisu_GeomSurfPrs::IsVisible() const
{
  return true;
}

//-----------------------------------------------------------------------------

//! Enables wireframe pipeline.
void asiVisu_GeomSurfPrs::DoWireframe()
{
  // Pipeline for wireframe surface
  Handle(asiVisu_FaceSurfacePipeline)
    wire_pl = Handle(asiVisu_FaceSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  //
  wire_pl->Actor()->SetVisibility(1);

  // Pipeline for shaded surface
  Handle(asiVisu_ShadedSurfacePipeline)
    shaded_pl = Handle(asiVisu_ShadedSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  shaded_pl->Actor()->SetVisibility(0);
}

//! Enables shaded pipeline.
void asiVisu_GeomSurfPrs::DoShaded()
{
  // Pipeline for wireframe surface
  Handle(asiVisu_FaceSurfacePipeline)
    wire_pl = Handle(asiVisu_FaceSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  //
  wire_pl->Actor()->SetVisibility(0);

  // Pipeline for shaded surface
  Handle(asiVisu_ShadedSurfacePipeline)
    shaded_pl = Handle(asiVisu_ShadedSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  shaded_pl->CurvatureOff();
  shaded_pl->ForceUpdate();
  shaded_pl->Actor()->SetVisibility(1);
}

//! Enables shaded pipeline with Gaussian curvature scalars.
void asiVisu_GeomSurfPrs::DoShadedAndGaussian()
{
  // Pipeline for wireframe surface
  Handle(asiVisu_FaceSurfacePipeline)
    wire_pl = Handle(asiVisu_FaceSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  //
  wire_pl->Actor()->SetVisibility(0);

  // Pipeline for shaded surface
  Handle(asiVisu_ShadedSurfacePipeline)
    shaded_pl = Handle(asiVisu_ShadedSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  shaded_pl->GaussianCurvatureOn();
  shaded_pl->ForceUpdate();
  shaded_pl->Actor()->SetVisibility(1);
}

//! Enables shaded pipeline with mean curvature scalars.
void asiVisu_GeomSurfPrs::DoShadedAndMean()
{
  // Pipeline for wireframe surface
  Handle(asiVisu_FaceSurfacePipeline)
    wire_pl = Handle(asiVisu_FaceSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Main) );
  //
  wire_pl->Actor()->SetVisibility(0);

  // Pipeline for shaded surface
  Handle(asiVisu_ShadedSurfacePipeline)
    shaded_pl = Handle(asiVisu_ShadedSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  shaded_pl->MeanCurvatureOn();
  shaded_pl->ForceUpdate();
  shaded_pl->Actor()->SetVisibility(1);
}

//! Checks whether the shaded pipeline is enabled or not.
//! \return true/false.
bool asiVisu_GeomSurfPrs::IsShaded() const
{
  // Pipeline for shaded surface
  Handle(asiVisu_ShadedSurfacePipeline)
    shaded_pl = Handle(asiVisu_ShadedSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  return shaded_pl->Actor()->GetVisibility() > 0;
}

//! Checks whether the shaded pipeline is enabled or not.
//! \return true/false.
bool asiVisu_GeomSurfPrs::IsShadedAndScalars() const
{
  // Pipeline for shaded surface
  Handle(asiVisu_ShadedSurfacePipeline)
    shaded_pl = Handle(asiVisu_ShadedSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );
  //
  return ( shaded_pl->Actor()->GetVisibility() > 0 ) &&
         ( shaded_pl->IsGaussianCurvature() || shaded_pl->IsMeanCurvature() );
}

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomSurfPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_GeomSurfPrs::afterInitPipelines()
{
  Handle(asiVisu_FaceDataProvider)
    DP = Handle(asiVisu_FaceDataProvider)::DownCast( this->dataProvider(Pipeline_Main) );

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
  else if ( !S.IsNull() && S->IsInstance( STANDARD_TYPE(Geom_CylindricalSurface) ) )
  {
    Handle(Geom_CylindricalSurface) CS = Handle(Geom_CylindricalSurface)::DownCast(S);
    const double r = CS->Radius();
    TITLE += "\nRadius: "; TITLE += r;
  }
  else if ( !S.IsNull() && S->IsInstance( STANDARD_TYPE(Geom_SphericalSurface) ) )
  {
    Handle(Geom_SphericalSurface) SS = Handle(Geom_SphericalSurface)::DownCast(S);
    const double r = SS->Radius();
    TITLE += "\nRadius: "; TITLE += r;
  }
  else if ( !S.IsNull() && S->IsInstance( STANDARD_TYPE(Geom_ToroidalSurface) ) )
  {
    Handle(Geom_ToroidalSurface) TS = Handle(Geom_ToroidalSurface)::DownCast(S);
    const double minr = TS->MinorRadius();
    const double majr = TS->MajorRadius();
    TITLE += "\nMinor radius: "; TITLE += minr;
    TITLE += "\nMajor radius: "; TITLE += majr;
  }

  // Update text on the annotation
  m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_GeomSurfPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_GeomSurfPrs::afterUpdatePipelines() const
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
    Handle(asiVisu_ShadedSurfacePipeline)
      shaded_pl = Handle(asiVisu_ShadedSurfacePipeline)::DownCast( this->GetPipeline(Pipeline_Shaded) );

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
void asiVisu_GeomSurfPrs::highlight(vtkRenderer*                  theRenderer,
                                    const asiVisu_PickResult&     thePickRes,
                                    const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(thePickRes);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for highlighting reset.
//! \param theRenderer [in] renderer.
void asiVisu_GeomSurfPrs::unHighlight(vtkRenderer*                  theRenderer,
                                      const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for rendering.
//! \param theRenderer [in] renderer.
void asiVisu_GeomSurfPrs::renderPipelines(vtkRenderer* theRenderer) const
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
void asiVisu_GeomSurfPrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);

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
asiVisu_SurfBtnCallback* asiVisu_SurfBtnCallback::New()
{
  return new asiVisu_SurfBtnCallback();
}

//! Callback for button clicking.
void asiVisu_SurfBtnCallback::Execute(vtkObject* theCaller,
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
