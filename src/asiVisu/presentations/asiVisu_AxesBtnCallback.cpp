//-----------------------------------------------------------------------------
// Created on: 19 May 2016
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiVisu_AxesBtnCallback.h>

// VTK includes
#include <vtkProperty.h>
#include <vtkTexturedButtonRepresentation2D.h>

//-----------------------------------------------------------------------------

//! Instantiation routine.
//! \return instance of the callback class.
asiVisu_AxesBtnCallback* asiVisu_AxesBtnCallback::New()
{
  return new asiVisu_AxesBtnCallback();
}

//-----------------------------------------------------------------------------

//! Callback for button clicking.
void asiVisu_AxesBtnCallback::Execute(vtkObject* caller,
                                      unsigned long,
                                      void*)
{
  vtkButtonWidget*                   pBtn  = vtkButtonWidget::SafeDownCast(caller);
  vtkTexturedButtonRepresentation2D* pRepr = vtkTexturedButtonRepresentation2D::SafeDownCast( pBtn->GetRepresentation() );
  //
  const int state = pRepr->GetState();

  if ( state == 1 )
  {
    m_axes->VisibilityOff();
  }
  else if ( state == 2 )
  {
    m_renderer->SetGradientBackground(false);
    m_renderer->SetBackground(1.0, 1.0, 1.0);
    //
    this->colorizeActors();
    //
    m_manager->isWhiteBackground = true;
  }
  else if ( state == 3 )
  {
    m_axes->VisibilityOn();
  }
  else
  {
    m_renderer->SetGradientBackground(false);
    m_renderer->SetBackground(0.15, 0.15, 0.15);
    //
    this->colorizeActors();
    //
    m_manager->isWhiteBackground = false;
  }
}

//-----------------------------------------------------------------------------

void asiVisu_AxesBtnCallback::colorizeActors()
{
  vtkActorCollection* actorCollection = m_renderer->GetActors();
  if ( actorCollection && actorCollection->GetNumberOfItems() > 0 )
  {
    actorCollection->InitTraversal();
    while ( vtkActor* actor = actorCollection->GetNextActor() )
    {
      vtkProperty* prop = actor->GetProperty();

      double r, g, b;
      prop->GetColor(r, g, b);

      // If actor is white...
      if ( Abs(r - m_manager->WhiteIntensity) < 1.0e-6 &&
           Abs(g - m_manager->WhiteIntensity) < 1.0e-6 &&
           Abs(b - m_manager->WhiteIntensity) < 1.0e-6 )
      {
        // ... it means the background is black, so we switch to white
        prop->SetColor(m_manager->BlackIntensity,
                       m_manager->BlackIntensity,
                       m_manager->BlackIntensity);
      }

      // If actor is black...
      else if ( Abs(r - m_manager->BlackIntensity) < 1.0e-6 &&
                Abs(g - m_manager->BlackIntensity) < 1.0e-6 &&
                Abs(b - m_manager->BlackIntensity) < 1.0e-6 )
      {
        // ... it means the background is white, so we switch to black
        prop->SetColor(m_manager->WhiteIntensity,
                       m_manager->WhiteIntensity,
                       m_manager->WhiteIntensity);
      }
    }
  }
}
