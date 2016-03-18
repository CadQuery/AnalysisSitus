//-----------------------------------------------------------------------------
// Created on: 15 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_xde_shape_prs.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBndLib.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <TDataStd_Name.hxx>
#include <TopoDS_Iterator.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>
#include <XCAFPrs_DataMapOfShapeStyle.hxx>

//-----------------------------------------------------------------------------

visu_xde_shape_prs::visu_xde_shape_prs(const TDF_Label&                theLabel,
                                       const visu_xde_style&           theStyle,
                                       const Graphic3d_MaterialAspect& theMaterial)
: AIS_ColoredShape ( TopoDS_Shape() ),
  myLabel          ( theLabel ),
  myDefStyle       ( theStyle )
{
  // AIS_ColoredShape sets free boundary aspect as usual aspect for any
  // line. This is not suitable in our case, so we have to override it here
  myDrawer->SetFreeBoundaryAspect( new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0) );
  //
  this->SetMaterial(theMaterial);
}

//-----------------------------------------------------------------------------

void visu_xde_shape_prs::Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                                 const Handle(Prs3d_Presentation)&           thePrs,
                                 const Standard_Integer                      theMode)
{
  TopoDS_Shape aShape;
  if ( !XCAFDoc_ShapeTool::GetShape(myLabel, aShape) || aShape.IsNull() )
    return;

  // Shape void -> Assemblage void
  if ( aShape.ShapeType() == TopAbs_COMPOUND )
  {
    TopoDS_Iterator anExplor(aShape);
    if ( !anExplor.More() )
      return;
  }

  // Set shape to visualize
  this->Set(aShape);
  this->ClearCustomAspects();

  // Get default style
  Quantity_Color aColorSurf = myDefStyle.GetColorSurf();
  //
  this->SetColors(myDrawer, aColorSurf);
  this->ApplyEdgesStyle();

  // Collect information on colored sub-shapes
  TopLoc_Location aLoc;
  XCAFPrs_DataMapOfShapeStyle aSettings;
  XCAFPrs::CollectStyleSettings(myLabel, aLoc, aSettings);
  //
  BRep_Builder aBuilder;
  NCollection_DataMap<XCAFPrs_Style, TopoDS_Compound, XCAFPrs_Style> aStyleGroups;
  //
  for ( XCAFPrs_DataMapIteratorOfDataMapOfShapeStyle it(aSettings); it.More(); it.Next() )
  {
    TopoDS_Compound aComp;
    if ( aStyleGroups.Find(it.Value(), aComp) )
    {
      aBuilder.Add( aComp, it.Key() );
      continue;
    }

    aBuilder.MakeCompound(aComp);
    aBuilder.Add( aComp, it.Key() );
    aStyleGroups.Bind(it.Value(), aComp);
  }
  aSettings.Clear();

  // Assign custom aspects
  for ( NCollection_DataMap<XCAFPrs_Style, TopoDS_Compound, XCAFPrs_Style>::Iterator it(aStyleGroups);
        it.More(); it.Next())
  {
    const TopoDS_Compound& aComp = it.Value();
    TopoDS_Iterator aShapeIter(aComp);
    TopoDS_Shape aShapeCur = aShapeIter.Value();
    aShapeIter.Next();
    //
    if ( aShapeIter.More() )
      aShapeCur = aComp;

    // Handle visibility
    Handle(AIS_ColoredDrawer) aDrawer = this->CustomAspects(aShapeCur);
    const XCAFPrs_Style& aStyle = it.Key();
    aDrawer->SetHidden( !aStyle.IsVisible() );

    // Handle colors
    aColorSurf = aStyle.IsSetColorSurf() ? aStyle.GetColorSurf() : myDefStyle.GetColorSurf();
    //
    this->SetColors(aDrawer, aColorSurf);
  }
  aStyleGroups.Clear();

  // Compute Presentation
  AIS_ColoredShape::Compute(thePresentationManager, thePrs, theMode);
}

//-----------------------------------------------------------------------------

void visu_xde_shape_prs::SetColors(const Handle(Prs3d_Drawer)& theDrawer,
                                   const Quantity_Color&       theColorSurf)
{
  // Choose color for tessellation edges
  double delta = 0.45;
  Quantity_Color darkColorSurf = theColorSurf;
  double surfR, surfG, surfB;
  darkColorSurf.Values(surfR, surfG, surfB, Quantity_TOC_RGB);
  surfR > delta ? surfR -= delta : surfR = 0.0;
  surfG > delta ? surfG -= delta : surfG = 0.0;
  surfB > delta ? surfB -= delta : surfB = 0.0;
  darkColorSurf.SetValues(surfR, surfG, surfB, Quantity_TOC_RGB);

  // Set color and material
  Graphic3d_MaterialAspect aMaterial = myDrawer->ShadingAspect()->Aspect()->FrontMaterial();
  aMaterial.SetColor(theColorSurf);
  theDrawer->ShadingAspect()->Aspect()->SetInteriorColor(theColorSurf);
  theDrawer->ShadingAspect()->Aspect()->SetFrontMaterial(aMaterial);

  // Set topological edges
  theDrawer->FaceBoundaryAspect()->SetWidth(2);
  theDrawer->FaceBoundaryAspect()->SetColor(darkColorSurf);
  theDrawer->FaceBoundaryAspect()->SetTypeOfLine(Aspect_TOL_SOLID);
  //
  const Handle(Prs3d_ShadingAspect)& shadingAspect = theDrawer->ShadingAspect();
  shadingAspect->SetColor(theColorSurf);
  //
  Handle(Graphic3d_AspectFillArea3d) fillingAspect = shadingAspect->Aspect();
  fillingAspect->SetEdgeColor(darkColorSurf);

  // Configure wireframe settings
  theDrawer->UIsoAspect()->SetNumber(0);
  theDrawer->VIsoAspect()->SetNumber(0);
  theDrawer->UnFreeBoundaryAspect()->SetColor(Quantity_NOC_SNOW);
}

//-----------------------------------------------------------------------------

void visu_xde_shape_prs::ApplyEdgesStyle()
{
  // Mesh edges
  if ( myDefStyle.GetMeshEdges() )
    myDrawer->ShadingAspect()->Aspect()->SetEdgeOn();
  else
    myDrawer->ShadingAspect()->Aspect()->SetEdgeOff();

  // Topological edges
  myDrawer->SetFaceBoundaryDraw( myDefStyle.GetTopoEdges() );
}
