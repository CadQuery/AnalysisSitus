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
#include <asiUI_DialogPipelines.h>
#include <asiUI_ViewerPart.h>

// asiEngine includes
#include <asiEngine_Part.h>
#include <asiEngine_Tessellation.h>

// asiVisu includes
#include <asiVisu_PartPrs.h>

// asiAlgo includes
#include <asiAlgo_JSON.h>
#include <asiAlgo_MeshConvert.h>
#include <asiAlgo_Utils.h>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidgetItemIterator>
#pragma warning(pop)

#define TREEVIEW_MINSIZE 200

//-----------------------------------------------------------------------------

//! Creates a new instance of tree view.
//! \param[in] model    Data Model instance.
//! \param[in] progress progress notifier.
//! \param[in] parent   parent widget.
asiUI_ObjectBrowser::asiUI_ObjectBrowser(const Handle(ActAPI_IModel)& model,
                                         ActAPI_ProgressEntry         progress,
                                         QWidget*                     parent)
: QTreeWidget(parent), m_progress(progress), m_model(model)
{
  // Configure.
  this->setMinimumWidth(TREEVIEW_MINSIZE);
  this->setEditTriggers(QAbstractItemView::NoEditTriggers);
  this->header()->hide();
  this->setColumnCount(1);
  this->setAutoExpandDelay(0);

  // Populate.
  this->Populate();

  // Configure selection.
  this->setSelectionMode(QAbstractItemView::ExtendedSelection);
  this->setSelectionBehavior(QAbstractItemView::SelectRows);

  // Reactions.
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
  // Clean up the existing contents.
  this->clear();

  // Add root node.
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

  // Add child nodes.
  this->addChildren(root_n, root_ui);

  // Expand tree.
  this->expandAll();
}

//-----------------------------------------------------------------------------

//! Searches for an item with the given index and set that item selected.
//! \param[in] nodeId target Node ID.
void asiUI_ObjectBrowser::SetSelectedNode(const ActAPI_DataObjectId& nodeId)
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
Handle(ActAPI_INode) asiUI_ObjectBrowser::GetSelectedNode() const
{
  Handle(ActAPI_INode) selected;
  if ( !this->selectedNode(selected) )
    return NULL;

  return selected;
}

//-----------------------------------------------------------------------------

//! \return selected Nodes or empty list if nothing is selected.
Handle(ActAPI_HNodeList) asiUI_ObjectBrowser::GetSelectedNodes() const
{
  Handle(ActAPI_HNodeList) selected;
  if ( !this->selectedNodes(selected) )
    return NULL;

  return selected;
}

//-----------------------------------------------------------------------------

//! Adds all child items under the given root.
//! \param[in] root_n  root Node in a Data Model.
//! \param[in] root_ui root item in a tree view.
void asiUI_ObjectBrowser::addChildren(const Handle(ActAPI_INode)& root_n,
                                      QTreeWidgetItem*            root_ui)
{
  if ( root_n.IsNull() )
    return;

  // Allow adding bad-formed Nodes, just mark them specifically.
  if ( !root_n->IsWellFormed() )
    root_ui->setBackgroundColor(0, Qt::darkRed);

  // Loop over the children.
  for ( Handle(ActAPI_IChildIterator) cit = root_n->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) child_n = cit->Value();
    //
    QTreeWidgetItem* child_ui = new QTreeWidgetItem( QStringList() << ExtStr2QStr( child_n->GetName() ) );
    child_ui->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    child_ui->setData( 0, BrowserRoleNodeId, AsciiStr2QStr( child_n->GetId() ) );
    //
    root_ui->addChild(child_ui);

    // Repeat recursively.
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
//! \param[in] pos position.
void asiUI_ObjectBrowser::onContextMenu(QPoint pos)
{
  Handle(ActAPI_HNodeList) selected_n;
  if ( !this->selectedNodes(selected_n) ) return;

  // Create and populate the menu.
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
  Handle(ActAPI_HNodeList) selected_n;
  if ( !this->selectedNodes(selected_n) ) return;

  for ( ActAPI_HNodeList::Iterator nit(*selected_n); nit.More(); nit.Next() )
  {
    const Handle(ActAPI_INode)& N = nit.Value();

    // Iterate over the associated viewer to find the one where the selected
    // Node is presented.
    for ( size_t k = 0; k < m_viewers.size(); ++k )
      if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(N) )
      {
        m_viewers[k]->PrsMgr()->DeRenderPresentation(N);
        m_viewers[k]->PrsMgr()->Actualize(N);
      }

    emit show( N->GetId() );
  }
}

//-----------------------------------------------------------------------------

//! Reaction on "show only" action.
void asiUI_ObjectBrowser::onShowOnly()
{
  Handle(ActAPI_HNodeList) selected_n;
  if ( !this->selectedNodes(selected_n) ) return;

  // Derender all presentations.
  for ( size_t k = 0; k < m_viewers.size(); ++k )
    if ( m_viewers[k] )
      m_viewers[k]->PrsMgr()->DeRenderAllPresentations();

  for ( ActAPI_HNodeList::Iterator nit(*selected_n); nit.More(); nit.Next() )
  {
    const Handle(ActAPI_INode)& N = nit.Value();

    // Iterate over the associated viewer to find the one where the selected
    // Node is presented.
    for ( size_t k = 0; k < m_viewers.size(); ++k )
      if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(N) )
        m_viewers[k]->PrsMgr()->Actualize(N);

    emit showOnly( N->GetId() );
  }
}

//-----------------------------------------------------------------------------

//! Reaction on "hide" action.
void asiUI_ObjectBrowser::onHide()
{
  Handle(ActAPI_HNodeList) selected_n;
  if ( !this->selectedNodes(selected_n) ) return;

  for ( ActAPI_HNodeList::Iterator nit(*selected_n); nit.More(); nit.Next() )
  {
    const Handle(ActAPI_INode)& N = nit.Value();

    // Iterate over the associated viewers to find the one where the selected
    // Node is presented.
    for ( size_t k = 0; k < m_viewers.size(); ++k )
      if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(N) )
      {
        m_viewers[k]->PrsMgr()->DeRenderPresentation(N);
        m_viewers[k]->Repaint();
      }

    emit hide( N->GetId() );
  }
}

//-----------------------------------------------------------------------------

//! Reaction on "manage pipelines" action.
void asiUI_ObjectBrowser::onManagePipelines()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  // Iterate over the associated viewers to find presentation.
  Handle(asiVisu_Prs)                 prs;
  vtkSmartPointer<asiVisu_PrsManager> prsMgr;
  //
  for ( size_t k = 0; k < m_viewers.size(); ++k )
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(selected_n) )
    {
      prsMgr = m_viewers[k]->PrsMgr();
      prs    = prsMgr->GetPresentation(selected_n);
      break;
    }

  // Show dialog to manage presentation.
  if ( !prs.IsNull() )
  {
    asiUI_DialogPipelines* pDlg = new asiUI_DialogPipelines(prs, prsMgr, m_progress, this);
    pDlg->show();
  }
  else
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot manage pipelines for a non-presented Node.");
  }
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
      Handle(asiVisu_PartPrs)
        prs = Handle(asiVisu_PartPrs)::DownCast( m_viewers[k]->PrsMgr()->GetPresentation(selected_n) );

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
      Handle(asiVisu_PartPrs)
        prs = Handle(asiVisu_PartPrs)::DownCast( m_viewers[k]->PrsMgr()->GetPresentation(selected_n) );

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

void asiUI_ObjectBrowser::onCopyAsJSON()
{
  Handle(ActAPI_INode) sel;
  if ( !this->selectedNode(sel) ) return;

  // Dump either curve or surface to JSON.
  std::ostringstream jsonStream;
  //
  if ( sel->IsKind( STANDARD_TYPE(asiData_IVCurveNode) ) )
  {
    asiAlgo_JSON::DumpCurve(Handle(asiData_IVCurveNode)::DownCast(sel)->GetCurve(),
                            jsonStream);
  }
  else if ( sel->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    asiAlgo_JSON::DumpSurface(Handle(asiData_IVSurfaceNode)::DownCast(sel)->GetSurface(),
                              jsonStream);
  }
  else
    return;

  // Copy to clipboard.
  QApplication::clipboard()->setText( jsonStream.str().c_str() );
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

  // Save shape.
  if ( !asiAlgo_Utils::WriteBRep( topoNode->GetShape(), QStr2AsciiStr(filename) ) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot save shape.");
    return;
  }
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onSetAsPart()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  if ( !selected_n->IsKind( STANDARD_TYPE(asiData_IVTopoItemNode) ) )
    return;

  // Convert to the only supported type.
  Handle(asiData_IVTopoItemNode)
    topoNode = Handle(asiData_IVTopoItemNode)::DownCast(selected_n);

  Handle(asiEngine_Model) M = Handle(asiEngine_Model)::DownCast(m_model);

  // Modify Data Model.
  M->OpenCommand();
  {
    asiEngine_Part(M).Update( topoNode->GetShape() );
  }
  M->CommitCommand();

  // Update UI.
  for ( size_t k = 0; k < m_viewers.size(); ++k )
  {
    // Clear topological item.
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(topoNode) )
      m_viewers[k]->PrsMgr()->DeRenderPresentation(topoNode);

    // Actualize part.
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented( M->GetPartNode() ) )
      m_viewers[k]->PrsMgr()->Actualize( M->GetPartNode() );
  }
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onPrintParameters()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  // Loop over the parameters.
  TCollection_AsciiString dump("Parameters of \'");
  dump += selected_n->GetName();
  dump += "\' [";
  dump += selected_n->GetId();
  dump += "]";
  dump += " (";
  dump += selected_n->DynamicType()->Name();
  dump += ")";
  dump += ":\n";
  //
  for ( Handle(ActAPI_IParamIterator) pit = selected_n->GetParamIterator(); pit->More(); pit->Next() )
  {
    const Handle(ActAPI_IUserParameter)& P = pit->Value();

    dump += "\t";
    dump += "[";
    dump += pit->Key();
    dump += "] ";
    dump += P->DynamicType()->Name();
    dump += "\n";
  }

  m_progress.SendLogMessage(LogInfo(Normal) << dump);
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onCopyName()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  // Set to clipboard.
  QClipboard* clipboard = QApplication::clipboard();
  clipboard->setText( ExtStr2QStr( selected_n->GetName() ) );

  // Notify.
  m_progress.SendLogMessage( LogInfo(Normal) << "Selected Node's name: %1"
                                             << selected_n->GetName() );
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onComputeNorms(const bool doElemNorms)
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  if ( !selected_n->IsKind( STANDARD_TYPE(asiData_TessNode) ) )
    return;

  Handle(asiData_TessNode)
    tessNode = Handle(asiData_TessNode)::DownCast(selected_n);

  Handle(asiEngine_Model) M = Handle(asiEngine_Model)::DownCast(m_model);

  // Modify Data Model.
  Handle(asiData_TessNormsNode) tessNormsNode;
  //
  M->OpenCommand();
  {
    tessNormsNode = asiEngine_Tessellation(M).ComputeNorms(tessNode, doElemNorms);
  }
  M->CommitCommand();

  // Update UI.
  for ( size_t k = 0; k < m_viewers.size(); ++k )
  {
    // Actualize in Part Viewer.
    asiUI_ViewerPart* pViewerPart = dynamic_cast<asiUI_ViewerPart*>(m_viewers[k]);
    //
    if ( pViewerPart )
      pViewerPart->PrsMgr()->Actualize(tessNormsNode);
  }
  //
  this->Populate();
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onComputeNodalNorms()
{
  this->onComputeNorms(false);
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onComputeElementalNorms()
{
  this->onComputeNorms(true);
}

//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onConvertToTris()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  if ( !selected_n->IsKind( STANDARD_TYPE(asiData_TessNode) ) )
    return;

  Handle(asiData_TessNode)
    tessNode = Handle(asiData_TessNode)::DownCast(selected_n);

  // Convert to Poly triangulation.
  Handle(Poly_Triangulation) polyTris;
  asiAlgo_MeshConvert::FromPersistent(tessNode->GetMesh(), polyTris);

  Handle(asiEngine_Model) M = Handle(asiEngine_Model)::DownCast(m_model);

  // Modify Data Model.
  M->OpenCommand();
  {
    M->GetTriangulationNode()->SetTriangulation(polyTris);
  }
  M->CommitCommand();

  // Update UI.
  for ( size_t k = 0; k < m_viewers.size(); ++k )
  {
    // Actualize in Part Viewer.
    asiUI_ViewerPart* pViewerPart = dynamic_cast<asiUI_ViewerPart*>(m_viewers[k]);
    //
    if ( pViewerPart )
      pViewerPart->PrsMgr()->Actualize( M->GetTriangulationNode() );
  }
}


//-----------------------------------------------------------------------------

void asiUI_ObjectBrowser::onConvertToTess()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  if ( !selected_n->IsKind( STANDARD_TYPE(asiData_TriangulationNode) ) )
    return;

  Handle(asiData_TriangulationNode)
    trisNode = Handle(asiData_TriangulationNode)::DownCast(selected_n);

  // Convert to Poly triangulation.
  Handle(ActData_Mesh) mesh;
  asiAlgo_MeshConvert::ToPersistent(trisNode->GetTriangulation(), mesh);

  Handle(asiEngine_Model) M = Handle(asiEngine_Model)::DownCast(m_model);

  // Modify Data Model.
  M->OpenCommand();
  {
    M->GetTessellationNode()->SetMesh(mesh);
  }
  M->CommitCommand();

  // Update UI.
  for ( size_t k = 0; k < m_viewers.size(); ++k )
  {
    // Actualize in Part Viewer.
    asiUI_ViewerPart* pViewerPart = dynamic_cast<asiUI_ViewerPart*>(m_viewers[k]);
    //
    if ( pViewerPart )
      pViewerPart->PrsMgr()->Actualize( M->GetTessellationNode() );
  }
}

//-----------------------------------------------------------------------------

//! Populates context menu with actions.
//! \param[in]     activeNodes currently active Nodes.
//! \param[in,out] pMenu       menu to populate.
void asiUI_ObjectBrowser::populateContextMenu(const Handle(ActAPI_HNodeList)& activeNodes,
                                              QMenu*                          pMenu)
{
  if ( activeNodes.IsNull() || activeNodes->IsEmpty() )
    return;

  // Get type of the first Node.
  const int             numSelected = activeNodes->Length();
  Handle(ActAPI_INode)  node        = activeNodes->First();
  Handle(Standard_Type) type        = node->DynamicType();

  // Check whether other Nodes are of the same type.
  bool isOfSameType = true;
  //
  for ( ActAPI_HNodeList::Iterator nit(*activeNodes); nit.More(); nit.Next() )
  {
    const Handle(ActAPI_INode)& N = nit.Value();
    //
    if ( N->DynamicType() != type )
    {
      isOfSameType = false;
      break;
    }
  }
  //
  if ( !isOfSameType )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "Selected Nodes are of different types.");
    return;
  }

  // Iterate over the associated viewer to find the one where the selected
  // Node is presented.
  bool isPresented = false;
  for ( size_t k = 0; k < m_viewers.size(); ++k )
  {
    if ( m_viewers[k] && m_viewers[k]->PrsMgr()->IsPresented(node) )
    {
      isPresented = true;
      break;
    }
  }

  if ( numSelected == 1 )
  {
    pMenu->addAction( "Print parameters", this, SLOT( onPrintParameters () ) );
    pMenu->addAction( "Copy name",        this, SLOT( onCopyName        () ) );
  }
  //
  if ( isPresented )
  {
    pMenu->addSeparator();
    pMenu->addAction( "Show",                this, SLOT( onShow            () ) );
    pMenu->addAction( "Show only",           this, SLOT( onShowOnly        () ) );
    pMenu->addAction( "Hide",                this, SLOT( onHide            () ) );

    if ( numSelected == 1 )
    {
      pMenu->addAction( "Manage pipelines...", this, SLOT( onManagePipelines () ) );

      if ( node->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
      {
        pMenu->addSeparator();
        pMenu->addAction( "Hide edges",         this, SLOT( onHidePartEdges () ) );
        pMenu->addAction( "Show edges",         this, SLOT( onShowPartEdges () ) );
        pMenu->addAction( "Reset presentation", this, SLOT( onResetPartPrs  () ) );
      }

      if ( node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) ||
           node->IsKind( STANDARD_TYPE(asiData_IVCurveNode) ) )
      {
        pMenu->addSeparator();
        pMenu->addAction( "Copy as JSON", this, SLOT( onCopyAsJSON () ) );
      }

      if ( node->IsKind( STANDARD_TYPE(asiData_IVTopoItemNode) ) )
      {
        pMenu->addSeparator();
        pMenu->addAction( "Save to BREP...", this, SLOT( onSaveToBREP () ) );
        pMenu->addAction( "Set as part",     this, SLOT( onSetAsPart  () ) );
      }

      if ( node->IsKind( STANDARD_TYPE(asiData_TessNode) ) )
      {
        pMenu->addSeparator();
        pMenu->addAction( "Compute nodal norms",      this, SLOT( onComputeNodalNorms     () ) );
        pMenu->addAction( "Compute elemental norms",  this, SLOT( onComputeElementalNorms () ) );
        pMenu->addAction( "Convert to triangulation", this, SLOT( onConvertToTris         () ) );
      }

      if ( node->IsKind( STANDARD_TYPE(asiData_TriangulationNode) ) )
      {
        pMenu->addSeparator();
        pMenu->addAction( "Convert to tessellation",  this, SLOT( onConvertToTess () ) );
      }
    }
  }
}

//-----------------------------------------------------------------------------

//! Returns the currently active Node.
//! \param[out] Node requested Node.
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

//-----------------------------------------------------------------------------

//! Returns the currently active Nodes.
//! \param[out] Nodes requested Nodes.
//! \return true in case of success, false -- otherwise.
bool asiUI_ObjectBrowser::selectedNodes(Handle(ActAPI_HNodeList)& Nodes) const
{
  QList<QTreeWidgetItem*> items = this->selectedItems();
  if ( !items.length() )
    return false;

  Nodes = new ActAPI_HNodeList;

  for ( QList<QTreeWidgetItem*>::iterator it = items.begin(); it != items.end(); ++it )
  {
    QTreeWidgetItem* item = *it;
    TCollection_AsciiString entry = QStr2AsciiStr( item->data(0, BrowserRoleNodeId).toString() );

    // Take the corresponding data object.
    Handle(ActAPI_INode) selected_n = m_model->FindNode(entry);
    //
    if ( selected_n.IsNull() || !selected_n->IsWellFormed() )
    {
      m_progress.SendLogMessage(LogWarn(Normal) << "Selected Node is invalid");
      continue;
    }

    // Populate the output list.
    Nodes->Append(selected_n);
  }

  return true;
}
