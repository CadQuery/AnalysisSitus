//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_object_browser_xde_h
#define gui_object_browser_xde_h

// A-Situs includes
#include <analysis_situs.h>

// Data Model includes
#include <xde_model.h>

// Qt includes
#pragma warning(push, 0)
#include <QtGui/QTreeWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------

// Qt role to store node ID near the tree item
#define BrowserRoleNodeId Qt::UserRole

//-----------------------------------------------------------------------------

//! Tree view for XDE Data Model.
class gui_object_browser_xde : public QTreeWidget
{
  Q_OBJECT

public:

  gui_object_browser_xde(QWidget* parent = NULL);
  virtual ~gui_object_browser_xde();

public:

  void Populate();
  //void SetSelected(const ActAPI_DataObjectId& nodeId);

protected:

  //void addChildren(const Handle(ActAPI_INode)& root_n, QTreeWidgetItem* root_ui);

//-----------------------------------------------------------------------------
signals:

  void nodeSelected();

//-----------------------------------------------------------------------------
protected slots:

  void onSelectionChanged();

};

#endif
