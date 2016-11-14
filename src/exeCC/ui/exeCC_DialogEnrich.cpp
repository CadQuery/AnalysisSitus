//-----------------------------------------------------------------------------
// Created on: 07 October 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeCC_DialogEnrich.h>

// exeCC includes
#include <exeCC_CommonFacilities.h>

// asiAlgo includes
#include <asiAlgo_HitFacet.h>
#include <asiAlgo_Timer.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asUI includes
#include <asiUI_Common.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

// OCCT includes
#include <TopExp_Explorer.hxx>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param bvh    [in] BVH for fast picking of facets.
//! \param parent [in] parent widget.
exeCC_DialogEnrich::exeCC_DialogEnrich(const Handle(asiAlgo_BVHFacets)& bvh,
                                       QWidget*                         parent)
//
: QDialog(parent),
  m_bvh(bvh)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Parameters");

  // Editors
  m_widgets.pNumPoints = new asiUI_LineEdit();

  // Sizing
  m_widgets.pNumPoints->setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  m_widgets.pNumPoints->setText("20");

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pPerform = new QPushButton("Enrich");

  // Sizing
  m_widgets.pPerform->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reaction
  connect( m_widgets.pPerform, SIGNAL( clicked() ), this, SLOT( onPerform() ) );

  //---------------------------------------------------------------------------
  // Line editors
  //---------------------------------------------------------------------------

  // Create layout
  QGridLayout* pGrid = new QGridLayout(pGroup);
  pGrid->setSpacing(5);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  pGrid->addWidget(new QLabel("Num. points:"), 0, 0);
  //
  pGrid->addWidget(m_widgets.pNumPoints, 0, 1);
  //
  pGrid->setColumnStretch(0, 0);
  pGrid->setColumnStretch(1, 1);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Configure main layout
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(m_widgets.pPerform);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Contour Enriching");
}

//! Destructor.
exeCC_DialogEnrich::~exeCC_DialogEnrich()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Perform" button.
void exeCC_DialogEnrich::onPerform()
{
  // Read user inputs
  const int nPoints = m_widgets.pNumPoints->text().toInt();

  // Get part Node
  Handle(asiData_PartNode)
    part_n = exeCC_CommonFacilities::Instance()->Model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    std::cout << "Error: part is not defined" << std::endl;
    return;
  }

  // Get contour Node
  Handle(asiData_ContourNode) contour_n = part_n->GetContour();
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
  {
    std::cout << "Error: contour is not defined" << std::endl;
    return;
  }

  ActAPI_PlotterEntry IV(exeCC_CommonFacilities::Instance()->Plotter);

  /* =======================================
   *  Enrich contour with additional points
   * ======================================= */

  const double minStep = 0.1;

  // Take points on faces
  TColgp_SequenceOfPnt poles;
  TColStd_SequenceOfInteger faces;
  //
  contour_n->AsPointsOnFaces(poles, faces);

  // Prepare a tool to find the intersected facet
  asiAlgo_HitFacet HitFacet(m_bvh,
                            exeCC_CommonFacilities::Instance()->Notifier,
                            exeCC_CommonFacilities::Instance()->Plotter);

  // Collection with new points and face indices
  TColgp_SequenceOfPnt enrichedPoints;
  TColStd_SequenceOfInteger enrichedFaces;

  // Loop over the "corner" points of the contour
  for ( int pidx = 1; pidx <= poles.Length(); ++pidx )
  {
    const gp_Pnt& p_current = poles(pidx);
    const int     f_current = faces(pidx);

    // Add corner point as-is
    enrichedPoints.Append(p_current);
    enrichedFaces.Append(f_current);

    // Take index of the next corner point
    int pidx_next = ( pidx == poles.Length() ) ? 1 : (pidx + 1);

    // Get next point
    const gp_Pnt&             p_next = poles(pidx_next);
    const gp_Vec     current_to_next = p_next.XYZ() - p_current.XYZ();
    const gp_Dir current_to_next_dir = current_to_next;

    // Calculate step to insert intermediate points
    const double dist = current_to_next.Magnitude();
    const double delta = dist / (nPoints + 1);
    //
    if ( delta < minStep )
      continue;

    // Insert intermediate points
    for ( int k = 1; k <= nPoints; ++k )
    {
      gp_Pnt P_new = p_current.XYZ() + current_to_next_dir.XYZ()*delta*k;

      // Find the nearest facet and its corresponding face
      int facet_idx;
      HitFacet(P_new, 1.0e-3, facet_idx);
      //
      if ( facet_idx != -1 )
      {
        const int face_idx = m_bvh->GetFacet(facet_idx).FaceIndex;
        //
        enrichedPoints.Append(P_new);
        enrichedFaces.Append(face_idx);
      }
    }
  }

  exeCC_CommonFacilities::Instance()->Model->OpenCommand();
  {
    // Clean up the cached geometry
    contour_n->SetGeometry( TopoDS_Shape() );

    // Set new enriched data
    contour_n->SetPointsOnFaces(enrichedPoints, enrichedFaces);
  }
  exeCC_CommonFacilities::Instance()->Model->CommitCommand();

  /* ==========
   *  Finalize
   * ========== */

  // Actualize
  exeCC_CommonFacilities::Instance()->Prs.Part->DeletePresentation( contour_n.get() );
  exeCC_CommonFacilities::Instance()->Prs.Part->Actualize( contour_n.get() );

  // Close
  this->close();
}
