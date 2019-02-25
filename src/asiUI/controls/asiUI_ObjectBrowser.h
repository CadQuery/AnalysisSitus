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
//    * Neither the name of the copyright holder(s) nor the
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
#include <asiUI_IParamEditor.h>
#include <asiUI_Viewer.h>

// asiEngine includes
#include <asiEngine_Model.h>

// Qt includes
#pragma warning(push, 0)
#include <QTreeWidget>
#pragma warning(pop)

#pragma warning(disable : 4251)

//-----------------------------------------------------------------------------

// Qt role to store node ID near the tree item
#define BrowserRoleNodeId Qt::UserRole

//-----------------------------------------------------------------------------

//! Tree view for A-Situs Data Model.
class asiUI_EXPORT asiUI_ObjectBrowser : public QTreeWidget
{
  Q_OBJECT

public:

  //! Creates a new instance of tree view.
  //! \param[in] model    Data Model instance.
  //! \param[in] progress progress notifier.
  //! \param[in] parent   parent widget.
  asiUI_ObjectBrowser(const Handle(asiEngine_Model)& model,
                      ActAPI_ProgressEntry           progress,
                      QWidget*                       parent = NULL);

  //! Destructor.
  virtual
    ~asiUI_ObjectBrowser();

public:

  //! Adds a viewer to be connected with the object browser to enable typical
  //! visualization commands such as "show", "hide", "show only".
  //! \param[in] pViewer viewer to associate.
  void
    AddAssociatedViewer(asiUI_Viewer* pViewer);

  //! Sets parameter editor associated with the tree.
  //! \param[in] editor parameter editor to set.
  void
    SetParameterEditor(const Handle(asiUI_IParamEditor)& editor);

  //! Populates tree view from the Data Model.
  void
    Populate();

  //! Searches for an item with the given index and set that item selected.
  //! \param[in] nodeId target Node ID.
  void
    SetSelectedNode(const ActAPI_DataObjectId& nodeId);

  //! \return selected Node or NULL if nothing is selected.
  Handle(ActAPI_INode)
    GetSelectedNode() const;

  //! \return selected Nodes or empty list if nothing is selected.
  Handle(ActAPI_HNodeList)
    GetSelectedNodes() const;

protected:

  //! Adds all child items under the given root.
  //! \param[in] root_n  root Node in a Data Model.
  //! \param[in] root_ui root item in a tree view.
  void addChildren(const Handle(ActAPI_INode)& root_n, QTreeWidgetItem* root_ui);

//-----------------------------------------------------------------------------
signals:

  void show         (const ActAPI_DataObjectId&);
  void showOnly     (const ActAPI_DataObjectId&);
  void hide         (const ActAPI_DataObjectId&);
  void nodeSelected ();

//-----------------------------------------------------------------------------
protected slots:

  void onSelectionChanged      ();
  void onContextMenu           (QPoint pos);
  void onShow                  ();
  void onShowOnly              ();
  void onHide                  ();
  void onManagePipelines       ();
  //
  void onHidePartEdges         ();
  void onShowPartEdges         ();
  void onResetPartPrs          ();
  //
  void onCopyAsJSON            ();
  void onSaveToBREP            ();
  void onSetAsPart             ();
  //
  void onPrintParameters       ();
  void onCopyName              ();
  //
  void onComputeNorms          (const bool);
  void onComputeNodalNorms     ();
  void onComputeElementalNorms ();
  void onConvertToTris         ();
  void onConvertToTess         ();

protected:

  virtual void
    populateContextMenu(const Handle(ActAPI_HNodeList)& activeNodes,
                        QMenu*                          pMenu);

protected:

  bool selectedNode  (Handle(ActAPI_INode)&     Node) const;
  bool selectedNodes (Handle(ActAPI_HNodeList)& Nodes) const;

protected:

  Handle(asiEngine_Model)    m_model;       //!< Data Model.
  Handle(asiUI_IParamEditor) m_paramEditor; //!< Associated parameter editor.
  std::vector<asiUI_Viewer*> m_viewers;     //!< Associated viewers.
  ActAPI_ProgressEntry       m_progress;    //!< Progress notifier.

};

#pragma warning(default : 4251)

#endif
