//-----------------------------------------------------------------------------
// Created on: 03 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_ObjectBrowser_h
#define asiUI_ObjectBrowser_h

// asiUI includes
#include <asiUI_ViewerPart.h>

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
class asiUI_EXPORT asiUI_ObjectBrowser : public QTreeWidget
{
  Q_OBJECT

public:

  asiUI_ObjectBrowser(const Handle(ActAPI_IModel)& model,
                      asiUI_ViewerPart*            pPartViewer,
                      QWidget*                     parent = NULL);

  virtual
    ~asiUI_ObjectBrowser();

public:

  void Populate();
  void SetSelected(const ActAPI_DataObjectId& nodeId);

protected:

  void addChildren(const Handle(ActAPI_INode)& root_n, QTreeWidgetItem* root_ui);

//-----------------------------------------------------------------------------
signals:

  void show         (const ActAPI_DataObjectId&);
  void showOnly     (const ActAPI_DataObjectId&);
  void hide         (const ActAPI_DataObjectId&);
  void nodeSelected ();

//-----------------------------------------------------------------------------
protected slots:

  void onSelectionChanged ();
  void onContextMenu      (QPoint pos);
  void onShow             ();
  void onShowOnly         ();
  void onHide             ();

protected:

  virtual void populateContextMenu(const Handle(ActAPI_INode)& activeNode,
                                   QMenu*                      pMenu);

private:

  bool selectedNode(Handle(ActAPI_INode)& Node) const;

protected:

  Handle(ActAPI_IModel) m_model;       //!< Data Model.
  asiUI_ViewerPart*     m_pPartViewer; //!< Part viewer.

};

#endif
