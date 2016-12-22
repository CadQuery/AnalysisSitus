//-----------------------------------------------------------------------------
// Created on: 01 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiUI_VtkWindow_h
#define asiUI_VtkWindow_h

// Analysis Situs includes
#include <asiUI.h>

// QVTK includes
#pragma warning(push, 0)
#include <QVTKWidget.h>
#pragma warning(pop)

//! Sub-class for QVTKWidget to override its closeEvent().
class asiUI_VtkWindow : public QVTKWidget
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_VtkWindow(QWidget* parent = NULL, Qt::WindowFlags f = 0);

  asiUI_EXPORT virtual
    ~asiUI_VtkWindow();

protected:

  virtual void
    closeEvent(QCloseEvent* pEvent);

signals:

  void windowClosed();

};


#endif