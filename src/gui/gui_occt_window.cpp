//-----------------------------------------------------------------------------
// Created on: 11 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_occt_window.h>

IMPLEMENT_STANDARD_RTTIEXT(gui_occt_window, Aspect_Window)

// =======================================================================
// function : OcctWindow
// purpose  :
// =======================================================================
gui_occt_window::gui_occt_window(QWidget*                   theWidget,
                                 const Quantity_NameOfColor theBackColor)
//
: Aspect_Window (),
  m_pWidget     (theWidget)
{
  this->SetBackground(theBackColor);
  //
  m_iXLeft   = m_pWidget->rect().left();
  m_iYTop    = m_pWidget->rect().top();
  m_iXRight  = m_pWidget->rect().right();
  m_iYBottom = m_pWidget->rect().bottom();
}

// =======================================================================
// function : Destroy
// purpose  :
// =======================================================================
void gui_occt_window::Destroy()
{
  m_pWidget = NULL;
}

// =======================================================================
// function : NativeParentHandle
// purpose  :
// =======================================================================
Aspect_Drawable gui_occt_window::NativeParentHandle() const
{
  QWidget* aParentWidget = m_pWidget->parentWidget();
  if ( aParentWidget != NULL )
    return (Aspect_Drawable) aParentWidget->winId();
  else
    return NULL;
}

// =======================================================================
// function : NativeHandle
// purpose  :
// =======================================================================
Aspect_Drawable gui_occt_window::NativeHandle() const
{
  return (Aspect_Drawable) m_pWidget->winId();
}

// =======================================================================
// function : IsMapped
// purpose  :
// =======================================================================
Standard_Boolean gui_occt_window::IsMapped() const
{
  return !( m_pWidget->isMinimized() || m_pWidget->isHidden() );
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void gui_occt_window::Map() const
{
  m_pWidget->show();
  m_pWidget->update();
}

// =======================================================================
// function : Unmap
// purpose  :
// =======================================================================
void gui_occt_window::Unmap() const
{
  m_pWidget->hide();
  m_pWidget->update();
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize gui_occt_window::DoResize() const
{
  int                 aMask = 0;
  Aspect_TypeOfResize aMode = Aspect_TOR_UNKNOWN;

  if ( !m_pWidget->isMinimized() )
  {
    if ( Abs ( m_pWidget->rect().left()   - m_iXLeft   ) > 2 ) aMask |= 1;
    if ( Abs ( m_pWidget->rect().right()  - m_iXRight  ) > 2 ) aMask |= 2;
    if ( Abs ( m_pWidget->rect().top()    - m_iYTop    ) > 2 ) aMask |= 4;
    if ( Abs ( m_pWidget->rect().bottom() - m_iYBottom ) > 2 ) aMask |= 8;

    switch ( aMask )
    {
      case 0:
        aMode = Aspect_TOR_NO_BORDER;
        break;
      case 1:
        aMode = Aspect_TOR_LEFT_BORDER;
        break;
      case 2:
        aMode = Aspect_TOR_RIGHT_BORDER;
        break;
      case 4:
        aMode = Aspect_TOR_TOP_BORDER;
        break;
      case 5:
        aMode = Aspect_TOR_LEFT_AND_TOP_BORDER;
        break;
      case 6:
        aMode = Aspect_TOR_TOP_AND_RIGHT_BORDER;
        break;
      case 8:
        aMode = Aspect_TOR_BOTTOM_BORDER;
        break;
      case 9:
        aMode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;
        break;
      case 10:
        aMode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER;
        break;
      default:
        break;
    }  // end switch

    *( (int*) &m_iXLeft  ) = m_pWidget->rect().left();
    *( (int*) &m_iXRight ) = m_pWidget->rect().right();
    *( (int*) &m_iYTop   ) = m_pWidget->rect().top();
    *( (int*) &m_iYBottom) = m_pWidget->rect().bottom();
  }

  return aMode;
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Quantity_Ratio gui_occt_window::Ratio() const
{
  QRect aRect = m_pWidget->rect();
  return Quantity_Ratio( aRect.right() - aRect.left() ) / Quantity_Ratio( aRect.bottom() - aRect.top() );
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void gui_occt_window::Size(int& theWidth, int& theHeight) const
{
  QRect aRect = m_pWidget->rect();
  theWidth    = aRect.right();
  theHeight   = aRect.bottom();
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void gui_occt_window::Position(int& theX1, int& theY1,
                               int& theX2, int& theY2) const
{
  theX1 = m_pWidget->rect().left();
  theX2 = m_pWidget->rect().right();
  theY1 = m_pWidget->rect().top();
  theY2 = m_pWidget->rect().bottom();
}
