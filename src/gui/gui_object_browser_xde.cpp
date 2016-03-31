//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_object_browser_xde.h>

// UI includes
#include <gui_common.h>

// Common includes
#include <common_facilities.h>

// OCCT includes
#pragma warning(push, 0)
#include <TDataStd_Name.hxx>
#include <TDF_Tool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidgetItemIterator>
#pragma warning(pop)

#define TREEVIEW_MINSIZE 200

//-----------------------------------------------------------------------------

inline bool isStringStartsWith(const Handle(TCollection_HAsciiString)& theString,
                               const TCollection_AsciiString&          theStart)
{
  return !theString.IsNull()
      &&  theString->Length() >= theStart.Length()
      &&  memcmp (theString->String().ToCString(), theStart.ToCString(), theStart.Length()) == 0;
}

inline bool isStringContains(const Handle(xde_shape_id)&    theID,
                             const TCollection_AsciiString& theSub)
{
  for ( int i = 1; i <= theID->Top2Bottom().Length(); ++i )
  {
    const TCollection_AsciiString& key = theID->Top2Bottom(i);
    if ( key.IsEqual(theSub) )
      return true;
  }
  return false;
}

//-----------------------------------------------------------------------------

bool isPrsIdInList(const NCollection_Array1<TCollection_AsciiString>& theIds,
                   const Handle(AIS_InteractiveObject)&               thePrs)
{
  Handle(xde_shape_id) ID = Handle(xde_shape_id)::DownCast( thePrs->GetOwner() );
  //
  for (NCollection_Array1<TCollection_AsciiString>::Iterator anIdIter (theIds); anIdIter.More(); anIdIter.Next())
  {
    if (isStringContains (ID, anIdIter.Value()))
    {
      return true;
    }
  }
  return false;
}

bool isPrsCorresponding(const Handle(xde_shape_id)&          treeId,
                        const Handle(AIS_InteractiveObject)& prs)
{
  Handle(xde_shape_id) prsId = Handle(xde_shape_id)::DownCast( prs->GetOwner() );
  //
  const bool isContained = prsId->Contains(treeId);
  return isContained;
}

//-----------------------------------------------------------------------------

//! Creates a new instance of tree view.
//! \param parent [in] parent widget.
gui_object_browser_xde::gui_object_browser_xde(QWidget* parent) : QTreeWidget(parent)
{
  common_facilities::Instance()->ObjectBrowser_XDE = this;

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

//! Destructor.
gui_object_browser_xde::~gui_object_browser_xde()
{}

//-----------------------------------------------------------------------------

//! Populates tree view from the Data Model.
void gui_object_browser_xde::Populate()
{
  // Initialize Model
  if ( common_facilities::Instance()->Model_XDE.IsNull() )
    return;
  //
  m_model = common_facilities::Instance()->Model_XDE;

  // Clean up the existing contents
  this->clear();

  //---------------------------------------------------------------------------
  // Populate tree view
  //---------------------------------------------------------------------------

  this->populateBranch(TCollection_AsciiString(), NULL);
  this->expandAll();
}

//-----------------------------------------------------------------------------

//! Adds all child items under the given root.
//! \param rootEntry [in] root OCAF entry.
//! \param rootUi    [in] root UI entry.
void gui_object_browser_xde::populateBranch(const TCollection_AsciiString& rootEntry,
                                            QTreeWidgetItem*               rootUi)
{
  Handle(TDocStd_Document)  Doc       = m_model->GetDocument();
  Handle(XCAFDoc_ShapeTool) ShapeTool = XCAFDoc_DocumentTool::ShapeTool( Doc->Main() );
  //
  TDF_LabelSequence Labels;
  if ( rootEntry.IsEmpty() )
  {
    // Get list of roots (disappointing "free" term means "root" here)
    ShapeTool->GetFreeShapes(Labels);
  }
  else
  {
    // Access label by its ID
    TDF_Label RootLabel;
    TDF_Tool::Label(Doc->Main().Data(), rootEntry, RootLabel);

    // Access all components of an assembly
    if ( ShapeTool->IsAssembly(RootLabel) )
      ShapeTool->GetComponents(RootLabel, Labels);
  }

  // Iterate over the root entities
  for ( int lit = 1; lit <= Labels.Length(); ++lit )
  {
    TDF_Label L = Labels.Value(lit);
    //
    TCollection_AsciiString RefName, EntryId, RefEntryId, Name;
    Handle(TDataStd_Name) NodeName;

    TDF_Tool::Entry(L, EntryId);
    if ( ShapeTool->IsReference(L) ) // If true, then it is a reference to part + location
    {
      if ( L.FindAttribute(TDataStd_Name::GetID(), NodeName) )
        RefName = TCollection_AsciiString( NodeName->Get() );

      TDF_Label RefLabel;
      if ( ShapeTool->GetReferredShape(L, RefLabel) ) // Get the real underlying part
      {
        L = RefLabel;
        TDF_Tool::Entry(RefLabel, RefEntryId);
      }
    }

    if ( L.FindAttribute(TDataStd_Name::GetID(), NodeName) )
    {
      Name = TCollection_AsciiString( NodeName->Get() );

      // Trim spaces
      RefName.RightAdjust();
      RefName.LeftAdjust();

      if ( !RefName.IsEmpty()           &&
            RefName.Search("=>[0:") != 1 &&
           !IsEqual(RefName, Name)  )
      {
        Name += TCollection_AsciiString (" [") + RefName + "]";
      }
    }

    //
    QTreeWidgetItem* nodeUi = new QTreeWidgetItem( QStringList() << AsciiStr2QStr(Name) );
    nodeUi->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    nodeUi->setData( 0, BrowserRoleNodeId, AsciiStr2QStr(EntryId) );
    //
    if ( rootUi )
      rootUi->addChild(nodeUi);
    else
      this->addTopLevelItem(nodeUi);
    //
    if ( RefEntryId.IsEmpty() )
      RefEntryId = EntryId;

    this->populateBranch(RefEntryId, nodeUi);
  }
}

//-----------------------------------------------------------------------------

//! Reaction on selection in a tree view.
void gui_object_browser_xde::onSelectionChanged()
{
  emit nodeSelected();
}

//-----------------------------------------------------------------------------

//! Reaction on context menu opening.
//! \param pos [in] position.
void gui_object_browser_xde::onContextMenu(QPoint pos)
{
  QList<QTreeWidgetItem*> items = this->selectedItems();
  if ( !items.length() || items.length() > 1 )
    return;

  QMenu* aMenu = new QMenu(this);
  aMenu->addAction( "Show Only", this, SLOT( onShowOnly() ) );
  aMenu->popup( this->mapToGlobal(pos) );
}

//-----------------------------------------------------------------------------

//! Reaction on "show only" action.
void gui_object_browser_xde::onShowOnly()
{
  QList<QTreeWidgetItem*> items = this->selectedItems();
  if ( !items.length() || items.length() > 1 )
    return;

  QTreeWidgetItem* item = items.at(0);
  TCollection_AsciiString entry = QStr2AsciiStr( item->data(0, BrowserRoleNodeId).toString() );
  //
  Handle(xde_shape_id) item_id = new xde_shape_id;
  item_id->Prepend(entry);

  // Access data object's entry
  QTreeWidgetItem* parent = NULL;
  do
  {
    parent = item->parent();
    if ( parent )
    {
      TCollection_AsciiString entry = QStr2AsciiStr( parent->data(0, BrowserRoleNodeId).toString() );
      item_id->Prepend(entry);
    }
    item = parent;
  }
  while ( parent );

  // Get AIS context
  Handle(AIS_InteractiveContext) ctx = common_facilities::Instance()->ViewerDMU->GetContext();

  ctx->DisplayAll(0);

  AIS_ListOfInteractive anDispObjs;
  ctx->DisplayedObjects(anDispObjs);
  for ( AIS_ListIteratorOfListOfInteractive aDispIter (anDispObjs); aDispIter.More(); aDispIter.Next() )
  {
    if (!isPrsCorresponding (item_id, aDispIter.Value()))
    {
      /*if (ctx->IsSelected (aDispIter.Value()))
      {
        ctx->AddOrRemoveSelected (aDispIter.Value(), Standard_False);
      }*/
      ctx->Erase (aDispIter.Value(), 0);
    }
  }

  //// display and select currently erased object in the list
  //AIS_ListOfInteractive anErasedObjs;
  //ctx->ErasedObjects (anErasedObjs);
  //for (AIS_ListIteratorOfListOfInteractive anObjIter (anErasedObjs); anObjIter.More(); anObjIter.Next())
  //{
  //  if (isPrsCorresponding (item_id, anObjIter.Value()))
  //  {
  //   // if ( ctx->IsDisplayed( anObjIter.Value() ) )
  //      //ctx->Erase(anObjIter.Value(), Standard_False);
  //      ctx->Display(anObjIter.Value(), Standard_False);

  //    //ctx->AddOrRemoveSelected (anObjIter.Value(), Standard_False);
  //  }
  //}

  //common_facilities::Instance()->ViewerDMU->GetView()->Invalidate();
  common_facilities::Instance()->ViewerDMU->GetView()->Update();
}
