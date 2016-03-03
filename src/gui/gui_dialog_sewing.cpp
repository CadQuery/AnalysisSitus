//-----------------------------------------------------------------------------
// Created on: 17 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_dialog_sewing.h>

// Common includes
#include <common_facilities.h>

// Geometry includes
#include <geom_utils.h>

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
gui_dialog_sewing::gui_dialog_sewing(QWidget* parent)
//
: QDialog(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("Properties");

  // Editors
  m_widgets.pTolerance = new gui_line_edit();

  // Sizing
  m_widgets.pTolerance->setMinimumWidth(CONTROL_EDIT_WIDTH);

  // Default values
  m_widgets.pTolerance->setText("0.00001");

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
  pGrid->addWidget(new QLabel("Tolerance:"), 0, 0);
  //
  pGrid->addWidget(m_widgets.pTolerance,  0, 1);
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
  this->setWindowTitle("Sewing parameters");
}

//! Destructor.
gui_dialog_sewing::~gui_dialog_sewing()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

//! Reaction on perform.
void gui_dialog_sewing::onPerform()
{
  // Convert user input to double
  const double toler = QVariant( m_widgets.pTolerance->text() ).toDouble();

  //---------------------------------------------------------------------------
  // Sewing
  //---------------------------------------------------------------------------

  // Get part labels
  TDF_LabelSequence labels;
  common_facilities::Instance()->Model_XDE->GetLabelsOfParts(labels);

  // Get Shape Tool
  Handle(XCAFDoc_ShapeTool) ShapeTool = common_facilities::Instance()->Model_XDE->GetShapeTool();

  // Sew each part
  common_facilities::Instance()->Model_XDE->OpenCommand();
  {
    for ( int l = 1; l <= labels.Length(); ++l )
    {
      TopoDS_Shape shape = ShapeTool->GetShape( labels(l) );

      // Perform sewing
      //
      std::cout << "Sewing tolerance = " << toler << std::endl;
      //
      if ( !geom_utils::Sew(shape, toler) )
      {
        std::cout << "Error: sewing failed" << std::endl;
        this->close();
      }
      //
      std::cout << "Sewing done. Visualizing..." << std::endl;

      // Set shape back to model
      ShapeTool->SetShape(labels(l), shape);
    }
  }
  common_facilities::Instance()->Model_XDE->CommitCommand();

  //---------------------------------------------------------------------------
  // UI updates
  //---------------------------------------------------------------------------

  // Access CAD to mesh
  TopoDS_Shape fullCAD = common_facilities::Instance()->Model_XDE->GetOneShape();

  // Update shape
  common_facilities::Instance()->ViewerDMU->GetContext()->EraseAll();
  common_facilities::Instance()->aisDMU = common_facilities::Instance()->ViewerDMU->VisualizeOnly(fullCAD, AIS_WireFrame);

  // Close
  this->close();
}
