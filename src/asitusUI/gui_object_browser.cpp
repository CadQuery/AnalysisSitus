//-----------------------------------------------------------------------------
// Created on: 03 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_object_browser.h>

// UI includes
#include <gui_common.h>

// Common includes
#include <common_facilities.h>

// Qt includes
#pragma warning(push, 0)
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidgetItemIterator>
#pragma warning(pop)

#define TREEVIEW_MINSIZE 200

//-----------------------------------------------------------------------------

//! Creates a new instance of tree view.
//! \param parent [in] parent widget.
gui_object_browser::gui_object_browser(QWidget* parent) : QTreeWidget(parent)
{
  common_facilities::Instance()->ObjectBrowser = this;

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
gui_object_browser::~gui_object_browser()
{}

//-----------------------------------------------------------------------------

//! Populates tree view from the Data Model.
void gui_object_browser::Populate()
{
  if ( common_facilities::Instance()->Model.IsNull() )
    return;

  // Clean up the existing contents
  this->clear();

  // Add root node
  Handle(common_root_node)
    root_n = Handle(common_root_node)::DownCast( common_facilities::Instance()->Model->GetRootNode() );
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
void gui_object_browser::SetSelected(const ActAPI_DataObjectId& nodeId)
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
void gui_object_browser::addChildren(const Handle(ActAPI_INode)& root_n,
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
void gui_object_browser::onSelectionChanged()
{
  emit nodeSelected();
}

//-----------------------------------------------------------------------------

//! Reaction on context menu opening.
//! \param pos [in] position.
void gui_object_browser::onContextMenu(QPoint pos)
{
  QMenu* aMenu = new QMenu(this);
  aMenu->addAction( "Show Only", this, SLOT( onShowOnly() ) );
  aMenu->popup( this->mapToGlobal(pos) );
}

//-----------------------------------------------------------------------------

//! Reaction on "show only" action.
void gui_object_browser::onShowOnly()
{
  Handle(ActAPI_INode) selected_n;
  if ( !this->selectedNode(selected_n) ) return;

  // Actualize viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->Actualize( selected_n.get() );
}

//-----------------------------------------------------------------------------

//! Returns the currently active Node.
//! \param Node [out] requested Node.
//! \return true in case of success, false -- otherwise.
bool gui_object_browser::selectedNode(Handle(ActAPI_INode)& Node) const
{
  QList<QTreeWidgetItem*> items = this->selectedItems();
  if ( !items.length() || items.length() > 1 )
    return false;

  QTreeWidgetItem* item = items.at(0);
  TCollection_AsciiString entry = QStr2AsciiStr( item->data(0, BrowserRoleNodeId).toString() );

  // Take the corresponding data object
  Handle(ActAPI_INode)
    selected_n = common_facilities::Instance()->Model->FindNode(entry);
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
