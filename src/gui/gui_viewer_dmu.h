//-----------------------------------------------------------------------------
// Created on: 11 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_viewer_dmu_h
#define gui_viewer_dmu_h

// Visualization includes
#include <visu_xde_shape_prs.h>

// XDE includes
#include <xde_model.h>
#include <xde_shape_id.h>

// Qt includes
#pragma warning(push, 0)
#include <QWidget>
#include <QAction>
#include <QList>
#pragma warning(pop)

// OCCT includes
#pragma warning(push, 0)
#include <AIS_DisplayMode.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <TDF_Label.hxx>
#include <TopLoc_Location.hxx>
#include <V3d_View.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs_Style.hxx>
#pragma warning(pop)

class TopoDS_Shape;
class QRubberBand;

//! Viewer for OCCT-based visualization.
class gui_viewer_dmu: public QWidget
{
  Q_OBJECT

protected:

  enum CurrentAction3d { CurAction3d_Nothing, CurAction3d_DynamicZooming,
                         CurAction3d_WindowZooming, CurAction3d_DynamicPanning,
                         CurAction3d_GlobalPanning, CurAction3d_DynamicRotation };

public:

  enum ViewAction { ViewFitAllId, ViewFitAreaId, ViewZoomId, ViewPanId, ViewGlobalPanId,
                    ViewFrontId, ViewBackId, ViewTopId, ViewBottomId, ViewLeftId, ViewRightId,
                    ViewAxoId, ViewRotationId, ViewResetId, ViewHlrOffId, ViewHlrOnId };

  gui_viewer_dmu( Handle(AIS_InteractiveContext) theContext, QWidget* parent );

  ~gui_viewer_dmu();

  Handle(V3d_View)&                     GetView();
  Handle(AIS_InteractiveContext)&       GetContext();
  void                                  Visualize(const Handle(xde_model)& model,
                                                  const AIS_DisplayMode    mode);
  void                                  Clear();

  virtual void                  init();
  bool                          dump( Standard_CString theFile );
  QList<QAction*>*              getViewActions();
  void                          noActiveActions();
  bool                          isShadingMode();

  static QString                GetMessages( int type,TopAbs_ShapeEnum aSubShapeType,
                                              TopAbs_ShapeEnum aShapeType );
  static QString                GetShapeType( TopAbs_ShapeEnum aShapeType );

  static void   OnButtonuseraction( int ExerciceSTEP,
                                    Handle(AIS_InteractiveContext)& );
  static void   DoSelection( int Id,
                             Handle(AIS_InteractiveContext)& );
  static void   OnSetSelectionMode( Handle(AIS_InteractiveContext)&,
                                    Standard_Integer&,
                                    TopAbs_ShapeEnum& SelectionMode,
                                    Standard_Boolean& );
  virtual QPaintEngine*         paintEngine() const;

signals:

  void                          selectionChanged();

public slots:

  void                          fitAll();
  void                          fitArea();
  void                          zoom();
  void                          pan();
  void                          globalPan();
  void                          front();
  void                          back();
  void                          top();
  void                          bottom();
  void                          left();
  void                          right();
  void                          axo();
  void                          rotation();
  void                          reset();
  void                          hlrOn();
  void                          hlrOff();
  void                          updateToggled( bool );
  void                          onBackground();

protected:

  virtual void                  paintEvent( QPaintEvent* );
  virtual void                  resizeEvent( QResizeEvent* );
  virtual void                  mousePressEvent( QMouseEvent* );
  virtual void                  mouseReleaseEvent(QMouseEvent* );
  virtual void                  mouseMoveEvent( QMouseEvent* );

  virtual void                  addItemInPopup( QMenu* );

  void                                  activateCursor( const CurrentAction3d );
  void                                  Popup( const int x, const int y );
  CurrentAction3d                       getCurrentMode();

  virtual void                          onLButtonDown ( const int nFlags, const QPoint point );
  virtual void                          onMButtonDown ( const int nFlags, const QPoint point );
  virtual void                          onRButtonDown ( const int nFlags, const QPoint point );
  virtual void                          onLButtonUp   ( const int nFlags, const QPoint point );
  virtual void                          onMButtonUp   ( const int nFlags, const QPoint point );
  virtual void                          onRButtonUp   ( const int nFlags, const QPoint point );
  virtual void                          onMouseMove   ( Qt::MouseButtons nFlags, const QPoint point );

private:

  void                          initCursors();
  void                          initViewActions();
  void                          DragEvent( const int x, const int y, const int TheState );
  void                          InputEvent( const int x, const int y );
  void                          MoveEvent( const int x, const int y );
  void                          MultiMoveEvent( const int x, const int y );
  void                          MultiDragEvent( const int x, const int y, const int TheState );
  void                          MultiInputEvent( const int x, const int y );
  void                          DrawRectangle( const int MinX, const int MinY,
                                                const int MaxX, const int MaxY, const bool Draw );

  void displayWithChildren(XCAFDoc_ShapeTool&             theShapeTool,
                           XCAFDoc_ColorTool&             theColorTool,
                           const TDF_Label&               theLabel,
                           const TopLoc_Location&         theParentTrsf,
                           const visu_xde_style&          theParentStyle,
                           const Handle(xde_shape_id)&    theParentId,
                           const int                      theDisplayMode,
                           visu_xde_shapes&               theMapOfShapes);

private:

  bool                            myDrawRect;           // set when a rect is used for selection or magnify 
  Handle(V3d_View)                myView;
  Handle(AIS_InteractiveContext)  myContext;
  CurrentAction3d                 myCurrentMode;
  Standard_Integer                myXmin;
  Standard_Integer                myYmin;
  Standard_Integer                myXmax;
  Standard_Integer                myYmax;
  Quantity_Factor                 myCurZoom;
  Standard_Boolean                myHlrModeIsOn;
  QList<QAction*>*                myViewActions;
  QMenu*                          myBackMenu;
  QRubberBand*                    myRectBand; //!< selection rectangle rubber band

public:

  visu_xde_shapes Shapes; //!< Visualized shapes.

};

#endif
