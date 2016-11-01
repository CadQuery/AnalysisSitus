//-----------------------------------------------------------------------------
// Created on: 28 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_PickCallback_h
#define asiUI_PickCallback_h

// A-Situs (visualization) includes
#include <asiVisu_ViewerCallback.h>

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
class asiUI_PickCallback : public QObject,
                             public asiVisu_ViewerCallback
{
  Q_OBJECT

public:

  static asiUI_PickCallback* New();
  static asiUI_PickCallback* New(asiUI_Viewer* theViewer);
  vtkTypeMacro(asiUI_PickCallback, asiVisu_ViewerCallback);

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

  asiUI_PickCallback  (asiUI_Viewer* theViewer);
  ~asiUI_PickCallback ();

protected:

  bool m_bSelectMeshNodes; //!< Indicates whether to select mesh nodes.

};

#endif
