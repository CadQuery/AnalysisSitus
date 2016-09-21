//-----------------------------------------------------------------------------
// Created on: 16 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_cc.h>

// Common includes
#include <common_draw_test_suite.h>
#include <common_facilities.h>

// Geometry includes
#include <geom_bvh_facets.h>
#include <geom_find_nearest_facet.h>

// GUI includes
#include <gui_common.h>

// Engine includes
#include <engine_part.h>

// Visualization includes
#include <visu_create_contour_callback.h>
#include <visu_geom_prs.h>

// VTK includes
#include <vtkActor.h>
#include <vtkProperty.h>

// Qt includes
#include <QGroupBox>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_cc::gui_controls_cc(QWidget* parent) : QWidget(parent), m_iPrevSelMode(0)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pPickContour = new QPushButton("Pick contour");
  m_widgets.pLocateFaces = new QPushButton("Locate faces");
  //
  m_widgets.pPickContour->setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLocateFaces->setMinimumWidth(BTN_MIN_WIDTH);

  // Other configurations
  m_widgets.pPickContour->setCheckable(true);

  // Group for buttons
  QGroupBox*   pContourGroup = new QGroupBox("Contour");
  QVBoxLayout* pContourLay   = new QVBoxLayout(pContourGroup);
  //
  pContourLay->addWidget(m_widgets.pPickContour);
  pContourLay->addWidget(m_widgets.pLocateFaces);

  // Set layout
  m_pMainLayout->addWidget(pContourGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pPickContour, SIGNAL( clicked() ), SLOT( onPickContour() ) );
  connect( m_widgets.pLocateFaces, SIGNAL( clicked() ), SLOT( onLocateFaces() ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
gui_controls_cc::~gui_controls_cc()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Allows user to pick a contour interactively.
void gui_controls_cc::onPickContour()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  const bool isOn = m_widgets.pPickContour->isChecked();

  // Get contour Node
  Handle(geom_contour_node)
    contour_n = common_facilities::Instance()->Model->GetPartNode()->GetContour();

  // Depending on the state of the control, either let user to pick some
  // points on the shape or finalize contour creation
  if ( isOn )
  {
    // Enable an appropriate selection mode
    m_iPrevSelMode = common_facilities::Instance()->Prs.Part->GetSelectionMode();
    common_facilities::Instance()->Prs.Part->SetSelectionMode(SelectionMode_Workpiece);

    // Add observer which takes responsibility to fill the data object with
    // the base points of the contour
    if ( !common_facilities::Instance()->Prs.Part->HasObserver(EVENT_PICK_WORLD_POINT) )
    {
      vtkSmartPointer<visu_create_contour_callback>
        cb = vtkSmartPointer<visu_create_contour_callback>::New();

      // Add observer
      common_facilities::Instance()->Prs.Part->AddObserver(EVENT_PICK_WORLD_POINT, cb);
    }
  }
  else
  {
    if ( !contour_n->IsClosed() )
    {
      common_facilities::Instance()->Model->OpenCommand();
      {
        contour_n->SetClosed(true);
      }
      common_facilities::Instance()->Model->CommitCommand();
    }

    common_facilities::Instance()->Prs.Part->Actualize( contour_n.get() );

    // Restore original selection mode
    common_facilities::Instance()->Prs.Part->SetSelectionMode(m_iPrevSelMode);
  }
}

//-----------------------------------------------------------------------------

//! Attempts to find faces which contain the polyline's nodes.
void gui_controls_cc::onLocateFaces()
{
  TopoDS_Shape part;
  if ( !gui_common::PartShape(part) ) return;

  // Build triangle set. Constructor will initialize the internal structures
  // storing the triangle nodes with references to the owning parts
  Handle(geom_bvh_facets)
    triangleSet = new geom_bvh_facets(part,
                                      common_facilities::Instance()->Notifier,
                                      common_facilities::Instance()->Plotter);
  //
  triangleSet->BVH(); // This invocation builds the BVH tree

  // Prepare a tool to find the nearest facet
  geom_find_nearest_facet nearest(triangleSet,
                                  common_facilities::Instance()->Notifier,
                                  common_facilities::Instance()->Plotter);

  // TODO: this is a test stuff
  gp_Pnt probe;

  // Find nearest
  int facet_idx;
  if ( !nearest(probe, facet_idx) )
  {
    std::cout << "Error: cannot find the nearest facet" << std::endl;
    return;
  }

  // TODO
}
