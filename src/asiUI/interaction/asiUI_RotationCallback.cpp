//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_RotationCallback.h>

// A-Situs (visualization) includes
#include <asiVisu_Utils.h>

// A-Situs (GUI) includes
#include <asiUI_ViewerPart.h>

// A-Situs (common) includes
#include <common_facilities.h>

// VTK includes
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

//! Constructor.
asiVisu_RotationCenterCallback::asiVisu_RotationCenterCallback() : asiVisu_MeshDataProvider()
{
  this->Init(0.0, 0.0, 0.0, 1.0);
}

//! Builds a new mesh around the given center point,
//! \param x    [in] x coordinate of the center point.
//! \param y    [in] y coordinate of the center point.
//! \param z    [in] z coordinate of the center point.
//! \param side [in] side of the rotation marker.
void asiVisu_RotationCenterCallback::Init(const double x,
                                         const double y,
                                         const double z,
                                         const double side)
{
  m_mesh = new Mesh;
  const int vertices[] = { m_mesh->AddNode(x + side, y - side, z - side),
                           m_mesh->AddNode(x + side, y + side, z - side),
                           m_mesh->AddNode(x - side, y + side, z - side),
                           m_mesh->AddNode(x - side, y - side, z - side),
                           //
                           m_mesh->AddNode(x + side, y - side, z + side),
                           m_mesh->AddNode(x + side, y + side, z + side),
                           m_mesh->AddNode(x - side, y + side, z + side),
                           m_mesh->AddNode(x - side, y - side, z + side) };

  m_mesh->AddFace(vertices[0], vertices[1], vertices[2], vertices[3]);
  m_mesh->AddFace(vertices[4], vertices[5], vertices[6], vertices[7]);
  m_mesh->AddFace(vertices[0], vertices[4], vertices[5], vertices[1]);
  m_mesh->AddFace(vertices[1], vertices[5], vertices[6], vertices[2]);
  m_mesh->AddFace(vertices[2], vertices[6], vertices[7], vertices[3]);
  m_mesh->AddFace(vertices[3], vertices[7], vertices[4], vertices[0]);
}

//-----------------------------------------------------------------------------
// Rotation callback
//-----------------------------------------------------------------------------

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_RotationCallback* asiUI_RotationCallback::New()
{
  return new asiUI_RotationCallback(NULL);
}

//! Instantiation routine accepting viewer.
//! \param theViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiUI_RotationCallback* asiUI_RotationCallback::New(asiUI_Viewer* theViewer)
{
  return new asiUI_RotationCallback(theViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param theViewer [in] owning viewer.
asiUI_RotationCallback::asiUI_RotationCallback(asiUI_Viewer* theViewer)
: asiVisu_ViewerCallback(theViewer),
  m_bIsStarted(false)
{
  m_pl  = new asiVisu_MeshPipeline;
  m_prv = new asiVisu_RotationCenterCallback;
}

//! Destructor.
asiUI_RotationCallback::~asiUI_RotationCallback()
{}

//! Listens to a dedicated event. Performs all useful operations.
//! \param theCaller   [in] caller instance.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void asiUI_RotationCallback::Execute(vtkObject*    asiVisu_NotUsed(theCaller),
                                     unsigned long theEventId,
                                     void*         asiVisu_NotUsed(theCallData))
{
  /* =======================================
   *  Check if the calling context is valid
   * ======================================= */

  if ( theEventId != EVENT_ROTATION_START && theEventId != EVENT_ROTATION_END )
    return;

  /* ==========================
   *  Perform something useful
   * ========================== */

  if ( !this->Viewer() )
    return;

  if ( !common_facilities::Instance()->Prs.Part )
    return;

  vtkRenderer* aRenderer = common_facilities::Instance()->Prs.Part->GetRenderer();

  // Add/Remove anchor to/from renderer
  if ( theEventId == EVENT_ROTATION_START )
  {
    if ( !m_bIsStarted )
      m_bIsStarted = true;
    else
      return;

    double X, Y, Z;
    vtkCamera* aCamera = aRenderer->GetActiveCamera();
    aCamera->GetFocalPoint(X, Y, Z);

    // Initialize Data Provider
    m_prv->Init( X, Y, Z, this->getScaledSize(aRenderer) );

    // Initialize pipeline
    m_pl->SetInput(m_prv);
    m_pl->Update();

    // Set visual properties
    this->setVisualProps();

    // Render
    m_pl->AddToRenderer(aRenderer);
  }
  else
  {
    m_bIsStarted = false;
    m_pl->RemoveFromRenderer(aRenderer);
  }

  this->Viewer()->Repaint();
}

//! Sets visualization properties for rotation anchor ball.
void asiUI_RotationCallback::setVisualProps()
{
  // Apply lighting rules
  m_pl->Actor()->GetProperty()->SetAmbient(0.3);
  m_pl->Actor()->GetProperty()->SetDiffuse(0.9);
  m_pl->Actor()->GetProperty()->SetSpecular(0.9);
  m_pl->Actor()->GetProperty()->SetSpecularPower(20);

  // Apply color
  m_pl->Actor()->GetProperty()->SetColor(1.0, 0.0, 0.0);
  m_pl->Actor()->GetProperty()->SetInterpolationToGouraud();
}


//! Returns size for rotation marker.
//! \param theRenderer [in] renderer instance.
//! \return radius.
double asiUI_RotationCallback::getScaledSize(vtkRenderer* theRenderer)
{
  vtkCamera* aCamera = theRenderer->GetActiveCamera();

  if ( !aCamera->GetParallelProjection() )
    return 0.0;

  double aScale        = aCamera->GetParallelScale();
  double aWorldScale   = 8*aScale;
  int    aWindowHeight = theRenderer->GetSize()[1];
  double aResultScale;

  if ( aWindowHeight > 0 )
    aResultScale = aWorldScale / aWindowHeight;
  else
    return 0.0;

  return aResultScale;
}

