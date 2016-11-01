//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiUI_LineEdit_h
#define asiUI_LineEdit_h

// A-Situs includes
#include <asiUI.h>

// Qt includes
#pragma warning(push, 0)
#include <QLineEdit>
#pragma warning(pop)

//! Editor with a desired policy of focus handling.
class asiUI_LineEdit : public QLineEdit
{
  Q_OBJECT

public:

  asiUI_LineEdit(const QString& value, QWidget* parent = 0) : QLineEdit(value, parent) {}
  asiUI_LineEdit(QWidget* parent = 0) : QLineEdit(parent) {}

};

#endif
