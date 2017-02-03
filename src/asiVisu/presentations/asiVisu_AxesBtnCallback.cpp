//-----------------------------------------------------------------------------
// Created on: 19 May 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
