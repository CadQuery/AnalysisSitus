//-----------------------------------------------------------------------------
// Created on: 01 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_VtkWindow.h>

//! Constructor.
//! \param parent [in] parent widget.
//! \param f      [in] window flags.
asiUI_VtkWindow::asiUI_VtkWindow(QWidget* parent, Qt::WindowFlags f)
: QVTKWidget(parent, f)
{}

//! Destructor.
asiUI_VtkWindow::~asiUI_VtkWindow()
{}

//! Custom reaction on close event.
//! \param pEvent [in] event instance.
void asiUI_VtkWindow::closeEvent(QCloseEvent* asiUI_NotUsed(pEvent))
{
  emit windowClosed();
}
