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

// Own include
#include <asiUI_ObjectBrowser.h>

// asiUI includes
#include <asiUI_Common.h>

// asiVisu includes
#include <asiVisu_GeomPrs.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Qt includes
#pragma warning(push, 0)
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidgetItemIterator>
#pragma warning(pop)

#define TREEVIEW_MINSIZE 200

//-----------------------------------------------------------------------------

//! Creates a new instance of tree view.
//! \param model   [in] Data Model instance.
//! \param parent  [in] parent widget.
asiUI_ObjectBrowser::asiUI_ObjectBrowser(const Handle(ActAPI_IModel)& model,
                                         QWidget*                     parent)
: QTreeWidget(parent), m_model(model)
{
  // Configure
  this->setMinimumWidth(TREEVIEW_MINSIZE);
  this->setEditTriggers(QAbstractItemView::NoEditTriggers);
  this->header()->hide();
  this->setColumnCount(1);
  this->setAutoExpandDelay(0);

  // Populate
  this->Populate();

  // Configure selection
  this->setSelectionMode(QAbstractItemView::SingleSelection);
  this->setSelectionBehavior(QAbstractItemView::SelectRows);

  // Reactions
  connect( this, SIGNAL( itemSelectionChanged() ), this, SLOT( onSelectionChanged() ) );
  //
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  //
  connect( this, SIGNAL( customContextMenuRequested(QPoint) ), this, SLOT( onContextMenu(QPoint) ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ObjectBrowser::~asiUI_ObjectBrowser()
{}

//-----------------------------------------------------------------------------

//! Adds a viewer to be connected with the object browser to enable typical
//! visualization commands such as "show", "hide", "show only".
//! \param[in] pViewer viewer to associate.
void asiUI_ObjectBrowser::AddAssociatedViewer(asiUI_Viewer* pViewer)
{
  m_viewers.push_back(pViewer);
}

//-----------------------------------------------------------------------------

//! Populates tree view from the Data Model.
void asiUI_ObjectBrowser::Populate()
{
  // Clean up the existing contents
  this->clear();

  // Add root node
  Handle(ActAPI_INode) root_n = m_model->GetRootNode();
  //
  if ( root_n.IsNull() || !root_n->IsWellFormed() )
    return;
  //
  QTreeWidgetItem* root_ui = new QTreeWidgetItem( QStringList() << ExtStr2QStr( root_n->GetName() ) );
  root_ui->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
  root_ui->setData( 0, BrowserRoleNodeId, AsciiStr2QStr( root_n->GetId() ) );
  //
  this->addTopLevelItem(root_ui);

  // Add child nodes
  this->addChildren(root_n, root_ui);

  // Expand tree
  this->expandAll();
}

//-----------------------------------------------------------------------------

//! Searches for an item with the given index and set that item selected.
//! \param nodeId [in] target Node ID.
void asiUI_ObjectBrowser::SetSelected(const ActAPI_DataObjectId& nodeId)
{
  QTreeWidgetItemIterator it(this);
  //
  while ( *it )
  {
    QString itemData = (*it)->data(0, BrowserRoleNodeId).toString();
    if ( QStr2AsciiStr(itemData) == nodeId )
    {
      (*it)->setSelected(true);
      break;
    }
    else
      ++it;
  }
}

//-----------------------------------------------------------------------------

//! \return selected Node or NULL if nothing is selected.
Handle(ActAPI_INode) asiUI_ObjectBrowser::GetSelected() const
{
  Handle(ActAPI_INode) selected;
  if ( !this->selectedNode(selected) )
    return NULL;

  return selected;
}

//-----------------------------------------------------------------------------

//! Adds all child items under the given root.
//! \param root_n  [in] root Node in a Data Model.
//! \param root_ui [in] root item in a tree view.
void asiUI_ObjectBrowser::addChildren(const Handle(ActAPI_INode)& root_n,
                                      QTreeWidgetItem*            root_ui)
{
  if ( root_n.IsNull() || !root_n->IsWellFormed() )
    return;

  for ( Handle(ActAPI_IChildIterator) cit = root_n->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) child_n = cit->Value();
    //
    QTreeWidgetItem* child_ui = new QTreeWidgetItem( QStringList() << ExtStr2QStr( child_n->GetName() ) );
    child_ui->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    child_ui->setData( 0, BrowserRoleNodeId, AsciiStr2QStr( child_n->GetId() ) );
    //
    root_ui->addChild(child_ui);

    // Repeat recursively
    this->addChildren(child_n, child_ui);
  }
}

//-----------------------------------------------------------------------------

//! Reaction on selection in a tree view.
void asiUI_ObjectBrowser::onSelectionChanged()
{
  emit nodeSelected();
}

//-----------------------------------------------------------------------------

//! Reaction on context menu opening.
//! \param pos [in] position.
void asiUI_ObjectBrowser::onContextMenu(QPoint pos)
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  // Create and populate the menu
  QMenu* pMenu = new QMenu(this);
  //
  this->populateContextMenu(selected_n, pMenu);
  //
  if ( !pMenu->isEmpty() )
    pMenu->popup( this->mapToGlobal(pos) );
}

//-----------------------------------------------------------------------------

//! Reaction on "show" action.
void asiUI_ObjectBrowser::onShow()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  // Iterate over the associated viewer to find the one where the selected
  // Node is presented
  for ( size_t k = 0; k < m_viewers.size(); ++k )
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(selected_n) )
    {
      m_viewers[k]->PrsMgr()->DeRenderPresentation(selected_n);
      m_viewers[k]->PrsMgr()->Actualize(selected_n);
    }

  emit show( selected_n->GetId() );
}

//-----------------------------------------------------------------------------

//! Reaction on "show only" action.
void asiUI_ObjectBrowser::onShowOnly()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  // Iterate over the associated viewer to find the one where the selected
  // Node is presented
  for ( size_t k = 0; k < m_viewers.size(); ++k )
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(selected_n) )
    {
      m_viewers[k]->PrsMgr()->DeRenderAllPresentations();
      m_viewers[k]->PrsMgr()->Actualize(selected_n);
    }

  emit showOnly( selected_n->GetId() );
}

//-----------------------------------------------------------------------------

//! Reaction on "hide" action.
void asiUI_ObjectBrowser::onHide()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  // Iterate over the associated viewer to find the one where the selected
  // Node is presented
  for ( size_t k = 0; k < m_viewers.size(); ++k )
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(selected_n) )
    {
      m_viewers[k]->PrsMgr()->DeRenderPresentation(selected_n);
      m_viewers[k]->Repaint();
    }

  emit hide( selected_n->GetId() );
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onHidePartEdges()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  if ( !selected_n->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
    return;

  for ( size_t k = 0; k < m_viewers.size(); ++k )
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(selected_n) )
    {
      Handle(asiVisu_GeomPrs)
        prs = Handle(asiVisu_GeomPrs)::DownCast( m_viewers[k]->PrsMgr()->GetPresentation(selected_n) );

      prs->ContourActor()->SetVisibility(0);

      m_viewers[k]->Repaint();
    }
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onShowPartEdges()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  if ( !selected_n->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
    return;

  for ( size_t k = 0; k < m_viewers.size(); ++k )
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(selected_n) )
    {
      Handle(asiVisu_GeomPrs)
        prs = Handle(asiVisu_GeomPrs)::DownCast( m_viewers[k]->PrsMgr()->GetPresentation(selected_n) );

      prs->ContourActor()->SetVisibility(1);

      m_viewers[k]->Repaint();
    }
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onResetPartPrs()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  if ( !selected_n->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
    return;

  for ( size_t k = 0; k < m_viewers.size(); ++k )
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(selected_n) )
    {
      m_viewers[k]->PrsMgr()->DeletePresentation(selected_n);
      m_viewers[k]->PrsMgr()->Actualize(selected_n, false, true);
    }
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onSaveToBREP()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  if ( !selected_n->IsKind( STANDARD_TYPE(asiData_IVTopoItemNode) ) )
    return;

  Handle(asiData_IVTopoItemNode)
    topoNode = Handle(asiData_IVTopoItemNode)::DownCast(selected_n);

  QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Save);

  // Save shape
  if ( !asiAlgo_Utils::WriteBRep( topoNode->GetShape(), QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save shape" << std::endl;
    return;
  }
}

//-----------------------------------------------------------------------------

//! Populates context menu with actions.
//! \param activeNode [in]      currently active Node.
//! \param pMenu      [in, out] menu to populate.
void asiUI_ObjectBrowser::populateContextMenu(const Handle(ActAPI_INode)& activeNode,
                                              QMenu*                      pMenu)
{
  // Iterate over the associated viewer to find the one where the selected
  // Node is presented
  bool isPresented = false;
  for ( size_t k = 0; k < m_viewers.size(); ++k )
  {
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(activeNode) )
    {
      isPresented = true;
      break;
    }
  }

  if ( isPresented )
  {
    pMenu->addAction( "Show",      this, SLOT( onShow()     ) );
    pMenu->addAction( "Show Only", this, SLOT( onShowOnly() ) );
    pMenu->addAction( "Hide",      this, SLOT( onHide()     ) );

    if ( activeNode->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
    {
      pMenu->addSeparator();
      pMenu->addAction( "Hide edges",         this, SLOT( onHidePartEdges() ) );
      pMenu->addAction( "Show edges",         this, SLOT( onShowPartEdges() ) );
      pMenu->addAction( "Reset presentation", this, SLOT( onResetPartPrs() ) );
    }

    if ( activeNode->IsKind( STANDARD_TYPE(asiData_IVTopoItemNode) ) )
    {
      pMenu->addSeparator();
      pMenu->addAction( "Save to BREP...", this, SLOT( onSaveToBREP() ) );
    }
  }
}

//-----------------------------------------------------------------------------

//! Returns the currently active Node.
//! \param Node [out] requested Node.
//! \return true in case of success, false -- otherwise.
bool asiUI_ObjectBrowser::selectedNode(Handle(ActAPI_INode)& Node) const
{
  QList<QTreeWidgetItem*> items = this->selectedItems();
  if ( !items.length() || items.length() > 1 )
    return false;

  QTreeWidgetItem* item = items.at(0);
  TCollection_AsciiString entry = QStr2AsciiStr( item->data(0, BrowserRoleNodeId).toString() );

  // Take the corresponding data object
  Handle(ActAPI_INode) selected_n = m_model->FindNode(entry);
  //
  if ( selected_n.IsNull() || !selected_n->IsWellFormed() )
  {
    std::cout << "Error: selected Node is invalid" << std::endl;
    return false;
  }

  // Set result
  Node = selected_n;
  return true;
}
