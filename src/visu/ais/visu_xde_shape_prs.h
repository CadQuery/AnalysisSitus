//-----------------------------------------------------------------------------
// Created on: 15 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_xde_shape_prs_h
#define visu_xde_shape_prs_h

// A-Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <AIS_ColoredShape.hxx>
#include <AIS_Shape.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Standard_Transient.hxx>
#include <TDF_Label.hxx>
#include <XCAFPrs_Style.hxx>

//-----------------------------------------------------------------------------

typedef NCollection_DataMap<TDF_Label,    Handle(AIS_InteractiveObject), TDF_LabelMapHasher>      visu_xde_shapes;
typedef NCollection_DataMap<TopoDS_Shape, Handle(AIS_ColoredDrawer),     TopTools_ShapeMapHasher> visu_xde_drawers;

//-----------------------------------------------------------------------------

class visu_xde_style : public XCAFPrs_Style
{
public:

  visu_xde_style() : XCAFPrs_Style  (),
                     m_bMeshEdgesOn (false),
                     m_bTopoEdgesOn (true)
  {}

public:

  inline void SetMeshEdges(const bool isOn) { m_bMeshEdgesOn = isOn; }
  inline void SetTopoEdges(const bool isOn) { m_bTopoEdgesOn = isOn; }
  inline bool GetMeshEdges() const          { return m_bMeshEdgesOn; }
  inline bool GetTopoEdges() const          { return m_bTopoEdgesOn; }

protected:

  bool m_bMeshEdgesOn; //!< Indicates whether to draw mesh edges.
  bool m_bTopoEdgesOn; //!< Indicates whether to draw topological edges.

};

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(visu_xde_shape_prs, AIS_Shape)

//! Interactive object for shape in XDE document.
class visu_xde_shape_prs : public AIS_ColoredShape
{
public:

  DEFINE_STANDARD_RTTI_INLINE(visu_xde_shape_prs, AIS_ColoredShape)

public:

  //! Constructor.
  visu_xde_shape_prs(const TDF_Label&                theLabel,
                     const visu_xde_style&           theStyle,
                     const Graphic3d_MaterialAspect& theMaterial);

public:

  //! Search custom aspect for specified shape.
  inline bool FindCustomAspects(const TopoDS_Shape&        theShape,
                                Handle(AIS_ColoredDrawer)& theAspects) const
  {
    return myShapeColors.Find(theShape, theAspects) > 0;
  }

  //! Accessor for the styles map.
  inline visu_xde_drawers& ShapeColors()
  {
    return myShapeColors;
  }

  //! Accessor for the default style.
  inline visu_xde_style& DefaultStyle()
  {
    return myDefStyle;
  }

public:

  //! Set colors to drawer
  void SetColors(const Handle(Prs3d_Drawer)& theDrawer,
                 const Quantity_Color&       theColorSurf);

  //! Applies style for drawing edges.
  void ApplyEdgesStyle();

protected:

  //! Redefined method to compute presentation.
  virtual void Compute(const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                       const Handle(Prs3d_Presentation)&           thePresentation,
                       const int                                   theMode);

protected:

  TDF_Label      myLabel;    //!< Owning OCAF Label.
  visu_xde_style myDefStyle; //!< Default style.

};

#endif
