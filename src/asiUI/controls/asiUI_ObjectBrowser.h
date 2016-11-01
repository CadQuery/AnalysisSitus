//-----------------------------------------------------------------------------
// Created on: 03 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_ObjectBrowser_h
#define asiUI_ObjectBrowser_h

// A-Situs includes
#include <asiUI.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// Qt includes
#pragma warning(push, 0)
#include <QTreeWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------

// Qt role to store node ID near the tree item
#define BrowserRoleNodeId Qt::UserRole

//-----------------------------------------------------------------------------

//! Tree view for A-Situs Data Model.
class asiUI_ObjectBrowser : public QTreeWidget
{
  Q_OBJECT

public:

  asiUI_ObjectBrowser(QWidget* parent = NULL);
  virtual ~asiUI_ObjectBrowser();

public:

  void Populate();
  void SetSelected(const ActAPI_DataObjectId& nodeId);

protected:

  void addChildren(const Handle(ActAPI_INode)& root_n, QTreeWidgetItem* root_ui);

//-----------------------------------------------------------------------------
signals:

  void nodeSelected();

//-----------------------------------------------------------------------------
protected slots:

  void onSelectionChanged ();
  void onContextMenu      (QPoint pos);
  void onShowOnly         ();

private:

  bool selectedNode(Handle(ActAPI_INode)& Node) const;

};

#endif
