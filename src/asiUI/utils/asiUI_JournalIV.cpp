//-----------------------------------------------------------------------------
// Created on: 11 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_JournalIV.h>

// asiEngine includes
#include <asiEngine_IV.h>

// asiVisu includes
#include <asiVisu_IVPointSetPrs.h>
#include <asiVisu_IVTopoItemPrs.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_ShapePipeline.h>
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_PointCloudUtils.h>
#include <asiAlgo_Utils.h>

// OCCT includes
#include <GC_MakeSegment.hxx>
#include <GCE2d_MakeSegment.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

//---------------------------------------------------------------------------//

void asiUI_JournalIV::ERASE_ALL()
{
  // Loop over the imperative plotter's objects
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->GetChildIterator(true);
        cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();

    if ( this->prsManager2d() )
    {
      if ( this->prsManager2d()->IsPresented(node) )
        this->prsManager2d()->DeletePresentation(node);
    }
    //
    if ( this->prsManager3d() )
    {
      if ( this->prsManager3d()->IsPresented(node) )
        this->prsManager3d()->DeletePresentation(node);
    }
  }

  // Clean up Data Model objects
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  asiEngine_IV(m_model).Clean_All();
  //
  if ( isTx )
    m_model->CommitCommand();
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::ERASE(const TCollection_AsciiString& name)
{
  // Loop over the imperative plotter's objects
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->GetChildIterator(true);
        cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node->GetName() != name )
      continue;

    if ( this->prsManager2d() )
    {
      if ( this->prsManager2d()->IsPresented(node) )
        this->prsManager2d()->DeletePresentation(node);
    }
    //
    if ( this->prsManager3d() )
    {
      if ( this->prsManager3d()->IsPresented(node) )
        this->prsManager3d()->DeletePresentation(node);
    }
  }

  // Clean up Data Model objects
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  asiEngine_IV(m_model).Clean(name);
  //
  if ( isTx )
    m_model->CommitCommand();
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_POINT(const gp_XY&                   coord,
                                 const Quantity_Color&          color,
                                 const TCollection_AsciiString& name)
{
  this->draw_point(coord, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_POINT(const gp_Pnt2d&                point,
                                 const Quantity_Color&          color,
                                 const TCollection_AsciiString& name)
{
  this->DRAW_POINT(point.XY(), color, name);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_POINT(const gp_XYZ&                  coord,
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

void asiUI_JournalIV::DRAW_POINT(const gp_Pnt&                  point,
                                 const Quantity_Color&          color,
                                 const TCollection_AsciiString& name)
{
  this->DRAW_POINT(point.XYZ(), color, name);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_POINT(const TCollection_AsciiString& name,
                                   const gp_XY&                   coord,
                                   const Quantity_Color&          color)
{
  this->draw_point(coord, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_POINT(const TCollection_AsciiString& name,
                                   const gp_Pnt2d&                coord,
                                   const Quantity_Color&          color)
{
  this->draw_point(coord.XY(), color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_POINT(const TCollection_AsciiString& name,
                                   const gp_XYZ&                  coord,
                                   const Quantity_Color&          color)
{
  Handle(HRealArray) coords = new HRealArray(0, 2);
  //
  coords->ChangeValue(0) = coord.X();
  coords->ChangeValue(1) = coord.Y();
  coords->ChangeValue(2) = coord.Z();

  this->draw_points(coords, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_POINT(const TCollection_AsciiString& name,
                                   const gp_Pnt&                  coords,
                                   const Quantity_Color&          color)
{
  this->REDRAW_POINT(name, coords.XYZ(), color);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_POINTS(const Handle(HRealArray)&      coords,
                                  const Quantity_Color&          color,
                                  const TCollection_AsciiString& name)
{
  this->draw_points(coords, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_POINTS(const TCollection_AsciiString& name,
                                    const Handle(HRealArray)&      coords,
                                    const Quantity_Color&          color)
{
  this->draw_points(coords, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_VECTOR_AT(const gp_Pnt&                  P,
                                     const gp_Vec&                  V,
                                     const Quantity_Color&          color,
                                     const TCollection_AsciiString& name)
{
  this->DRAW_LINK(P, P.XYZ() + V.XYZ(), color, name);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_VECTOR_AT(const TCollection_AsciiString& name,
                                       const gp_Pnt&                  P,
                                       const gp_Vec&                  V,
                                       const Quantity_Color&          color)
{
  this->REDRAW_LINK(name, P, P.XYZ() + V.XYZ(), color);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_CURVE(const Handle(Geom_Curve)&      curve,
                                 const Quantity_Color&          color,
                                 const TCollection_AsciiString& name)
{
  this->draw_curve(curve, color, name, false, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_CURVE(const TCollection_AsciiString& name,
                                   const Handle(Geom_Curve)&      curve,
                                   const Quantity_Color&          color)
{
  this->draw_curve(curve, color, name, false, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                                   const Quantity_Color&          color,
                                   const TCollection_AsciiString& name)
{
  this->DRAW_SURFACE(surface, 1000, 1000, color, 1.0, name);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                                   const Quantity_Color&          color,
                                   const double                   opacity,
                                   const TCollection_AsciiString& name)
{
  this->DRAW_SURFACE(surface, 1000, 1000, color, opacity, name);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                                   const double                   uLimit,
                                   const double                   vLimit,
                                   const Quantity_Color&          color,
                                   const TCollection_AsciiString& name)
{
  this->DRAW_SURFACE(surface, uLimit, vLimit, color, 1.0, name);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                                   const double                   uLimit,
                                   const double                   vLimit,
                                   const Quantity_Color&          color,
                                   const double                   opacity,
                                   const TCollection_AsciiString& name)
{
  this->draw_surface(surface, uLimit, vLimit, color, opacity, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_SURFACE(const TCollection_AsciiString& name,
                                     const Handle(Geom_Surface)&    surface,
                                     const Quantity_Color&          color)
{
  this->REDRAW_SURFACE(name, surface, 1000, 1000, color, 1.0);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_SURFACE(const TCollection_AsciiString& name,
                                     const Handle(Geom_Surface)&    surface,
                                     const Quantity_Color&          color,
                                     const double                   opacity)
{
  this->REDRAW_SURFACE(name, surface, 1000, 1000, color, opacity);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_SURFACE(const TCollection_AsciiString& name,
                                     const Handle(Geom_Surface)&    surface,
                                     const double                   uLimit,
                                     const double                   vLimit,
                                     const Quantity_Color&          color)
{
  this->REDRAW_SURFACE(name, surface, uLimit, vLimit, color, 1.0);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_SURFACE(const TCollection_AsciiString& name,
                                     const Handle(Geom_Surface)&    surface,
                                     const double                   uLimit,
                                     const double                   vLimit,
                                     const Quantity_Color&          color,
                                     const double                   opacity)
{
  this->draw_surface(surface, uLimit, vLimit, color, opacity, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                                 const TCollection_AsciiString& name)
{
  this->DRAW_SHAPE(shape, 1.0, name);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                                 const Quantity_Color&          color,
                                 const TCollection_AsciiString& name)
{
  this->DRAW_SHAPE(shape, color, 1.0, name);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                                 const double                   opacity,
                                 const TCollection_AsciiString& name)
{
  this->draw_shape(shape, false, Quantity_Color(), opacity, false, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                                 const Quantity_Color&          color,
                                 const double                   opacity,
                                 const TCollection_AsciiString& name)
{
  this->draw_shape(shape, true, color, opacity, false, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                                 const Quantity_Color&          color,
                                 const double                   opacity,
                                 const bool                     isWireframe,
                                 const TCollection_AsciiString& name)
{
  this->draw_shape(shape, true, color, opacity, isWireframe, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_SHAPE(const TCollection_AsciiString& name,
                                   const TopoDS_Shape&            shape)
{
  this->REDRAW_SHAPE(name, shape, 1.0);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_SHAPE(const TCollection_AsciiString& name,
                                   const TopoDS_Shape&            shape,
                                   const Quantity_Color&          color)
{
  this->REDRAW_SHAPE(name, shape, color, 1.0);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_SHAPE(const TCollection_AsciiString& name,
                                   const TopoDS_Shape&            shape,
                                   const double                   opacity)
{
  this->draw_shape(shape, false, Quantity_Color(), opacity, false, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_SHAPE(const TCollection_AsciiString& name,
                                   const TopoDS_Shape&            shape,
                                   const Quantity_Color&          color,
                                   const double                   opacity)
{
  this->draw_shape(shape, true, color, opacity, false, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_SHAPE(const TCollection_AsciiString& name,
                                   const TopoDS_Shape&            shape,
                                   const Quantity_Color&          color,
                                   const double                   opacity,
                                   const bool                     isWireframe)
{
  this->draw_shape(shape, true, color, opacity, isWireframe, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_LINK(const gp_Pnt&                  p1,
                                const gp_Pnt&                  p2,
                                const Quantity_Color&          color,
                                const TCollection_AsciiString& name)
{
  this->draw_link(p1.XYZ(), p2.XYZ(), color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_LINK(const gp_XYZ&                  p1,
                                const gp_XYZ&                  p2,
                                const Quantity_Color&          color,
                                const TCollection_AsciiString& name)
{
  this->draw_link(p1, p2, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_LINK(const gp_Pnt2d&                p1,
                                const gp_Pnt2d&                p2,
                                const Quantity_Color&          color,
                                const TCollection_AsciiString& name)
{
  this->draw_link(p1.XY(), p2.XY(), color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_LINK(const gp_XY&                   p1,
                                const gp_XY&                   p2,
                                const Quantity_Color&          color,
                                const TCollection_AsciiString& name)
{
  this->draw_link(p1, p2, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_LINK(const TCollection_AsciiString& name,
                                  const gp_Pnt&                  p1,
                                  const gp_Pnt&                  p2,
                                  const Quantity_Color&          color)
{
  this->draw_link(p1.XYZ(), p2.XYZ(), color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_LINK(const TCollection_AsciiString& name,
                                  const gp_XYZ&                  p1,
                                  const gp_XYZ&                  p2,
                                  const Quantity_Color&          color)
{
  this->draw_link(p1, p2, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_LINK(const TCollection_AsciiString& name,
                                  const gp_Pnt2d&                p1,
                                  const gp_Pnt2d&                p2,
                                  const Quantity_Color&          color)
{
  this->draw_link(p1.XY(), p2.XY(), color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_LINK(const TCollection_AsciiString& name,
                                  const gp_XY&                   p1,
                                  const gp_XY&                   p2,
                                  const Quantity_Color&          color)
{
  this->draw_link(p1, p2, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_POLYLINE(const std::vector<gp_XYZ>&     poles,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name)
{
  this->draw_polyline(poles, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_POLYLINE(const std::vector<gp_XY>&      poles,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name)
{
  this->draw_polyline(poles, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_POLYLINE(const TCollection_AsciiString& name,
                                      const std::vector<gp_XY>&      poles,
                                      const Quantity_Color&          color)
{
  this->draw_polyline(poles, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_POLYLINE(const TCollection_AsciiString& name,
                                      const std::vector<gp_XYZ>&     poles,
                                      const Quantity_Color&          color)
{
  this->draw_polyline(poles, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_TRIANGLE(const gp_Pnt&                  p1,
                                    const gp_Pnt&                  p2,
                                    const gp_Pnt&                  p3,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name)
{
  this->draw_triangle(p1.XYZ(), p2.XYZ(), p3.XYZ(), color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_TRIANGLE(const gp_XYZ&                  p1,
                                    const gp_XYZ&                  p2,
                                    const gp_XYZ&                  p3,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name)
{
  this->draw_triangle(p1, p2, p3, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_TRIANGLE(const gp_Pnt2d&                p1,
                                    const gp_Pnt2d&                p2,
                                    const gp_Pnt2d&                p3,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name)
{
  this->draw_triangle(p1.XY(), p2.XY(), p3.XY(), color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_TRIANGLE(const gp_XY&                   p1,
                                    const gp_XY&                   p2,
                                    const gp_XY&                   p3,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name)
{
  this->draw_triangle(p1, p2, p3, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_TRIANGLE(const TCollection_AsciiString& name,
                                      const gp_Pnt&                  p1,
                                      const gp_Pnt&                  p2,
                                      const gp_Pnt&                  p3,
                                      const Quantity_Color&          color)
{
  this->draw_triangle(p1.XYZ(), p2.XYZ(), p3.XYZ(), color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_TRIANGLE(const TCollection_AsciiString& name,
                                      const gp_XYZ&                  p1,
                                      const gp_XYZ&                  p2,
                                      const gp_XYZ&                  p3,
                                      const Quantity_Color&          color)
{
  this->draw_triangle(p1, p2, p3, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_TRIANGLE(const TCollection_AsciiString& name,
                                      const gp_Pnt2d&                p1,
                                      const gp_Pnt2d&                p2,
                                      const gp_Pnt2d&                p3,
                                      const Quantity_Color&          color)
{
  this->draw_triangle(p1.XY(), p2.XY(), p3.XY(), color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_TRIANGLE(const TCollection_AsciiString& name,
                                      const gp_XY&                   p1,
                                      const gp_XY&                   p2,
                                      const gp_XY&                   p3,
                                      const Quantity_Color&          color)
{
  this->draw_triangle(p1, p2, p3, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_TRIANGULATION(const Handle(Poly_Triangulation)& tris,
                                         const Quantity_Color&             color,
                                         const double                      opacity,
                                         const TCollection_AsciiString&    name)
{
  this->draw_triangulation(tris, color, opacity, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_TRIANGULATION(const TCollection_AsciiString&    name,
                                           const Handle(Poly_Triangulation)& tris,
                                           const Quantity_Color&             color,
                                           const double                      opacity)
{
  this->draw_triangulation(tris, color, opacity, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_TEXT(const TCollection_AsciiString& text,
                                const TCollection_AsciiString& name)
{
  this->draw_text(text, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::REDRAW_TEXT(const TCollection_AsciiString& name,
                                  const TCollection_AsciiString& text)
{
  this->draw_text(text, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_ASI_POINTSF(const Handle(asiAlgo_BaseCloud<float>)& points,
                                       const Quantity_Color&                   color,
                                       const TCollection_AsciiString&          name)
{
  // Create a new Node for the given point set
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVPointSetNode)
    points_n = asiEngine_IV(m_model).Create_PointSet(asiAlgo_PointCloudUtils::PointCloudfAsPointCloudd(points), name, true);
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, points_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_ASI_POINTS(const std::vector<gp_Pnt2d>&   points,
                                      const Quantity_Color&          color,
                                      const TCollection_AsciiString& name)
{
  // Create a new Node for the given point set
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVPointSet2dNode)
    points_n = asiEngine_IV(m_model).Create_PointSet2d(points, name, true);
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(true, points_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::visualize(const bool                  is2d,
                                const Handle(ActAPI_INode)& node,
                                const bool                  hasColor,
                                const Quantity_Color&       color,
                                const double                opacity,
                                const bool                  isWireframe) const
{
  if ( !this->prsManager(is2d)->IsPresented(node) )
    this->prsManager(is2d)->SetPresentation(node);

  // Set visualization properties
  Handle(asiVisu_Prs) prs = this->prsManager(is2d)->GetPresentation(node);

  // Specific treatment for predefined pipelines
  if ( prs->IsKind( STANDARD_TYPE(asiVisu_IVTopoItemPrs) ) )
  {
    Handle(asiVisu_IVTopoItemPrs)
      topo_prs = Handle(asiVisu_IVTopoItemPrs)::DownCast(prs);
    //
    Handle(asiVisu_ShapePipeline)
      pl = Handle(asiVisu_ShapePipeline)::DownCast( topo_prs->GetPipeline(asiVisu_IVTopoItemPrs::Pipeline_Main) );
    //
    if ( pl.IsNull() )
    {
      std::cout << "Pipeline is NULL" << std::endl;
      return;
    }

    // Configure shape visualization
    TopoDS_Shape shape = Handle(asiData_IVTopoItemNode)::DownCast(node)->GetShape();
    //
    if ( isWireframe )
    {
      pl->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_Wireframe);
    }

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
    Handle(asiVisu_HPipelineList) pipelines = prs->GetPipelineList();
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
  m_pBrowser->Populate();
}

//---------------------------------------------------------------------------//

const vtkSmartPointer<asiVisu_PrsManager>& asiUI_JournalIV::prsManager(const bool is2d) const
{
  return is2d ? this->prsManager2d() : this->prsManager3d();
}

//---------------------------------------------------------------------------//

const vtkSmartPointer<asiVisu_PrsManager>& asiUI_JournalIV::prsManager3d() const
{
  return m_prsMgr3d;
}

//---------------------------------------------------------------------------//

const vtkSmartPointer<asiVisu_PrsManager>& asiUI_JournalIV::prsManager2d() const
{
  return m_prsMgr2d;
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_point(const gp_XY&                   coord,
                                 const Quantity_Color&          color,
                                 const TCollection_AsciiString& name,
                                 const bool                     newPrimitive)
{
  Handle(HRealArray) coords = new HRealArray(0, 1);
  //
  coords->ChangeValue(0) = coord.X();
  coords->ChangeValue(1) = coord.Y();

  // Open transaction
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }

  // Modify data
  Handle(asiData_IVPointSet2dNode) points_n;
  //
  bool doCreate = newPrimitive;
  //
  if ( !doCreate )
  {
    points_n = asiEngine_IV(m_model).Find_PointSet2d(name);
    //
    if ( !points_n.IsNull() )
      points_n->SetPoints(coords);
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    points_n = asiEngine_IV(m_model).Create_PointSet2d(coords, name, newPrimitive);
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(true, points_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_points(const Handle(HRealArray)&      coords,
                                  const Quantity_Color&          color,
                                  const TCollection_AsciiString& name,
                                  const bool                     newPrimitive)
{
  // Open transaction
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }

  // Modify data
  Handle(asiData_IVPointSetNode) points_n;
  //
  bool doCreate = newPrimitive;
  //
  if ( !doCreate )
  {
    points_n = asiEngine_IV(m_model).Find_PointSet(name);
    //
    if ( !points_n.IsNull() )
      points_n->SetPoints( asiAlgo_PointCloudUtils::AsPointCloud(coords) );
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    points_n = asiEngine_IV(m_model).Create_PointSet(asiAlgo_PointCloudUtils::AsPointCloud(coords), name, newPrimitive);
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, points_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_curve(const Handle(Geom_Curve)&      curve,
                                 const Quantity_Color&          color,
                                 const TCollection_AsciiString& name,
                                 const bool                     is2d,
                                 const bool                     newPrimitive)
{
  // Open transaction
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }

  // Modify data
  Handle(asiData_IVCurveNode) curve_n;
  //
  bool doCreate = newPrimitive;
  //
  if ( !doCreate )
  {
    curve_n = asiEngine_IV(m_model).Find_Curve(name);
    //
    if ( !curve_n.IsNull() )
      asiEngine_IV(m_model).Update_Curve(curve_n, curve, 1000);
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    curve_n = asiEngine_IV(m_model).Create_Curve(curve, 1000, name, newPrimitive);
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(is2d, curve_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_surface(const Handle(Geom_Surface)&    surface,
                                   const double                   uLimit,
                                   const double                   vLimit,
                                   const Quantity_Color&          color,
                                   const double                   opacity,
                                   const TCollection_AsciiString& name,
                                   const bool                     newPrimitive)
{
  // Open transaction
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }

  // Modify data
  Handle(asiData_IVSurfaceNode) surface_n;
  //
  bool doCreate = newPrimitive;
  //
  if ( !doCreate )
  {
    surface_n = asiEngine_IV(m_model).Find_Surface(name);
    //
    if ( !surface_n.IsNull() )
      asiEngine_IV(m_model).Update_Surface(surface_n, surface, uLimit, vLimit);
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    surface_n = asiEngine_IV(m_model).Create_Surface(surface, uLimit, vLimit, name, newPrimitive);
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, surface_n, true, color, opacity, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_shape(const TopoDS_Shape&            shape,
                                 const bool                     hasColor,
                                 const Quantity_Color&          color,
                                 const double                   opacity,
                                 const bool                     isWireframe,
                                 const TCollection_AsciiString& name,
                                 const bool                     newPrimitive)
{
  // Open transaction
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }

  // Modify data
  Handle(asiData_IVTopoItemNode) item_n;
  //
  bool doCreate = newPrimitive;
  //
  if ( !doCreate )
  {
    item_n = asiEngine_IV(m_model).Find_TopoItem(name);
    //
    if ( !item_n.IsNull() )
      asiEngine_IV(m_model).Update_TopoItem(item_n, shape);
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    item_n = asiEngine_IV(m_model).Create_TopoItem(shape, name, newPrimitive);
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, hasColor, color, opacity, isWireframe);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_link(const gp_XYZ&                  p1,
                                const gp_XYZ&                  p2,
                                const Quantity_Color&          color,
                                const TCollection_AsciiString& name,
                                const bool                     is2d,
                                const bool                     newPrimitive)
{
  if ( (p1 - p2).Modulus() < 1.0e-5 )
    return;

  // Create a straight line segment between two passed points
  Handle(Geom_TrimmedCurve) C = GC_MakeSegment(p1, p2);

  // Open transaction
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }

  // Modify data
  Handle(asiData_IVCurveNode) curve_n;
  //
  bool doCreate = newPrimitive;
  //
  if ( !doCreate )
  {
    curve_n = asiEngine_IV(m_model).Find_Curve(name);
    //
    if ( !curve_n.IsNull() )
      asiEngine_IV(m_model).Update_Curve( curve_n, C, Precision::Infinite() );
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    curve_n = asiEngine_IV(m_model).Create_Curve(C, Precision::Infinite(), name, newPrimitive);
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(is2d, curve_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_link(const gp_XYZ&                  p1,
                                const gp_XYZ&                  p2,
                                const Quantity_Color&          color,
                                const TCollection_AsciiString& name,
                                const bool                     newPrimitive)
{
  this->draw_link(p1, p2, color, name, false, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_link(const gp_XY&                   p1,
                                const gp_XY&                   p2,
                                const Quantity_Color&          color,
                                const TCollection_AsciiString& name,
                                const bool                     newPrimitive)
{
  gp_XYZ p1_3d(p1.X(), p1.Y(), 0.0), p2_3d(p2.X(), p2.Y(), 0.0);

  this->draw_link(p1_3d, p2_3d, color, name, true, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_polyline(const std::vector<gp_XYZ>&     poles,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name,
                                    const bool                     is2d,
                                    const bool                     newPrimitive)
{
  Handle(Geom_BSplineCurve) curve = asiAlgo_Utils::PolylineAsSpline(poles);

  this->draw_curve(curve, color, name, is2d, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_polyline(const std::vector<gp_XYZ>&     poles,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name,
                                    const bool                     newPrimitive)
{
  Handle(Geom_BSplineCurve) curve = asiAlgo_Utils::PolylineAsSpline(poles);

  this->draw_curve(curve, color, name, false, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_polyline(const std::vector<gp_XY>&      poles,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name,
                                    const bool                     newPrimitive)
{
  // Convert 2-dimensional poles to 3-dimensional poles for unification
  std::vector<gp_XYZ> poles3d;
  //
  for ( size_t k = 0; k < poles.size(); ++k )
  {
    poles3d.push_back( gp_XYZ( poles[k].X(), poles[k].Y(), 0.0) );
  }

  this->draw_polyline(poles3d, color, name, true, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_triangle(const gp_XYZ&                  p1,
                                    const gp_XYZ&                  p2,
                                    const gp_XYZ&                  p3,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name,
                                    const bool                     newPrimitive)
{
  std::vector<gp_XYZ> poles = {p1, p2, p3, p1};

  this->draw_polyline(poles, color, name, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_triangle(const gp_XY&                   p1,
                                    const gp_XY&                   p2,
                                    const gp_XY&                   p3,
                                    const Quantity_Color&          color,
                                    const TCollection_AsciiString& name,
                                    const bool                     newPrimitive)
{
  std::vector<gp_XY> poles = {p1, p2, p3, p1};

  this->draw_polyline(poles, color, name, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_triangulation(const Handle(Poly_Triangulation)& tris,
                                         const Quantity_Color&             color,
                                         const double                      opacity,
                                         const TCollection_AsciiString&    name,
                                         const bool                        newPrimitive)
{
  // Open transaction
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }

  // Modify data
  Handle(asiData_IVTessItemNode) item_n;
  //
  bool doCreate = newPrimitive;
  //
  if ( !doCreate )
  {
    item_n = asiEngine_IV(m_model).Find_TessItem(name);
    //
    if ( !item_n.IsNull() )
      asiEngine_IV(m_model).Update_TessItem(item_n, tris);
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    item_n = asiEngine_IV(m_model).Create_TessItem(tris, name, newPrimitive);
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, true, color, opacity, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::draw_text(const TCollection_AsciiString& text,
                                const TCollection_AsciiString& name,
                                const bool                     newPrimitive)
{
  // Open transaction
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }

  // Modify data
  Handle(asiData_IVTextItemNode) item_n;
  //
  bool doCreate = newPrimitive;
  //
  if ( !doCreate )
  {
    item_n = asiEngine_IV(m_model).Find_TextItem(name);
    //
    if ( !item_n.IsNull() )
      asiEngine_IV(m_model).Update_TextItem(item_n, text);
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    item_n = asiEngine_IV(m_model).Create_TextItem(text, name, newPrimitive);
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, false, Quantity_Color(), 0.0, false);
}
