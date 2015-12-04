//-----------------------------------------------------------------------------
// Created on: 03 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
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
  this->setSelectionMode(QAbstractItemView::SingleSelection);
  this->header()->hide();
  this->setColumnCount(1);

  // Populate
  this->Populate();
}

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

  //---------------------------------------------------------------------------
  // Add root node
  //---------------------------------------------------------------------------

  Handle(common_root_node)
    root_n = Handle(common_root_node)::DownCast( common_facilities::Instance()->Model->GetRootNode() );
  //
  if ( root_n.IsNull() || !root_n->IsWellFormed() )
    return;
  //
  QTreeWidgetItem* root_ui = new QTreeWidgetItem( QStringList() << ExtStr2QStr( root_n->GetName() ) );
  root_ui->setFlags(Qt::ItemIsEnabled);
  //
  this->addTopLevelItem(root_ui);

  //---------------------------------------------------------------------------
  // Add child nodes
  //---------------------------------------------------------------------------

  this->addChildren(root_n, root_ui);
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
    child_ui->setFlags(Qt::ItemIsEnabled);
    //
    root_ui->addChild(child_ui);

    // Repeat recursively
    this->addChildren(child_n, child_ui);
  }
}
