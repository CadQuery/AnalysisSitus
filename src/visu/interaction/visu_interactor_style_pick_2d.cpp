//-----------------------------------------------------------------------------
// Created on: 17 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_interactor_style_pick_2d.h>

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>

// Qt includes
#pragma warning(push, 0)
#include <QPoint>
#pragma warning(pop)

vtkStandardNewMacro(visu_interactor_style_pick_2d);

//! Default constructor.
visu_interactor_style_pick_2d::visu_interactor_style_pick_2d()
: vtkInteractorStyleImage()
{
  m_pPickInput = new visu_pick_input();
}

//! Default destructor.
visu_interactor_style_pick_2d::~visu_interactor_style_pick_2d() 
{
  delete m_pPickInput;
}

//! Prohibited copy constructor.
visu_interactor_style_pick_2d::visu_interactor_style_pick_2d(const visu_interactor_style_pick_2d&)
: vtkInteractorStyleImage(),
  m_pPickInput(NULL)
{
}

//! Prohibited assignment operator.
void visu_interactor_style_pick_2d::operator=(const visu_interactor_style_pick_2d&)
{
}

//! Sets Renderer instance.
//! \param theRenderer [in] Renderer instance to set.
void visu_interactor_style_pick_2d::SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer)
{
  m_renderer = theRenderer;
}

//! Returns Renderer instance.
//! \return Renderer instance.
vtkSmartPointer<vtkRenderer> visu_interactor_style_pick_2d::GetRenderer() const
{
  return m_renderer;
}

//! Callback for "Mouse Move" event.
void visu_interactor_style_pick_2d::OnMouseMove()
{
  // Invoke basic method
  vtkInteractorStyleImage::OnMouseMove();

  const int pos[2] = { this->Interactor->GetEventPosition()[0],
                       this->Interactor->GetEventPosition()[1] };

  QPoint aPickPoint(pos[0], pos[1]);
  m_pPickInput->Start      = aPickPoint;
  m_pPickInput->Finish     = aPickPoint;
  m_pPickInput->IsMultiple = false;

  // Invoke observers
  this->InvokeEvent(EVENT_DETECT_DEFAULT, m_pPickInput);
}

//! Callback for "Left Button Down" event.
void visu_interactor_style_pick_2d::OnLeftButtonDown()
{
  this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                          this->Interactor->GetEventPosition()[1]);

  if ( this->CurrentRenderer == NULL )
    return;

  const int pos[2] = { this->Interactor->GetEventPosition()[0],
                       this->Interactor->GetEventPosition()[1] };
  //
  QPoint aPickPoint(pos[0], pos[1]);
  m_pPickInput->Start  = aPickPoint;
  m_pPickInput->Finish = aPickPoint;

  if ( this->Interactor->GetShiftKey() )
    m_pPickInput->IsMultiple = true;
  else
    m_pPickInput->IsMultiple = false;

  // Invoke observers
  this->InvokeEvent(EVENT_PICK_DEFAULT, m_pPickInput);
}

//! Callback for "Key Press" event.
void visu_interactor_style_pick_2d::OnKeyPress()
{
  std::string key = this->Interactor->GetKeySym();
  if ( key == "Delete" )
    this->InvokeEvent(EVENT_DELETE);
  if ( key == "j" )
    this->InvokeEvent(EVENT_JOIN);
}
