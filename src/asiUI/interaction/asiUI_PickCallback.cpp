//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_PickCallback.h>

// A-Situs (common) includes
#include <common_facilities.h>

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
//! \param theViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
asiUI_PickCallback* asiUI_PickCallback::New(asiUI_Viewer* theViewer)
{
  return new asiUI_PickCallback(theViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param theViewer [in] owning viewer.
asiUI_PickCallback::asiUI_PickCallback(asiUI_Viewer* theViewer)
: asiVisu_ViewerCallback(theViewer),
  m_bSelectMeshNodes(false)
{}

//! Default destructor.
asiUI_PickCallback::~asiUI_PickCallback()
{}

//-----------------------------------------------------------------------------

//! Listens to a dedicated event. Performs all useful operations.
//! \param theCaller   [in] caller instance.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void asiUI_PickCallback::Execute(vtkObject*    vtkNotUsed(theCaller),
                                 unsigned long theEventId,
                                 void*         theCallData)
{
  vtkSmartPointer<asiVisu_PrsManager> mgr = this->Viewer()->PrsMgr();

  // Delegate execution to a proper branch of logic
  if ( mgr == common_facilities::Instance()->Prs.Part )
  {
    this->executePart(theEventId, theCallData);
  }
  else if ( mgr == common_facilities::Instance()->Prs.Mesh )
  {
    this->executeMesh(theEventId, theCallData);
  }
  else if ( mgr == common_facilities::Instance()->Prs.Domain )
  {
    this->executeDomain(theEventId, theCallData);
  }
  else if ( mgr == common_facilities::Instance()->Prs.Section )
  {
    this->executeSection(theEventId, theCallData);
  }
}

//-----------------------------------------------------------------------------

//! Answers to a picking event for Part view.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void asiUI_PickCallback::executePart(unsigned long theEventId,
                                     void*         theCallData)
{
  // Check if the calling context is valid
  if ( theEventId != EVENT_PICK_DEFAULT && theEventId != EVENT_DETECT_DEFAULT )
    return;
  //
  if ( !this->Viewer() || !common_facilities::Instance()->Prs.Part )
    return;

  // Access selection context
  const int selMode = common_facilities::Instance()->Prs.Part->GetSelectionMode();

  // Skip for disabled selection
  if ( selMode & SelectionMode_None )
    return;

  // Do not allow detection on global selection
  if ( (selMode & SelectionMode_Workpiece) && theEventId == EVENT_DETECT_DEFAULT )
    return;

  // Now pick
  asiVisu_PickInput* pickInput = reinterpret_cast<asiVisu_PickInput*>(theCallData);
  //
  const asiVisu_SelectionNature sel_type = (theEventId == EVENT_PICK_DEFAULT) ? SelectionNature_Pick
                                                                            : SelectionNature_Detection;
  common_facilities::Instance()->Prs.Part->Pick(pickInput, sel_type, PickType_World);

  // Notify observers
  if ( theEventId == EVENT_PICK_DEFAULT && (selMode & SelectionMode_Face ||
                                            selMode & SelectionMode_Edge ||
                                            selMode & SelectionMode_Vertex) )
    emit partPicked();
}

//-----------------------------------------------------------------------------

//! Answers to a picking event for Mesh view.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void asiUI_PickCallback::executeMesh(unsigned long theEventId,
                                     void*         theCallData)
{
  // Check if the calling context is valid
  if ( theEventId != EVENT_PICK_DEFAULT && theEventId != EVENT_DETECT_DEFAULT )
    return;
  //
  if ( !this->Viewer() || !common_facilities::Instance()->Prs.Mesh )
    return;

  // Now pick
  asiVisu_PickInput* pickInput = reinterpret_cast<asiVisu_PickInput*>(theCallData);
  //
  const asiVisu_SelectionNature sel_type = (theEventId == EVENT_PICK_DEFAULT) ? SelectionNature_Pick
                                                                            : SelectionNature_Detection;
  common_facilities::Instance()->Prs.Mesh->Pick(pickInput, sel_type, m_bSelectMeshNodes ? PickType_Point : PickType_Cell);

  // Notify listeners
  if ( m_bSelectMeshNodes )
    emit meshNodePicked();
  else
    emit meshElemPicked();
}

//-----------------------------------------------------------------------------

//! Answers to a picking event for Domain view.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void asiUI_PickCallback::executeDomain(unsigned long theEventId,
                                       void*         theCallData)
{
  // Check if the calling context is valid
  if ( theEventId != EVENT_PICK_DEFAULT && theEventId != EVENT_DETECT_DEFAULT )
    return;
  //
  if ( !this->Viewer() || !common_facilities::Instance()->Prs.Domain )
    return;

  // Now pick
  asiVisu_PickInput* pickInput = reinterpret_cast<asiVisu_PickInput*>(theCallData);
  //
  const asiVisu_SelectionNature sel_type = (theEventId == EVENT_PICK_DEFAULT) ? SelectionNature_Pick
                                                                            : SelectionNature_Detection;
  common_facilities::Instance()->Prs.Domain->Pick(pickInput, sel_type);

  // Notify listeners
  if ( theEventId == EVENT_PICK_DEFAULT )
    emit domainPicked();
}

//-----------------------------------------------------------------------------

//! Answers to a picking event for Section view.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void asiUI_PickCallback::executeSection(unsigned long theEventId,
                                        void*         theCallData)
{
  // Check if the calling context is valid
  if ( theEventId != EVENT_PICK_DEFAULT && theEventId != EVENT_DETECT_DEFAULT )
    return;
  //
  if ( !this->Viewer() || !common_facilities::Instance()->Prs.Section )
    return;

  // Now pick
  asiVisu_PickInput* pickInput = reinterpret_cast<asiVisu_PickInput*>(theCallData);
  //
  const asiVisu_SelectionNature sel_type = (theEventId == EVENT_PICK_DEFAULT) ? SelectionNature_Pick
                                                                            : SelectionNature_Detection;
  common_facilities::Instance()->Prs.Section->Pick(pickInput, sel_type);

  // Notify listeners
  emit sectionPicked();
}
