//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_interactor_style_pick.h>

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>

// Qt includes
#pragma warning(push, 0)
#include <QPoint>
#pragma warning(pop)

vtkStandardNewMacro(visu_interactor_style_pick);

//! Default constructor.
visu_interactor_style_pick::visu_interactor_style_pick()
: vtkInteractorStyleTrackballCamera(),
  m_bIsLeftButtonDown(false)
{
  m_pPickInput = new visu_pick_input();
  //
  m_PickedPos[0] = m_PickedPos[1] = 0;
}

//! Default destructor.
visu_interactor_style_pick::~visu_interactor_style_pick() 
{
  delete m_pPickInput;
}

//! Prohibited copy constructor.
visu_interactor_style_pick::visu_interactor_style_pick(const visu_interactor_style_pick&)
: vtkInteractorStyleTrackballCamera(),
  m_pPickInput(NULL),
  m_bIsLeftButtonDown(false)
{
}

//! Prohibited assignment operator.
void visu_interactor_style_pick::operator=(const visu_interactor_style_pick&)
{
}

//! Sets Renderer instance.
//! \param theRenderer [in] Renderer instance to set.
void visu_interactor_style_pick::SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer)
{
  m_renderer = theRenderer;
}

//! Returns Renderer instance.
//! \return Renderer instance.
vtkSmartPointer<vtkRenderer> visu_interactor_style_pick::GetRenderer() const
{
  return m_renderer;
}

//! Callback for "Mouse Move" event.
void visu_interactor_style_pick::OnMouseMove()
{
  // Invoke basic method
  vtkInteractorStyleTrackballCamera::OnMouseMove();

  if ( m_bIsLeftButtonDown )
    return;

  m_PickedPos[0] = this->Interactor->GetEventPosition()[0];
  m_PickedPos[1] = this->Interactor->GetEventPosition()[1];

  QPoint aPickPoint(m_PickedPos[0], m_PickedPos[1]);
  m_pPickInput->Start      = aPickPoint;
  m_pPickInput->Finish     = aPickPoint;
  m_pPickInput->IsMultiple = false;

  // Invoke observers
  this->InvokeEvent(EVENT_DETECT_DEFAULT, m_pPickInput);
  //
  if ( m_bIsLeftButtonDown && !this->Interactor->GetShiftKey() )
  {
    this->InvokeEvent(EVENT_ROTATION_START, NULL);
    //
    for ( NCollection_Sequence<unsigned long>::Iterator it(m_rotationCallbackIds);
          it.More(); it.Next() )
    {
      this->InvokeEvent( it.Value() );
    }
  }
}

//! Callback for "Left Button Down" event.
void visu_interactor_style_pick::OnLeftButtonDown()
{
  this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                          this->Interactor->GetEventPosition()[1]);

  if ( this->CurrentRenderer == NULL )
    return;

  m_bIsLeftButtonDown = true;

  m_PickedPos[0] = this->Interactor->GetEventPosition()[0];
  m_PickedPos[1] = this->Interactor->GetEventPosition()[1];

  this->StartRotate();
}

//! Callback for "Left Button Up" event.
void visu_interactor_style_pick::OnLeftButtonUp()
{
  vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
  m_bIsLeftButtonDown = false;

  const int pos[2] = { this->Interactor->GetEventPosition()[0],
                       this->Interactor->GetEventPosition()[1] };
  //
  if ( pos[0] == m_PickedPos[0] && pos[1] == m_PickedPos[1] )
  {
    QPoint aPickPoint(m_PickedPos[0], m_PickedPos[1]);
    m_pPickInput->Start  = aPickPoint;
    m_pPickInput->Finish = aPickPoint;

    if ( this->Interactor->GetShiftKey() )
    {
      m_pPickInput->IsMultiple = true;
    }
    else
    {
      m_pPickInput->IsMultiple = false;
    }

    // Invoke observers
    this->InvokeEvent(EVENT_PICK_DEFAULT, m_pPickInput);
  }
  else
  {
    m_PickedPos[0] = pos[0];
    m_PickedPos[1] = pos[1];
  }

  // Invoke observers
  this->EndRotate();
  this->InvokeEvent(EVENT_ROTATION_END, NULL);
}

//! Callback for "Key Press" event.
void visu_interactor_style_pick::OnKeyPress()
{
  std::string key = this->Interactor->GetKeySym();
  if ( this->Interactor->GetControlKey() && key == "f" )
    this->InvokeEvent(EVENT_FIND_FACE);
}

//! Callback for rotation finishing action.
void visu_interactor_style_pick::EndRotate()
{
  vtkInteractorStyleTrackballCamera::EndRotate();
}

//! Adds custom callback.
//! \param theEventID  [in] ID of callback action.
//! \param theCallback [in] command to be added.
//! \return tag of the event (see vtkObject::AddObserver() method for VTK event tags).
unsigned long visu_interactor_style_pick::AddRotationCallback(unsigned long theEventID,
                                                              vtkCommand* theCallback)
{
  m_rotationCallbackIds.Append(theEventID);
  return this->AddObserver(theEventID, theCallback);
}

//! Removes a callback with tag that is returned by AddRotationCallback() method.
//! \param theEventID [in] the event ID on which events are invoked
//! \param theTag     [in] the tag which is assigned to particular event entity.
//! \return true/false.
bool visu_interactor_style_pick::RemoveRotationCallback(unsigned long theEventID,
                                                        unsigned long theTag)
{
  if ( m_rotationCallbackIds.IsEmpty() )
    return false;

  this->RemoveObserver( theTag );

  if ( !this->HasObserver(theEventID) )
    m_rotationCallbackIds.Remove( theEventID );

  return true;
}
