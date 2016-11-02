//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_PartCallback.h>

// UI includes
#include <asiUI_Viewer.h>

// Visualization includes
#include <asiVisu_PrsManager.h>

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PartCallback* asiUI_PartCallback::New()
{
  return new asiUI_PartCallback(NULL);
}

//! Instantiation routine accepting viewer.
//! \param theViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiUI_PartCallback* asiUI_PartCallback::New(asiUI_Viewer* theViewer)
{
  return new asiUI_PartCallback(theViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param theViewer [in] owning viewer.
asiUI_PartCallback::asiUI_PartCallback(asiUI_Viewer* theViewer)
: asiUI_ViewerCallback(theViewer)
{}

//! Destructor.
asiUI_PartCallback::~asiUI_PartCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to events. Performs all useful operations.
//! \param theCaller   [in] caller instance.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void asiUI_PartCallback::Execute(vtkObject*    vtkNotUsed(theCaller),
                                 unsigned long theEventId,
                                 void*         asiVisu_NotUsed(theCallData))
{
  vtkSmartPointer<asiVisu_PrsManager> mgr = this->Viewer()->PrsMgr();

  if ( theEventId == EVENT_FIND_FACE )
    emit findFace();
}
