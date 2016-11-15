//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_PDomainCallback.h>

// UI includes
#include <asiUI_Viewer.h>

// Visualization includes
#include <asiVisu_PrsManager.h>

//! Instantiation routine.
//! \return instance of the callback class.
asiUI_PDomainCallback* asiUI_PDomainCallback::New()
{
  return new asiUI_PDomainCallback(NULL);
}

//! Instantiation routine accepting viewer.
//! \param pViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiUI_PDomainCallback* asiUI_PDomainCallback::New(asiUI_Viewer* pViewer)
{
  return new asiUI_PDomainCallback(pViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param pViewer [in] owning viewer.
asiUI_PDomainCallback::asiUI_PDomainCallback(asiUI_Viewer* pViewer)
: asiUI_ViewerCallback(pViewer)
{}

//! Destructor.
asiUI_PDomainCallback::~asiUI_PDomainCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to events. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void asiUI_PDomainCallback::Execute(vtkObject*    vtkNotUsed(pCaller),
                                    unsigned long eventId,
                                    void*         asiVisu_NotUsed(pCallData))
{
  vtkSmartPointer<asiVisu_PrsManager> mgr = this->GetViewer()->PrsMgr();

  if ( eventId == EVENT_DELETE )
    emit killEdges();
  else if ( eventId == EVENT_JOIN )
    emit joinEdges();
}
