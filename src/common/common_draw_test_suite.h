//-----------------------------------------------------------------------------
// Created on: 23 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef common_draw_test_suite_h
#define common_draw_test_suite_h

// OCCT includes
#pragma warning(push, 0)
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Builder.hxx>
#include <Draw.hxx>
#include <Draw_Box.hxx>
#include <Draw_Color.hxx>
#include <Draw_MarkerShape.hxx>
#include <DrawTrSurf.hxx>
#include <DrawTrSurf_Curve.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GCE2d_MakeLine.hxx>
#include <gce_MakeLin.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <TopoDS_Shape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Edge.hxx>
#pragma warning(pop)

//! \ingroup DRAW
//!
//! Enumeration for used colors.
enum Draw_PaletteColor
{
  PaletteColor_White   = Draw_blanc,   //! White.
  PaletteColor_Red     = Draw_rouge,   //! Red.
  PaletteColor_Green   = Draw_vert,    //! Green.
  PaletteColor_Blue    = Draw_bleu,    //! Blue.
  PaletteColor_Cyan    = Draw_cyan,    //! Cyan.
  PaletteColor_Golden  = Draw_or,      //! Golden.
  PaletteColor_Magenta = Draw_magenta, //! Magenta.
  PaletteColor_Brown   = Draw_marron,  //! Brown.
  PaletteColor_Orange  = Draw_orange,  //! Orange.
  PaletteColor_Pink    = Draw_rose,    //! Pink.
  PaletteColor_Salmon  = Draw_saumon,  //! Salmon.
  PaletteColor_Violet  = Draw_violet,  //! Violet.
  PaletteColor_Yellow  = Draw_jaune,   //! Yellow.
  PaletteColor_Olive   = Draw_kaki,    //! Dark-olive green.
  PaletteColor_Coral   = Draw_corail,  //! Coral.
  PaletteColor_NumberOfColors
};

/************************************************************************
                           INLINE RENDERING
 ************************************************************************/

//! For any entity which we are going to visualize we need to generate some
//! unique name to be used as a name of the corresponding DRAW variable.
//! The applied approach consists in reserving some big amount of indices
//! presuming that we cannot have e.g. more than 1 million points to render.
//! If it is not the case, just increase the SHIFT_INDEX value
#define SHIFT_INDEX 0

//! Initializes a scope of DRAW variables to store any type of entities
#define DRAW_INITGROUP(P_BASENAME) \
  static int P_BASENAME ## _count = 1;

//! Initializes a scope of DRAW variables to store any type of entities.
//! Unlike DRAW_INITGROUP, it does not use static memory
#define DRAW_INITLOCALGROUP(P_BASENAME) \
  int P_BASENAME ## _count = 1;

//! Puts a unique variable representing a point into DRAW geometric
//! context (DrawTrSurf). Thus you're able to dump any points into the
//! DRAW viewer (non-OpenGL 2D or 3D) with some custom parameters like
//! rendering marker or color. Use it in the following way:
//!
//! #if defined DRAW_INITGROUP && defined DRAW_POINT
//!   DRAW_INITGROUP(some_unique_prefix_)
//!   DRAW_POINT(aPnt, some_unique_prefix_, Draw_blanc, Draw_X)
//! #endif
//!
//! Here,
//!
//! - P_ENTITY is your gp_Pnt or gp_Pnt2d variable;
//!
//! - P_BASENAME is some unique prefix for automatically generated
//!   name of the corresponding DRAW variable for your point. Note that
//!   this name must be previously registered via DRAW_INITGROUP invocation;
//!
//! - P_POINT_COLOR is a color to render your point,
//!   e.g. Draw_blanc   -- white,
//!        Draw_rouge   -- red,
//!        Draw_vert    -- green,
//!        Draw_bleu    -- blue,
//!        Draw_cyan    -- cyan,
//!        Draw_or      -- golden,
//!        Draw_magenta -- magenta,
//!        Draw_marron  -- brown,
//!        Draw_orange  -- orange,
//!        Draw_rose    -- pink,
//!        Draw_saumon  -- salmon,
//!        Draw_violet  -- violet,
//!        Draw_jaune   -- yellow,
//!        Draw_kaki    -- dark-olive green,
//!        Draw_corail  -- coral;
//!
//! - P_POINT_MARKER is a marker to render your point,
//!   e.g. Draw_Square,
//!        Draw_Losange,
//!        Draw_X,
//!        Draw_Plus,
//!        Draw_Circle,
//!        Draw_CircleZoom;
#define DRAW_POINT(P_ENTITY, P_BASENAME, P_POINT_COLOR, P_POINT_MARKER) \
  { \
    TCollection_AsciiString __debug_draw_BaseName(#P_BASENAME); \
    TCollection_AsciiString __debug_draw_EntityName = __debug_draw_BaseName.Cat( TCollection_AsciiString(SHIFT_INDEX + (P_BASENAME ## _count)++) ); \
    Draw_Color aPrevPntColor = DrawTrSurf_PointColor(P_POINT_COLOR); \
    Draw_MarkerShape aPrevPntMarker = DrawTrSurf_PointMarker(P_POINT_MARKER); \
    DrawTrSurf::Set(__debug_draw_EntityName.ToCString(), P_ENTITY); \
    DrawTrSurf_PointColor(aPrevPntColor); \
    DrawTrSurf_PointMarker(aPrevPntMarker); \
  }

// ------------------- [BEGIN]: EXPERIMENTAL: Rendering steps of algo ---------------- //

#define DRAW_INITSTEPGROUP(P_GROUPNAME, P_STEPNUM) \
  TopoDS_Compound aGroupCompound; \
  BRep_Builder aGroupBuilder; \
  aGroupBuilder.MakeCompound(aGroupCompound); \
  TCollection_AsciiString aDrawBaseName(#P_GROUPNAME); \
  TCollection_AsciiString aDrawName = aDrawBaseName.Cat( TCollection_AsciiString(P_STEPNUM) );

#define DRAW_DUMPSTEPGROUP(P_GROUPNAME) \
  DBRep::Set(aDrawName.ToCString(), aGroupCompound);

#define DRAW_POINTGROUP(P_ENTITY, P_BASENAME, P_POINT_COLOR, P_POINT_MARKER) \
  { \
    DRAW_POINT(P_ENTITY, P_BASENAME, P_POINT_COLOR, P_POINT_MARKER) \
    aGroupBuilder.Add( BRepBuilderAPI_MakeVertex(P_ENTITY), aGroupCompound ); \
  }

// ------------------- [END]: EXPERIMENTAL: Rendering steps of algo ------------------ //

//! Hides the entire group.
//!  - P_BASENAME -- group to hide;
//!  - P_NB_ENTITIES -- number of entities in the group;
//!  - P_DI -- Draw Interpreter;
//!
//! IMPORTANT: this function is VERY (!) slow as it erases entities one-by-one via DRAW interpreter.
#define DRAW_ERASE_GROUP(P_BASENAME, P_NB_ENTITIES, P_DI) \
  { \
    TCollection_AsciiString __debug_draw_BaseName(#P_BASENAME); \
    for ( int anIndex = 1; anIndex <= P_NB_ENTITIES; anIndex++ ) \
    { \
      TCollection_AsciiString __debug_draw_EntityName = __debug_draw_BaseName.Cat( TCollection_AsciiString(SHIFT_INDEX + anIndex) ); \
      TCollection_AsciiString __debug_draw_EraseCommand("erase "); \
      TCollection_AsciiString __debug_draw_EraseGroupCommand = __debug_draw_EraseCommand.Cat(__debug_draw_EntityName); \
      P_DI.Eval( __debug_draw_EraseGroupCommand.ToCString() ); \
    } \
  }

//! Draws a 2D line thru the passed 2D points (P_POINT1, P_POINT2).
//! The P_BASENAME must be previously registered via DRAW_INITGROUP invocation.
//!
//! Usage example:
//!
//! #ifdef DRAW_LINE2D
//!   DRAW_INITGROUP(emesh_quad_link_)
//!   DRAW_LINE2D(aPnt2d1, aPnt2d2, emesh_quad_link_, Draw_cyan)
//! #endif
#define DRAW_LINE2D(P_POINT1, P_POINT2, P_BASENAME, P_CURVE_COLOR) \
  { \
    double aLen = P_POINT1.Distance(P_POINT2); \
    TCollection_AsciiString __debug_draw_BaseName(#P_BASENAME); \
    TCollection_AsciiString __debug_draw_EntityName = __debug_draw_BaseName.Cat( TCollection_AsciiString(SHIFT_INDEX + (P_BASENAME ## _count)++) ); \
    Draw_Color aPrevCurveColor = DrawTrSurf_CurveColor(P_CURVE_COLOR); \
    DrawTrSurf::Set( __debug_draw_EntityName.ToCString(), \
                     new Geom2d_TrimmedCurve(GCE2d_MakeLine(P_POINT1, P_POINT2), 0.0, aLen), \
                     0 ); \
    DrawTrSurf_CurveColor(aPrevCurveColor); \
  }

//! Draws a 3D line thru the passed 3D points (P_POINT1, P_POINT2).
//! The P_BASENAME must be previously registered via DRAW_INITGROUP invocation.
//!
//! Usage example:
//!
//! #ifdef DRAW_LINE3D
//!   DRAW_INITGROUP(emesh_quad_link_)
//!   DRAW_LINE3D(aPnt1, aPnt2, emesh_tri_link_, Draw_bleu)
//! #endif
#define DRAW_LINE3D(P_POINT1, P_POINT2, P_BASENAME, P_CURVE_COLOR) \
  { \
    double aLen = P_POINT1.Distance(P_POINT2); \
    TCollection_AsciiString __debug_draw_BaseName(#P_BASENAME); \
    TCollection_AsciiString __debug_draw_EntityName = __debug_draw_BaseName.Cat( TCollection_AsciiString(SHIFT_INDEX + (P_BASENAME ## _count)++) ); \
    Draw_Color aPrevCurveColor = DrawTrSurf_CurveColor(P_CURVE_COLOR); \
    DrawTrSurf::Set( __debug_draw_EntityName.ToCString(), \
                     new Geom_TrimmedCurve(new Geom_Line(gce_MakeLin(P_POINT1, P_POINT2)), 0.0, aLen), \
                     0 ); \
    DrawTrSurf_CurveColor(aPrevCurveColor); \
  }

//! Draws a 3D triangle by three points (P_POINT1, P_POINT2, P_POINT3).
//! The links will be: P_POINT1 <---> P_POINT2,
//!                    P_POINT1 <---> P_POINT3,
//!                    P_POINT2 <---> P_POINT3;
#define DRAW_TRIANGLE3D(P_POINT1, P_POINT2, P_POINT3, P_BASENAME, P_COLOR) \
  { \
    DRAW_LINE3D(P_POINT1, P_POINT2, P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P_POINT1, P_POINT3, P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P_POINT2, P_POINT3, P_BASENAME, P_COLOR) \
  }

//! Draws a 3D quadrangle by four points (P_POINT1, P_POINT2, P_POINT3, P_POINT4).
//! The links will be: P_POINT1 <---> P_POINT2,
//!                    P_POINT2 <---> P_POINT3,
//!                    P_POINT3 <---> P_POINT4;
//!                    P_POINT4 <---> P_POINT1;
#define DRAW_QUADRANGLE3D(P_POINT1, P_POINT2, P_POINT3, P_POINT4, P_BASENAME, P_COLOR) \
  { \
    if ( P_POINT1.Distance(P_POINT2) > Precision::Confusion() ) \
    { \
      DRAW_LINE3D(P_POINT1, P_POINT2, P_BASENAME, P_COLOR) \
    } \
    if ( P_POINT2.Distance(P_POINT3) > Precision::Confusion() ) \
    { \
      DRAW_LINE3D(P_POINT2, P_POINT3, P_BASENAME, P_COLOR) \
    } \
    if ( P_POINT3.Distance(P_POINT4) > Precision::Confusion() ) \
    { \
      DRAW_LINE3D(P_POINT3, P_POINT4, P_BASENAME, P_COLOR) \
    } \
    if ( P_POINT4.Distance(P_POINT1) > Precision::Confusion() ) \
    { \
      DRAW_LINE3D(P_POINT4, P_POINT1, P_BASENAME, P_COLOR) \
    } \
  }

//! Draws a bounding box.
#define DRAW_BOX_LINES(BOX, P_BASENAME, P_COLOR) \
  { \
    const gp_Pnt Pmin = BOX.CornerMin(); \
    const gp_Pnt Pmax = BOX.CornerMax(); \
    const gp_Pnt P2(Pmax.X(), Pmin.Y(), Pmin.Z()); \
    const gp_Pnt P3(Pmax.X(), Pmax.Y(), Pmin.Z()); \
    const gp_Pnt P4(Pmin.X(), Pmax.Y(), Pmin.Z()); \
    const gp_Pnt P5(Pmin.X(), Pmin.Y(), Pmax.Z()); \
    const gp_Pnt P6(Pmax.X(), Pmin.Y(), Pmax.Z()); \
    const gp_Pnt P8(Pmin.X(), Pmax.Y(), Pmax.Z()); \
    DRAW_LINE3D(Pmin, P2,   P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P2,   P3,   P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P3,   P4,   P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P4,   Pmin, P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P5,   P6,   P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P6,   Pmax, P_BASENAME, P_COLOR) \
    DRAW_LINE3D(Pmax, P8,   P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P8,   P5,   P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P6,   P2,   P_BASENAME, P_COLOR) \
    DRAW_LINE3D(Pmax, P3,   P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P8,   P4,   P_BASENAME, P_COLOR) \
    DRAW_LINE3D(P5,   Pmin, P_BASENAME, P_COLOR) \
  }

//! Draws a bounding box.
#define DRAW_BOX(BOX, P_BASENAME, P_COLOR) \
  { \
    const gp_Pnt Pmin = BOX.CornerMin(); \
    const gp_Pnt Pmax = BOX.CornerMax(); \
    Handle(Draw_Box) aDrawBox = new Draw_Box(Pmin, Pmax, P_COLOR); \
    TCollection_AsciiString __debug_draw_BaseName(#P_BASENAME); \
    TCollection_AsciiString __debug_draw_EntityName = __debug_draw_BaseName.Cat( TCollection_AsciiString(SHIFT_INDEX + (P_BASENAME ## _count)++) ); \
    Draw::Set(__debug_draw_EntityName.ToCString(), aDrawBox); \
  }

//! Draws a curve.
#define DRAW_CURVE2D(CURVE, P_BASENAME, P_COLOR) \
  { \
    Handle(DrawTrSurf_Curve2d) aDrawCurve = new DrawTrSurf_Curve2d(CURVE); \
    aDrawCurve->SetColor(P_COLOR); \
    TCollection_AsciiString __debug_draw_BaseName(#P_BASENAME); \
    TCollection_AsciiString __debug_draw_EntityName = __debug_draw_BaseName.Cat( TCollection_AsciiString(SHIFT_INDEX + (P_BASENAME ## _count)++) ); \
    Draw::Set(__debug_draw_EntityName.ToCString(), aDrawCurve); \
  }

//! Draws a curve.
#define DRAW_CURVE(CURVE, P_BASENAME, P_COLOR) \
  { \
    Handle(DrawTrSurf_Curve) aDrawCurve = new DrawTrSurf_Curve(CURVE); \
    aDrawCurve->SetColor(P_COLOR); \
    TCollection_AsciiString __debug_draw_BaseName(#P_BASENAME); \
    TCollection_AsciiString __debug_draw_EntityName = __debug_draw_BaseName.Cat( TCollection_AsciiString(SHIFT_INDEX + (P_BASENAME ## _count)++) ); \
    Draw::Set(__debug_draw_EntityName.ToCString(), aDrawCurve); \
  }

//! Draws an edge.
#define DRAW_EDGE(EDGE, P_BASENAME, P_COLOR) \
  { \
     Handle(DBRep_DrawableShape) \
       Dr = new DBRep_DrawableShape(EDGE, \
                                    Draw_vert, Draw_jaune, \
                                    P_COLOR, \
                                    Draw_bleu, 100., 2, 30); \
    TCollection_AsciiString __debug_draw_BaseName(#P_BASENAME); \
    TCollection_AsciiString __debug_draw_EntityName = __debug_draw_BaseName.Cat( TCollection_AsciiString(SHIFT_INDEX + (P_BASENAME ## _count)++) ); \
    Draw::Set(__debug_draw_EntityName.ToCString(), Dr); \
  }

//! Draws a face.
#define DRAW_FACE(FACE, P_BASENAME) \
  { \
    TCollection_AsciiString __debug_draw_BaseName(#P_BASENAME); \
    TCollection_AsciiString __debug_draw_EntityName = __debug_draw_BaseName.Cat( TCollection_AsciiString(SHIFT_INDEX + (P_BASENAME ## _count)++) ); \
    DBRep::Set(__debug_draw_EntityName.ToCString(), FACE); \
  }

//! Externals for inline drawings
Standard_IMPORT Draw_Color DrawTrSurf_CurveColor(const Draw_Color col);
Standard_IMPORT Draw_Color DrawTrSurf_PointColor(const Draw_Color col);
Standard_IMPORT Draw_MarkerShape DrawTrSurf_PointMarker(const Draw_MarkerShape marker);

/************************************************************************
                           MEASURING PERFORMANCE
 ************************************************************************/

//! Example:
//!
//! #ifdef TIMER_NEW
//!   TIMER_NEW
//!   TIMER_RESET
//!   TIMER_GO
//! #endif
//!
//! ... YOUR AD_ALGO GOES HERE ...
//!
//! #ifdef TIMER_NEW
//!   TIMER_FINISH
//!   TIMER_COUT_RESULT
//! #endif

#define TIMER_NEW \
  OSD_Timer __aux_debug_Timer; \
  double __aux_debug_Seconds, __aux_debug_CPUTime; \
  int __aux_debug_Minutes, __aux_debug_Hours;

#define TIMER_RESET \
  __aux_debug_Seconds = __aux_debug_CPUTime = 0.0; \
  __aux_debug_Minutes = __aux_debug_Hours = 0; \
  __aux_debug_Timer.Reset();

#define TIMER_GO \
  __aux_debug_Timer.Start();

#define TIMER_FINISH \
  __aux_debug_Timer.Stop(); \
  __aux_debug_Timer.Show(__aux_debug_Seconds, __aux_debug_Minutes, __aux_debug_Hours, __aux_debug_CPUTime);

#define TIMER_COUT_RESULT \
  { \
    TIMER_COUT_RESULT_MSG(""); \
  }

#define TIMER_COUT_RESULT_MSG(Msg) \
  { \
    std::cout << "\n=============================================" << std::endl; \
    TCollection_AsciiString ascii_msg(Msg); \
    if ( !ascii_msg.IsEmpty() ) \
    { \
      std::cout << Msg                                               << std::endl; \
      std::cout << "---------------------------------------------" << std::endl; \
    } \
    std::cout << "Seconds:  " << __aux_debug_Seconds               << std::endl; \
    std::cout << "Minutes:  " << __aux_debug_Minutes               << std::endl; \
    std::cout << "Hours:    " << __aux_debug_Hours                 << std::endl; \
    std::cout << "CPU time: " << __aux_debug_CPUTime               << std::endl; \
    std::cout << "=============================================\n" << std::endl; \
  }

#endif
