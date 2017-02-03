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

      if ( r == WHITE_COMPONENT && g == WHITE_COMPONENT && b == WHITE_COMPONENT )
      {
        m_manager->ActorColorRed   = BLACK_COMPONENT;
        m_manager->ActorColorGreen = BLACK_COMPONENT;
        m_manager->ActorColorBlue  = BLACK_COMPONENT;
      }
      else if ( r == BLACK_COMPONENT && g == BLACK_COMPONENT && b == BLACK_COMPONENT )
      {
        m_manager->ActorColorRed   = WHITE_COMPONENT;
        m_manager->ActorColorGreen = WHITE_COMPONENT;
        m_manager->ActorColorBlue  = WHITE_COMPONENT;
      }
      else
        continue;

      prop->SetColor(m_manager->ActorColorRed,
                     m_manager->ActorColorGreen,
                     m_manager->ActorColorBlue);
    }
  }
}
