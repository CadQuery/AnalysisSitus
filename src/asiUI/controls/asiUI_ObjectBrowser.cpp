//-----------------------------------------------------------------------------
// Created on: 03 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ObjectBrowser.h>

// UI includes
#include <asiUI_Common.h>

// Qt includes
#pragma warning(push, 0)
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidgetItemIterator>
#pragma warning(pop)

#define TREEVIEW_MINSIZE 200

//-----------------------------------------------------------------------------

//! Creates a new instance of tree view.
//! \param model       [in] Data Model instance.
//! \param pPartViewer [in] part viewer.
//! \param parent      [in] parent widget.
asiUI_ObjectBrowser::asiUI_ObjectBrowser(const Handle(ActAPI_IModel)& model,
                                         asiUI_ViewerPart*            pPartViewer,
                                         QWidget*                     parent)
: QTreeWidget(parent), m_model(model), m_pPartViewer(pPartViewer)
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
    QString data = (*it)->data(0, BrowserRoleNodeId).toString();
    if ( QStr2AsciiStr(data) == nodeId )
    {
      (*it)->setSelected(true);
      break;
    }
    else
      ++it;
  }
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

  if ( m_pPartViewer && m_pPartViewer->PrsMgr()->IsPresented(selected_n) )
  {
    m_pPartViewer->PrsMgr()->DeRenderPresentation(selected_n);
    m_pPartViewer->PrsMgr()->Actualize(selected_n);
  }

  emit show( selected_n->GetId() );
}

//-----------------------------------------------------------------------------

//! Reaction on "show only" action.
void asiUI_ObjectBrowser::onShowOnly()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  if ( m_pPartViewer && m_pPartViewer->PrsMgr()->IsPresented(selected_n) )
  {
    m_pPartViewer->PrsMgr()->DeRenderAllPresentations();
    m_pPartViewer->PrsMgr()->Actualize(selected_n);
  }

  emit showOnly( selected_n->GetId() );
}

//-----------------------------------------------------------------------------

//! Reaction on "hide" action.
void asiUI_ObjectBrowser::onHide()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  if ( m_pPartViewer && m_pPartViewer->PrsMgr()->IsPresented(selected_n) )
  {
    m_pPartViewer->PrsMgr()->DeRenderPresentation(selected_n);
    m_pPartViewer->Repaint();
  }

  emit hide( selected_n->GetId() );
}

//-----------------------------------------------------------------------------

//! Populates context menu with actions.
//! \param activeNode [in]      currently active Node.
//! \param pMenu      [in, out] menu to populate.
void asiUI_ObjectBrowser::populateContextMenu(const Handle(ActAPI_INode)& activeNode,
                                              QMenu*                      pMenu)
{
  if ( m_pPartViewer && m_pPartViewer->PrsMgr()->IsPresented(activeNode) )
  {
    pMenu->addAction( "Show",      this, SLOT( onShow()     ) );
    pMenu->addAction( "Show Only", this, SLOT( onShowOnly() ) );
    pMenu->addAction( "Hide",      this, SLOT( onHide()     ) );
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
