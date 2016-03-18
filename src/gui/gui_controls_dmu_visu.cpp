//-----------------------------------------------------------------------------
// Created on: 19 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_dmu_visu.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// OCCT includes
#include <AIS_DisplayMode.hxx>
#include <OSD_Environment.hxx>
#include <Prs3d_ShadingAspect.hxx>

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_dmu_visu::gui_controls_dmu_visu(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QGridLayout();

  // Get directory containing resources
  TCollection_AsciiString ico_dir = OSD_Environment("AS_RESOURCES").Value();
  ico_dir += "/";

  // Buttons
  m_widgets.pToggleLinks = new QToolButton(this);
  m_widgets.pToggleLinks->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_visu_mesh_links.png" ) );
  m_widgets.pToggleLinks->setToolButtonStyle(Qt::ToolButtonIconOnly);
  m_widgets.pToggleLinks->setIconSize( QSize(32, 32) );
  m_widgets.pToggleLinks->setToolTip("Render facet links");
  //
  m_widgets.pToggleEdges = new QToolButton(this);
  m_widgets.pToggleEdges->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_visu_mesh_topo.png" ) );
  m_widgets.pToggleEdges->setToolButtonStyle(Qt::ToolButtonIconOnly);
  m_widgets.pToggleEdges->setIconSize( QSize(32, 32) );
  m_widgets.pToggleEdges->setToolTip("Render CAD topology");
  //
  m_widgets.pToggleTransparency = new QToolButton(this);
  m_widgets.pToggleTransparency->setIcon( QIcon( AsciiStr2QStr(ico_dir) + "toolbar_visu_transparency.png" ) );
  m_widgets.pToggleTransparency->setToolButtonStyle(Qt::ToolButtonIconOnly);
  m_widgets.pToggleTransparency->setIconSize( QSize(32, 32) );
  m_widgets.pToggleTransparency->setToolTip("Transparency");

  // Set layout
  m_pMainLayout->setSpacing(0);
  //
  m_pMainLayout->addWidget(m_widgets.pToggleLinks,        0, 0);
  m_pMainLayout->addWidget(m_widgets.pToggleEdges,        0, 1);
  m_pMainLayout->addWidget(m_widgets.pToggleTransparency, 1, 0);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pToggleLinks,        SIGNAL( clicked() ), SLOT( onToggleLinks        () ) );
  connect( m_widgets.pToggleEdges,        SIGNAL( clicked() ), SLOT( onToggleEdges        () ) );
  connect( m_widgets.pToggleTransparency, SIGNAL( clicked() ), SLOT( onToggleTransparency () ) );
}

//! Destructor.
gui_controls_dmu_visu::~gui_controls_dmu_visu()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

//! On toggle links.
void gui_controls_dmu_visu::onToggleLinks()
{
  const visu_xde_shapes& shapes = common_facilities::Instance()->ViewerDMU->Shapes;
  for ( visu_xde_shapes::Iterator it(shapes); it.More(); it.Next() )
  {
    Handle(visu_xde_shape_prs) prs = Handle(visu_xde_shape_prs)::DownCast( it.Value() );
    //
    prs->DefaultStyle().SetMeshEdges( !prs->DefaultStyle().GetMeshEdges() );
    prs->ApplyEdgesStyle();
    //
    common_facilities::Instance()->ViewerDMU->GetContext()->Erase(prs, 0);
    common_facilities::Instance()->ViewerDMU->GetContext()->Redisplay(prs, 0, 1);
  }
  common_facilities::Instance()->ViewerDMU->GetContext()->UpdateCurrentViewer();
}

//! On toggle edges.
void gui_controls_dmu_visu::onToggleEdges()
{
  const visu_xde_shapes& shapes = common_facilities::Instance()->ViewerDMU->Shapes;
  for ( visu_xde_shapes::Iterator it(shapes); it.More(); it.Next() )
  {
    Handle(visu_xde_shape_prs) prs = Handle(visu_xde_shape_prs)::DownCast( it.Value() );
    //
    prs->DefaultStyle().SetTopoEdges( !prs->DefaultStyle().GetTopoEdges() );
    prs->ApplyEdgesStyle();
    //
    common_facilities::Instance()->ViewerDMU->GetContext()->Erase(prs, 0);
    common_facilities::Instance()->ViewerDMU->GetContext()->Redisplay(prs, 0, 1);
  }
  common_facilities::Instance()->ViewerDMU->GetContext()->UpdateCurrentViewer();
}

//! On toggle transparency.
void gui_controls_dmu_visu::onToggleTransparency()
{
  const visu_xde_shapes& shapes = common_facilities::Instance()->ViewerDMU->Shapes;
  for ( visu_xde_shapes::Iterator it(shapes); it.More(); it.Next() )
  {
    Handle(visu_xde_shape_prs) prs = Handle(visu_xde_shape_prs)::DownCast( it.Value() );
    //
    if ( prs->IsTransparent() )
      common_facilities::Instance()->ViewerDMU->GetContext()->UnsetTransparency(prs);
    else
      common_facilities::Instance()->ViewerDMU->GetContext()->SetTransparency(prs, 0.5);
    //
    common_facilities::Instance()->ViewerDMU->GetContext()->Erase(prs, 0);
    common_facilities::Instance()->ViewerDMU->GetContext()->Redisplay(prs, 0, 1);
  }
  common_facilities::Instance()->ViewerDMU->GetContext()->UpdateCurrentViewer();
}
