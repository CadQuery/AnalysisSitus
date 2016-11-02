//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_viewer_section.h>

// Common includes
#include <common_facilities.h>

// Visualization includes
#include <visu_utils.h>

// GUI includes
#include <gui_controls_section.h>

// Qt-VTK includes
#include <QVTKWidget.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#pragma warning(pop)

// VTK includes
#include <vtkCell.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextActor.h>
#include <vtkTextRepresentation.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//! Creates a new instance of viewer.
//! \param withControlPanel [in] indicates whether to enable control panel or not.
//! \param parent           [in] parent widget.
gui_viewer_section::gui_viewer_section(const bool withControlPanel, QWidget* parent) : gui_viewer(parent)
{
  // Initialize presentation manager along with QVTK widget
  common_facilities::Instance()->Prs.Section = vtkSmartPointer<visu_prs_manager>::New();
  m_prs_mgr = common_facilities::Instance()->Prs.Section;
  //
  m_prs_mgr->Initialize(this);
  m_prs_mgr->SetInteractionMode(visu_prs_manager::InteractionMode_2D);
  m_prs_mgr->SetSelectionMode(SelectionMode_Workpiece);

  // Widgets and layouts
  gui_controls_section* pControlPane = NULL;
  if ( withControlPanel )
    pControlPane = new gui_controls_section(this);
  //
  QVTKWidget*  pViewer      = m_prs_mgr->GetQVTKWidget();
  QVBoxLayout* pBaseLayout  = new QVBoxLayout();

  // Configure layout
  pBaseLayout->setSpacing(0);
  pBaseLayout->addWidget(pViewer);
  //
  if ( withControlPanel )
    pBaseLayout->addWidget(pControlPane);
  //
  pBaseLayout->setStretchFactor(pViewer, 1);
  //
  if ( withControlPanel )
    pBaseLayout->setStretchFactor(pControlPane, 0);
  //
  pBaseLayout->setAlignment(Qt::AlignTop);
  pBaseLayout->setContentsMargins(0, 0, 0, 0);

  // Set central widget
  this->setLayout(pBaseLayout);

  /* ===================================
   *  Setting up picking infrastructure
   * =================================== */

  // Initialize Callback instance for Pick operation
  m_pickCallback = vtkSmartPointer<visu_pick_callback>::New();
  m_pickCallback->SetViewer(this);

  // Set observer for detection
  if ( !m_prs_mgr->GetImageInteractorStyle()->HasObserver(EVENT_PICK_DEFAULT) )
    m_prs_mgr->GetImageInteractorStyle()->AddObserver(EVENT_PICK_DEFAULT, m_pickCallback);

  // Set observer for detection
  if ( !m_prs_mgr->GetImageInteractorStyle()->HasObserver(EVENT_DETECT_DEFAULT) )
    m_prs_mgr->GetImageInteractorStyle()->AddObserver(EVENT_DETECT_DEFAULT, m_pickCallback);

  // Get notified once any sensitive is picked on a section
  connect( m_pickCallback, SIGNAL( sectionPicked() ), this, SLOT( onSectionPicked() ) );

  /* =====================================
   *  Finalize initial state of the scene
   * ===================================== */

  // Initialize text widget used for annotations
  m_textWidget = vtkSmartPointer<vtkTextWidget>::New();
  visu_utils::InitTextWidget(m_textWidget);
  //
  vtkTextRepresentation* rep = vtkTextRepresentation::SafeDownCast( m_textWidget->GetRepresentation() );
  rep->SetWindowLocation(vtkTextRepresentation::UpperRightCorner);
  //
  m_textWidget->SetInteractor      ( m_prs_mgr->GetRenderer()->GetRenderWindow()->GetInteractor() );
  m_textWidget->SetDefaultRenderer ( m_prs_mgr->GetRenderer() );
  m_textWidget->SetCurrentRenderer ( m_prs_mgr->GetRenderer() );

  // Reset camera
  this->onResetView();
}

//! Destructor.
gui_viewer_section::~gui_viewer_section()
{
}

//-----------------------------------------------------------------------------

//! Updates viewer.
void gui_viewer_section::Repaint()
{
  m_prs_mgr->GetQVTKWidget()->repaint();
}

//-----------------------------------------------------------------------------

//! Resets view.
void gui_viewer_section::onResetView()
{
  visu_utils::CameraOnTop( m_prs_mgr->GetRenderer() );
  this->Repaint();
}

//-----------------------------------------------------------------------------

//! Callback for picking event.
void gui_viewer_section::onSectionPicked()
{
  // Access picking results
  const asiVisu_ActualSelection& sel      = m_prs_mgr->GetCurrentSelection();
  const visu_pick_result&      pick_res = sel.PickResult(SelectionNature_Detection);
  const visu_actor_elem_map&   elem_map = pick_res.GetPickMap();

  // Check if there is anything selected
  if ( elem_map.IsEmpty() )
  {
    m_textWidget->Off();
    return;
  }

  // Prepare cumulative set of all picked element IDs
  for ( visu_actor_elem_map::Iterator it(elem_map); it.More(); it.Next() )
  {
    const vtkSmartPointer<vtkActor>&  picked_actor = it.Key();
    const TColStd_PackedMapOfInteger& cellGIDs     = it.Value();

    // Access polygonal data mapper
    vtkPolyDataMapper* pMapper = vtkPolyDataMapper::SafeDownCast( picked_actor->GetMapper() );
    if ( !pMapper )
    {
      m_textWidget->Off();
      return;
    }

    // Access polygonal data
    vtkPolyData* pData = vtkPolyData::SafeDownCast( pMapper->GetInput() );
    if ( !pData )
    {
      m_textWidget->Off();
      return;
    }

    TCollection_AsciiString TITLE;

    // Loop over the selected cells
    for ( TColStd_MapIteratorOfPackedMapOfInteger it(cellGIDs); it.More(); it.Next() )
    {
      const int  cellGID = it.Key();
      vtkCell*   pCell   = pData->GetCell(cellGID);
      vtkIdList* pids    = pCell->GetPointIds();

      // Access coordinates
      double coord[3] = {0.0, 0.0, 0.0};

      for ( vtkIdType pointId = 0; pointId < pids->GetNumberOfIds(); ++pointId )
      {
        pData->GetPoints()->GetPoint(pids->GetId(pointId), coord);

        // Prepare label
        TITLE += "("; TITLE += coord[0]; TITLE += ", "; TITLE += coord[1]; TITLE += ", "; TITLE += coord[2]; TITLE += ")\n";
      }
    }

    // Update text on the annotation
    m_textWidget->GetTextActor()->SetInput( TITLE.ToCString() );
    m_textWidget->On();
  }
}
