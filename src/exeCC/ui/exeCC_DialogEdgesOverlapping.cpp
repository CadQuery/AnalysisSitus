//-----------------------------------------------------------------------------
// Created on: 26 October 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeCC_DialogEdgesOverlapping.h>

// exeCC includes
#include <exeCC_CommonFacilities.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiUI includes
#include <asiUI_Common.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

// OCCT includes
#include <TopoDS.hxx>

// SPE includes
#include <CC_EdgeEdgeDist.h>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
exeCC_DialogEdgesOverlapping::exeCC_DialogEdgesOverlapping(QWidget* parent)
//
: QDialog(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Parameters");

  // Editors
  m_widgets.pPrecision     = new asiUI_LineEdit();
  m_widgets.pMaxElevation  = new asiUI_LineEdit();
  m_widgets.pMinLaidLength = new asiUI_LineEdit();

  // Sizing
  m_widgets.pPrecision     -> setMinimumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pMaxElevation  -> setMinimumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pMinLaidLength -> setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  m_widgets.pPrecision     -> setText("0.1");
  m_widgets.pMaxElevation  -> setText("2.0");
  m_widgets.pMinLaidLength -> setText("50.0");

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pPerform = new QPushButton("Perform");

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
  pGrid->addWidget(new QLabel("Precision:"),       0, 0);
  pGrid->addWidget(new QLabel("Max elevation:"),   1, 0);
  pGrid->addWidget(new QLabel("Min laid length:"), 2, 0);
  //
  pGrid->addWidget(m_widgets.pPrecision,     0, 1);
  pGrid->addWidget(m_widgets.pMaxElevation,  1, 1);
  pGrid->addWidget(m_widgets.pMinLaidLength, 2, 1);
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
  this->setWindowTitle("Check Edges Overlapping");
}

//! Destructor.
exeCC_DialogEdgesOverlapping::~exeCC_DialogEdgesOverlapping()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Perform" button.
void exeCC_DialogEdgesOverlapping::onPerform()
{
  // Read user inputs
  const double precision     = m_widgets.pPrecision->text().toDouble();
  const double maxElevation  = m_widgets.pMaxElevation->text().toDouble();
  const double minLaidLength = m_widgets.pMinLaidLength->text().toDouble();

  // Get all highlighted sub-shapes
  TopTools_IndexedMapOfShape subShapes;
  asiEngine_Part(exeCC_CommonFacilities::Instance()->Model,
                 exeCC_CommonFacilities::Instance()->Prs.Part).GetHighlightedSubShapes(subShapes);

  // Take edges
  NCollection_Sequence<TopoDS_Edge> edges;
  //
  for ( int k = 1; k <= subShapes.Extent(); ++k )
  {
    const TopoDS_Shape& subShape = subShapes(k);
    //
    if ( subShape.ShapeType() != TopAbs_EDGE )
      continue;
    //
    const TopoDS_Edge& edge = TopoDS::Edge(subShape);
    edges.Append(edge);
  }
  //
  if ( edges.Length() != 2 )
  {
    std::cout << "Error: two edges are expected" << std::endl;
    return;
  }

  // Prepare overlapping checker
  double dist;
  bool hasOverlapping = false;
  //
  CC_EdgeEdgeDist EEDist(edges(1), precision);
  //
  std::cout << "---"                                                                            << std::endl;
  std::cout << "Parameters:"                                                                    << std::endl;
  std::cout << "\t>>> Precision: "                                             << precision     << std::endl;
  std::cout << "\t>>> Max allowed elevation still recognized as overlapping: " << maxElevation  << std::endl;
  std::cout << "\t>>> Min allowed laid length to recognize overlapping: "      << minLaidLength << std::endl;
  std::cout << "---"                                                                            << std::endl;
  std::cout << "Results of CC_EdgeEdgeDist:"                                                    << std::endl;

  TIMER_NEW
  TIMER_GO

  if ( !EEDist(edges(2), false, dist, maxElevation, minLaidLength, hasOverlapping, NULL) )
  {
    std::cout << "\tError: cannot calculate distance" << std::endl;
    return;
  }

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("Check Edge Overlapping")

  std::cout << "\t>>> Overlapping = " << (hasOverlapping ? "yes" : "no") << std::endl;
  std::cout << "\t>>> Distance = "    << dist << " units"                << std::endl;

  // Imperative plotter
  ActAPI_PlotterEntry IV(exeCC_CommonFacilities::Instance()->Plotter);

  // Post-process the detected overlapping zones
  if ( hasOverlapping )
  {
    // Get sampled points
    const TColgp_SequenceOfPnt& samplePts = EEDist.GetBasePts();

    // Get overlapping zones
    const CC_EdgeEdgeDist::t_overlap_zones& zones = EEDist.GetOverlappings();
    //
    std::cout << "---" << std::endl;
    std::cout << "Number of detected overlapping zones: " << zones.size() << std::endl;
    //
    for ( size_t z = 0; z < zones.size(); ++z )
    {
      const CC_EdgeEdgeDist::t_overlap_zone& zone = zones[z];
      //
      std::cout << "\t>>> Zone " << (z + 1) << ": ("
                << zone.first << ", " << zone.second << ") with elevation "
                << zone.elevation << std::endl;

      // Create a compound of overlapping points
      const int            nPts = zone.second - zone.first + 1;
      Handle(HRealArray) coords = new HRealArray(0, nPts*3 - 1);
      //
      for ( int p = 1, coord_idx = 0; p <= samplePts.Length(); ++p )
      {
        if ( p >= zone.first && p <= zone.second )
        {
          coords->ChangeValue(coord_idx++) = samplePts(p).X();
          coords->ChangeValue(coord_idx++) = samplePts(p).Y();
          coords->ChangeValue(coord_idx++) = samplePts(p).Z();
        }
      }
      //
      IV.DRAW_POINTS(coords, Color_Red, "overlapping");
    }
  }

  // Close
  this->close();
}
