//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_MeshContourPipeline.h>

// Visualization includes
#include <asiVisu_MeshUtils.h>

// VTK includes
#include <vtkExtractEdges.h>

//! Creates new Mesh Contour Pipeline instance.
//! \param doUseDefaultColor [in] indicates whether to use default color.
asiVisu_MeshContourPipeline::asiVisu_MeshContourPipeline(const bool doUseDefaultColor)
: asiVisu_MeshPipeline()
{
  /* ======================
   *  Basic initialization
   * ====================== */

  if ( doUseDefaultColor )
    asiVisu_MeshUtils::DefaultContourColor(m_fColor[0], m_fColor[1], m_fColor[2]);

  m_fOpacity = asiVisu_MeshUtils::DefaultContourOpacity();
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
void asiVisu_MeshContourPipeline::callback_add_to_renderer(vtkRenderer*)
{
}

//! Callback for RemoveFromRenderer() base routine.
void asiVisu_MeshContourPipeline::callback_remove_from_renderer(vtkRenderer*)
{
}

//! Callback for Update() routine.
void asiVisu_MeshContourPipeline::callback_update()
{
}
