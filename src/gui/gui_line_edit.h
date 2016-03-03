//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_line_edit_h
#define gui_line_edit_h

// A-Situs includes
#include <analysis_situs.h>

// Qt includes
#pragma warning(push, 0)
#include <QLineEdit>
#pragma warning(pop)

//! Editor with a desired policy of focus handling.
class gui_line_edit : public QLineEdit
{
  Q_OBJECT

public:

  gui_line_edit(const QString& value, QWidget* parent = 0) : QLineEdit(value, parent) {}
  gui_line_edit(QWidget* parent = 0) : QLineEdit(parent) {}

};

#endif
