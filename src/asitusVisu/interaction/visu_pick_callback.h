//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_pick_callback_h
#define visu_pick_callback_h

// A-Situs (visualization) includes
#include <visu_viewer_callback.h>

// VIS includes
#include <IVtk_Types.hxx>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for pick operation. Cooperates with VTK Selector via Qt signals
//! and with VTK window via VTK Command pattern. Therefore, this class is a
//! messaging port between Qt part of the application and its interactive
//! VTK-based part. Notice that not all callbacks have to play such a role.
//! For example, Zoom Area Callback is a pure VTK class as zooming
//! functionality is somewhat completely covered by VTK library. Unlike
//! zooming, the picking operation requires at least some synchronization with
//! Qt-based GUI. That is why such operations are involved into more
//! sophisticated event chaining mechanism:
//!
//! User-click in VTK window -> Pick Callback -> GUI Observers
class visu_pick_callback : public QObject,
                           public visu_viewer_callback
{
  Q_OBJECT

public:

  static visu_pick_callback* New();
  static visu_pick_callback* New(gui_viewer* theViewer);
  vtkTypeMacro(visu_pick_callback, visu_viewer_callback);

public:

  void SelectMeshNodesOn()  { m_bSelectMeshNodes = true; }
  void SelectMeshNodesOff() { m_bSelectMeshNodes = false; }

public:

  virtual void Execute(vtkObject*    theCaller,
                       unsigned long theEventId,
                       void*         theCallData);

protected:

  //---------------------------------------------------------------------------
  void executePart(unsigned long theEventId,
                   void*         theCallData);
  //---------------------------------------------------------------------------
  void executeMesh(unsigned long theEventId,
                   void*         theCallData);
  //---------------------------------------------------------------------------
  void executeDomain(unsigned long theEventId,
                     void*         theCallData);
  //---------------------------------------------------------------------------
  void executeSection(unsigned long theEventId,
                      void*         theCallData);
  //---------------------------------------------------------------------------

signals:

  void partPicked();     //!< Some sensitives were picked in a part.
  void meshNodePicked(); //!< Some sensitives were picked in a mesh.
  void meshElemPicked(); //!< Some sensitives were picked in a mesh.
  void domainPicked();   //!< Some sensitives were picked in a domain.
  void sectionPicked();  //!< Some sensitives were picked in a section.

private:

  visu_pick_callback  (gui_viewer* theViewer);
  ~visu_pick_callback ();

protected:

  bool m_bSelectMeshNodes; //!< Indicates whether to select mesh nodes.

};

#endif
