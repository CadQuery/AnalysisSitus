//-----------------------------------------------------------------------------
// Created on: 13 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_decomposition_browser_h
#define gui_decomposition_browser_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_volume_node.h>

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

//! Tree view for B-Rep decomposition on volumetric features.
class gui_decomposition_browser : public QTreeWidget
{
  Q_OBJECT

public:

  gui_decomposition_browser(QWidget* parent = NULL);
  virtual ~gui_decomposition_browser();

public:

  void                Populate    ();
  void                SetSelected (const ActAPI_DataObjectId& nodeId);
  ActAPI_DataObjectId GetSelected () const;

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
  void onGenerate         ();

private:

  bool volumeNode(Handle(geom_volume_node)& Node) const;

};

#endif
