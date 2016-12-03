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
asiUI_RotationCenterProvider::asiUI_RotationCenterProvider() : asiVisu_MeshDataProvider()
{
  this->Init(0.0, 0.0, 0.0, 1.0);
}

//! Builds a new mesh around the given center point,
//! \param x    [in] x coordinate of the center point.
//! \param y    [in] y coordinate of the center point.
//! \param z    [in] z coordinate of the center point.
//! \param side [in] side of the rotation marker.
void asiUI_RotationCenterProvider::Init(const double x,
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
//! \param pViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiUI_RotationCallback* asiUI_RotationCallback::New(asiUI_Viewer* pViewer)
{
  return new asiUI_RotationCallback(pViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param pViewer [in] owning viewer.
asiUI_RotationCallback::asiUI_RotationCallback(asiUI_Viewer* pViewer)
: asiUI_ViewerCallback(pViewer),
  m_bIsStarted(false)
{
  m_pl  = new asiVisu_MeshPipeline;
  m_prv = new asiUI_RotationCenterProvider;

  // Initialize world picker for center of rotation
  m_worldPicker = vtkSmartPointer<vtkWorldPointPicker>::New();
}

//! Destructor.
asiUI_RotationCallback::~asiUI_RotationCallback()
{}

//! Listens to a dedicated event. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void asiUI_RotationCallback::Execute(vtkObject*    asiVisu_NotUsed(pCaller),
                                     unsigned long eventId,
                                     void*         pCallData)
{
  /* =======================================
   *  Check if the calling context is valid
   * ======================================= */

  if ( eventId != EVENT_ROTATION_START && eventId != EVENT_ROTATION_END )
    return;

  /* ==========================
   *  Perform something useful
   * ========================== */

  if ( !this->GetViewer() )
    return;

  const vtkSmartPointer<asiVisu_PrsManager>& prsMgr   = this->GetViewer()->PrsMgr();
  vtkRenderer*                               renderer = prsMgr->GetRenderer();

  // Add/Remove anchor to/from renderer
  if ( eventId == EVENT_ROTATION_START )
  {
    if ( !m_bIsStarted )
      m_bIsStarted = true;
    else
      return;

    // Rotation center
    double X, Y, Z;

    // Pick position
    asiVisu_PickInput* pickInput = reinterpret_cast<asiVisu_PickInput*>(pCallData);
    //
    m_worldPicker->Pick(pickInput->Start.x(), pickInput->Start.y(), 0, renderer);
    {
      double coord[3];
      m_worldPicker->GetPickPosition(coord);
      //
      X = coord[0];
      Y = coord[1];
      Z = coord[2];

      // The idea is to change focal point of the camera to the picked position because the
      // focal point is the center of rotation. However, simply doing so is not enough as
      // the center will be automatically moved to the focal point. Therefore, it is necessary
      // to manipulate the center manually. We take the display coordinates of the picked point
      // and apply simple linear transformation with respect to the window's normalized
      // coordinates [1,-1]x[1,-1]

      int*   viewport = renderer->GetSize();
      int xDisplayMax = viewport[0];
      int yDisplayMax = viewport[1];
      //
      double xWindowCenter = -2.0 * ( (double) pickInput->Start.x() ) / xDisplayMax + 1;
      double yWindowCenter = -2.0 * ( (double) pickInput->Start.y() ) / yDisplayMax + 1;
      //
      vtkCamera* pCamera = renderer->GetActiveCamera();
      //
      pCamera->SetFocalPoint(X, Y, Z);
      pCamera->SetWindowCenter(xWindowCenter, yWindowCenter);
    }

    // Initialize Data Provider
    m_prv->Init( X, Y, Z, this->getScaledSize(renderer) );

    // Initialize pipeline
    m_pl->SetInput(m_prv);
    m_pl->Update();

    // Set visual properties
    this->setVisualProps();

    // Render
    m_pl->AddToRenderer(renderer);
  }
  else
  {
    m_bIsStarted = false;
    m_pl->RemoveFromRenderer(renderer);
  }

  this->GetViewer()->Repaint();
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
//! \param pRenderer [in] renderer instance.
//! \return radius.
double asiUI_RotationCallback::getScaledSize(vtkRenderer* pRenderer)
{
  vtkCamera* pCamera = pRenderer->GetActiveCamera();

  if ( !pCamera->GetParallelProjection() )
    return 0.0;

  const double scale        = pCamera->GetParallelScale();
  const double worldScale   = 8*scale;
  const int    windowHeight = pRenderer->GetSize()[1];
  double       resultScale;

  if ( windowHeight > 0 )
    resultScale = worldScale / windowHeight;
  else
    return 0.0;

  return resultScale;
}

