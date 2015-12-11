//-----------------------------------------------------------------------------
// Created on: 08 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_section.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (geometry) includes
#include <geom_section_node.h>
#include <geom_utils.h>

// Active Data includes
#include <ActData_UniqueNodeName.h>

// Qt includes
#pragma warning(push, 0)
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTabWidget>
#pragma warning(pop)

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------

//! Takes care of proper creation of a Section Node which would contain
//! geometry of a single section for skinning.
//! \param shape [in] geometry to initialize the Section Node with.
//! \return instance of the just created Node.
static Handle(geom_section_node) CreateSectionNode(const TopoDS_Shape& shape)
{
  Handle(geom_section_node) section_n;
  //
  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    // Add Section Node to Partition
    Handle(ActAPI_INode) section_base = geom_section_node::Instance();
    common_facilities::Instance()->Model->SectionPartition()->AddNode(section_base);

    // Initialize
    section_n = Handle(geom_section_node)::DownCast(section_base);
    section_n->Init();
    section_n->SetGeometry(shape);

    // Set as a child for root
    Handle(geom_sections_node) group = common_facilities::Instance()->Model->SectionsNode();
    group->AddChildNode(section_n);

    // Choose unique name
    TCollection_ExtendedString name("Section");
    name = ActData_UniqueNodeName::Generate( ActData_SiblingNodes::CreateForChild(section_n, group),
                                             name );
    section_n->SetName(name);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit
  //
  return section_n;
}

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_section::gui_controls_section(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QHBoxLayout();

  // Left layout
  QVBoxLayout* pLeftLayout = new QVBoxLayout();
  pLeftLayout->setSpacing(5);

  // Group box for transformation
  QGroupBox* pGroup = new QGroupBox("Placement");

  // Editors
  m_widgets.pTx = new gui_line_edit();
  m_widgets.pTy = new gui_line_edit();
  m_widgets.pTz = new gui_line_edit();
  m_widgets.pRx = new gui_line_edit();
  m_widgets.pRy = new gui_line_edit();
  m_widgets.pRz = new gui_line_edit();

  // Sizing
  m_widgets.pTx->setMaximumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pTy->setMaximumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pTz->setMaximumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pRx->setMaximumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pRy->setMaximumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pRz->setMaximumWidth(CONTROL_EDIT_WIDTH);

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pCreateSection = new QPushButton("Create Section");

  // Sizing
  m_widgets.pCreateSection->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reaction
  connect( m_widgets.pCreateSection, SIGNAL( clicked() ), this, SLOT( onCreateSection() ) );

  //---------------------------------------------------------------------------
  // Line editors
  //---------------------------------------------------------------------------

  // Create layout
  QGridLayout* pTxGrid = new QGridLayout(pGroup);
  pTxGrid->setSpacing(5);
  pTxGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  pTxGrid->addWidget(new QLabel("Tx:"), 0, 0);
  pTxGrid->addWidget(new QLabel("Ty:"), 1, 0);
  pTxGrid->addWidget(new QLabel("Tz:"), 2, 0);
  pTxGrid->addWidget(new QLabel("Rx:"), 3, 0);
  pTxGrid->addWidget(new QLabel("Ry:"), 4, 0);
  pTxGrid->addWidget(new QLabel("Rz:"), 5, 0);
  //
  pTxGrid->addWidget(m_widgets.pTx, 0, 1);
  pTxGrid->addWidget(m_widgets.pTy, 1, 1);
  pTxGrid->addWidget(m_widgets.pTz, 2, 1);
  pTxGrid->addWidget(m_widgets.pRx, 3, 1);
  pTxGrid->addWidget(m_widgets.pRy, 4, 1);
  pTxGrid->addWidget(m_widgets.pRz, 5, 1);
  //
  pTxGrid->addWidget(m_widgets.pTy, 1, 1);
  pTxGrid->addWidget(m_widgets.pTz, 2, 1);
  pTxGrid->addWidget(m_widgets.pRx, 3, 1);
  pTxGrid->addWidget(m_widgets.pRy, 4, 1);
  pTxGrid->addWidget(m_widgets.pRz, 5, 1);
  //
  pTxGrid->setColumnStretch(0, 0);
  pTxGrid->setColumnStretch(1, 0);

  //---------------------------------------------------------------------------
  // Tab view with circular profile definition
  //---------------------------------------------------------------------------

  QTabWidget* pTabWidget = new QTabWidget();

  // Circular section definition tab
  QWidget* pCircleTab = new QWidget();
  //
  m_widgets.pCircleRad = new gui_line_edit();
  //
  QLabel*      pCircleLabel  = new QLabel("Radius:");
  QHBoxLayout* pCircleLayout = new QHBoxLayout();
  //
  m_widgets.pCircleRad->setMaximumWidth(CONTROL_EDIT_WIDTH);
  //
  pCircleLayout->addWidget(pCircleLabel);
  pCircleLayout->addWidget(m_widgets.pCircleRad);
  pCircleLayout->addStretch();
  pCircleLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  pCircleTab->setLayout(pCircleLayout);

  // Populate tabs
  pTabWidget->addTab(pCircleTab, "Circular");

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Configure left layout
  pLeftLayout->addWidget(m_widgets.pCreateSection);
  pLeftLayout->addWidget(pGroup);
  pLeftLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  // Configure main layout
  m_pMainLayout->addLayout(pLeftLayout);
  m_pMainLayout->addWidget(pTabWidget);
  //
  m_pMainLayout->setStretchFactor(pLeftLayout, 0);
  m_pMainLayout->setStretchFactor(pTabWidget, 1);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);
  //
  this->setLayout(m_pMainLayout);

  //---------------------------------------------------------------------------

  // Configure reaction on selection of items in a tree view
  connect( common_facilities::Instance()->ObjectBrowser, SIGNAL( nodeSelected() ),
           this,                                         SLOT( onBrowserSelection() ) );
}

//! Destructor.
gui_controls_section::~gui_controls_section()
{
  delete m_pMainLayout;
  //
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Reaction to a section creation request.
void gui_controls_section::onCreateSection()
{
  Handle(geom_section_node)
    section_n = ::CreateSectionNode( geom_utils::CreateCircularWire(1.0) );

  // Update browser
  common_facilities::Instance()->ObjectBrowser->Populate();
  common_facilities::Instance()->ObjectBrowser->SetSelected( section_n->GetId() );

  // Update Presentation
  common_facilities::Instance()->Prs.Section->Actualize( common_facilities::Instance()->CurrentNode );
  common_facilities::Instance()->Prs.Skinner->Actualize( common_facilities::Instance()->Model->SectionsNode().get(), false, true );
}

//-----------------------------------------------------------------------------

//! Reaction on selection in a browser.
void gui_controls_section::onBrowserSelection()
{
  QList<QTreeWidgetItem*> selected = common_facilities::Instance()->ObjectBrowser->selectedItems();
  if ( selected.size() != 1 )
    return;

  // Get ID of the selected Node
  QTreeWidgetItem* pItem = selected[0];
  ActAPI_DataObjectId id = QStr2AsciiStr( pItem->data(0, BrowserRoleNodeId).toString() );

  // Get instance of the Data Model
  Handle(ActAPI_INode) node = common_facilities::Instance()->Model->FindNode(id);
  if ( node.IsNull() || !node->IsWellFormed() )
    return;

  // Set current Node
  common_facilities::Instance()->CurrentNode = node;

  // Actualize Presentations
  if ( common_facilities::Instance()->Prs.Section )
    common_facilities::Instance()->Prs.Section->DeRenderAllPresentations();
  //
  if ( node->IsInstance( STANDARD_TYPE(geom_section_node) ) )
  {
    common_facilities::Instance()->Prs.Section->Actualize( node.get() );

    // Remove possibly existing Data-Views
    for ( int k = 0; k < m_DV.size(); ++k )
    {
      delete m_DV[k]; m_DV[k] = NULL;
    }
    m_DV.clear();

    // Construct Data-View
    for ( int k = 0; k < t_widgets::LastEditor; ++k )
    {
      gui_prim_data_view* pDV = new gui_prim_data_view(this);
      m_DV.push_back(pDV);

      // We have to listen for editing event from the DV in order to
      // adapt Presentations
      connect( pDV, SIGNAL( valueChanged(QWidget*) ), this, SLOT( onValueChanged() ) );
    }

    // Initialize Data-Views
    Handle(geom_section_node)
      sct_n = Handle(geom_section_node)::DownCast(common_facilities::Instance()->CurrentNode);
    //
    if ( sct_n.IsNull() || !sct_n->IsWellFormed() ) return;
    //
    (*m_DV[t_widgets::edit_Tx])      << new gui_prim_data_adaptor(sct_n, geom_section_node::PID_Tx)           << m_widgets.pTx;
    (*m_DV[t_widgets::edit_Ty])      << new gui_prim_data_adaptor(sct_n, geom_section_node::PID_Ty)           << m_widgets.pTy;
    (*m_DV[t_widgets::edit_Tz])      << new gui_prim_data_adaptor(sct_n, geom_section_node::PID_Tz)           << m_widgets.pTz;
    (*m_DV[t_widgets::edit_Rx])      << new gui_prim_data_adaptor(sct_n, geom_section_node::PID_Rx)           << m_widgets.pRx;
    (*m_DV[t_widgets::edit_Ry])      << new gui_prim_data_adaptor(sct_n, geom_section_node::PID_Ry)           << m_widgets.pRy;
    (*m_DV[t_widgets::edit_Rz])      << new gui_prim_data_adaptor(sct_n, geom_section_node::PID_Rz)           << m_widgets.pRz;
    (*m_DV[t_widgets::edit_CircRad]) << new gui_prim_data_adaptor(sct_n, geom_section_node::PID_CircleRadius) << m_widgets.pCircleRad;

    // Read data to views
    for ( int k = 0; k < t_widgets::LastEditor; ++k )
      m_DV[k]->DataToView();
  }
}

//-----------------------------------------------------------------------------

//! Reaction to changing any value managed by Data-View.
void gui_controls_section::onValueChanged()
{
  Handle(geom_section_node)
    section_n = Handle(geom_section_node)::DownCast( common_facilities::Instance()->CurrentNode );

  if ( section_n.IsNull() || !section_n->IsWellFormed() )
    return;

  //---------------------------------------------------------------------------
  // Now we have a specific processing for radius. We may actually use a Tree
  // function to build a circle, but let's keep a simple approach here
  if ( this->sender() == m_DV[t_widgets::edit_CircRad] )
  {
    TopoDS_Shape W = geom_utils::CreateCircularWire( section_n->GetRadius() );
    //
    common_facilities::Instance()->Model->OpenCommand(); // tx start
    {
      section_n->SetGeometry(W);
    }
    common_facilities::Instance()->Model->CommitCommand(); // tx commit
  }

  // Update Presentations
  common_facilities::Instance()->Prs.Section->Actualize( section_n.get(), false, false );
  common_facilities::Instance()->Prs.Skinner->Actualize( common_facilities::Instance()->Model->SectionsNode().get(), false, true );
}
