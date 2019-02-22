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

#include "QDS_CheckBox.h"

#include <QCheckBox>

/*
  \class QDS_CheckBox
  \brief Datum with control corresponding to check box.

  This control can have only two states:
  - 1 (on/true)
  - 0 (off/false).

  QDS_CheckBox don't take into account standard parameter properties
  (minimum, maximum, filter, etc).

  QDS_CheckBox can set and get following values for access methods (setStringValue(),
  setIntegerValue(), setDoubleValue(), stringValue(), integerValue(), doubleValue()):
  - "1"  - check box state is switched on.
  - "0"  - check box state is switched off.
  - "-1" - check box state is "PartiallyChecked" (undefined).

  User can set and test "PartiallyChecked" check using methods clear() 
  and isEmpty() correspondingly.
*/

/*!
  \brief Constructor. 

  Create check box datum object with datum identifier \a id 
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
QDS_CheckBox::QDS_CheckBox( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  \brief Destructor.
*/
QDS_CheckBox::~QDS_CheckBox()
{
}

/*!
  \brief Set the state "PartiallyChecked" (undefined) for checkbox.
*/
void QDS_CheckBox::clear()
{
  setStringValue( "-1" );
}

/*!
  \brief Get string value from the widget.
  \return "1" if check box is checked on and "0" otherwise
*/
QString QDS_CheckBox::getString() const
{
  QString val;
  if ( checkBox() && checkBox()->checkState() != Qt::PartiallyChecked )
    val = checkBox()->isChecked() ? "1" : "0";
  return val;
}

/*!
  \brief Set the string value into the widget.

  If string \a txt contains "1", then check box state is switched on.
  If string \a txt contains "0", then check box state is switched on.
  If string \a txt contains "-1", then check box is reset to 
  "PartiallyChecked" (undefined) state.

  \param txt string value
*/
void QDS_CheckBox::setString( const QString& txt )
{
  if ( !checkBox() )
    return;

  bool isOk;
  int val = (int)txt.toDouble( &isOk );
  if ( isOk && val < 0 )
  {
    checkBox()->setTristate();
    checkBox()->setCheckState(Qt::PartiallyChecked);
  }
  else
    checkBox()->setChecked( isOk && val != 0 );
}

/*!
  \brief Get internal check box.
  \return pointer to QCheckBox widget
*/
QCheckBox* QDS_CheckBox::checkBox() const
{
  return ::qobject_cast<QCheckBox*>( controlWidget() );
}

/*!
  \brief Create internal check box as control widget.
  \param parent parent widget
  \return created check box widget
*/
QWidget* QDS_CheckBox::createControl( QWidget* parent )
{
  QCheckBox* cb = new QCheckBox( parent );
  connect( cb, SIGNAL( stateChanged( int ) ), SLOT( onParamChanged() ) );
  connect( cb, SIGNAL( toggled( bool ) ), SIGNAL( toggled( bool ) ) );
  connect( cb, SIGNAL( stateChanged( int ) ), this, SLOT( onStateChanged( int ) ) );
  return cb;
}

/*!
  \brief Called when check box is switched.

  Emits signal paramChanged() to notify about changing of the control state.
*/
void QDS_CheckBox::onParamChanged()
{
  emit paramChanged();
}

/*!
  \brief Called when check box is switched.

  Switch off check box property "tristate" when state is changed by the user.

  \param state new check box state
*/
void QDS_CheckBox::onStateChanged( int state )
{
  if ( state != Qt::PartiallyChecked && checkBox() )
    checkBox()->setTristate( false );
}

/*!
  \brief Set the check box state to \a theState.
  \param theState new check box state
*/
void QDS_CheckBox::setChecked( const bool theState )
{
  if ( checkBox() )
    checkBox()->setChecked( theState );
}

/*!
  \brief Get current check box state.
  \return check box state
*/
bool QDS_CheckBox::isChecked() const
{
  return checkBox() ? checkBox()->isChecked() : false;
}

/*!
  \fn void QDS_CheckBox::toggled( bool on );
  \brief Emitted when the check box state is toggled.
  \param on new check box state
*/
