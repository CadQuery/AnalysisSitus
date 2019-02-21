//-----------------------------------------------------------------------------
// Created on: 28 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_RotationCallback.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiUI includes
#include <asiUI_ViewerPart.h>

// VTK includes
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#undef EXPERIMENTAL_ROTO_AROUND_POINT

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
  m_mesh = new ActData_Mesh;
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
}

//! Destructor.
asiUI_RotationCallback::~asiUI_RotationCallback()
{}

//! \todo rotation around custom rotation center is done incorrectly. This need to be rewritten.
//!
//! Listens to a dedicated event. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void asiUI_RotationCallback::Execute(vtkObject*    pCaller,
                                     unsigned long eventId,
                                     void*         pCallData)
{
  asiVisu_NotUsed(pCaller);
  asiVisu_NotUsed(pCallData);

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

    // Camera
    vtkCamera* pCamera = renderer->GetActiveCamera();
    pCamera->GetFocalPoint(X, Y, Z);

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

