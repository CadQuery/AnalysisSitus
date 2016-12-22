//-----------------------------------------------------------------------------
// Created on: 03 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_ObjectBrowser_h
#define asiUI_ObjectBrowser_h

// A-Situs includes
#include <asiUI.h>

// Active Data (API) includes
#include <ActAPI_IModel.h>

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

  asiUI_EXPORT
    asiUI_ObjectBrowser(const Handle(ActAPI_IModel)& model,
                        QWidget*                     parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_ObjectBrowser();

public:

  void Populate();
  void SetSelected(const ActAPI_DataObjectId& nodeId);

protected:

  void addChildren(const Handle(ActAPI_INode)& root_n, QTreeWidgetItem* root_ui);

//-----------------------------------------------------------------------------
signals:

  void showOnly     (const ActAPI_DataObjectId&);
  void nodeSelected ();

//-----------------------------------------------------------------------------
protected slots:

  void onSelectionChanged ();
  void onContextMenu      (QPoint pos);
  void onShowOnly         ();

private:

  bool selectedNode(Handle(ActAPI_INode)& Node) const;

protected:

  Handle(ActAPI_IModel) m_model; //!< Data Model.

};

#endif
