//-----------------------------------------------------------------------------
// Created on: 13 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_decomposition_browser.h>

// UI includes
#include <gui_common.h>
#include <gui_dialog_decomp_generate.h>

// Engine includes
#include <engine_volume.h>

// Common includes
#include <common_facilities.h>

// Qt includes
#pragma warning(push, 0)
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidgetItemIterator>
#pragma warning(pop)

// VTK includes
#include <vtkActor.h>
#include <vtkProperty.h>

#define TREEVIEW_MINSIZE 200

//-----------------------------------------------------------------------------

//! Creates a new instance of tree view.
//! \param parent [in] parent widget.
gui_decomposition_browser::gui_decomposition_browser(QWidget* parent) : QTreeWidget(parent)
{
  common_facilities::Instance()->DecompBrowser = this;

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
gui_decomposition_browser::~gui_decomposition_browser()
{}

//-----------------------------------------------------------------------------

//! Populates tree view from the Data Model.
void gui_decomposition_browser::Populate()
{
  Handle(common_model) M = common_facilities::Instance()->Model;
  //
  if ( M.IsNull() )
    return;

  // Clean up the existing contents
  this->clear();

  // Find root volumes
  Handle(ActAPI_HNodeList) rootNodes = new ActAPI_HNodeList;
  //
  for ( ActData_BasePartition::Iterator it( M->GetGeomVolumePartition() ); it.More(); it.Next() )
  {
    if ( it.Value()->GetParentNode().IsNull() )
      rootNodes->Append( it.Value() );
  }

  // Add tree items
  for ( int n = 1; n <= rootNodes->Length(); ++n )
  {
    const Handle(ActAPI_INode) root_n = rootNodes->Value(n);
    //
    QTreeWidgetItem* root_ui = new QTreeWidgetItem( QStringList() << ExtStr2QStr( root_n->GetName() ) );
    root_ui->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    root_ui->setData( 0, BrowserRoleNodeId, AsciiStr2QStr( root_n->GetId() ) );
    //
    this->addTopLevelItem(root_ui);
    this->addChildren(root_n, root_ui);
  }

  // Expand tree
  this->expandAll();
}

//-----------------------------------------------------------------------------

//! Searches for an item with the given index and set that item selected.
//! \param nodeId [in] target Node ID.
void gui_decomposition_browser::SetSelected(const ActAPI_DataObjectId& nodeId)
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

//! Returns selected item.
//! \return ID of the selected Node.
ActAPI_DataObjectId gui_decomposition_browser::GetSelected() const
{
  QList<QTreeWidgetItem*> items = this->selectedItems();
  if ( !items.length() || items.length() > 1 )
    return ActAPI_DataObjectId();

  QTreeWidgetItem*    item = items.at(0);
  ActAPI_DataObjectId id   = QStr2AsciiStr( item->data(0, BrowserRoleNodeId).toString() );
  //
  return id;
}

//-----------------------------------------------------------------------------

//! Adds all child items under the given root.
//! \param root_n  [in] root Node in a Data Model.
//! \param root_ui [in] root item in a tree view.
void gui_decomposition_browser::addChildren(const Handle(ActAPI_INode)& root_n,
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
void gui_decomposition_browser::onSelectionChanged()
{
  emit nodeSelected();
}

//-----------------------------------------------------------------------------

//! Reaction on context menu opening.
//! \param pos [in] position.
void gui_decomposition_browser::onContextMenu(QPoint pos)
{
  Handle(geom_volume_node) volume_n;
  if ( !this->volumeNode(volume_n) ) return;

  QMenu* aMenu = new QMenu(this);

  // Recognize a part by the Node's name
  TCollection_AsciiString name = volume_n->GetName();
  name.LowerCase();
  if ( name.Search("part") != -1 )
    aMenu->addAction( "Generate", this, SLOT( onGenerate() ) );

  // Complete context menu
  aMenu->addAction( "Show Only", this, SLOT( onShowOnly() ) );
  aMenu->popup( this->mapToGlobal(pos) );
}

//-----------------------------------------------------------------------------

//! Reaction on "show only" action.
void gui_decomposition_browser::onShowOnly()
{
  Handle(geom_volume_node) volume_n;
  if ( !this->volumeNode(volume_n) ) return;

  // Actualize viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->Actualize( volume_n.get() );
}

//-----------------------------------------------------------------------------

//! Reaction on "generate" action.
void gui_decomposition_browser::onGenerate()
{
  Handle(geom_volume_node) volume_n;
  if ( !this->volumeNode(volume_n) ) return;

  Handle(ActAPI_INode) stock_n = engine_volume::Get_RootVolume();

  // Prepare visualization facilities: show transparent stock
  const vtkSmartPointer<visu_prs_manager>& prs_mgr = common_facilities::Instance()->Prs.Part;
  prs_mgr->DeRenderAllPresentations();
  //
  if ( !prs_mgr->IsPresented(stock_n) )
    prs_mgr->SetPresentation(stock_n);
  //
  //
  Handle(visu_prs)             stock_prs = prs_mgr->GetPresentation(stock_n);
  Handle(h_visu_pipeline_list) pipelines = stock_prs->GetPipelineList();
  //
  for ( int p = 1; p <= pipelines->Length(); ++p )
  {
    pipelines->Value(p)->Actor()->GetProperty()->SetColor(1.0, 1.0, 0.0);
    pipelines->Value(p)->Actor()->GetProperty()->SetOpacity(0.5);
  }
  //
  prs_mgr->Actualize( stock_n.get() );

  // Show features processing dialog
  gui_dialog_decomp_generate* genDlg = new gui_dialog_decomp_generate(volume_n);
  genDlg->show();
}

//-----------------------------------------------------------------------------

//! Returns the currently active Volume Node.
//! \param Node [out] requested Node.
//! \return true in case of success, false -- otherwise.
bool gui_decomposition_browser::volumeNode(Handle(geom_volume_node)& Node) const
{
  QList<QTreeWidgetItem*> items = this->selectedItems();
  if ( !items.length() || items.length() > 1 )
    return false;

  QTreeWidgetItem* item = items.at(0);
  TCollection_AsciiString entry = QStr2AsciiStr( item->data(0, BrowserRoleNodeId).toString() );

  // Take the corresponding data object
  Handle(geom_volume_node)
    volume_n = Handle(geom_volume_node)::DownCast( common_facilities::Instance()->Model->FindNode(entry) );
  //
  if ( volume_n.IsNull() || !volume_n->IsWellFormed() )
  {
    std::cout << "Error: selected Node is of unexpected type" << std::endl;
    return false;
  }

  // Set result
  Node = volume_n;
  return true;
}
