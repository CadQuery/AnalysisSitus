//-----------------------------------------------------------------------------
// Created on: 03 December 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
  //
  void onSaveToBREP       ();
  void onSetAsPart        ();

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
