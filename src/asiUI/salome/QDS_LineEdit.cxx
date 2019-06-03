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

#include "QDS_LineEdit.h"

#pragma warning(push, 0)
#include <QLineEdit>
#include <QValidator>
#pragma warning(pop)

/*
  \class QDS_LineEdit
  
  \brief Datum with control corresponding to the line edit. 

  User can enter parameter value in single line editor.
  The value entered by the user is checked by the validator according to item type
  according to the item properties (minimum, maximum, filter, precision, etc).

  If user input is not valid, the value is displayed in red color.
*/

/*!
  \brief Constructor. 

  Create line edit datum object with datum identifier \a id and parent widget \a parent. 

  Parameter \a flags defines behaviour of datum and set of created
  subwidgets. Default value of this parameter is QDS::All.

  Parameter \a comp specifies the component name which will be used
  when searching the dictionary item.

  \param id datum identifier
  \param parent parent widget
  \param flags datum flags
  \param comp component
*/
QDS_LineEdit::QDS_LineEdit( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  \brief Destructor.
*/
QDS_LineEdit::~QDS_LineEdit()
{
}

/*!
  \brief Process notification about active units system changing.

  Update validator settings for line edit.

  \param system new active units system
*/
void QDS_LineEdit::unitSystemChanged( const QString& system )
{
  QDS_Datum::unitSystemChanged( system );

  QLineEdit* le = lineEdit();
  if ( !le )
    return;
  
  delete le->validator();
  le->setValidator(0);
  QValidator* valid = validator();
  if ( valid )
    le->setValidator( valid );

  QString aFormat = format();
  int num = 0;
  int pos = aFormat.indexOf( '%' );
  if ( pos != -1 )
  {
    pos++;
    QString aLen;
    while ( pos < (int)aFormat.length() && aFormat.at( pos ).isDigit() )
      aLen += aFormat.at( pos++ );
    if ( pos < (int)aFormat.length() && aFormat.at( pos ) == '.' )
      num += 1;
    if ( !aLen.isEmpty() )
      num += aLen.toInt();
  }
  
  int zeroLen = formatValue( 0 ).length();
  int minLen  = formatValue( minValue() ).length();
  int maxLen  = formatValue( maxValue() ).length();

  num = qMax( qMax( num, zeroLen ), qMax( minLen, maxLen ) );
  ((Editor*)le)->setNumber( num );
}

/*!
  \brief Select all text in the editor.
*/
void QDS_LineEdit::selectAll()
{
  if ( lineEdit() )
    lineEdit()->selectAll();
}

/*!
  \brief Deselect all text in the editor.
*/
void QDS_LineEdit::deselect()
{
  if ( lineEdit() )
    lineEdit()->deselect();
}

/*!
  \brief Select/deselect all text in the editor.
  \param on select text if \c true and deselect if \c false
*/
void QDS_LineEdit::setSelection( const bool on )
{
  if ( on )
    selectAll();
  else
    deselect();
}

/*!
  \brief Check if there is selection in the editor.
  \return \c true if the editor has selected text
*/
bool QDS_LineEdit::hasSelection() const
{
  return lineEdit() ? lineEdit()->hasSelectedText() : false;
}

/*!
  \brief Set the aligment for the line edit.
  \param align alignment type (Qt::Alignment)
  \param type ORed subwidget flags
*/
void QDS_LineEdit::setAlignment( const int align, const int type )
{
  if ( ( type & Control ) && lineEdit() )
    lineEdit()->setAlignment( Qt::Alignment(align) );

  QDS_Datum::setAlignment( align, type );
}

/*!
  \brief Get string value from datum.
  \return datum string value
*/
QString QDS_LineEdit::getString() const
{
  QString res;
  if ( lineEdit() )
    res = lineEdit()->text();
  return res;
}

/*!
  \brief Set string value to datum.
  \param txt new datum string value
*/
void QDS_LineEdit::setString( const QString& txt )
{
  if ( lineEdit() )
    lineEdit()->setText( txt );
}

/*!
  \brief Get line edit widget.
  \return pointer to the QLineEdit widget
*/
QLineEdit* QDS_LineEdit::lineEdit() const
{
  return ::qobject_cast<QLineEdit*>( controlWidget() );
}

/*!
  \brief Create line edit widget as control subwidget.
  \param parent parent widget
  \return created line edit widget
*/
QWidget* QDS_LineEdit::createControl( QWidget* parent )
{
  Editor* le = new Editor( parent );
  connect( le, SIGNAL( returnPressed() ), this, SIGNAL( returnPressed() ) );
  connect( le, SIGNAL( textChanged( const QString& ) ), this, SLOT( onTextChanged( const QString& ) ) );
  return le;
}

/*!
  \brief Called when text in the edit box is modified by the user.

  Notify about text changing in the line edit.

  \param txt current text in the line edit widget (not used)
*/
void QDS_LineEdit::onTextChanged( const QString& /*txt*/ )
{
  invalidateCache();

  onParamChanged();
  QString str = getString();
  emit paramChanged();
  emit paramChanged( str );
}

/*!
  \brief Called when text is changed.

  Validate the current parameter value. If value is not valid then set text color as red.

  Emits signal paramChanged() to notify about changing of the control state.
*/
void QDS_LineEdit::onParamChanged()
{
  QLineEdit* anEdit = lineEdit();
  if ( !anEdit )
    return;

  bool aValid = isValid( false );

  QPalette aPal = anEdit->palette();
  if ( !aValid )
    aPal.setColor( QPalette::Active, QPalette::Text, QColor( 255, 0, 0 ) );
  else
    aPal.setColor( QPalette::Active, QPalette::Text, QColor( 0, 0, 0 ) );

  anEdit->setPalette( aPal );
}

/*!
  \brief void QDS_LineEdit::returnPressed();
  \brief The signal is emitted when user presses \c Enter key in the line edit.
*/
