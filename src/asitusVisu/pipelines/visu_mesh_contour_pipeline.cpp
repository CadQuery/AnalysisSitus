//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_contour_pipeline.h>

// Visualization includes
#include <visu_mesh_utils.h>

// VTK includes
#include <vtkExtractEdges.h>

//! Creates new Mesh Contour Pipeline instance.
//! \param doUseDefaultColor [in] indicates whether to use default color.
visu_mesh_contour_pipeline::visu_mesh_contour_pipeline(const bool doUseDefaultColor)
: visu_mesh_pipeline()
{
  /* ======================
   *  Basic initialization
   * ====================== */

  if ( doUseDefaultColor )
    visu_mesh_utils::DefaultContourColor(m_fColor[0], m_fColor[1], m_fColor[2]);

  m_fOpacity = visu_mesh_utils::DefaultContourOpacity();
  m_fLineWidth = 1.0;

  /* ========================
   *  Prepare custom filters
   * ======================== */

  // Filter to extract edges
  vtkSmartPointer<vtkExtractEdges> aEEFilter = vtkSmartPointer<vtkExtractEdges>::New();

  /* =========================
   *  Register custom filters
   * ========================= */

  m_filterMap.Bind(Filter_ExtractEdges, aEEFilter);

  this->append( m_filterMap.Find(Filter_ExtractEdges) );
}

//! Callback for AddToRenderer() base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_mesh_contour_pipeline::callback_add_to_renderer(vtkRenderer*)
{
}

//! Callback for RemoveFromRenderer() base routine.
void visu_mesh_contour_pipeline::callback_remove_from_renderer(vtkRenderer*)
{
}

//! Callback for Update() routine.
void visu_mesh_contour_pipeline::callback_update()
{
}
