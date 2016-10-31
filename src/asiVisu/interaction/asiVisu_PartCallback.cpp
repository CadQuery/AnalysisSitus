//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_part_callback.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (GUI) includes
#include <gui_viewer_part.h>

//! Instantiation routine.
//! \return instance of the callback class.
visu_part_callback* visu_part_callback::New()
{
  return new visu_part_callback(NULL);
}

//! Instantiation routine accepting viewer.
//! \param theViewer [in] viewer to bind callback object to.
//! \return instance of the callback class.
visu_part_callback* visu_part_callback::New(gui_viewer* theViewer)
{
  return new visu_part_callback(theViewer);
}

//! Constructor accepting owning viewer as a parameter.
//! \param theViewer [in] owning viewer.
visu_part_callback::visu_part_callback(gui_viewer* theViewer)
: visu_viewer_callback(theViewer)
{}

//! Destructor.
visu_part_callback::~visu_part_callback()
{}

//-----------------------------------------------------------------------------

//! Listens to events. Performs all useful operations.
//! \param theCaller   [in] caller instance.
//! \param theEventId  [in] ID of the event triggered this listener.
//! \param theCallData [in] invocation context.
void visu_part_callback::Execute(vtkObject*    vtkNotUsed(theCaller),
                                 unsigned long theEventId,
                                 void*         ASitus_NotUsed(theCallData))
{
  vtkSmartPointer<visu_prs_manager> mgr = this->Viewer()->PrsMgr();
  //
  if ( mgr != common_facilities::Instance()->Prs.Part )
    return;

  if ( theEventId == EVENT_FIND_FACE )
    emit findFace();
}
