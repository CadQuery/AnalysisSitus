//-----------------------------------------------------------------------------
// Created on: 02 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_ubend.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (calculus) includes
#include <calculus_design_law_node.h>

// A-Situs (geometry) includes
#include <geom_sections_node.h>
#include <geom_STEP.h>
#include <geom_ubend.h>
#include <geom_ubend_law_node.h>
#include <geom_utils.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_ubend::gui_controls_ubend(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // "Do everything" button
  m_widgets.pBuildUBend = new QPushButton("Build u-bend");
  m_widgets.pBuildUBend->setMaximumWidth(CONTROL_BTN_WIDTH);
  //
  connect( m_widgets.pBuildUBend, SIGNAL( clicked() ), this, SLOT( onDoTheMagic() ) );

  // STEP export button
  m_widgets.pSaveSTEP = new QPushButton("Save to STEP");
  m_widgets.pSaveSTEP->setMaximumWidth(CONTROL_BTN_WIDTH);
  //
  connect( m_widgets.pSaveSTEP, SIGNAL( clicked() ), this, SLOT( onSaveSTEP() ) );

  // Set layout
  m_pMainLayout->addWidget(m_widgets.pBuildUBend);
  m_pMainLayout->addWidget(m_widgets.pSaveSTEP);
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Visualize sections
  this->RenderSections();

  //---------------------------------------------------------------------------

  // Configure reaction on selection of items in a tree view
  connect( common_facilities::Instance()->ObjectBrowser, SIGNAL( nodeSelected() ),
           this,                                         SLOT( onBrowserSelection() ) );
}

//! Destructor.
gui_controls_ubend::~gui_controls_ubend()
{
  delete m_pMainLayout;
  //
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Prepares initial sections for the u-bend.
void gui_controls_ubend::RenderSections()
{
  Handle(geom_ubend_node)
    ubend_n = common_facilities::Instance()->Model->UBendNode();
  //
  if ( ubend_n.IsNull() || !ubend_n->IsWellFormed() )
    return;

  // Update Presentation
  common_facilities::Instance()->Prs.UBend->Actualize(ubend_n.get(), false, true);
}

//-----------------------------------------------------------------------------

//! Reaction to modeling request.
void gui_controls_ubend::onDoTheMagic()
{
  Handle(geom_ubend_node)
    ubend_n = common_facilities::Instance()->Model->UBendNode();
  //
  if ( ubend_n.IsNull() || !ubend_n->IsWellFormed() )
    return;

  // Access sections
  TopoDS_Compound C = TopoDS::Compound( ubend_n->GetSections() );
  TopoDS_Compound CTop, CRight, CBottom, CLeft;
  //
  TopoDS_Iterator it(C);
  CTop    = TopoDS::Compound( it.Value() ); it.Next();
  CRight  = TopoDS::Compound( it.Value() ); it.Next();
  CBottom = TopoDS::Compound( it.Value() ); it.Next();
  CLeft   = TopoDS::Compound( it.Value() );

  // Build skins
  TopoDS_Compound CSkins;
  geom_ubend::CreateSkinsTopo(CTop, CRight, CBottom, CLeft, CSkins);

  // Sew faces
  TopoDS_Shape SewedSkins = geom_ubend::AsShell(CSkins);

  // Store geometry
  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    ubend_n->SetSkin(SewedSkins);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit

  // Update Presentation
  common_facilities::Instance()->Prs.UBend->Actualize(ubend_n.get(), false, true);
}

//-----------------------------------------------------------------------------

//! Reaction on selection in a browser.
void gui_controls_ubend::onBrowserSelection()
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

  //---------------------------------------------------------------------------

  // Clear all Presentations
  if ( common_facilities::Instance()->Prs.UBend_Xt )
    common_facilities::Instance()->Prs.UBend_Xt->DeRenderAllPresentations();
  //
  if ( common_facilities::Instance()->Prs.UBend_Yt )
    common_facilities::Instance()->Prs.UBend_Yt->DeRenderAllPresentations();
  //
  if ( common_facilities::Instance()->Prs.UBend_XY )
    common_facilities::Instance()->Prs.UBend_XY->DeRenderAllPresentations();

  //---------------------------------------------------------------------------

  // Actualize Presentations
  if ( node->IsInstance( STANDARD_TYPE(calculus_design_law_node) ) )
  {
    vtkSmartPointer<visu_prs_manager> prs_mgr;

    // Check if the law corresponds to X or Y
    Handle(geom_ubend_law_node)
      law_n = Handle(geom_ubend_law_node)::DownCast( node->GetParentNode() );
    //
    Handle(ActAPI_IChildIterator) cit = law_n->GetChildIterator();
    if ( cit->Value()->GetId() == node->GetId() )
      prs_mgr = common_facilities::Instance()->Prs.UBend_Xt;
    else
      prs_mgr = common_facilities::Instance()->Prs.UBend_Yt;

    // Visualize law as explicit function
    prs_mgr->Actualize( node.get(), false, true, true );
  }
  else if ( node->IsInstance( STANDARD_TYPE(geom_ubend_law_node) ) )
  {
    // Visualize law as parametric function
    common_facilities::Instance()->Prs.UBend_XY->Actualize( node.get(), false, true, true );

    // Visualize two explicit functions
    Handle(geom_ubend_law_node) ubend_law_n = Handle(geom_ubend_law_node)::DownCast(node);
    //
    common_facilities::Instance()->Prs.UBend_Xt->Actualize( ubend_law_n->GetLaw_X().get(), false, true, true );
    common_facilities::Instance()->Prs.UBend_Yt->Actualize( ubend_law_n->GetLaw_Y().get(), false, true, true );
  }
}

//! Saves U-bend to STEP file.
void gui_controls_ubend::onSaveSTEP()
{
  QString filename = gui_common::selectSTEPFile(gui_common::OpenSaveAction_Save);

  // Access U-bend Node
  Handle(geom_ubend_node) N = common_facilities::Instance()->Model->UBendNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return;

  // Save to STEP
  if ( !geom_STEP::Write( N->GetSkin(), QStr2AsciiStr(filename) ) )
  {
    std::cout << "Error: cannot save U-bend geometry to STEP file" << std::endl;
    return;
  }
}
