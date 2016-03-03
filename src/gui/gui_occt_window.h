//-----------------------------------------------------------------------------
// Created on: 11 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_occt_window_h
#define gui_occt_window_h

// OCCT includes
#include <Aspect_Window.hxx>

// Qt includes
#pragma warning(push, 0)
#include <QWidget>
#pragma warning(pop)

class gui_occt_window;

/*
  gui_occt_window class implements Aspect_Window interface using Qt API
  as a platform-independent source of window geometry information.
  A similar class should be used instead of platform-specific OCCT
  classes (WNT_Window, Xw_Window) in any Qt 5 application using OCCT
  3D visualization.

  With Qt 5, the requirement for a Qt-based application to rely fully
  on Qt public API and stop using platform-specific APIs looks mandatory.
  An example of this is changed QWidget event sequence: when a widget is
  first shown on the screen, a resize event is generated before the
  underlying native window is resized correctly, however the QWidget instance
  already holds correct size information at that moment. The OCCT classes
  acting as a source of window geometry for V3d_View class (WNT_Window, Xw_Window)
  are no longer compatible with changed Qt behavior because they rely on
  platform-specific API that cannot return correct window geometry information
  in some cases. A reasonable solution is to provide a Qt-based implementation
  of Aspect_Window interface at application level.
*/
class gui_occt_window : public Aspect_Window
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTIEXT(OcctWindow, Aspect_Window)

public:

  gui_occt_window(QWidget*                   theWidget,
                  const Quantity_NameOfColor theBackColor = Quantity_NOC_MATRAGRAY);

  //! Destructor
  ~gui_occt_window()
  {
    Destroy();
  }

  virtual void Destroy();

public:

  //! Returns native Window handle
  virtual Aspect_Drawable NativeHandle() const;

  //! Returns parent of native Window handle.
  virtual Aspect_Drawable NativeParentHandle() const;

  //! Applies the resizing to the window <me>
  virtual Aspect_TypeOfResize DoResize() const;

  //! Returns True if the window <me> is opened
  //! and False if the window is closed.
  virtual Standard_Boolean IsMapped() const;

  //! Apply the mapping change to the window <me>
  //! and returns TRUE if the window is mapped at screen.
  virtual Standard_Boolean DoMapping() const { return Standard_True; }

  //! Opens the window <me>.
  virtual void Map() const;

  //! Closes the window <me>.
  virtual void Unmap() const;

  virtual void Position(int& theX1, int& theY1,
                        int& theX2, int& theY2) const;

  //! Returns The Window RATIO equal to the physical
  //! WIDTH/HEIGHT dimensions.
  virtual Quantity_Ratio Ratio() const;

  virtual void Size(int& theWidth, int& theHeight) const;

  virtual Aspect_FBConfig NativeFBConfig() const Standard_OVERRIDE { return NULL; }

protected:

  int      m_iXLeft;
  int      m_iYTop;
  int      m_iXRight;
  int      m_iYBottom;
  QWidget* m_pWidget;

};


#endif