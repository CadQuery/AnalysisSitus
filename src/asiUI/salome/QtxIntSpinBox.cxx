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

// File:      QtxIntSpinBox.cxx
// Author:    Sergey TELKOV
//
#include "QtxIntSpinBox.h"

#pragma warning(push, 0)
#include <QLineEdit>
#pragma warning(pop)

/*!
  \class QtxIntSpinBox
  \brief Enhanced version of the Qt's spin box.

  The QtxIntSpinBox class represents the widget for entering the
  integer values. In addition to the functionality provided by
  QSpinBox, this class supports "cleared" state - this is the
  state corresponding to "None" (or empty) entered value.

  To set "cleared" state use setCleared() method. To check if the spin
  box stores "cleared" state, use isCleared() method.
  For example:
  \code
  if (mySpinBox->isCleared()) {
    ... // process "None" state
  }
  else {
    int value = mySpinBox->value();
    ... // process entered value
  }
  \endcode
*/

/*!
  \brief Constructor.

  Constructs a spin box with 0 as minimum value and 99 as maximum value, 
  a step value of 1. The value is initially set to 0.

  \param parent parent object
*/
QtxIntSpinBox::QtxIntSpinBox( QWidget* parent )
: QSpinBox( parent ),
  myCleared( false )
{
  // VSR 01/07/2010: Disable thousands separator for spin box
  // (to avoid incosistency of double-2-string and string-2-double conversion)
  QLocale loc;
  loc.setNumberOptions(loc.numberOptions() | QLocale::OmitGroupSeparator | QLocale::RejectGroupSeparator);
  setLocale(loc);

  setCorrectionMode( QSpinBox::CorrectToNearestValue );
  connect( lineEdit(), SIGNAL( textChanged( const QString& ) ), 
           this, SLOT( onTextChanged( const QString& ) ) );
}

/*!
  \brief Constructor.

  Constructs a spin box with specified minimum, maximum and step value.
  The value is initially set to the minimum value.

  \param min spin box minimum possible value
  \param max spin box maximum possible value
  \param step spin box increment/decrement value
  \param parent parent object
*/
QtxIntSpinBox::QtxIntSpinBox( int min, int max, int step, QWidget* parent )
: QSpinBox( parent ),
  myCleared( false )
{
  // VSR 01/07/2010: Disable thousands separator for spin box
  // (to avoid incosistency of double-2-string and string-2-double conversion)
  QLocale loc;
  loc.setNumberOptions(loc.numberOptions() | QLocale::OmitGroupSeparator | QLocale::RejectGroupSeparator);
  setLocale(loc);

  setMinimum( min );
  setMaximum( max );
  setSingleStep( step );
  setCorrectionMode( QSpinBox::CorrectToNearestValue );

  connect( lineEdit(), SIGNAL( textChanged( const QString& ) ), 
           this, SLOT( onTextChanged( const QString& ) ) );
}

/*!
  \brief Destructor.
*/
QtxIntSpinBox::~QtxIntSpinBox()
{
}

/*!
  \brief Check if spin box is in the "cleared" state.
  \return \c true if spin box is cleared
*/
bool QtxIntSpinBox::isCleared() const
{
  return myCleared;
}

/*!
  \brief Change "cleared" status of the spin box.
  \param on new "cleared" status
*/
void QtxIntSpinBox::setCleared( const bool on )
{
  if ( myCleared == on )
    return;
  
  myCleared = on;
  setSpecialValueText( specialValueText() );
}

/*!
  \brief Convert value to the text.
  \param val value being converted
  \return string containing the converted value
*/
QString QtxIntSpinBox::textFromValue( int val ) const
{
  return myCleared ? QString() : QSpinBox::textFromValue( val );
}

/*!
  \brief Perform \a steps increment/decrement steps.
  
  The \a steps value can be any integer number. If it is > 0,
  the value incrementing is done, otherwise value is decremented
  \a steps times.  

  \param steps number of increment/decrement steps
*/
void QtxIntSpinBox::stepBy( int steps )
{
  myCleared = false;

  QSpinBox::stepBy( steps );
}

/*!
  \brief Called when user enters the text in the spin box.
  \param txt current spin box text (not used)
*/
void QtxIntSpinBox::onTextChanged( const QString& )
{
  myCleared = false;
}
