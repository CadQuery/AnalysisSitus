//-----------------------------------------------------------------------------
// Created on: 01 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <gui_vtk_window.h>

//! Constructor.
//! \param parent [in] parent widget.
//! \param f      [in] window flags.
gui_vtk_window::gui_vtk_window(QWidget* parent, Qt::WindowFlags f)
: QVTKWidget(parent, f)
{}

//! Destructor.
gui_vtk_window::~gui_vtk_window()
{}

//! Custom reaction on close event.
//! \param pEvent [in] event instance.
void gui_vtk_window::closeEvent(QCloseEvent* ASitus_NotUsed(pEvent))
{
  emit windowClosed();
}
