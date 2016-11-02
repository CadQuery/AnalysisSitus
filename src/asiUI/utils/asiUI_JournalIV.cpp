//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_JournalIV.h>

// Engine includes
#include <asiEngine_IV.h>

// Visualization includes
#include <asiVisu_IVPointSetPrs.h>
#include <asiVisu_IVTopoItemPrs.h>
#include <asiVisu_PointsPipeline.h>
#include <asiVisu_ShapePipeline.h>
#include <asiVisu_Utils.h>

// OCCT includes
#include <GC_MakeSegment.hxx>

// VTK includes
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkProperty.h>

//---------------------------------------------------------------------------//

void asiUI_JournalIV::CLEAN()
{
  this->prsManager2d()->DeleteAllPresentations();
  this->prsManager2d()->InitializePickers();
  //
  this->prsManager3d()->DeleteAllPresentations();
  this->prsManager3d()->InitializePickers();
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_POINT(const gp_XY&                   coord,
                                 const Quantity_Color&          color,
                                 const TCollection_AsciiString& name)
{
  Handle(HRealArray) coords = new HRealArray(0, 1);
  //
  coords->ChangeValue(0) = coord.X();
  coords->ChangeValue(1) = coord.Y();

  // Create a new Node for the given point set
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVPointSet2dNode)
    points_n = asiEngine_IV(m_model).Create_PointSet2d(coords, name);
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(true, points_n, true, color, 1.0, false);
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

void asiUI_JournalIV::DRAW_POINTS(const Handle(HRealArray)&      coords,
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
  Handle(asiData_IVPointSetNode)
    points_n = asiEngine_IV(m_model).Create_PointSet( new asiAlgo_PointCloud(coords), name );
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, points_n, true, color, 1.0, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_LINK(const gp_Pnt&                  p1,
                                const gp_Pnt&                  p2,
                                const Quantity_Color&          color,
                                const TCollection_AsciiString& name)
{
  if ( p1.Distance(p2) < 1.0e-5 )
    return;

  // Create a straight line segment between two passed points
  Handle(Geom_TrimmedCurve) C = GC_MakeSegment(p1, p2);

  // Create a curve Node
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVCurveNode)
    curve_n = asiEngine_IV(m_model).Create_Curve( C, Precision::Infinite(), name );
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, curve_n, true, color, 1.0, false);
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

void asiUI_JournalIV::DRAW_CURVE(const Handle(Geom_Curve)&      curve,
                                 const Quantity_Color&          color,
                                 const TCollection_AsciiString& name)
{
  // Create a curve Node
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVCurveNode)
    curve_n = asiEngine_IV(m_model).Create_Curve( curve, 1000, name );
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, curve_n, true, color, 1.0, false);
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
  // Create a surface Node
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVSurfaceNode)
    surface_n = asiEngine_IV(m_model).Create_Surface(surface, uLimit, vLimit, name);
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, surface_n, true, color, opacity, false);
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
  // Create a Node for topology item
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVTopoItemNode)
    item_n = asiEngine_IV(m_model).Create_TopoItem(shape, name);
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, false, Quantity_Color(), opacity, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                                 const Quantity_Color&          color,
                                 const double                   opacity,
                                 const TCollection_AsciiString& name)
{
  // Create a Node for topology item
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVTopoItemNode)
    item_n = asiEngine_IV(m_model).Create_TopoItem(shape, name);
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, true, color, opacity, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_SHAPE(const TopoDS_Shape&            shape,
                                 const Quantity_Color&          color,
                                 const double                   opacity,
                                 const bool                     isWireframe,
                                 const TCollection_AsciiString& name)
{
  // Create a Node for topology item
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVTopoItemNode)
    item_n = asiEngine_IV(m_model).Create_TopoItem(shape, name);
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, true, color, opacity, isWireframe);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_TRIANGULATION(const Handle(Poly_Triangulation)& shape,
                                         const Quantity_Color&             color,
                                         const double                      opacity,
                                         const TCollection_AsciiString&    name)
{
  // Create a Node for tessellation item
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVTessItemNode)
    item_n = asiEngine_IV(m_model).Create_TessItem(shape, name);
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, true, color, opacity, false);
}

//---------------------------------------------------------------------------//

void asiUI_JournalIV::DRAW_TEXT(const TCollection_AsciiString& text)
{
  // Create a Node for text item
  bool isTx = false;
  if ( !m_model->HasOpenCommand() )
  {
    m_model->OpenCommand();
    isTx = true;
  }
  //
  Handle(asiData_IVTextItemNode)
    item_n = asiEngine_IV(m_model).Create_TextItem(text);
  //
  if ( isTx )
    m_model->CommitCommand();

  // Visualize
  this->visualize(false, item_n, false, Quantity_Color(), 0.0, false);
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

    // Configure shape visualization
    TopoDS_Shape shape = Handle(asiData_IVTopoItemNode)::DownCast(node)->GetShape();
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

//----------------------------------------------------------------------------//

const vtkSmartPointer<asiVisu_PrsManager>& asiUI_JournalIV::prsManager(const bool is2d) const
{
  return is2d ? this->prsManager2d() : this->prsManager3d();
}

//----------------------------------------------------------------------------//

const vtkSmartPointer<asiVisu_PrsManager>& asiUI_JournalIV::prsManager3d() const
{
  return m_prsMgr3d;
}

//----------------------------------------------------------------------------//

const vtkSmartPointer<asiVisu_PrsManager>& asiUI_JournalIV::prsManager2d() const
{
  return m_prsMgr2d;
}
