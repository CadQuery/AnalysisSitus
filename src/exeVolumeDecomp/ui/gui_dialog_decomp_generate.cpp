//-----------------------------------------------------------------------------
// Created on: 20 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_dialog_decomp_generate.h>

// GUI includes
#include <gui_common.h>

// Engine includes
#include <engine_volume.h>

// Geometry includes
#include <geom_utils.h>

// Visualization includes
#include <visu_pipeline.h>

// Common includes
#include <common_facilities.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

// VTK includes
#include <vtkActor.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------

// Qt role to store node ID near the list item
#define ListRoleNodeId Qt::UserRole

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param Node   [in] working Node.
//! \param parent [in] parent widget.
gui_dialog_decomp_generate::gui_dialog_decomp_generate(const Handle(geom_volume_node)& Node,
                                                       QWidget*                        parent)
//
: QDialog(parent),
  m_volume(Node)
{
  //---------------------------------------------------------------------------
  // Lists of features
  //---------------------------------------------------------------------------

  // Group box for parameters
  QGroupBox* pKeepGroup    = new QGroupBox("Features to keep");
  QGroupBox* pExcludeGroup = new QGroupBox("Features to exclude");

  // Lists of features
  m_widgets.pFeatures2Keep    = new QListWidget();
  m_widgets.pFeatures2Exclude = new QListWidget();

  // Create layout for the list of features to keep
  QVBoxLayout* pFeatures2KeepLay = new QVBoxLayout(pKeepGroup);
  pFeatures2KeepLay->setSpacing(5);
  pFeatures2KeepLay->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  pFeatures2KeepLay->addWidget(m_widgets.pFeatures2Keep);
  pFeatures2KeepLay->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  // Create layout for the list of features to exclude
  QVBoxLayout* pFeatures2ExcludeLay = new QVBoxLayout(pExcludeGroup);
  pFeatures2ExcludeLay->setSpacing(5);
  pFeatures2ExcludeLay->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  pFeatures2ExcludeLay->addWidget(m_widgets.pFeatures2Exclude);
  pFeatures2ExcludeLay->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  // Reactions
  connect( m_widgets.pFeatures2Keep,    SIGNAL( itemSelectionChanged() ), this, SLOT( onFeatureSelected() ) );
  connect( m_widgets.pFeatures2Exclude, SIGNAL( itemSelectionChanged() ), this, SLOT( onFeatureSelected() ) );

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  // Buttons to transfer features between the lists
  m_widgets.pMove2ExcludeList = new QPushButton(">>");
  m_widgets.pMove2KeepList    = new QPushButton("<<");

  // Button to generate the new part
  m_widgets.pGenerate = new QPushButton("Generate");
  m_widgets.pGenerate->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reactions
  connect( m_widgets.pMove2ExcludeList, SIGNAL( clicked() ), this, SLOT( onMove2Exclude() ) );
  connect( m_widgets.pMove2KeepList,    SIGNAL( clicked() ), this, SLOT( onMove2Keep() ) );
  connect( m_widgets.pGenerate,         SIGNAL( clicked() ), this, SLOT( onGenerate() ) );

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Vertical layout for feature transfer buttons
  QVBoxLayout* pMoveLayout = new QVBoxLayout();
  pMoveLayout->addWidget(m_widgets.pMove2ExcludeList);
  pMoveLayout->addWidget(m_widgets.pMove2KeepList);
  pMoveLayout->setAlignment(Qt::AlignCenter | Qt::AlignLeft);

  // Horizontal layout for lists
  QHBoxLayout* pFeaturesLay = new QHBoxLayout();
  pFeaturesLay->addWidget(pKeepGroup);
  pFeaturesLay->addLayout(pMoveLayout);
  pFeaturesLay->addWidget(pExcludeGroup);
  pFeaturesLay->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  // Main layout
  m_pMainLayout = new QVBoxLayout();
  m_pMainLayout->addLayout(pFeaturesLay);
  m_pMainLayout->addWidget(m_widgets.pGenerate);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Configure generative process");

  // Populate
  this->populateFeatureLists();
}

//! Destructor.
gui_dialog_decomp_generate::~gui_dialog_decomp_generate()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! This method is responsible for moving a feature from one list to another.
void gui_dialog_decomp_generate::onMove2Exclude()
{
  QList<QListWidgetItem*> items = m_widgets.pFeatures2Keep->selectedItems();
  if ( !items.length() || items.length() > 1 )
    return;

  QListWidgetItem*         item = items.at(0);
  TCollection_AsciiString entry = QStr2AsciiStr( item->data( ListRoleNodeId).toString() );

  // Get feature Node
  Handle(ActAPI_INode) feature_n = common_facilities::Instance()->Model->FindNode(entry);

  // Transfer feature
  common_facilities::Instance()->Model->OpenCommand();
  {
    engine_volume::Move_Feature2Excluded(m_volume, feature_n);
  }
  common_facilities::Instance()->Model->CommitCommand();
  //
  this->populateFeatureLists();
}

//-----------------------------------------------------------------------------

//! This method is responsible for moving a feature from one list to another.
void gui_dialog_decomp_generate::onMove2Keep()
{
  QList<QListWidgetItem*> items = m_widgets.pFeatures2Exclude->selectedItems();
  if ( !items.length() || items.length() > 1 )
    return;

  QListWidgetItem*         item = items.at(0);
  TCollection_AsciiString entry = QStr2AsciiStr( item->data( ListRoleNodeId).toString() );

  // Get feature Node
  Handle(ActAPI_INode) feature_n = common_facilities::Instance()->Model->FindNode(entry);

  // Transfer feature
  common_facilities::Instance()->Model->OpenCommand();
  {
    engine_volume::Move_Feature2Kept(m_volume, feature_n);
  }
  common_facilities::Instance()->Model->CommitCommand();
  //
  this->populateFeatureLists();
}

//-----------------------------------------------------------------------------

//! This method is invoked once user requests generation.
void gui_dialog_decomp_generate::onGenerate()
{
  // Take all items for the list of features to keep
  Handle(ActAPI_HDataCursorList) features_n = m_volume->GetFeatures2Keep();

  // Take the stock shape
  Handle(geom_volume_node)
    stock_n = Handle(geom_volume_node)::DownCast( engine_volume::Get_RootVolume() );
  //
  TopoDS_Shape stock = stock_n->GetShape();

  // Cut features one by one
  TopoDS_Shape part = stock;
  for ( int f = 1; f <= features_n->Length(); ++f )
  {
    // Take the feature shape
    Handle(geom_volume_node)
      feature_n = Handle(geom_volume_node)::DownCast( features_n->Value(f) );
    //
    TopoDS_Shape feature = feature_n->GetShape();

    // Cut
    part = geom_utils::BooleanCut(part, feature);
  }

  // Set final geometry
  common_facilities::Instance()->Model->OpenCommand();
  {
    m_volume->SetShape(part);
  }
  common_facilities::Instance()->Model->CommitCommand();

  // Actualize viewer
  common_facilities::Instance()->Prs.DeleteAll();
  common_facilities::Instance()->Prs.Part->Actualize( m_volume.get() );

  // Close
  this->close();
}

//-----------------------------------------------------------------------------

//! This method populates the lists of features being included and excluded
//! for the given volume.
void gui_dialog_decomp_generate::populateFeatureLists()
{
  // Features to keep
  this->populateFeatureList(m_volume->GetFeatures2Keep(), m_widgets.pFeatures2Keep);

  // Features to exclude
  this->populateFeatureList(m_volume->GetFeatures2Exclude(), m_widgets.pFeatures2Exclude);
}

//-----------------------------------------------------------------------------

//! Populates the given list widget with the items corresponding to the passed
//! list of features.
//! \param features [in] list of features to put into the widget.
//! \param widget   [in] target widget.
void gui_dialog_decomp_generate::populateFeatureList(const Handle(ActAPI_HDataCursorList)& features,
                                                     QListWidget*                          widget)
{
  widget->clear();
  //
  if ( !features.IsNull() && !features->IsEmpty() )
  {
    for ( int f = 1; f <= features->Length(); ++f )
    {
      const Handle(ActAPI_IDataCursor)& cursor = features->Value(f);
      if ( cursor.IsNull() || !cursor->IsWellFormed() )
        continue;

      Handle(geom_volume_node)
        feature_n = Handle(geom_volume_node)::DownCast(cursor);

      // Create a list item representing the feature
      QListWidgetItem* item = new QListWidgetItem( ExtStr2QStr( feature_n->GetName() ) );
      item->setData( ListRoleNodeId, AsciiStr2QStr( feature_n->GetId() ) );

      // Add item to the list of features
      widget->addItem(item);
    }
  }
}

//-----------------------------------------------------------------------------

//! Reaction on feature selection.
void gui_dialog_decomp_generate::onFeatureSelected()
{
  QListWidget* widget = dynamic_cast<QListWidget*>( this->sender() );
  //
  if ( !widget )
    return;

  // Get selected item
  QList<QListWidgetItem*> items = widget->selectedItems();
  if ( !items.length() || items.length() > 1 )
    return;
  //
  QListWidgetItem*         item = items.at(0);
  TCollection_AsciiString entry = QStr2AsciiStr( item->data( ListRoleNodeId).toString() );
  //
  Handle(ActAPI_INode) volume_n = common_facilities::Instance()->Model->FindNode(entry);

  /* =====================
   *  Actualize in viewer
   * ===================== */

  // Take presentation manager
  const vtkSmartPointer<visu_prs_manager>& prs_mgr = common_facilities::Instance()->Prs.Part;

  // Remove all possibly existing feature presentations
  Handle(ActAPI_HDataCursorList) volumes = engine_volume::Get_LeafVolumes();
  for ( int v = 1; v <= volumes->Length(); ++v )
  {
    prs_mgr->DeRenderPresentation( volumes->Value(v)->GetId() );
  }

  // Configure presentation of a selected feature
  if ( !prs_mgr->IsPresented(volume_n) )
    prs_mgr->SetPresentation(volume_n);
  //
  Handle(visu_prs)             feature_prs = prs_mgr->GetPresentation(volume_n);
  Handle(h_visu_pipeline_list) pipelines   = feature_prs->GetPipelineList();
  //
  for ( int p = 1; p <= pipelines->Length(); ++p )
  {
    pipelines->Value(p)->Actor()->GetProperty()->SetColor(0.0, 0.0, 1.0);
    pipelines->Value(p)->Actor()->GetProperty()->SetOpacity(0.5);
  }
  //
  prs_mgr->Actualize( volume_n.get() );
}
