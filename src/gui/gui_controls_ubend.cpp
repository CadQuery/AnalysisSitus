//-----------------------------------------------------------------------------
// Created on: 02 February 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_ubend.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (geometry) includes
#include <geom_sections_node.h>
#include <geom_ubend.h>
#include <geom_utils.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>

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

  // Set layout
  m_pMainLayout->addWidget(m_widgets.pBuildUBend);
  //
  this->setLayout(m_pMainLayout);
}

//! Destructor.
gui_controls_ubend::~gui_controls_ubend()
{
  delete m_pMainLayout;
  //
  m_widgets.Release();
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

  // TODO

  // Create path curve
  TopoDS_Edge EPath = geom_ubend::CreatePathTopo();

  // Create sections
  //
  double f, l;
  Handle(Geom_Curve) c3d = BRep_Tool::Curve(EPath, f, l);
  //
  TopoDS_Compound CTop, CRight, CBottom, CLeft;
  geom_ubend::CreateSectionsTopo(c3d, UBEND_NUM_SECTIONS, geom_ubend::Side_Top,    CTop);
  geom_ubend::CreateSectionsTopo(c3d, UBEND_NUM_SECTIONS, geom_ubend::Side_Right,  CRight);
  geom_ubend::CreateSectionsTopo(c3d, UBEND_NUM_SECTIONS, geom_ubend::Side_Bottom, CBottom);
  geom_ubend::CreateSectionsTopo(c3d, UBEND_NUM_SECTIONS, geom_ubend::Side_Left,   CLeft);

  // Build a single compound of all sections
  TopoDS_Compound CSections;
  BRep_Builder().MakeCompound(CSections);
  BRep_Builder().Add(CSections, CTop);
  BRep_Builder().Add(CSections, CRight);
  BRep_Builder().Add(CSections, CBottom);
  BRep_Builder().Add(CSections, CLeft);

  // Build skins
  TopoDS_Compound CSkins;
  geom_ubend::CreateSkinsTopo(CTop, CRight, CBottom, CLeft, CSkins);

  // Store geometry
  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    ubend_n->SetPath     (EPath);
    ubend_n->SetSections (CSections);
    ubend_n->SetSkin     (CSkins);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit

  // Update Presentation
  common_facilities::Instance()->Prs.UBend->Actualize( ubend_n.get(), false, true );
}
