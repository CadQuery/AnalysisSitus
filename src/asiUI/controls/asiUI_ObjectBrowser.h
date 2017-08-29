//-----------------------------------------------------------------------------
// Created on: 03 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
                      QWidget*                     parent = NULL);

  virtual
    ~asiUI_ObjectBrowser();

public:

  void
    AddAssociatedViewer(asiUI_Viewer* pViewer);

  void
    Populate();

  void
    SetSelected(const ActAPI_DataObjectId& nodeId);

  Handle(ActAPI_INode)
    GetSelected() const;

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
  //
  void onHidePartEdges    ();
  void onShowPartEdges    ();
  void onResetPartPrs     ();

protected:

  virtual void populateContextMenu(const Handle(ActAPI_INode)& activeNode,
                                   QMenu*                      pMenu);

protected:

  bool selectedNode(Handle(ActAPI_INode)& Node) const;

protected:

  Handle(ActAPI_IModel)      m_model;   //!< Data Model.
  std::vector<asiUI_Viewer*> m_viewers; //!< Associated viewers.

};

#endif
