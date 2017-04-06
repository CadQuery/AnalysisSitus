//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
//! \param pViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiUI_PartCallback* asiUI_PartCallback::New(asiUI_Viewer* pViewer)
{
  return new asiUI_PartCallback(pViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param pViewer [in] owning viewer.
asiUI_PartCallback::asiUI_PartCallback(asiUI_Viewer* pViewer)
: asiUI_ViewerCallback(pViewer)
{}

//! Destructor.
asiUI_PartCallback::~asiUI_PartCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to events. Performs all useful operations.
//! \param pCaller   [in] caller instance.
//! \param eventId   [in] ID of the event triggered this listener.
//! \param pCallData [in] invocation context.
void asiUI_PartCallback::Execute(vtkObject*    vtkNotUsed(pCaller),
                                 unsigned long eventId,
                                 void*         asiVisu_NotUsed(pCallData))
{
  const vtkSmartPointer<asiVisu_PrsManager>& mgr = this->GetViewer()->PrsMgr();

  if ( eventId == EVENT_FIND_FACE )
    emit findFace();

  if ( eventId == EVENT_FIND_EDGE )
    emit findEdge();

  if ( eventId == EVENT_REFINE_TESSELLATION )
    emit refineTessellation();
}
