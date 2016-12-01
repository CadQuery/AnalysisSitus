//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_PickCallback.h>

// A-Situs (GUI) includes
#include <asiUI_ViewerPart.h>

// A-Situs (visualization) includes
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

// VIS includes
#pragma warning(push, 0)
#include <IVtkTools_ShapePicker.hxx>
#pragma warning(pop)

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PickCallback* asiUI_PickCallback::New()
{
  return new asiUI_PickCallback(NULL);
}

//! Instantiation routine accepting viewer.
//! \param pViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiUI_PickCallback* asiUI_PickCallback::New(asiUI_Viewer* pViewer)
{
  return new asiUI_PickCallback(pViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param pViewer [in] owning viewer.
asiUI_PickCallback::asiUI_PickCallback(asiUI_Viewer* pViewer)
: asiUI_ViewerCallback(pViewer), m_pickType(PickType_World)
{}

//! Default destructor.
asiUI_PickCallback::~asiUI_PickCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void asiUI_PickCallback::Execute(vtkObject*    vtkNotUsed(pCaller),
                                 unsigned long eventId,
                                 void*         pCallData)
{
  vtkSmartPointer<asiVisu_PrsManager> mgr = this->GetViewer()->PrsMgr();

  // Check if the calling context is valid
  if ( eventId != EVENT_PICK_DEFAULT && eventId != EVENT_DETECT_DEFAULT )
    return;
  //
  if ( !this->GetViewer() )
    return;

  // Access selection context
  const int selMode = mgr->GetSelectionMode();

  // Skip for disabled selection
  if ( selMode & SelectionMode_None )
    return;

  // Do not allow detection on global selection
  if ( (selMode & SelectionMode_Workpiece) && eventId == EVENT_DETECT_DEFAULT )
    return;

  // Now pick
  asiVisu_PickInput* pickInput = reinterpret_cast<asiVisu_PickInput*>(pCallData);
  //
  const asiVisu_SelectionNature sel_type = (eventId == EVENT_PICK_DEFAULT) ? SelectionNature_Pick
                                                                           : SelectionNature_Detection;
  mgr->Pick(pickInput, sel_type, m_pickType);

  // Notify observers
  if ( eventId == EVENT_PICK_DEFAULT )
    emit picked();
}
