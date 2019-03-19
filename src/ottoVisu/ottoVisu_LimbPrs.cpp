//-----------------------------------------------------------------------------
// Created on: 08 March 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoVisu_LimbPrs.h>

// asiVisu includes
#include <asiVisu_ShapeDataProvider.h>
#include <asiVisu_ShapePipeline.h>
#include <asiVisu_TriangulationDataProvider.h>
#include <asiVisu_TriangulationLinksPipeline.h>
#include <asiVisu_TriangulationPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

ottoVisu_LimbPrs::ottoVisu_LimbPrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  // Get triangulation parameter.
  Handle(ottoData_LimbNode)
    limbNode = Handle(ottoData_LimbNode)::DownCast(N);

  /* ==========================
   *  Pipeline for shaded mesh
   * ========================== */

  Handle(ActData_TriangulationParameter)
    TP = Handle(ActData_TriangulationParameter)::DownCast( limbNode->Parameter(ottoData_LimbNode::PID_Mesh) );

  // Create Data Provider
  Handle(asiVisu_TriangulationDataProvider)
    dp = new asiVisu_TriangulationDataProvider(TP);

  // Main pipeline
  Handle(asiVisu_TriangulationPipeline) pl = new asiVisu_TriangulationPipeline();
  //
  this->addPipeline        ( Pipeline_Triangulation, pl );
  this->assignDataProvider ( Pipeline_Triangulation, dp );

  // Set point size and line width
  pl->Actor()->GetProperty()->SetPointSize(5.0f);
  pl->Actor()->GetProperty()->SetLineWidth(1.5f);

  // !!! For performance reasons !!!
  pl->Actor()->SetPickable(0);

  /* =========================
   *  Pipeline for mesh links
   * ========================= */

  // Create pipeline for links
  Handle(asiVisu_TriangulationLinksPipeline)
    contour_pl = new asiVisu_TriangulationLinksPipeline( pl->GetSource() );

  // Adjust props
  contour_pl->Actor()->GetProperty()->SetOpacity(0.5);
  contour_pl->Actor()->GetProperty()->SetLineWidth(1.0f);
  contour_pl->Actor()->SetPickable(0);
  //
  this->addPipeline        ( Pipeline_TriangulationLinks, contour_pl );
  this->assignDataProvider ( Pipeline_TriangulationLinks, dp );

  /* ==================
   *  Pipeline for OBB
   * ================== */

  // Create Data Provider
  Handle(asiVisu_ShapeDataProvider)
    obbDP = new asiVisu_ShapeDataProvider( N->GetId(),
                                           ActParamStream() << N->Parameter(ottoData_LimbNode::PID_OBB) );

  // Create pipeline
  Handle(asiVisu_ShapePipeline) obbPL = new asiVisu_ShapePipeline(false);
  //
  obbPL->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_Wireframe);

  // Register pipeline
  this->addPipeline        ( Pipeline_OBB, obbPL );
  this->assignDataProvider ( Pipeline_OBB, obbDP );

  /* =======================
   *  Pipeline for sections
   * ======================= */

  // Create Data Provider
  Handle(asiVisu_ShapeDataProvider)
    secDP = new asiVisu_ShapeDataProvider( N->GetId(),
                                           ActParamStream() << N->Parameter(ottoData_LimbNode::PID_Sections) );

  // Create pipeline
  Handle(asiVisu_ShapePipeline) secPL = new asiVisu_ShapePipeline(false);
  //
  secPL->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_Wireframe);

  // Register pipeline
  this->addPipeline        ( Pipeline_Sections, secPL );
  this->assignDataProvider ( Pipeline_Sections, secDP );

  // Configure
  secPL->Actor()->GetProperty()->SetLineWidth(5.0f);
  secPL->Actor()->GetProperty()->SetColor(1., 0., 0.);

  /* ======================
   *  Pipeline for surface
   * ====================== */

  // Create Data Provider
  Handle(asiVisu_ShapeDataProvider)
    surfDP = new asiVisu_ShapeDataProvider( N->GetId(),
                                            ActParamStream() << N->Parameter(ottoData_LimbNode::PID_Surface) );

  // Create pipeline
  Handle(asiVisu_ShapePipeline) surfPL = new asiVisu_ShapePipeline(false);
  //
  surfPL->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_Shaded);

  // Register pipeline
  this->addPipeline        ( Pipeline_Surface, surfPL );
  this->assignDataProvider ( Pipeline_Surface, surfDP );

  // Configure
  surfPL->Actor()->GetProperty()->SetLineWidth(5.0f);

  // Resolve coincident topology between shaded facets and border links
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
}

//-----------------------------------------------------------------------------

//! Factory method for Presentation.
//! \param[in] N Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) ottoVisu_LimbPrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new ottoVisu_LimbPrs(N);
}
