//-----------------------------------------------------------------------------
// Created on: 04 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef gui_viewer_h
#define gui_viewer_h

// A-Situs includes
#include <analysis_situs.h>

// Qt includes
#pragma warning(push, 0)
#include <QMainWindow>
#pragma warning(pop)

//! Base class for all viewers.
class gui_viewer : public QMainWindow
{
  Q_OBJECT

public:

  gui_viewer(QWidget* parent = NULL);

  virtual ~gui_viewer();

public:

  virtual void Repaint() = 0;

};

#endif
