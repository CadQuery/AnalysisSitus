//-----------------------------------------------------------------------------
// Created on: 19 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_axes_btn_callback.h>

// VTK includes
#include <vtkTexturedButtonRepresentation2D.h>

//! Instantiation routine.
//! \return instance of the callback class.
visu_axes_btn_callback* visu_axes_btn_callback::New()
{
  return new visu_axes_btn_callback();
}

//! Callback for button clicking.
void visu_axes_btn_callback::Execute(vtkObject* caller,
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
  }
  else if ( state == 3 )
  {
    m_axes->VisibilityOn();
  }
  else
  {
    m_renderer->SetBackground(0.15, 0.15, 0.15);
  }
}
