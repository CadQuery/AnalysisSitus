//-----------------------------------------------------------------------------
// Created on: 10 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DialogGapFilling.h>

// asiUI includes
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_PlateOnEdges.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// OCCT includes
#include <BRep_Builder.hxx>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param model    [in] Data Model instance.
//! \param prsMgr   [in] presentation manager.
//! \param pViewer  [in] connected viewer.
//! \param progress [in] progress notifier.
//! \param plotter  [in] imperative plotter.
//! \param parent   [in] parent widget.
asiUI_DialogGapFilling::asiUI_DialogGapFilling(const Handle(asiEngine_Model)&             model,
                                               const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                                               asiUI_ViewerPart*                          pViewer,
                                               ActAPI_ProgressEntry                       progress,
                                               ActAPI_PlotterEntry                        plotter,
                                               QWidget*                                   parent)
//
: QDialog    (parent),
  m_model    (model),
  m_prsMgr   (prsMgr),
  m_pViewer  (pViewer),
  m_notifier (progress),
  m_plotter  (plotter)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Filling parameters");

  // Editors
  m_widgets.pOrder = new QComboBox();
  //
  m_widgets.pOrder->addItem("0", 0);
  m_widgets.pOrder->addItem("1", 1);
  m_widgets.pOrder->addItem("2", 2);
  
  // Sizing
  m_widgets.pOrder->setMinimumWidth(CONTROL_EDIT_WIDTH);

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
  //
  pGrid->addWidget(new QLabel("Order of continuity:"), 0, 0);
  //
  pGrid->addWidget(m_widgets.pOrder, 0, 1);
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
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Gap filling");
}

//! Destructor.
asiUI_DialogGapFilling::~asiUI_DialogGapFilling()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Perform" button.
void asiUI_DialogGapFilling::onPerform()
{
  const int continuity = m_widgets.pOrder->currentIndex();
  m_notifier.SendLogMessage(LogInfo(Normal) << "Smoothness C%1 requested" << continuity);
  //
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  TColStd_PackedMapOfInteger edgeIndices;
  asiEngine_Part(m_model, m_prsMgr).GetHighlightedEdges(edgeIndices);
  //
  if ( !edgeIndices.Extent() )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "No seed edges selected");
    return;
  }

  /* =====================
   *  Perform gap filling
   * ===================== */

  Handle(Geom_BSplineSurface) supportSurf;
  TopoDS_Face patchFace;
  BRep_Builder BB;

  // Perform gap filling
  asiAlgo_PlateOnEdges PlateOnEdges(m_model->GetPartNode()->GetAAG(), m_notifier, m_plotter);
  //
  if ( !PlateOnEdges.Build(edgeIndices, continuity, supportSurf, patchFace) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Gap filling failed");
    return;
  }

  // Build a compound of the original geometry and a patch
  TopoDS_Compound comp;
  BB.MakeCompound(comp);
  BB.Add(comp, part);
  BB.Add(comp, patchFace);

  // Update Data Model
  m_model->OpenCommand();
  {
    // Set new geometry
    asiEngine_Part(m_model, m_prsMgr).Update(comp);
  }
  m_model->CommitCommand();

  // Close
  this->close();
}
