//-----------------------------------------------------------------------------
// Created on: 03 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef gui_object_browser_h
#define gui_object_browser_h

// A-Situs includes
#include <analysis_situs.h>

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
class gui_object_browser : public QTreeWidget
{
  Q_OBJECT

public:

  gui_object_browser(QWidget* parent = NULL);
  virtual ~gui_object_browser();

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

  void onSelectionChanged();

};

#endif
