//-----------------------------------------------------------------------------
// Created on: 11 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_IV.h>

// asiEngine includes
#include <asiEngine_IV.h>
#include <asiEngine_Part.h>

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
#pragma warning(push, 0)
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>
#pragma warning(pop)

//---------------------------------------------------------------------------//

void asiUI_IV::FIT3D()
{
  if ( m_prsMgr3d )
  {
    asiVisu_Utils::AdjustCamera( m_prsMgr3d->GetRenderer(),
                                 m_prsMgr3d->PropsByTrihedron() );
    //
    if ( m_prsMgr3d->GetQVTKWidget() )
      m_prsMgr3d->GetQVTKWidget()->GetRenderWindow()->Render();
  }
}

//---------------------------------------------------------------------------//

void asiUI_IV::ERASE_ALL()
{
  // Loop over the imperative plotter's objects
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->GetChildIterator(true);
        cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();

    if ( m_prsMgr2d )
    {
      if ( m_prsMgr2d->IsPresented(node) )
        m_prsMgr2d->DeletePresentation(node);
    }
    //
    if ( m_prsMgr3d )
    {
      if ( m_prsMgr3d->IsPresented(node) )
        m_prsMgr3d->DeletePresentation(node);
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

void asiUI_IV::ERASE(const TCollection_AsciiString& name)
{
  // Loop over the imperative plotter's objects
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->GetChildIterator(true);
        cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node->GetName() != name )
      continue;

    if ( m_prsMgr2d )
    {
      if ( m_prsMgr2d->IsPresented(node) )
        m_prsMgr2d->DeletePresentation(node);
    }
    //
    if ( m_prsMgr3d )
    {
      if ( m_prsMgr3d->IsPresented(node) )
        m_prsMgr3d->DeletePresentation(node);
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

void asiUI_IV::DUMP_PNG3D(const char* filename)
{
  if ( m_prsMgr3d )
    m_prsMgr3d->DumpPNG(filename);
}

//---------------------------------------------------------------------------//

void asiUI_IV::UPDATE_PART()
{
  if ( m_prsMgr3d )
    m_prsMgr3d->Actualize( m_model->GetPartNode() );
  //
  if ( m_pBrowser )
    m_pBrowser->Populate();
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_POINT(const gp_XY&                   coord,
                          const ActAPI_Color&            color,
                          const TCollection_AsciiString& name)
{
  this->draw_point(coord, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_POINT(const gp_Pnt2d&                point,
                          const ActAPI_Color&            color,
                          const TCollection_AsciiString& name)
{
  this->DRAW_POINT(point.XY(), color, name);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_POINT(const gp_XYZ&                  coord,
                          const ActAPI_Color&            color,
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

void asiUI_IV::DRAW_POINT(const gp_Pnt&                  point,
                          const ActAPI_Color&            color,
                          const TCollection_AsciiString& name)
{
  this->DRAW_POINT(point.XYZ(), color, name);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_POINT(const TCollection_AsciiString& name,
                            const gp_XY&                   coord,
                            const ActAPI_Color&            color)
{
  this->draw_point(coord, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_POINT(const TCollection_AsciiString& name,
                            const gp_Pnt2d&                coord,
                            const ActAPI_Color&            color)
{
  this->draw_point(coord.XY(), color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_POINT(const TCollection_AsciiString& name,
                            const gp_XYZ&                  coord,
                            const ActAPI_Color&            color)
{
  Handle(HRealArray) coords = new HRealArray(0, 2);
  //
  coords->ChangeValue(0) = coord.X();
  coords->ChangeValue(1) = coord.Y();
  coords->ChangeValue(2) = coord.Z();

  this->draw_points(coords, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_POINT(const TCollection_AsciiString& name,
                            const gp_Pnt&                  coords,
                            const ActAPI_Color&            color)
{
  this->REDRAW_POINT(name, coords.XYZ(), color);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_POINTS(const Handle(HRealArray)&      coords,
                           const ActAPI_Color&            color,
                           const TCollection_AsciiString& name)
{
  this->draw_points(coords, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_POINTS(const TCollection_AsciiString& name,
                             const Handle(HRealArray)&      coords,
                             const ActAPI_Color&            color)
{
  this->draw_points(coords, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_VECTOR_AT(const gp_Pnt&                  P,
                              const gp_Vec&                  V,
                              const ActAPI_Color&            color,
                              const TCollection_AsciiString& name)
{
  this->DRAW_LINK(P, P.XYZ() + V.XYZ(), color, name);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_VECTOR_AT(const TCollection_AsciiString& name,
                                const gp_Pnt&                  P,
                                const gp_Vec&                  V,
                                const ActAPI_Color&            color)
{
  this->REDRAW_LINK(name, P, P.XYZ() + V.XYZ(), color);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_CURVE(const Handle(Geom_Curve)&      curve,
                          const ActAPI_Color&            color,
                          const TCollection_AsciiString& name)
{
  this->draw_curve(curve, color, name, true, false, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_CURVE(const Handle(Geom_Curve)&      curve,
                          const ActAPI_Color&            color,
                          const bool                     drawOri,
                          const TCollection_AsciiString& name)
{
  this->draw_curve(curve, color, name, drawOri, false, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_CURVE(const TCollection_AsciiString& name,
                            const Handle(Geom_Curve)&      curve,
                            const ActAPI_Color&            color)
{
  this->draw_curve(curve, color, name, true, false, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_CURVE(const TCollection_AsciiString& name,
                            const Handle(Geom_Curve)&      curve,
                            const ActAPI_Color&            color,
                            const bool                     drawOri)
{
  this->draw_curve(curve, color, name, drawOri, false, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_CURVE2D(const Handle(Geom2d_Curve)&    curve,
                            const ActAPI_Color&            color,
                            const TCollection_AsciiString& name)
{
  this->draw_curve2d(curve, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_CURVE2D(const TCollection_AsciiString& name,
                              const Handle(Geom2d_Curve)&    curve,
                              const ActAPI_Color&            color)
{
  this->draw_curve2d(curve, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                            const ActAPI_Color&            color,
                            const TCollection_AsciiString& name)
{
  if ( surface.IsNull() )
    return;

  double uMin, uMax, vMin, vMax;
  surface->Bounds(uMin, uMax, vMin, vMax);
  //
  uMin = asiVisu_Utils::TrimInf(uMin);
  uMax = asiVisu_Utils::TrimInf(uMax);
  vMin = asiVisu_Utils::TrimInf(vMin);
  vMax = asiVisu_Utils::TrimInf(vMax);

  this->DRAW_SURFACE(surface, uMin, uMax, vMin, vMax, color, 1.0, name);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                            const ActAPI_Color&            color,
                            const double                   opacity,
                            const TCollection_AsciiString& name)
{
  if ( surface.IsNull() )
    return;

  double uMin, uMax, vMin, vMax;
  surface->Bounds(uMin, uMax, vMin, vMax);
  //
  uMin = asiVisu_Utils::TrimInf(uMin);
  uMax = asiVisu_Utils::TrimInf(uMax);
  vMin = asiVisu_Utils::TrimInf(vMin);
  vMax = asiVisu_Utils::TrimInf(vMax);

  this->DRAW_SURFACE(surface, uMin, uMax, vMin, vMax, color, opacity, name);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                            const double                   uMin,
                            const double                   uMax,
                            const double                   vMin,
                            const double                   vMax,
                            const ActAPI_Color&            color,
                            const TCollection_AsciiString& name)
{
  this->DRAW_SURFACE(surface, uMin, uMax, vMin, vMax, color, 1.0, name);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SURFACE(const Handle(Geom_Surface)&    surface,
                            const double                   uMin,
                            const double                   uMax,
                            const double                   vMin,
                            const double                   vMax,
                            const ActAPI_Color&            color,
                            const double                   opacity,
                            const TCollection_AsciiString& name)
{
  this->draw_surface(surface, uMin, uMax, vMin, vMax, color, opacity, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_SURFACE(const TCollection_AsciiString& name,
                              const Handle(Geom_Surface)&    surface,
                              const ActAPI_Color&            color)
{
  if ( surface.IsNull() )
    return;

  double uMin, uMax, vMin, vMax;
  surface->Bounds(uMin, uMax, vMin, vMax);
  //
  uMin = asiVisu_Utils::TrimInf(uMin);
  uMax = asiVisu_Utils::TrimInf(uMax);
  vMin = asiVisu_Utils::TrimInf(vMin);
  vMax = asiVisu_Utils::TrimInf(vMax);

  this->REDRAW_SURFACE(name, surface, uMin, uMax, vMin, vMax, color, 1.0);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_SURFACE(const TCollection_AsciiString& name,
                              const Handle(Geom_Surface)&    surface,
                              const ActAPI_Color&            color,
                              const double                   opacity)
{
  if ( surface.IsNull() )
    return;

  double uMin, uMax, vMin, vMax;
  surface->Bounds(uMin, uMax, vMin, vMax);
  //
  uMin = asiVisu_Utils::TrimInf(uMin);
  uMax = asiVisu_Utils::TrimInf(uMax);
  vMin = asiVisu_Utils::TrimInf(vMin);
  vMax = asiVisu_Utils::TrimInf(vMax);

  this->REDRAW_SURFACE(name, surface, uMin, uMax, vMin, vMax, color, opacity);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_SURFACE(const TCollection_AsciiString& name,
                              const Handle(Geom_Surface)&    surface,
                              const double                   uMin,
                              const double                   uMax,
                              const double                   vMin,
                              const double                   vMax,
                              const ActAPI_Color&            color)
{
  this->REDRAW_SURFACE(name, surface, uMin, uMax, vMin, vMax, color, 1.0);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_SURFACE(const TCollection_AsciiString& name,
                              const Handle(Geom_Surface)&    surface,
                              const double                   uMin,
                              const double                   uMax,
                              const double                   vMin,
                              const double                   vMax,
                              const ActAPI_Color&            color,
                              const double                   opacity)
{
  this->draw_surface(surface, uMin, uMax, vMin, vMax, color, opacity, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                          const TCollection_AsciiString& name)
{
  this->DRAW_SHAPE(shape, 1.0, name);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                          const ActAPI_Color&            color,
                          const TCollection_AsciiString& name)
{
  this->DRAW_SHAPE(shape, color, 1.0, name);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                          const double                   opacity,
                          const TCollection_AsciiString& name)
{
  this->draw_shape(shape, false, ActAPI_Color(), opacity, false, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                          const ActAPI_Color&            color,
                          const double                   opacity,
                          const TCollection_AsciiString& name)
{
  this->draw_shape(shape, true, color, opacity, false, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                          const ActAPI_Color&            color,
                          const double                   opacity,
                          const bool                     isWireframe,
                          const TCollection_AsciiString& name)
{
  this->draw_shape(shape, true, color, opacity, isWireframe, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_SHAPE(const TCollection_AsciiString& name,
                            const TopoDS_Shape&            shape)
{
  this->REDRAW_SHAPE(name, shape, 1.0);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_SHAPE(const TCollection_AsciiString& name,
                            const TopoDS_Shape&            shape,
                            const ActAPI_Color&            color)
{
  this->REDRAW_SHAPE(name, shape, color, 1.0);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_SHAPE(const TCollection_AsciiString& name,
                            const TopoDS_Shape&            shape,
                            const double                   opacity)
{
  this->draw_shape(shape, false, ActAPI_Color(), opacity, false, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_SHAPE(const TCollection_AsciiString& name,
                            const TopoDS_Shape&            shape,
                            const ActAPI_Color&            color,
                            const double                   opacity)
{
  this->draw_shape(shape, true, color, opacity, false, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_SHAPE(const TCollection_AsciiString& name,
                            const TopoDS_Shape&            shape,
                            const ActAPI_Color&            color,
                            const double                   opacity,
                            const bool                     isWireframe)
{
  this->draw_shape(shape, true, color, opacity, isWireframe, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_LINK(const gp_Pnt&                  p1,
                         const gp_Pnt&                  p2,
                         const ActAPI_Color&            color,
                         const TCollection_AsciiString& name)
{
  this->draw_link(p1.XYZ(), p2.XYZ(), color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_LINK(const gp_XYZ&                  p1,
                         const gp_XYZ&                  p2,
                         const ActAPI_Color&            color,
                         const TCollection_AsciiString& name)
{
  this->draw_link(p1, p2, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_LINK(const gp_Pnt2d&                p1,
                         const gp_Pnt2d&                p2,
                         const ActAPI_Color&            color,
                         const TCollection_AsciiString& name)
{
  this->draw_link(p1.XY(), p2.XY(), color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_LINK(const gp_XY&                   p1,
                         const gp_XY&                   p2,
                         const ActAPI_Color&            color,
                         const TCollection_AsciiString& name)
{
  this->draw_link(p1, p2, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_LINK(const TCollection_AsciiString& name,
                           const gp_Pnt&                  p1,
                           const gp_Pnt&                  p2,
                           const ActAPI_Color&            color)
{
  this->draw_link(p1.XYZ(), p2.XYZ(), color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_LINK(const TCollection_AsciiString& name,
                           const gp_XYZ&                  p1,
                           const gp_XYZ&                  p2,
                           const ActAPI_Color&            color)
{
  this->draw_link(p1, p2, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_LINK(const TCollection_AsciiString& name,
                           const gp_Pnt2d&                p1,
                           const gp_Pnt2d&                p2,
                           const ActAPI_Color&            color)
{
  this->draw_link(p1.XY(), p2.XY(), color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_LINK(const TCollection_AsciiString& name,
                           const gp_XY&                   p1,
                           const gp_XY&                   p2,
                           const ActAPI_Color&            color)
{
  this->draw_link(p1, p2, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_POLYLINE(const std::vector<gp_XYZ>&     poles,
                             const ActAPI_Color&            color,
                             const TCollection_AsciiString& name)
{
  this->draw_polyline(poles, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_POLYLINE(const std::vector<gp_XY>&      poles,
                             const ActAPI_Color&            color,
                             const TCollection_AsciiString& name)
{
  this->draw_polyline(poles, color, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_POLYLINE(const TCollection_AsciiString& name,
                               const std::vector<gp_XY>&      poles,
                               const ActAPI_Color&            color)
{
  this->draw_polyline(poles, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_POLYLINE(const TCollection_AsciiString& name,
                               const std::vector<gp_XYZ>&     poles,
                               const ActAPI_Color&            color)
{
  this->draw_polyline(poles, color, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_TRIANGLE(const gp_Pnt&                  p1,
                             const gp_Pnt&                  p2,
                             const gp_Pnt&                  p3,
                             const ActAPI_Color&            color,
                             const TCollection_AsciiString& name)
{
  this->draw_triangle(p1.XYZ(), p2.XYZ(), p3.XYZ(), color, name, true, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_TRIANGLE(const gp_XYZ&                  p1,
                             const gp_XYZ&                  p2,
                             const gp_XYZ&                  p3,
                             const ActAPI_Color&            color,
                             const TCollection_AsciiString& name)
{
  this->draw_triangle(p1, p2, p3, color, name, true, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_TRIANGLE(const gp_Pnt2d&                p1,
                             const gp_Pnt2d&                p2,
                             const gp_Pnt2d&                p3,
                             const ActAPI_Color&            color,
                             const TCollection_AsciiString& name)
{
  this->draw_triangle(p1.XY(), p2.XY(), p3.XY(), color, name, true, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_TRIANGLE(const gp_XY&                   p1,
                             const gp_XY&                   p2,
                             const gp_XY&                   p3,
                             const ActAPI_Color&            color,
                             const TCollection_AsciiString& name)
{
  this->draw_triangle(p1, p2, p3, color, name, true, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_TRIANGLE(const TCollection_AsciiString& name,
                               const gp_Pnt&                  p1,
                               const gp_Pnt&                  p2,
                               const gp_Pnt&                  p3,
                               const ActAPI_Color&            color)
{
  this->draw_triangle(p1.XYZ(), p2.XYZ(), p3.XYZ(), color, name, false, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_TRIANGLE(const TCollection_AsciiString& name,
                               const gp_XYZ&                  p1,
                               const gp_XYZ&                  p2,
                               const gp_XYZ&                  p3,
                               const ActAPI_Color&            color)
{
  this->draw_triangle(p1, p2, p3, color, name, false, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_TRIANGLE(const TCollection_AsciiString& name,
                               const gp_Pnt2d&                p1,
                               const gp_Pnt2d&                p2,
                               const gp_Pnt2d&                p3,
                               const ActAPI_Color&            color)
{
  this->draw_triangle(p1.XY(), p2.XY(), p3.XY(), color, name, false, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_TRIANGLE(const TCollection_AsciiString& name,
                               const gp_XY&                   p1,
                               const gp_XY&                   p2,
                               const gp_XY&                   p3,
                               const ActAPI_Color&            color)
{
  this->draw_triangle(p1, p2, p3, color, name, false, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_TRIANGULATION(const Handle(Poly_Triangulation)& tris,
                                  const ActAPI_Color&               color,
                                  const double                      opacity,
                                  const TCollection_AsciiString&    name)
{
  this->draw_triangulation(tris, color, opacity, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_TRIANGULATION(const TCollection_AsciiString&    name,
                                    const Handle(Poly_Triangulation)& tris,
                                    const ActAPI_Color&               color,
                                    const double                      opacity)
{
  this->draw_triangulation(tris, color, opacity, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_TEXT(const TCollection_AsciiString& text,
                         const TCollection_AsciiString& name)
{
  this->draw_text(text, name, true);
}

//---------------------------------------------------------------------------//

void asiUI_IV::REDRAW_TEXT(const TCollection_AsciiString& name,
                           const TCollection_AsciiString& text)
{
  this->draw_text(text, name, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_ASI_POINTSF(const Handle(asiAlgo_BaseCloud<float>)& points,
                                const ActAPI_Color&                     color,
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
    points_n = asiEngine_IV(m_model).Create_PointSet(asiAlgo_PointCloudUtils::CloudfAsCloudd(points), name, true);
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, points_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_ASI_POINTS(const std::vector<gp_Pnt2d>&   points,
                               const ActAPI_Color&            color,
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

void asiUI_IV::BROWSER_OFF()
{
  m_bBrowserOn = false;
}

//---------------------------------------------------------------------------//

void asiUI_IV::BROWSER_ON()
{
  m_bBrowserOn = true;

  if ( m_bBrowserOn && m_pBrowser )
    m_pBrowser->Populate();
}

//---------------------------------------------------------------------------//

void asiUI_IV::VISUALIZATION_OFF()
{
  m_bVisuOn = false;
}

//---------------------------------------------------------------------------//

void asiUI_IV::VISUALIZATION_ON()
{
  m_bVisuOn = true;
}

//---------------------------------------------------------------------------//

void asiUI_IV::REPAINT_OFF()
{
  m_bRepaintOn = false;
}

//---------------------------------------------------------------------------//

void asiUI_IV::REPAINT_ON()
{
  m_bRepaintOn = true;

  this->prsManager(false)->GetQVTKWidget()->GetRenderWindow()->Render();
  this->prsManager(true)->GetQVTKWidget()->GetRenderWindow()->Render();
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SHAPE_AS_PART_OFF()
{
  m_bAsPart = false;
}

//---------------------------------------------------------------------------//

void asiUI_IV::DRAW_SHAPE_AS_PART_ON()
{
  m_bAsPart = true;
}

//---------------------------------------------------------------------------//

void asiUI_IV::visualize(const bool                  is2d,
                         const Handle(ActAPI_INode)& node,
                         const bool                  hasColor,
                         const ActAPI_Color&         color,
                         const double                opacity,
                         const bool                  isWireframe) const
{
  if ( m_bBrowserOn && m_pBrowser )
    m_pBrowser->Populate();

  if ( !this->prsManager(is2d) )
    return;

  if ( !this->prsManager(is2d)->IsPresented(node) )
    this->prsManager(is2d)->SetPresentation(node);

  const bool colorize = hasColor && color.IsDefined();

  // Get presentation.
  Handle(asiVisu_Prs) prs = this->prsManager(is2d)->GetPresentation(node);
  //
  if ( prs.IsNull() )
    return;

  // Specific treatment for predefined pipelines.
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
      std::cout << "Pipeline is null" << std::endl;
      return;
    }

    // Configure shape visualization.
    TopoDS_Shape shape = Handle(asiData_IVTopoItemNode)::DownCast(node)->GetShape();
    //
    if ( isWireframe )
    {
      pl->GetDisplayModeFilter()->SetDisplayMode(ShapeDisplayMode_WireframeAndVertices);
    }

    // Process color.
    if ( colorize )
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
    // Set common properties for all pipelines.
    Handle(asiVisu_HPipelineList) pipelines = prs->GetPipelineList();
    for ( int p = 1; p <= pipelines->Length(); ++p )
    {
      if ( colorize )
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

  // Visualize (trihedron is not adjusted for better performance).
  if ( m_bVisuOn )
    this->prsManager(is2d)->Actualize(node.get(), false, false, m_bRepaintOn, false);
}

//---------------------------------------------------------------------------//

const vtkSmartPointer<asiVisu_PrsManager>& asiUI_IV::prsManager(const bool is2d) const
{
  return is2d ? this->GetPrsMgr2d() : this->GetPrsMgr3d();
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_point(const gp_XY&                   coord,
                          const ActAPI_Color&            color,
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

    // Update the last created object
    m_lastObj = points_n;
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(true, points_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_points(const Handle(HRealArray)&      coords,
                           const ActAPI_Color&            color,
                           const TCollection_AsciiString& name,
                           const bool                     newPrimitive)
{
  if ( coords.IsNull() )
    return;

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
      points_n->SetPoints( asiAlgo_PointCloudUtils::AsCloudd(coords) );
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    points_n = asiEngine_IV(m_model).Create_PointSet(asiAlgo_PointCloudUtils::AsCloudd(coords), name, newPrimitive);

    // Update the last created object
    m_lastObj = points_n;
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, points_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_curve(const Handle(Geom_Curve)&      curve,
                          const ActAPI_Color&            color,
                          const TCollection_AsciiString& name,
                          const bool                     drawOri,
                          const bool                     is2dViewer,
                          const bool                     newPrimitive)
{
  if ( curve.IsNull() )
    return;

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
      asiEngine_IV(m_model).Update_Curve(curve_n, curve, 1000, drawOri);
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    curve_n = asiEngine_IV(m_model).Create_Curve(curve, 1000, drawOri, name, newPrimitive);

    // Update the last created object
    m_lastObj = curve_n;
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(is2dViewer, curve_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_curve2d(const Handle(Geom2d_Curve)&    curve,
                            const ActAPI_Color&            color,
                            const TCollection_AsciiString& name,
                            const bool                     newPrimitive)
{
  if ( curve.IsNull() )
    return;

  // Open transaction
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }

  // Prepare host surface
  Handle(Geom_Plane) hostPlane = new Geom_Plane( gp::Origin(), gp::DZ() );

  // Modify data
  Handle(asiData_IVCurve2dNode) curve_n;
  //
  bool doCreate = newPrimitive;
  //
  if ( !doCreate )
  {
    curve_n = asiEngine_IV(m_model).Find_Curve2d(name);
    //
    if ( !curve_n.IsNull() )
      asiEngine_IV(m_model).Update_Curve2d(curve_n, curve, hostPlane, 1000);
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    curve_n = asiEngine_IV(m_model).Create_Curve2d(curve, hostPlane, 1000, name, newPrimitive);

    // Update the last created object
    m_lastObj = curve_n;
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(true, curve_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_surface(const Handle(Geom_Surface)&    surface,
                            const double                   uMin,
                            const double                   uMax,
                            const double                   vMin,
                            const double                   vMax,
                            const ActAPI_Color&            color,
                            const double                   opacity,
                            const TCollection_AsciiString& name,
                            const bool                     newPrimitive)
{
  if ( surface.IsNull() )
    return;

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
      asiEngine_IV(m_model).Update_Surface(surface_n, surface,
                                           uMin, uMax, vMin, vMax);
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    surface_n = asiEngine_IV(m_model).Create_Surface(surface,
                                                     uMin, uMax,
                                                     vMin, vMax,
                                                     name, newPrimitive);

    // Update the last created object
    m_lastObj = surface_n;
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, surface_n, true, color, opacity, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_shape(const TopoDS_Shape&            shape,
                          const bool                     hasColor,
                          const ActAPI_Color&            color,
                          const double                   opacity,
                          const bool                     isWireframe,
                          const TCollection_AsciiString& name,
                          const bool                     newPrimitive)
{
  if ( shape.IsNull() )
    return;

  // Open transaction.
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }

  Handle(ActAPI_INode) shape_n;

  // Draw either as a part or as a variable from IV.
  if ( m_bAsPart )
  {
    Handle(asiData_PartNode) part_n = asiEngine_Part(m_model).Update(shape);
    //
    shape_n = part_n;
  }
  else
  {
    bool doCreate = newPrimitive;
    //
    if ( !doCreate )
    {
      shape_n = asiEngine_IV(m_model).Find_TopoItem(name);
      //
      if ( !shape_n.IsNull() )
        asiEngine_IV(m_model).Update_TopoItem(Handle(asiData_IVTopoItemNode)::DownCast(shape_n), shape);
      else
        doCreate = true;
    }

    if ( doCreate )
    {
      shape_n = asiEngine_IV(m_model).Create_TopoItem(shape, name, newPrimitive);

      // Update the last created object.
      m_lastObj = shape_n;
    }
  }

  // Commit transaction.
  if ( isTx )
    m_model->CommitCommand();

  // Visualize.
  this->visualize(false, shape_n, hasColor, color, opacity, isWireframe);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_link(const gp_XYZ&                  p1,
                         const gp_XYZ&                  p2,
                         const ActAPI_Color&            color,
                         const TCollection_AsciiString& name,
                         const bool                     is2dViewer,
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
      asiEngine_IV(m_model).Update_Curve( curve_n, C, Precision::Infinite(), false );
    else
      doCreate = true;
  }

  if ( doCreate )
  {
    curve_n = asiEngine_IV(m_model).Create_Curve(C, Precision::Infinite(), false, name, newPrimitive);

    // Update the last created object
    m_lastObj = curve_n;
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(is2dViewer, curve_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_link(const gp_XYZ&                  p1,
                         const gp_XYZ&                  p2,
                         const ActAPI_Color&            color,
                         const TCollection_AsciiString& name,
                         const bool                     newPrimitive)
{
  this->draw_link(p1, p2, color, name, false, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_link(const gp_XY&                   p1,
                         const gp_XY&                   p2,
                         const ActAPI_Color&            color,
                         const TCollection_AsciiString& name,
                         const bool                     newPrimitive)
{
  gp_XYZ p1_3d(p1.X(), p1.Y(), 0.0), p2_3d(p2.X(), p2.Y(), 0.0);

  this->draw_link(p1_3d, p2_3d, color, name, true, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_polyline(const std::vector<gp_XYZ>&     poles,
                             const ActAPI_Color&            color,
                             const TCollection_AsciiString& name,
                             const bool                     is2dViewer,
                             const bool                     newPrimitive)
{
  Handle(Geom_BSplineCurve) curve = asiAlgo_Utils::PolylineAsSpline(poles);

  this->draw_curve(curve, color, name, false, is2dViewer, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_polyline(const std::vector<gp_XYZ>&     poles,
                             const ActAPI_Color&            color,
                             const TCollection_AsciiString& name,
                             const bool                     newPrimitive)
{
  Handle(Geom_BSplineCurve) curve = asiAlgo_Utils::PolylineAsSpline(poles);

  this->draw_curve(curve, color, name, false, false, newPrimitive);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_polyline(const std::vector<gp_XY>&      poles,
                             const ActAPI_Color&            color,
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

void asiUI_IV::draw_triangle(const gp_XYZ&                  p1,
                             const gp_XYZ&                  p2,
                             const gp_XYZ&                  p3,
                             const ActAPI_Color&            color,
                             const TCollection_AsciiString& name,
                             const bool                     newPrimitive,
                             const bool                     isWireframe)
{
  if ( isWireframe )
  {
    std::vector<gp_XYZ> poles = {p1, p2, p3, p1};

    this->draw_polyline(poles, color, name, newPrimitive);
  }
  else
  {
    TColgp_Array1OfPnt nodes(1, 3);
    nodes(1) = p1;
    nodes(2) = p2;
    nodes(3) = p3;
    //
    Poly_Array1OfTriangle tris(1, 1); tris(1) = Poly_Triangle(1, 2, 3);
    //
    Handle(Poly_Triangulation) polyTri = new Poly_Triangulation(nodes, tris);

    this->draw_triangulation(polyTri, color, 1.0, name, newPrimitive);
  }
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_triangle(const gp_XY&                   p1,
                             const gp_XY&                   p2,
                             const gp_XY&                   p3,
                             const ActAPI_Color&            color,
                             const TCollection_AsciiString& name,
                             const bool                     newPrimitive,
                             const bool                     isWireframe)
{
  if ( isWireframe )
  {
    std::vector<gp_XY> poles = {p1, p2, p3, p1};

    this->draw_polyline(poles, color, name, newPrimitive);
  }
  else
  {
    TColgp_Array1OfPnt nodes(1, 3);
    nodes(1) = gp_Pnt(p1.X(), p1.Y(), 0);
    nodes(2) = gp_Pnt(p2.X(), p2.Y(), 0);
    nodes(3) = gp_Pnt(p3.X(), p3.Y(), 0);
    //
    Poly_Array1OfTriangle tris(1, 1); tris(1) = Poly_Triangle(1, 2, 3);
    //
    Handle(Poly_Triangulation) polyTri = new Poly_Triangulation(nodes, tris);

    this->draw_triangulation(polyTri, color, 1.0, name, newPrimitive);
  }
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_triangulation(const Handle(Poly_Triangulation)& tris,
                                  const ActAPI_Color&               color,
                                  const double                      opacity,
                                  const TCollection_AsciiString&    name,
                                  const bool                        newPrimitive)
{
  if ( tris.IsNull() )
    return;

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

    // Update the last created object
    m_lastObj = item_n;
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, true, color, opacity, false);
}

//---------------------------------------------------------------------------//

void asiUI_IV::draw_text(const TCollection_AsciiString& text,
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

    // Update the last created object
    m_lastObj = item_n;
  }

  // Commit transaction
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, false, ActAPI_Color(), 0.0, false);
}
