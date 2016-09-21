//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <journal_iv.h>

// Common includes
#include <common_facilities.h>

// Engine includes
#include <engine_iv.h>

// Visualization includes
#include <visu_iv_point_set_prs.h>
#include <visu_iv_topo_item_prs.h>
#include <visu_points_pipeline.h>
#include <visu_shape_pipeline.h>
#include <visu_utils.h>

// OCCT includes
#include <GC_MakeSegment.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

//---------------------------------------------------------------------------//

void journal_iv::CLEAN()
{
  this->prsManager2d()->DeleteAllPresentations();
  this->prsManager2d()->InitializePickers();
  //
  this->prsManager3d()->DeleteAllPresentations();
  this->prsManager3d()->InitializePickers();
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_POINT(const gp_XY&                   coord,
                            const Quantity_Color&          color,
                            const TCollection_AsciiString& name)
{
  Handle(HRealArray) coords = new HRealArray(0, 1);
  //
  coords->ChangeValue(0) = coord.X();
  coords->ChangeValue(1) = coord.Y();

  // Create a new Node for the given point set
  common_facilities::Instance()->Model->OpenCommand();
  //
  Handle(visu_iv_point_set_2d_node)
    points_n = engine_iv::Create_PointSet2d(coords, name);
  //
  common_facilities::Instance()->Model->CommitCommand();

  // Visualize
  this->visualize(true, points_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_POINT(const gp_Pnt2d&                point,
                            const Quantity_Color&          color,
                            const TCollection_AsciiString& name)
{
  this->DRAW_POINT(point.XY(), color, name);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_POINT(const gp_XYZ&                  coord,
                            const Quantity_Color&          color,
                            const TCollection_AsciiString& name)
{
  Handle(HRealArray) coords = new HRealArray(0, 2);
  //
  coords->ChangeValue(0) = coord.X();
  coords->ChangeValue(1) = coord.Y();
  coords->ChangeValue(2) = coord.Z();

  this->DRAW_POINTS(coords, color, name);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_POINT(const gp_Pnt&                  point,
                            const Quantity_Color&          color,
                            const TCollection_AsciiString& name)
{
  this->DRAW_POINT(point.XYZ(), color, name);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_POINTS(const Handle(HRealArray)&      coords,
                             const Quantity_Color&          color,
                             const TCollection_AsciiString& name)
{
  // Create a new Node for the given point set
  common_facilities::Instance()->Model->OpenCommand();
  //
  Handle(visu_iv_point_set_node)
    points_n = engine_iv::Create_PointSet( new geom_point_cloud(coords), name );
  //
  common_facilities::Instance()->Model->CommitCommand();

  // Visualize
  this->visualize(false, points_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_LINK(const gp_Pnt&                  p1,
                           const gp_Pnt&                  p2,
                           const Quantity_Color&          color,
                           const TCollection_AsciiString& name)
{
  if ( p1.Distance(p2) < 1.0e-5 )
    return;

  // Create a straight line segment between two passed points
  Handle(Geom_TrimmedCurve) C = GC_MakeSegment(p1, p2);

  // Create a curve Node
  common_facilities::Instance()->Model->OpenCommand();
  //
  Handle(visu_iv_curve_node)
    curve_n = engine_iv::Create_Curve( C, Precision::Infinite(), name );
  //
  common_facilities::Instance()->Model->CommitCommand();

  // Visualize
  this->visualize(false, curve_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_VECTOR_AT(const gp_Pnt&                  P,
                                const gp_Vec&                  V,
                                const Quantity_Color&          color,
                                const TCollection_AsciiString& name)
{
  this->DRAW_LINK(P, P.XYZ() + V.XYZ(), color, name);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_CURVE(const Handle(Geom_Curve)&      curve,
                            const Quantity_Color&          color,
                            const TCollection_AsciiString& name)
{
  // Create a curve Node
  common_facilities::Instance()->Model->OpenCommand();
  //
  Handle(visu_iv_curve_node)
    curve_n = engine_iv::Create_Curve( curve, 1000, name );
  //
  common_facilities::Instance()->Model->CommitCommand();

  // Visualize
  this->visualize(false, curve_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                              const Quantity_Color&          color,
                              const TCollection_AsciiString& name)
{
  this->DRAW_SURFACE(surface, 1000, 1000, color, 1.0, name);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                              const Quantity_Color&          color,
                              const double                   opacity,
                              const TCollection_AsciiString& name)
{
  this->DRAW_SURFACE(surface, 1000, 1000, color, opacity, name);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                              const double                   uLimit,
                              const double                   vLimit,
                              const Quantity_Color&          color,
                              const TCollection_AsciiString& name)
{
  this->DRAW_SURFACE(surface, uLimit, vLimit, color, 1.0, name);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                              const double                   uLimit,
                              const double                   vLimit,
                              const Quantity_Color&          color,
                              const double                   opacity,
                              const TCollection_AsciiString& name)
{
  // Create a surface Node
  common_facilities::Instance()->Model->OpenCommand();
  //
  Handle(visu_iv_surface_node)
    surface_n = engine_iv::Create_Surface(surface, uLimit, vLimit, name);
  //
  common_facilities::Instance()->Model->CommitCommand();

  // Visualize
  this->visualize(false, surface_n, true, color, opacity, false);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_SHAPE(const TopoDS_Shape&            shape,
                            const TCollection_AsciiString& name)
{
  this->DRAW_SHAPE(shape, 1.0, name);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_SHAPE(const TopoDS_Shape&            shape,
                            const Quantity_Color&          color,
                            const TCollection_AsciiString& name)
{
  this->DRAW_SHAPE(shape, color, 1.0, name);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_SHAPE(const TopoDS_Shape&            shape,
                            const double                   opacity,
                            const TCollection_AsciiString& name)
{
  // Create a Node for topology item
  common_facilities::Instance()->Model->OpenCommand();
  //
  Handle(visu_iv_topo_item_node)
    item_n = engine_iv::Create_TopoItem(shape, name);
  //
  common_facilities::Instance()->Model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, false, Quantity_Color(), opacity, false);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_SHAPE(const TopoDS_Shape&            shape,
                            const Quantity_Color&          color,
                            const double                   opacity,
                            const TCollection_AsciiString& name)
{
  // Create a Node for topology item
  common_facilities::Instance()->Model->OpenCommand();
  //
  Handle(visu_iv_topo_item_node)
    item_n = engine_iv::Create_TopoItem(shape, name);
  //
  common_facilities::Instance()->Model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, true, color, opacity, false);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_SHAPE(const TopoDS_Shape&            shape,
                            const Quantity_Color&          color,
                            const double                   opacity,
                            const bool                     isWireframe,
                            const TCollection_AsciiString& name)
{
  // Create a Node for topology item
  common_facilities::Instance()->Model->OpenCommand();
  //
  Handle(visu_iv_topo_item_node)
    item_n = engine_iv::Create_TopoItem(shape, name);
  //
  common_facilities::Instance()->Model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, true, color, opacity, isWireframe);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_TRIANGULATION(const Handle(Poly_Triangulation)& shape,
                                    const Quantity_Color&             color,
                                    const double                      opacity,
                                    const TCollection_AsciiString&    name)
{
  // Create a Node for tessellation item
  common_facilities::Instance()->Model->OpenCommand();
  //
  Handle(visu_iv_tess_item_node)
    item_n = engine_iv::Create_TessItem(shape, name);
  //
  common_facilities::Instance()->Model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, true, color, opacity, false);
}

//---------------------------------------------------------------------------//

void journal_iv::DRAW_TEXT(const TCollection_AsciiString& text)
{
  // Create a Node for text item
  common_facilities::Instance()->Model->OpenCommand();
  //
  Handle(visu_iv_text_item_node)
    item_n = engine_iv::Create_TextItem(text);
  //
  common_facilities::Instance()->Model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, false, Quantity_Color(), 0.0, false);
}

//---------------------------------------------------------------------------//

void journal_iv::visualize(const bool                  is2d,
                           const Handle(ActAPI_INode)& node,
                           const bool                  hasColor,
                           const Quantity_Color&       color,
                           const double                opacity,
                           const bool                  isWireframe) const
{
  if ( !this->prsManager(is2d)->IsPresented(node) )
    this->prsManager(is2d)->SetPresentation(node);

  // Set visualization properties
  Handle(visu_prs) prs = this->prsManager(is2d)->GetPresentation(node);

  // Specific treatment for predefined pipelines
  if ( prs->IsKind( STANDARD_TYPE(visu_iv_topo_item_prs) ) )
  {
    Handle(visu_iv_topo_item_prs)
      topo_prs = Handle(visu_iv_topo_item_prs)::DownCast(prs);
    //
    Handle(visu_shape_pipeline)
      pl = Handle(visu_shape_pipeline)::DownCast( topo_prs->GetPipeline(visu_iv_topo_item_prs::Pipeline_Main) );

    // Configure shape visualization
    TopoDS_Shape shape = Handle(visu_iv_topo_item_node)::DownCast(node)->GetShape();
    if ( shape.ShapeType() == TopAbs_EDGE || shape.ShapeType() == TopAbs_WIRE )
    {
      pl->WireframeModeOn();
      pl->Actor()->GetProperty()->SetLineWidth(3.0);
    }
    else if ( isWireframe )
      pl->WireframeModeOn();

    // Process color
    if ( hasColor )
    {
      pl->Mapper()->ScalarVisibilityOff();
      pl->Actor()->GetProperty()->SetColor( color.Red(),
                                            color.Green(),
                                            color.Blue() );
      pl->Actor()->GetProperty()->SetOpacity(opacity);
      //
      pl->Actor()->GetProperty()->SetAmbient(0.8);
      pl->Actor()->GetProperty()->SetDiffuse(0.2);
      pl->Actor()->GetProperty()->SetSpecular(0.0);
    }
  }
  else
  {
    // Set common properties for all pipelines
    Handle(h_visu_pipeline_list) pipelines = prs->GetPipelineList();
    for ( int p = 1; p <= pipelines->Length(); ++p )
    {
      if ( hasColor )
      {
        pipelines->Value(p)->Mapper()->ScalarVisibilityOff();
        pipelines->Value(p)->Actor()->GetProperty()->SetColor( color.Red(),
                                                               color.Green(),
                                                               color.Blue() );
      }
      //
      pipelines->Value(p)->Actor()->GetProperty()->SetOpacity(opacity);
    }
  }

  // Update UI
  this->prsManager(is2d)->Actualize(node.get(), false, false, true);
  //
  common_facilities::Instance()->ObjectBrowser->Populate();
}

//----------------------------------------------------------------------------//

const vtkSmartPointer<visu_prs_manager>& journal_iv::prsManager(const bool is2d) const
{
  return is2d ? this->prsManager2d() : this->prsManager3d();
}

//----------------------------------------------------------------------------//

const vtkSmartPointer<visu_prs_manager>& journal_iv::prsManager3d() const
{
  return common_facilities::Instance()->Prs.Part;
}

//----------------------------------------------------------------------------//

const vtkSmartPointer<visu_prs_manager>& journal_iv::prsManager2d() const
{
  return common_facilities::Instance()->Prs.Domain;
}
