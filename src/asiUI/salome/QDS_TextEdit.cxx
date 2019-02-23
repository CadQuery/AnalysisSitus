// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "QDS_TextEdit.h"

#include <QTextEdit>

/*
  \class QDS_TextEdit
  \brief Datum with control corresponding to the text edit. 

  User can enter parameter value in multi line editor.
*/

/*!
  \brief Constructor. 

  Create combobox datum object with datum identifier \a id 
  and parent widget \a parent. 

  Parameter \a flags defines behaviour of datum and set of created
  subwidgets. Default value of this parameter is QDS::All.

  Parameter \a comp specifies the component name which will be used
  when searching the dictionary item.

  \param id datum identifier
  \param parent parent widget
  \param flags datum flags
  \param comp component
*/
QDS_TextEdit::QDS_TextEdit( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  \brief Destructor.
*/
QDS_TextEdit::~QDS_TextEdit()
{
}

/*!
  \brief Get string value from datum.
  \return datum string value
*/
QString QDS_TextEdit::getString() const
{
  QString res;
  if ( textEdit() )
    res = textEdit()->toPlainText();
  return res;
}

/*!
  \brief Set string value to datum.
  \param txt new datum string value
*/
void QDS_TextEdit::setString( const QString& txt )
{
  if ( textEdit() )
    textEdit()->setText( txt );
}

/*!
  \brief Get text edit widget.
  \return internaltext edit widget
*/
QTextEdit* QDS_TextEdit::textEdit() const
{
  return ::qobject_cast<QTextEdit*>( controlWidget() );
}

/*!
  \brief Create text edit widget as control subwidget.
  \param parent parent widget
  \return created text edit widget
*/
QWidget* QDS_TextEdit::createControl( QWidget* parent )
{
  QTextEdit* te = new QTextEdit( parent );
  connect( te, SIGNAL( textChanged() ), this, SLOT( onTextChanged() ) );
  return te;
}

/*!
  \brief Called when text is changed by the user.

  Notify about text changing in text edit.
*/
void QDS_TextEdit::onTextChanged()
{
  invalidateCache();

  onParamChanged();

  QString str = getString();

  emit paramChanged();
  emit paramChanged( str );
}

/*!
  \fn void QDS_TextEdit::returnPressed();
  \brief The signal is emitted when user presses \c Enter key in the text edit.
*/
