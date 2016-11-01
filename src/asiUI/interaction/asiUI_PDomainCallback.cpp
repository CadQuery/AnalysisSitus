//-----------------------------------------------------------------------------
// Created on: 30 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PDomainCallback.h>

// Visualization includes
#include <asiVisu_PrsManager.h>

//! Instantiation routine.
//! \return instance of the callback class.
asiVisu_PDomainCallback* asiVisu_PDomainCallback::New()
{
  return new asiVisu_PDomainCallback(NULL);
}

//! Instantiation routine accepting viewer.
//! \param theViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiVisu_PDomainCallback* asiVisu_PDomainCallback::New(asiUI_Viewer* theViewer)
{
  return new asiVisu_PDomainCallback(theViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param theViewer [in] owning viewer.
asiVisu_PDomainCallback::asiVisu_PDomainCallback(asiUI_Viewer* theViewer)
: asiVisu_ViewerCallback(theViewer)
{}

//! Destructor.
asiVisu_PDomainCallback::~asiVisu_PDomainCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to events. Performs all useful operations.
//! \param theCaller   [in] caller instance.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void asiVisu_PDomainCallback::Execute(vtkObject*    vtkNotUsed(theCaller),
                                      unsigned long theEventId,
                                      void*         asiVisu_NotUsed(theCallData))
{
  vtkSmartPointer<asiVisu_PrsManager> mgr = this->Viewer()->PrsMgr();

  if ( theEventId == EVENT_DELETE )
    emit killEdges();
  else if ( theEventId == EVENT_JOIN )
    emit joinEdges();
}
