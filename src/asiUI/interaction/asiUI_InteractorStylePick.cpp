//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_InteractorStylePick.h>

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>

// Qt includes
#pragma warning(push, 0)
#include <QPoint>
#pragma warning(pop)

vtkStandardNewMacro(asiUI_InteractorStylePick);

//! Default constructor.
asiUI_InteractorStylePick::asiUI_InteractorStylePick()
: vtkInteractorStyleTrackballCamera(),
  m_bIsLeftButtonDown(false)
{
  m_pPickInput = new asiVisu_PickInput();
  //
  m_PickedPos[0] = m_PickedPos[1] = 0;
}

//! Default destructor.
asiUI_InteractorStylePick::~asiUI_InteractorStylePick() 
{
  delete m_pPickInput;
}

//! Prohibited copy constructor.
asiUI_InteractorStylePick::asiUI_InteractorStylePick(const asiUI_InteractorStylePick&)
: vtkInteractorStyleTrackballCamera(),
  m_pPickInput(NULL),
  m_bIsLeftButtonDown(false)
{
}

//! Prohibited assignment operator.
void asiUI_InteractorStylePick::operator=(const asiUI_InteractorStylePick&)
{
}

//! Sets Renderer instance.
//! \param theRenderer [in] Renderer instance to set.
void asiUI_InteractorStylePick::SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer)
{
  m_renderer = theRenderer;
}

//! Returns Renderer instance.
//! \return Renderer instance.
const vtkSmartPointer<vtkRenderer>& asiUI_InteractorStylePick::GetRenderer() const
{
  return m_renderer;
}

//! Callback for "Mouse Move" event.
void asiUI_InteractorStylePick::OnMouseMove()
{
  // Invoke basic method
  vtkInteractorStyleTrackballCamera::OnMouseMove();

  if ( m_bIsLeftButtonDown )
  {
    this->InvokeEvent(EVENT_ROTATION_START, NULL);
    return;
  }

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
void asiUI_InteractorStylePick::OnLeftButtonDown()
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
void asiUI_InteractorStylePick::OnLeftButtonUp()
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
void asiUI_InteractorStylePick::OnKeyPress()
{
  std::string key = this->Interactor->GetKeySym();
  if ( this->Interactor->GetControlKey() && key == "f" )
    this->InvokeEvent(EVENT_FIND_FACE);
}

//! Callback for rotation finishing action.
void asiUI_InteractorStylePick::EndRotate()
{
  vtkInteractorStyleTrackballCamera::EndRotate();
}

//! Adds custom callback.
//! \param theEventID  [in] ID of callback action.
//! \param theCallback [in] command to be added.
//! \return tag of the event (see vtkObject::AddObserver() method for VTK event tags).
unsigned long asiUI_InteractorStylePick::AddRotationCallback(unsigned long theEventID,
                                                               vtkCommand*   theCallback)
{
  m_rotationCallbackIds.Append(theEventID);
  return this->AddObserver(theEventID, theCallback);
}

//! Removes a callback with tag that is returned by AddRotationCallback() method.
//! \param theEventID [in] the event ID on which events are invoked
//! \param theTag     [in] the tag which is assigned to particular event entity.
//! \return true/false.
bool asiUI_InteractorStylePick::RemoveRotationCallback(unsigned long theEventID,
                                                         unsigned long theTag)
{
  if ( m_rotationCallbackIds.IsEmpty() )
    return false;

  this->RemoveObserver( theTag );

  if ( !this->HasObserver(theEventID) )
    m_rotationCallbackIds.Remove( theEventID );

  return true;
}
