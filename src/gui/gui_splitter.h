//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef gui_splitter_h
#define gui_splitter_h

// A-Situs includes
#include <analysis_situs.h>

// Qt includes
#pragma warning(push, 0)
#include <QSplitter>
#pragma warning(pop)

//! Custom splitter widget with plastique style.
class gui_splitter : public QSplitter
{
  Q_OBJECT

public:

  gui_splitter(Qt::Orientation theOrientation,
               QWidget*        theParent = 0);

  gui_splitter(QWidget* theParent = 0);

};

#endif
