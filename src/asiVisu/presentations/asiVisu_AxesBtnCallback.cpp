//-----------------------------------------------------------------------------
// Created on: 19 May 2016
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
