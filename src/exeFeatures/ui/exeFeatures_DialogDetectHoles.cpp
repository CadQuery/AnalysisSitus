//-----------------------------------------------------------------------------
// Created on: 13 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeFeatures_DialogDetectHoles.h>

// exeFeatures includes
#include <exeFeatures_CommonFacilities.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiUI includes
#include <asiUI_Common.h>

// Feature includes
#include <feature_detect_choles.h>

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
//! \param parent [in] parent widget.
exeFeatures_DialogDetectHoles::exeFeatures_DialogDetectHoles(QWidget* parent)
//
: QDialog(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Parameters");

  // Editors
  m_widgets.pRadius            = new asiUI_LineEdit();
  m_widgets.pIsHardFeatureMode = new QCheckBox();

  // Sizing
  m_widgets.pRadius->setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  m_widgets.pRadius->setText("10");
  m_widgets.pIsHardFeatureMode->setChecked(false);

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pIdentify = new QPushButton("Identify");

  // Sizing
  m_widgets.pIdentify->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reaction
  connect( m_widgets.pIdentify, SIGNAL( clicked() ), this, SLOT( onIdentify() ) );

  //---------------------------------------------------------------------------
  // Line editors
  //---------------------------------------------------------------------------

  // Create layout
  QGridLayout* pGrid = new QGridLayout(pGroup);
  pGrid->setSpacing(5);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  pGrid->addWidget(new QLabel("Radius:"),            0, 0);
  pGrid->addWidget(new QLabel("Hard feature mode:"), 1, 0);
  //
  pGrid->addWidget(m_widgets.pRadius,            0, 1);
  pGrid->addWidget(m_widgets.pIsHardFeatureMode, 1, 1);
  //
  pGrid->setColumnStretch(0, 0);
  pGrid->setColumnStretch(1, 1);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Configure main layout
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(m_widgets.pIdentify);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Holes identification");
}

//! Destructor.
exeFeatures_DialogDetectHoles::~exeFeatures_DialogDetectHoles()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on clicking "Identify" button.
void exeFeatures_DialogDetectHoles::onIdentify()
{
  const Handle(asiEngine_Model)&             model  = exeFeatures_CommonFacilities::Instance()->Model;
  const vtkSmartPointer<asiVisu_PrsManager>& prsMgr = exeFeatures_CommonFacilities::Instance()->Prs.Part;
  Handle(asiData_PartNode)                   part_n;
  TopoDS_Shape                               part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Read user inputs
  const double radius     = m_widgets.pRadius->text().toDouble();
  const bool   isHardMode = m_widgets.pIsHardFeatureMode->isChecked();

  // Identify holes
  feature_detect_choles detector(part, radius, NULL,
                                 exeFeatures_CommonFacilities::Instance()->Notifier,
                                 exeFeatures_CommonFacilities::Instance()->Plotter);
  //
  detector.SetHardFeatureMode(isHardMode);
  //
  if ( !detector.Perform() )
  {
    std::cout << "Error: cannot identify holes" << std::endl;
    return;
  }

  // Get detected holes
  const TopTools_IndexedMapOfShape& holes = detector.GetResultFaces();
  if ( holes.IsEmpty() )
  {
    std::cout << "No holes detected with radius <= " << radius << std::endl;
    return;
  }
  else
    std::cout << holes.Extent() << " hole(s) detected with radius <= " << radius << std::endl;

  // Highlight
  asiEngine_Part(model, prsMgr).HighlightSubShapes(holes);

  // Close
  this->close();
}
