//-----------------------------------------------------------------------------
// Created on: 11 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#if !defined _WIN32
  #define QT_CLEAN_NAMESPACE /* avoid definition of INT32 and INT8 */
#endif

// Own include
#include <gui_viewer_dmu.h>

// GUI includes
#include <gui_occt_window.h>

// icons
////
#include "../../res/xpm/v3d_cursor_rotate.xpm"
#include "../../res/xpm/v3d_cursor_zoom.xpm"

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QPainter>
#include <QMenu>
#include <QColorDialog>
#include <QCursor>
#include <QFileInfo>
#include <QFileDialog>
#include <QMouseEvent>
#include <QRubberBand>
#include <QMdiSubWindow>
#include <QStyleFactory>
#pragma warning(pop)

// OCCT includes
#pragma warning(push, 0)
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_ExportFormat.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureEnv.hxx>
#pragma warning(pop)

// the key for multi selection :
#define MULTISELECTIONKEY Qt::ShiftModifier

// the key for shortcut ( use to activate dynamic rotation, panning )
#define CASCADESHORTCUTKEY Qt::ControlModifier

// for elastic bean selection
#define ValZWMin 1

static QCursor* defCursor     = NULL;
static QCursor* handCursor    = NULL;
static QCursor* panCursor     = NULL;
static QCursor* globPanCursor = NULL;
static QCursor* zoomCursor    = NULL;
static QCursor* rotCursor     = NULL;

//! Creates DMU viewer.
gui_viewer_dmu::gui_viewer_dmu( Handle(AIS_InteractiveContext) theContext, QWidget* parent )
: QWidget       (parent),
  myViewActions (NULL),
  myBackMenu    (NULL)
{
  myContext  = theContext;
  myXmin     = 0;
  myYmin     = 0;
  myXmax     = 0;
  myYmax     = 0;
  myCurZoom  = 0;
  myRectBand = 0;

  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_NoSystemBackground);

  myCurrentMode = CurAction3d_Nothing;
  myHlrModeIsOn = Standard_False;
  setMouseTracking( true );

  initViewActions();
  initCursors();

  setBackgroundRole( QPalette::NoRole );//NoBackground );
  // set focus policy to threat QContextMenuEvent from keyboard  
  setFocusPolicy( Qt::StrongFocus );
  setAttribute( Qt::WA_PaintOnScreen );
  setAttribute( Qt::WA_NoSystemBackground );
  init();
}

//! Destructor.
gui_viewer_dmu::~gui_viewer_dmu()
{
  delete myBackMenu;
}

//! \return OpenCascade's "View".
Handle(V3d_View)& gui_viewer_dmu::GetView()
{
  return myView;
}

//! \return OpenCascade's "Interactive Context".
Handle(AIS_InteractiveContext)& gui_viewer_dmu::GetContext()
{
  return myContext;
}

//! Visualizes the passed shape.
//! \param shape [in] CAD model to visualize.
//! \param mode  [in] display mode.
//! \return constructed interactive object.
Handle(AIS_Shape) gui_viewer_dmu::Visualize(const TopoDS_Shape&   shape,
                                            const AIS_DisplayMode mode)
{
  Handle(AIS_Shape) aisShape = new AIS_Shape(shape);
  //
  aisShape->UnsetDisplayMode();
  aisShape->SetDisplayMode(mode);
  //
  myContext->ClearCurrents();
  myContext->Display(aisShape);
  //
  return aisShape;
}

//! Visualizes the passed shape only.
//! \param shape [in] CAD model to visualize.
//! \param mode  [in] display mode.
//! \return constructed interactive object.
Handle(AIS_Shape) gui_viewer_dmu::VisualizeOnly(const TopoDS_Shape&   shape,
                                                const AIS_DisplayMode mode)
{
  myContext->EraseAll();
  return this->Visualize(shape, mode);
}

//! Clears the viewer.
void gui_viewer_dmu::Clear()
{
  myContext->EraseAll();
}

void gui_viewer_dmu::init()
{
  if ( myView.IsNull() )
    myView = myContext->CurrentViewer()->CreateView();

  Handle(gui_occt_window) hWnd = new gui_occt_window(this);
  myView->SetWindow(hWnd);
  if ( !hWnd->IsMapped() )
  {
    hWnd->Map();
  }
  myView->SetBackgroundColor (Quantity_NOC_BLACK);
  myView->MustBeResized();
}

void gui_viewer_dmu::paintEvent( QPaintEvent *  )
{
  myView->Redraw();
}

void gui_viewer_dmu::resizeEvent( QResizeEvent * )
{
  if( !myView.IsNull() )
  {
    myView->MustBeResized();
  }
}

void gui_viewer_dmu::fitAll()
{
  myView->FitAll();
  myView->ZFitAll();
  myView->Redraw();
}

void gui_viewer_dmu::fitArea()
{
  myCurrentMode = CurAction3d_WindowZooming;
}

void gui_viewer_dmu::zoom()
{
  myCurrentMode = CurAction3d_DynamicZooming;
}

void gui_viewer_dmu::pan()
{
  myCurrentMode = CurAction3d_DynamicPanning;
}

void gui_viewer_dmu::rotation()
{
  myCurrentMode = CurAction3d_DynamicRotation;
}

void gui_viewer_dmu::globalPan()
{
  // save the current zoom value
  myCurZoom = myView->Scale();
  // Do a Global Zoom
  myView->FitAll();
  // Set the mode
  myCurrentMode = CurAction3d_GlobalPanning;
}

void gui_viewer_dmu::front()
{
  myView->SetProj( V3d_Xpos );
}

void gui_viewer_dmu::back()
{
  myView->SetProj( V3d_Xneg );
}

void gui_viewer_dmu::top()
{
  myView->SetProj( V3d_Zpos );
}

void gui_viewer_dmu::bottom()
{
  myView->SetProj( V3d_Zneg );
}

void gui_viewer_dmu::left()
{
  myView->SetProj( V3d_Ypos );
}

void gui_viewer_dmu::right()
{
  myView->SetProj( V3d_Yneg );
}

void gui_viewer_dmu::axo()
{
  myView->SetProj( V3d_XposYnegZpos );
}

void gui_viewer_dmu::reset()
{
  myView->Reset();
}

void gui_viewer_dmu::hlrOff()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  myHlrModeIsOn = Standard_False;
  myView->SetComputedMode (myHlrModeIsOn);
  QApplication::restoreOverrideCursor();
}

void gui_viewer_dmu::hlrOn()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  myHlrModeIsOn = Standard_True;
  myView->SetComputedMode (myHlrModeIsOn);
  QApplication::restoreOverrideCursor();
}

void gui_viewer_dmu::updateToggled( bool isOn )
{
  QAction* sentBy = (QAction*)sender();

  if( !isOn )
    return;

  for ( int i = ViewFitAllId; i < ViewHlrOffId; i++ )
  {
    QAction* anAction = myViewActions->at( i );
    
    if ( ( anAction == myViewActions->at( ViewFitAreaId ) ) ||
         ( anAction == myViewActions->at( ViewZoomId ) ) ||
         ( anAction == myViewActions->at( ViewPanId ) ) ||
         ( anAction == myViewActions->at( ViewGlobalPanId ) ) ||
         ( anAction == myViewActions->at( ViewRotationId ) ) )
    {
      if ( anAction && ( anAction != sentBy ) )
      {
        anAction->setCheckable( true );
        anAction->setChecked( false );
      }
      else
      {
        if ( sentBy == myViewActions->at( ViewFitAreaId ) )
          setCursor( *handCursor );
        else if ( sentBy == myViewActions->at( ViewZoomId ) )
          setCursor( *zoomCursor );
        else if ( sentBy == myViewActions->at( ViewPanId ) )
          setCursor( *panCursor );
        else if ( sentBy == myViewActions->at( ViewGlobalPanId ) )
          setCursor( *globPanCursor );
        else if ( sentBy == myViewActions->at( ViewRotationId ) )
          setCursor( *rotCursor );
        else
          setCursor( *defCursor );

        sentBy->setCheckable( false );
      }
    }
  }
}

void gui_viewer_dmu::initCursors()
{
  if ( !defCursor )
    defCursor = new QCursor( Qt::ArrowCursor );
  if ( !handCursor )
    handCursor = new QCursor( Qt::PointingHandCursor );
  if ( !panCursor )
    panCursor = new QCursor( Qt::SizeAllCursor );
  if ( !globPanCursor )
    globPanCursor = new QCursor( Qt::CrossCursor );
  if ( !zoomCursor )
    zoomCursor = new QCursor( QPixmap(v3d_cursor_zoom_xpm) );
  if ( !rotCursor )
    rotCursor = new QCursor( QPixmap(v3d_cursor_rotate_xpm) );
}

QList<QAction*>* gui_viewer_dmu::getViewActions()
{
  initViewActions();
  return myViewActions;
}

/*!
  Get paint engine for the OpenGL viewer. [ virtual public ]
*/
QPaintEngine* gui_viewer_dmu::paintEngine() const
{
  return NULL;
}

void gui_viewer_dmu::initViewActions()
{
  if ( myViewActions )
    return;

  myViewActions = new QList<QAction*>();
  QString dir = /*ApplicationCommonWindow::getResourceDir() +*/ QString( "/" );
  QAction* a;

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_FITALL") ), QObject::tr("MNU_FITALL"), this );
  a->setToolTip( QObject::tr("TBR_FITALL") );
  a->setStatusTip( QObject::tr("TBR_FITALL") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( fitAll() ) );
  myViewActions->insert(ViewFitAllId, a);

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_FITAREA") ), QObject::tr("MNU_FITAREA"), this );
  a->setToolTip( QObject::tr("TBR_FITAREA") );
  a->setStatusTip( QObject::tr("TBR_FITAREA") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( fitArea() ) );

  a->setCheckable( true );
  connect( a, SIGNAL( toggled( bool ) ) , this, SLOT( updateToggled( bool ) ) );
  myViewActions->insert( ViewFitAreaId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_ZOOM") ), QObject::tr("MNU_ZOOM"), this );
  a->setToolTip( QObject::tr("TBR_ZOOM") );
  a->setStatusTip( QObject::tr("TBR_ZOOM") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( zoom() ) );

  a->setCheckable( true );
  connect( a, SIGNAL( toggled(bool) ) , this, SLOT( updateToggled(bool) ) );
  myViewActions->insert( ViewZoomId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_PAN") ), QObject::tr("MNU_PAN"), this );
  a->setToolTip( QObject::tr("TBR_PAN") );
  a->setStatusTip( QObject::tr("TBR_PAN") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( pan() ) );

  a->setCheckable( true );
  connect( a, SIGNAL( toggled(bool) ) , this, SLOT( updateToggled(bool) ) );
  myViewActions->insert( ViewPanId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_GLOBALPAN") ), QObject::tr("MNU_GLOBALPAN"), this );
  a->setToolTip( QObject::tr("TBR_GLOBALPAN") );
  a->setStatusTip( QObject::tr("TBR_GLOBALPAN") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( globalPan() ) );

  a->setCheckable( true );
  connect( a, SIGNAL( toggled(bool) ) , this, SLOT( updateToggled(bool) ) );
  myViewActions->insert( ViewGlobalPanId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_FRONT") ), QObject::tr("MNU_FRONT"), this );
  a->setToolTip( QObject::tr("TBR_FRONT") );
  a->setStatusTip( QObject::tr("TBR_FRONT") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( front() ) );
  myViewActions->insert( ViewFrontId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_BACK") ), QObject::tr("MNU_BACK"), this );
  a->setToolTip( QObject::tr("TBR_BACK") );
  a->setStatusTip( QObject::tr("TBR_BACK") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( back() ) );
  myViewActions->insert(ViewBackId, a);

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_TOP") ), QObject::tr("MNU_TOP"), this );
  a->setToolTip( QObject::tr("TBR_TOP") );
  a->setStatusTip( QObject::tr("TBR_TOP") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( top() ) );
  myViewActions->insert( ViewTopId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_BOTTOM") ), QObject::tr("MNU_BOTTOM"), this );
  a->setToolTip( QObject::tr("TBR_BOTTOM") );
  a->setStatusTip( QObject::tr("TBR_BOTTOM") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( bottom() ) );
  myViewActions->insert( ViewBottomId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_LEFT") ), QObject::tr("MNU_LEFT"), this );
  a->setToolTip( QObject::tr("TBR_LEFT") );
  a->setStatusTip( QObject::tr("TBR_LEFT") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( left() ) );
  myViewActions->insert( ViewLeftId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_RIGHT") ), QObject::tr("MNU_RIGHT"), this );
  a->setToolTip( QObject::tr("TBR_RIGHT") );
  a->setStatusTip( QObject::tr("TBR_RIGHT") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( right() ) );
  myViewActions->insert( ViewRightId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_AXO") ), QObject::tr("MNU_AXO"), this );
  a->setToolTip( QObject::tr("TBR_AXO") );
  a->setStatusTip( QObject::tr("TBR_AXO") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( axo() ) );
  myViewActions->insert( ViewAxoId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_ROTATION") ), QObject::tr("MNU_ROTATION"), this );
  a->setToolTip( QObject::tr("TBR_ROTATION") );
  a->setStatusTip( QObject::tr("TBR_ROTATION") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( rotation() ) );
  a->setCheckable( true );
  connect( a, SIGNAL( toggled(bool) ) , this, SLOT( updateToggled(bool) ) );
  myViewActions->insert( ViewRotationId, a );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_RESET") ), QObject::tr("MNU_RESET"), this );
  a->setToolTip( QObject::tr("TBR_RESET") );
  a->setStatusTip( QObject::tr("TBR_RESET") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( reset() ) );
  myViewActions->insert( ViewResetId, a );

  QActionGroup* ag = new QActionGroup( this );

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_HLROFF") ), QObject::tr("MNU_HLROFF"), this );
  a->setToolTip( QObject::tr("TBR_HLROFF") );
  a->setStatusTip( QObject::tr("TBR_HLROFF") );
  connect( a, SIGNAL( triggered() ) , this, SLOT( hlrOff() ) );
  a->setCheckable( true );
  ag->addAction(a);
  myViewActions->insert(ViewHlrOffId, a);

  a = new QAction( QPixmap( dir+QObject::tr("ICON_VIEW_HLRON") ), QObject::tr("MNU_HLRON"), this );
  a->setToolTip( QObject::tr("TBR_HLRON") );
  a->setStatusTip( QObject::tr("TBR_HLRON") );
  connect( a, SIGNAL( triggered() ) ,this, SLOT( hlrOn() ) );
  
  a->setCheckable( true );
  ag->addAction(a);
  myViewActions->insert( ViewHlrOnId, a );
}

void gui_viewer_dmu::mousePressEvent( QMouseEvent* e )
{
  if ( e->button() == Qt::LeftButton )
    onLButtonDown( ( e->buttons() | e->modifiers() ), e->pos() );
  else if ( e->button() == Qt::MidButton )
    onMButtonDown( e->buttons() | e->modifiers(), e->pos() );
  else if ( e->button() == Qt::RightButton )
    onRButtonDown( e->buttons() | e->modifiers(), e->pos() );
}

void gui_viewer_dmu::mouseReleaseEvent(QMouseEvent* e)
{
  if ( e->button() == Qt::LeftButton )
    onLButtonUp( ( e->buttons() | e->modifiers() ), e->pos() );
  else if ( e->button() == Qt::MidButton )
    onMButtonUp( ( e->buttons() | e->modifiers() ), e->pos() );
  else if( e->button() == Qt::RightButton )
    onRButtonUp( ( e->buttons() | e->modifiers() ), e->pos() );
}

void gui_viewer_dmu::mouseMoveEvent(QMouseEvent* e)
{
  onMouseMove( e->buttons(), e->pos() );
}

void gui_viewer_dmu::activateCursor( const CurrentAction3d mode )
{
  switch( mode )
  {
    case CurAction3d_DynamicPanning:
      setCursor( *panCursor );
      break;
    case CurAction3d_DynamicZooming:
      setCursor( *zoomCursor );
      break;
    case CurAction3d_DynamicRotation:
      setCursor( *rotCursor );
      break;
    case CurAction3d_GlobalPanning:
      setCursor( *globPanCursor );
      break;
    case CurAction3d_WindowZooming:
      setCursor( *handCursor );
      break;
    case CurAction3d_Nothing:
    default:
      setCursor( *defCursor );
      break;
  }
}

void gui_viewer_dmu::onLButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint point )
{
  //  save the current mouse coordinate in min
  myXmin = point.x();
  myYmin = point.y();
  myXmax = point.x();
  myYmax = point.y();

  if ( nFlags & CASCADESHORTCUTKEY )
  {
    myCurrentMode = CurAction3d_DynamicZooming;
  }
  //else
  //{
  //  switch ( myCurrentMode )
  //  {
  //    case CurAction3d_Nothing:
  //         if ( nFlags & MULTISELECTIONKEY )
  //           MultiInputEvent( myXmax, myYmax, 0 );
  //         else
  //           DragEvent( myXmax, myYmax, -1 );
  //         break;
  //    case CurAction3d_DynamicZooming:
  //         break;
  //    case CurAction3d_WindowZooming:
  //         break;
  //    case CurAction3d_DynamicPanning:
  //         break;
  //    case CurAction3d_GlobalPanning:
  //         break;
  //    case CurAction3d_DynamicRotation:
  //         if (myHlrModeIsOn)
  //         {
  //           myView->SetComputedMode (Standard_False);
  //         }
  //         myView->StartRotation( point.x(), point.y() );
  //         break;
  //    default:
  //         Standard_Failure::Raise( "incompatible Current Mode" );
  //         break;
  //  }
  //}
  activateCursor( myCurrentMode );
}

void gui_viewer_dmu::onMButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint /*point*/ )
{
  if ( nFlags & CASCADESHORTCUTKEY )
    myCurrentMode = CurAction3d_DynamicPanning;
  activateCursor( myCurrentMode );
}

void gui_viewer_dmu::onRButtonDown( const int/*Qt::MouseButtons*/ nFlags, const QPoint point )
{
  if ( nFlags & CASCADESHORTCUTKEY )
  {
    if (myHlrModeIsOn)
    {
      myView->SetComputedMode (Standard_False);
    }
    myCurrentMode = CurAction3d_DynamicRotation;
    myView->StartRotation( point.x(), point.y() );
  }
  else
  {
    Popup( point.x(), point.y() );
  }
  activateCursor( myCurrentMode );
}

void gui_viewer_dmu::onLButtonUp( const int nFlags, const QPoint point )
{
    switch( myCurrentMode )
    {
        case CurAction3d_Nothing:
            if ( point.x() == myXmin && point.y() == myYmin )
            {
              // no offset between down and up --> selectEvent
              myXmax = point.x();
              myYmax = point.y();
              if ( nFlags & MULTISELECTIONKEY )
                  MultiInputEvent( point.x(), point.y() );
              else
                  InputEvent( point.x(), point.y() );
            }
            /*else
            {
              DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );
              myXmax = point.x();
              myYmax = point.y();
              if ( nFlags & MULTISELECTIONKEY )
                  MultiDragEvent( point.x(), point.y(), 1 );
              else
                  DragEvent( point.x(), point.y(), 1 );
            }*/
            break;
        case CurAction3d_DynamicZooming:
            myCurrentMode = CurAction3d_Nothing;
            noActiveActions();
            break;
        case CurAction3d_WindowZooming:
            DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );//,LongDash);
            myXmax = point.x();
            myYmax = point.y();
            if ( (abs( myXmin - myXmax ) > ValZWMin ) ||
                 (abs( myYmin - myYmax ) > ValZWMin ) )
              myView->WindowFitAll( myXmin, myYmin, myXmax, myYmax );
            myCurrentMode = CurAction3d_Nothing;
            noActiveActions();
            break;
        case CurAction3d_DynamicPanning:
            myCurrentMode = CurAction3d_Nothing;
            noActiveActions();
            break;
        case CurAction3d_GlobalPanning :
            myView->Place( point.x(), point.y(), myCurZoom );
            myCurrentMode = CurAction3d_Nothing;
            noActiveActions();
            break;
        case CurAction3d_DynamicRotation:
            myCurrentMode = CurAction3d_Nothing;
            noActiveActions();
            break;
        default:
          Standard_Failure::Raise(" incompatible Current Mode ");
            break;
    }
    activateCursor( myCurrentMode );
    //ApplicationCommonWindow::getApplication()->onSelectionChanged();
}

void gui_viewer_dmu::onMButtonUp( const int nFlags, const QPoint /*point*/ )
{
    myCurrentMode = CurAction3d_Nothing;
    activateCursor( myCurrentMode );
}

void gui_viewer_dmu::onRButtonUp( const int nFlags, const QPoint point )
{
    if ( myCurrentMode == CurAction3d_Nothing )
        Popup( point.x(), point.y() );
    else
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );
        // reset tyhe good Degenerated mode according to the strored one
        //   --> dynamic rotation may have change it
        myView->SetComputedMode (myHlrModeIsOn);
        QApplication::restoreOverrideCursor();
        myCurrentMode = CurAction3d_Nothing;
    }
    activateCursor( myCurrentMode );
}

void gui_viewer_dmu::onMouseMove( Qt::MouseButtons nFlags, const QPoint point )
{
    if ( nFlags & Qt::LeftButton || nFlags & Qt::RightButton || nFlags & Qt::MidButton )
    {
    switch ( myCurrentMode )
    {
        case CurAction3d_Nothing:
          myXmax = point.x();
          myYmax = point.y();
         /* DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );
          if ( nFlags & MULTISELECTIONKEY )
              MultiDragEvent( myXmax, myYmax, 0 );
          else
            DragEvent( myXmax, myYmax, 0 );
            DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_True );*/
            break;
        case CurAction3d_DynamicZooming:
          myView->Zoom( myXmax, myYmax, point.x(), point.y() );
          myXmax = point.x();
          myYmax = point.y();
          break;
        case CurAction3d_WindowZooming:
          myXmax = point.x();
          myYmax = point.y();
          DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_False );
          DrawRectangle( myXmin, myYmin, myXmax, myYmax, Standard_True );
          break;
        case CurAction3d_DynamicPanning:
          myView->Pan( point.x() - myXmax, myYmax - point.y() );
          myXmax = point.x();
          myYmax = point.y();
          break;
        case CurAction3d_GlobalPanning:
          break;
        case CurAction3d_DynamicRotation:
          myView->Rotation( point.x(), point.y() );
          myView->Redraw();
          break;
        default:
          Standard_Failure::Raise( "incompatible Current Mode" );
          break;
    }
    }
    else
    {
        myXmax = point.x();
        myYmax = point.y();
      if ( nFlags & MULTISELECTIONKEY )
          MultiMoveEvent( point.x(), point.y() );
      else
          MoveEvent( point.x(), point.y() );
    }
}

void gui_viewer_dmu::DragEvent( const int x, const int y, const int TheState )
{
    // TheState == -1  button down
    // TheState ==  0  move
    // TheState ==  1  button up

    static Standard_Integer theButtonDownX = 0;
    static Standard_Integer theButtonDownY = 0;

    if ( TheState == -1 )
    {
        theButtonDownX = x;
        theButtonDownY = y;
    }

    if ( TheState == 1 )
    {
        myContext->Select( theButtonDownX, theButtonDownY, x, y, myView );
        emit selectionChanged();
    }
}

void gui_viewer_dmu::InputEvent( const int /*x*/, const int /*y*/ )
{
  myContext->Select();
  emit selectionChanged();
}

void gui_viewer_dmu::MoveEvent( const int x, const int y )
{
  myContext->MoveTo( x, y, myView );
}

void gui_viewer_dmu::MultiMoveEvent( const int x, const int y )
{
  myContext->MoveTo( x, y, myView );
}

void gui_viewer_dmu::MultiDragEvent( const int x, const int y, const int TheState )
{
    static Standard_Integer theButtonDownX = 0;
    static Standard_Integer theButtonDownY = 0;

    if ( TheState == -1 )
    {
        theButtonDownX = x;
        theButtonDownY = y;
    }
    if ( TheState == 0 )
    {
        myContext->ShiftSelect( theButtonDownX, theButtonDownY, x, y, myView );
        emit selectionChanged();
    }
}

void gui_viewer_dmu::MultiInputEvent( const int /*x*/, const int /*y*/ )
{
  myContext->ShiftSelect();
  emit selectionChanged();
}

void gui_viewer_dmu::Popup( const int /*x*/, const int /*y*/ )
{
  /*ApplicationCommonWindow* stApp = ApplicationCommonWindow::getApplication();
  QMdiArea* ws = ApplicationCommonWindow::getWorkspace();
  QMdiSubWindow* w = ws->activeSubWindow();
  if ( myContext->NbSelected() )
  {
    QList<QAction*>* aList = stApp->getToolActions();
    QMenu* myToolMenu = new QMenu( 0 );
    myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolWireframeId ) );
    myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolShadingId ) );
    myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolColorId ) );

    QMenu* myMaterMenu = new QMenu( myToolMenu );

    QList<QAction*>* aMeterActions = ApplicationCommonWindow::getApplication()->getMaterialActions();

    QString dir = ApplicationCommonWindow::getResourceDir() + QString( "/" );
    myMaterMenu = myToolMenu->addMenu( QPixmap( dir+QObject::tr("ICON_TOOL_MATER")), QObject::tr("MNU_MATER") );
    for ( int i = 0; i < aMeterActions->size(); i++ )
      myMaterMenu->addAction( aMeterActions->at( i ) );
       
    myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolTransparencyId ) );
    myToolMenu->addAction( aList->at( ApplicationCommonWindow::ToolDeleteId ) );
    addItemInPopup(myToolMenu);
    myToolMenu->exec( QCursor::pos() );
    delete myToolMenu;
  }
  else
  {
    if (!myBackMenu)
    {
      myBackMenu = new QMenu( 0 );

      QAction* a = new QAction( QObject::tr("MNU_CH_BACK"), this );
      a->setToolTip( QObject::tr("TBR_CH_BACK") );
      connect( a, SIGNAL( activated() ), this, SLOT( onBackground() ) );
      myBackMenu->addAction( a );  
      addItemInPopup(myBackMenu);

      a = new QAction( QObject::tr("MNU_CH_ENV_MAP"), this );
      a->setToolTip( QObject::tr("TBR_CH_ENV_MAP") );
      connect( a, SIGNAL( activated() ), this, SLOT( onEnvironmentMap() ) );
      a->setCheckable( true );
      a->setChecked( false );
      myBackMenu->addAction( a );  
      addItemInPopup(myBackMenu);
    }

    myBackMenu->exec( QCursor::pos() );
  }
  if ( w )
    w->setFocus();*/
}

void gui_viewer_dmu::addItemInPopup( QMenu* /*theMenu*/)
{
}

void gui_viewer_dmu::DrawRectangle(const int MinX, const int MinY,
                                   const int MaxX, const int MaxY, const bool Draw)
{ 
  static Standard_Integer StoredMinX, StoredMaxX, StoredMinY, StoredMaxY;
  static Standard_Boolean m_IsVisible;

  StoredMinX = (MinX < MaxX) ? MinX: MaxX ;
  StoredMinY = (MinY < MaxY) ? MinY: MaxY ;
  StoredMaxX = (MinX > MaxX) ? MinX: MaxX ;
  StoredMaxY = (MinY > MaxY) ? MinY: MaxY ;

  QRect aRect;
  aRect.setRect( StoredMinX, StoredMinY, abs(StoredMaxX-StoredMinX), abs(StoredMaxY-StoredMinY));

  if ( !myRectBand ) 
  {
    myRectBand = new QRubberBand( QRubberBand::Rectangle, this );
    myRectBand->setStyle( QStyleFactory::create("windows") );
    myRectBand->setGeometry( aRect );
    myRectBand->show();

    /*QPalette palette;
    palette.setColor(myRectBand->foregroundRole(), Qt::white);
    myRectBand->setPalette(palette);*/
  }

  if ( m_IsVisible && !Draw ) // move or up  : erase at the old position
  {
    myRectBand->hide();
    delete myRectBand;
    myRectBand = 0;
    m_IsVisible = false;
  }

  if (Draw) // move : draw
  {
    //aRect.setRect( StoredMinX, StoredMinY, abs(StoredMaxX-StoredMinX), abs(StoredMaxY-StoredMinY));
    m_IsVisible = true;
    myRectBand->setGeometry( aRect );
    //myRectBand->show();
  }
}

void gui_viewer_dmu::noActiveActions()
{
  for ( int i = ViewFitAllId; i < ViewHlrOffId ; i++ )
  {
    QAction* anAction = myViewActions->at( i );
    if ( ( anAction == myViewActions->at( ViewFitAreaId ) ) ||
         ( anAction == myViewActions->at( ViewZoomId ) ) ||
         ( anAction == myViewActions->at( ViewPanId ) ) ||
         ( anAction == myViewActions->at( ViewGlobalPanId ) ) ||
         ( anAction == myViewActions->at( ViewRotationId ) ) )
    {
      setCursor( *defCursor );
      anAction->setCheckable( true );
      anAction->setChecked( false );
    }
  }
}

void gui_viewer_dmu::onBackground()
{
  QColor aColor ;
  Standard_Real R1;
  Standard_Real G1;
  Standard_Real B1;
  myView->BackgroundColor(Quantity_TOC_RGB, R1, G1, B1);
  aColor.setRgb( (int) R1*255, (int) G1*255, (int) B1*255 );

  QColor aRetColor = QColorDialog::getColor(aColor);

  if ( aRetColor.isValid() )
  {
      R1 = aRetColor.red()/255.;
      G1 = aRetColor.green()/255.;
      B1 = aRetColor.blue()/255.;
      myView->SetBackgroundColor(Quantity_TOC_RGB,R1,G1,B1);
  }
  myView->Redraw();
}

bool gui_viewer_dmu::dump(Standard_CString theFile)
{
  myView->Redraw();
  QString ext = QFileInfo( QString( theFile ) ).completeSuffix();
  if ( !ext.compare("ps") || !ext.compare("eps") || !ext.compare("tex") || !ext.compare("pdf") || !ext.compare("svg") || !ext.compare("pgf") )
  {
    Graphic3d_ExportFormat exFormat;
    if ( !ext.compare("ps") )
      exFormat = Graphic3d_EF_PostScript;
    if ( !ext.compare("eps") )
      exFormat = Graphic3d_EF_EnhPostScript;
    if ( !ext.compare("tex") )
      exFormat = Graphic3d_EF_TEX;
    if ( !ext.compare("pdf") )
      exFormat = Graphic3d_EF_PDF;
    if ( !ext.compare("svg") )
      exFormat = Graphic3d_EF_SVG;
    if ( !ext.compare("pgf") )
      exFormat = Graphic3d_EF_PGF;
    try
    {
      myView->Export( theFile, exFormat );
    }
    catch(...)
    {
      return false;
    }
    return true;
  }
  return myView->Dump(theFile) > 0;
}

gui_viewer_dmu::CurrentAction3d gui_viewer_dmu::getCurrentMode()
{
  return myCurrentMode;
}
