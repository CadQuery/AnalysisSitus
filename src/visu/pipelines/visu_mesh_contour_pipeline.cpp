//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <visu_mesh_contour_pipeline.h>

// Visualization includes
#include <visu_mesh_utils.h>

// VTK includes
#include <vtkExtractEdges.h>
#include <vtkProperty.h>

//! Creates new Mesh Contour Pipeline instance.
visu_mesh_contour_pipeline::visu_mesh_contour_pipeline()
: visu_mesh_pipeline()
{
  /* ======================
   *  Basic initialization
   * ====================== */

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

//! Sets color for the mesh contour.
//! \param fR [in] red factor.
//! \param fG [in] green factor.
//! \param fB [in] blue factor.
void visu_mesh_contour_pipeline::SetColor(const double fR,
                                          const double fG,
                                          const double fB)
{
  m_fColor[0] = fR;
  m_fColor[1] = fG;
  m_fColor[2] = fB;
}

//! Sets opacity of the mesh contour.
//! \param fOp [in] opacity factor to set.
void visu_mesh_contour_pipeline::SetOpacity(const double fOp)
{
  m_fOpacity = fOp;
}

//! Sets line width of the mesh contour.
//! \param theLineWidth [in] line width to set.
void visu_mesh_contour_pipeline::SetLineWidth(const double theLineWidth)
{
  m_fLineWidth = theLineWidth;
}

//! Callback for AddToRenderer() base routine. Good place to adjust visualization
//! properties of the pipeline's actor.
void visu_mesh_contour_pipeline::callback_add_to_renderer(vtkRenderer*)
{
  this->Mapper()->ScalarVisibilityOff();
  this->Actor()->GetProperty()->SetColor(m_fColor);
  this->Actor()->GetProperty()->SetOpacity(m_fOpacity);
  this->Actor()->GetProperty()->SetLineWidth(m_fLineWidth);
}

//! Callback for RemoveFromRenderer() base routine.
void visu_mesh_contour_pipeline::callback_remove_from_renderer(vtkRenderer*)
{
}

//! Callback for Update() routine.
void visu_mesh_contour_pipeline::callback_update()
{
}
