//-----------------------------------------------------------------------------
// Created on: 21 July 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <exeFeatures_ControlsEuler.h>

// exeFeatures includes
#include <exeFeatures_CommonFacilities.h>

// asiAlgo includes
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiUI includes
#include <asiUI_Common.h>
#include <exeFeatures_DialogEuler.h>

// Feature includes
#include <feature_euler_KEF.h>
#include <feature_euler_KEV.h>

// OCCT includes
#include <TopExp.hxx>
#include <TopoDS.hxx>

// Qt includes
#include <QGroupBox>

//-----------------------------------------------------------------------------

//! Handy utility designed to apply Euler operators with max code reusability.
class ApplyEuler
{
public:

  //! Supported types of Euler operators.
  enum EulerOp
  {
    EulerOp_KEV = 1,
    EulerOp_KEF
  };

public:

  static void KEV()
  {
    perform(EulerOp_KEV);
  }

  static void KEF()
  {
    perform(EulerOp_KEF);
  }

private:

  static void dumpHistoryOnFaces(const TopTools_IndexedMapOfShape& facesBefore,
                                 const Handle(feature_euler)&      OP)
  {
    // Dump modification history
    for ( int f = 1; f <= facesBefore.Extent(); ++f )
    {
      const TopoDS_Shape&     face_before = facesBefore(f);
      TCollection_AsciiString old_addr    = asiAlgo_Utils::ShapeAddr(face_before).c_str();
      old_addr.LowerCase();

      // Attempt to find its image
      TopoDS_Shape            face_after = OP->GetContext()->Value(face_before);
      TCollection_AsciiString new_addr   = asiAlgo_Utils::ShapeAddr(face_after).c_str();
      new_addr.LowerCase();

      if ( !old_addr.IsEqual(new_addr) )
      {
        std::cout << "  [M]: " << old_addr.ToCString() << " -> "
                  << ( new_addr.IsEmpty() ? "null" : new_addr.ToCString() )
                  << std::endl;
      }
    }
  }

  static void perform(const EulerOp op)
  {
    /* =========
     *  Prepare
     * ========= */

    const Handle(asiEngine_Model)&             model  = exeFeatures_CommonFacilities::Instance()->Model;
    const vtkSmartPointer<asiVisu_PrsManager>& prsMgr = exeFeatures_CommonFacilities::Instance()->Prs.Part;
    Handle(asiData_PartNode)                   part_n;
    TopoDS_Shape                               part;
    //
    if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

    // Get highlighted sub-shapes
    TopTools_IndexedMapOfShape subshapes;
    asiEngine_Part(model, prsMgr).GetHighlightedSubShapes(subshapes);
    //
    if ( subshapes.IsEmpty() )
    {
      std::cout << "Error: no sub-shapes selected" << std::endl;
      return;
    }

    TopoDS_Shape result = part;

    // Get all faces BEFORE modification
    TopTools_IndexedMapOfShape facesBefore;
    TopExp::MapShapes(result, TopAbs_FACE, facesBefore);

    TIMER_NEW
    TIMER_GO

    /* =======================
     *  KEV: kill-edge-vertex
     * ======================= */

    if ( op == EulerOp_KEV )
    {
      Handle(feature_euler)
        OP = new feature_euler_KEV(result, subshapes, NULL, NULL);
      //
      if ( !OP->Perform() )
      {
        std::cout << "Error: Euler operation failed" << std::endl;
        return;
      }

      // Update result
      result = OP->GetResult();

      // Dump history
      dumpHistoryOnFaces(facesBefore, OP);
    }

    /* =====================
     *  KEF: kill-edge-face
     * ===================== */

    else if ( op == EulerOp_KEF )
    {
      for ( int ss = 1; ss <= subshapes.Extent(); ++ss )
      {
        const TopoDS_Shape& current = subshapes(ss);
        //
        if ( current.ShapeType() != TopAbs_FACE )
        {
          std::cout << "Warning: selected shape is not a face: SKIPPED" << std::endl;
          continue;
        }

        Handle(feature_euler)
          OP = new feature_euler_KEF(result, TopoDS::Face(current), NULL, NULL);
        //
        if ( !OP->Perform() )
        {
          std::cout << "Error: Euler operation failed" << std::endl;
          return;
        }

        // Update result
        result = OP->GetResult();

        // Dump history
        dumpHistoryOnFaces(facesBefore, OP);
      }
    }

    /* ==========
     *  Finalize
     * ========== */

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Euler operation")

    // Perform modification
    model->OpenCommand();
    {
      part_n->SetShape(result);
    }
    model->CommitCommand();
    //
    std::cout << "Euler operation done. Visualizing..." << std::endl;

    // Clean up
    model->Clear();

    // Actualize
    prsMgr->InitializePickers();
    prsMgr->Actualize( part_n.get() );
    prsMgr->SetSelectionMode(SelectionMode_None);
  }

};

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
exeFeatures_ControlsEuler::exeFeatures_ControlsEuler(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pCheckEulerPoincare = new QPushButton("Check Euler-Poincare");
  m_widgets.pKEV                = new QPushButton("KEV");
  m_widgets.pKEF                = new QPushButton("KEF");
  //
  m_widgets.pCheckEulerPoincare -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pKEV                -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pKEF                -> setMinimumWidth(BTN_MIN_WIDTH);

  // Group for checker
  QGroupBox*   pCheckGroup = new QGroupBox("Check");
  QVBoxLayout* pCheckLay   = new QVBoxLayout(pCheckGroup);
  //
  pCheckLay->addWidget(m_widgets.pCheckEulerPoincare);

  // Group for Kill operators
  QGroupBox*   pKillGroup = new QGroupBox("Kill");
  QVBoxLayout* pKillLay   = new QVBoxLayout(pKillGroup);
  //
  pKillLay->addWidget(m_widgets.pKEV);
  pKillLay->addWidget(m_widgets.pKEF);

  // Set layout
  m_pMainLayout->addWidget(pCheckGroup);
  m_pMainLayout->addWidget(pKillGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pCheckEulerPoincare, SIGNAL( clicked() ), SLOT( onCheckEulerPoincare () ) );
  connect( m_widgets.pKEV,                SIGNAL( clicked() ), SLOT( onKEV                () ) );
  connect( m_widgets.pKEF,                SIGNAL( clicked() ), SLOT( onKEF                () ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
exeFeatures_ControlsEuler::~exeFeatures_ControlsEuler()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Checks Euler-Poincare equation.
void exeFeatures_ControlsEuler::onCheckEulerPoincare()
{
  const Handle(asiEngine_Model)& model  = exeFeatures_CommonFacilities::Instance()->Model;
  Handle(asiData_PartNode)       part_n;
  TopoDS_Shape                   part;
  //
  if ( !asiUI_Common::PartShape(model, part_n, part) ) return;

  // Run dialog
  exeFeatures_DialogEuler* wEuler = new exeFeatures_DialogEuler(model, this);
  wEuler->show();
}

//-----------------------------------------------------------------------------

//! KEV.
void exeFeatures_ControlsEuler::onKEV()
{
  ApplyEuler::KEV();
}

//-----------------------------------------------------------------------------

//! KEF.
void exeFeatures_ControlsEuler::onKEF()
{
  ApplyEuler::KEF();
}
