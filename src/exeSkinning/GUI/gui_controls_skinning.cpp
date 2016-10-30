//-----------------------------------------------------------------------------
// Created on: 11 (* thanks to my mother, Irina) December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_controls_skinning.h>

// A-Situs (GUI) includes
#include <gui_common.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (geometry) includes
#include <geom_sections_node.h>
#include <geom_utils.h>

//-----------------------------------------------------------------------------

#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param parent [in] parent widget.
gui_controls_skinning::gui_controls_skinning(QWidget* parent) : QWidget(parent)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // "Do everything" button
  m_widgets.pDoSkinning = new QPushButton("Skin Surface");
  m_widgets.pDoSkinning->setMaximumWidth(CONTROL_BTN_WIDTH);
  //
  connect( m_widgets.pDoSkinning, SIGNAL( clicked() ), this, SLOT( onDoTheMagic() ) );

  // Set layout
  m_pMainLayout->addWidget(m_widgets.pDoSkinning);
  //
  this->setLayout(m_pMainLayout);
}

//! Destructor.
gui_controls_skinning::~gui_controls_skinning()
{
  delete m_pMainLayout;
  //
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Reaction to skinning request.
void gui_controls_skinning::onDoTheMagic()
{
  Handle(geom_sections_node)
    sections_n = common_facilities::Instance()->Model->GetSectionsNode();
  //
  if ( sections_n.IsNull() || !sections_n->IsWellFormed() )
    return;

  // Gather all sections into a sequence
  TopTools_SequenceOfShape sections;
  for ( Handle(ActAPI_IChildIterator) it = sections_n->GetChildIterator(); it->More(); it->Next() )
  {
    Handle(geom_section_node) section_n = Handle(geom_section_node)::DownCast( it->Value() );
    if ( section_n.IsNull() || !section_n->IsWellFormed() )
      continue;

    // Get stored Transformation coefficients
    double Tx, Ty, Tz, Rx, Ry, Rz;
    section_n->GetTransform(Tx, Ty, Tz, Rx, Ry, Rz);

    // Apply transformation to the geometry
    gp_Trsf T = geom_utils::Transformation(Tx, Ty, Tz, Rx, Ry, Rz);
    TopoDS_Shape section = geom_utils::ApplyTransformation(section_n->GetGeometry(), T, true);

    // Append geometry of a section
    sections.Append(section);
  }

  //---------------------------------------------------------------------------
  // Build skinned surface
  TopoDS_Shape result = geom_utils::MakeSkin(sections);
  //
  if ( result.IsNull() )
  {
    std::cout << "Error: cannot build skinned surface" << std::endl;
    return;
  }

  // Store geometry
  common_facilities::Instance()->Model->OpenCommand(); // tx start
  {
    sections_n->SetGeometry(result);
  }
  common_facilities::Instance()->Model->CommitCommand(); // tx commit

  // Update Presentation
  common_facilities::Instance()->Prs.Skinner->Actualize( sections_n.get(), false, true );
}
