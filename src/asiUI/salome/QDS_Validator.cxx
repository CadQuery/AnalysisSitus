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

#include "QDS_Validator.h"

#pragma warning(disable : 4714)

/*!
  \class QDS_IntegerValidator
  \brief A validator for integer values.

  This class provides validation of the strings entered by the user for datum with 'Integer'
  value type according to datum properties: minimum value, maximum value, filter.

  Inherits QIntValidator.
*/

/*!
  \brief Constructor.

  Create empty validator object.

  \param p parent object
*/
QDS_IntegerValidator::QDS_IntegerValidator( QObject* p )
: QIntValidator( p )
{
}

/*!
  \brief Constructor.
  
  Create validator object with filter string \a f.

  \param f filter string
  \param p parent object
*/
QDS_IntegerValidator::QDS_IntegerValidator( const QString& f, QObject* p )
: QIntValidator( p ),
  myFilter( f )
{
}

/*!
  \brief Destructor.
*/
QDS_IntegerValidator::~QDS_IntegerValidator()
{
}

/*!
  \brief Validate the entered string \a input. 

  Reimplemented from QIntValidator. 
  
  Perform standard check by QIntValidator::validate(). If returned state 
  is not \c QValidator::Invalid and filter is specified then perform validation
  with filter by QRegExpValidator.

  Returns \c QValidator::Invalid if input is invalid according to this validator's rules,
  \c QValidator::Intermediate if it is likely that a little more editing will make the
  input acceptable and \c QValidator::Acceptable if the input is valid.

  \param input string being validated
  \param pos cursor position
  \return validation state (\c QValidator::State)
*/
QValidator::State QDS_IntegerValidator::validate( QString& input, int& pos ) const
{
  State rgState = Acceptable;
  State ivState = QIntValidator::validate( input, pos );
  if ( ivState != Invalid && !myFilter.isEmpty() )
    rgState = QRegExpValidator( QRegExp( myFilter ), 0 ).validate( input, pos );

  ivState = qMin( ivState, rgState );

  return ivState;
}

/*!
  \class QDS_DoubleValidator
  \brief A validator for floating point values.

  This class provides validation of the strings entered by the user for datum with 'Float'
  value type according to datum properties: minimum value, maximum value, precision, filter.

  Inherits QDoubleValidator.
*/

/*!
  \brief Constructor.
  
  Create empty validator object.
  
  \param p parent object
*/
QDS_DoubleValidator::QDS_DoubleValidator( QObject* p )
: QDoubleValidator( p )
{
}

/*!
  \brief Constructor.
  
  Create validator object with filter string \a f.
  
  \param f filter string
  \param p parent object
*/
QDS_DoubleValidator::QDS_DoubleValidator( const QString& f, QObject* p )
: QDoubleValidator( p ),
  myFilter( f )
{
}

/*!
  \brief Destructor.
*/
QDS_DoubleValidator::~QDS_DoubleValidator()
{
}

/*!
  \brief Validate the entered string \a input. 

  Reimplemented from QDoubleValidator. 
  
  Perform standard check by QDoubleValidator::validate(). If returned state 
  is not \c QValidator::Invalid and filter is specified then perform validation
  with filter by QRegExpValidator.

  Returns \c QValidator::Invalid if input is invalid according to this validator's rules,
  \c QValidator::Intermediate if it is likely that a little more editing will make the
  input acceptable and \c QValidator::Acceptable if the input is valid.

  \param input string being validated
  \param pos cursor position
  \return validation state (\c QValidator::State)
*/
QValidator::State QDS_DoubleValidator::validate( QString& input, int& pos ) const
{
  State rgState = Acceptable;
  State dvState = QDoubleValidator::validate( input, pos );
  if ( dvState != Invalid && !myFilter.isEmpty() )
    rgState = QRegExpValidator( QRegExp( myFilter ), 0 ).validate( input, pos );

  dvState = qMin( dvState, rgState );

  return dvState;
}

/*!
  \class QDS_StringValidator
  \brief A validator for string values.

  This class provides validation of the strings entered by the user for datum with 'String'
  value type according to datum properties: format, filter.

  Inherits from QValidator.
*/

/*!
  \brief Constructor.

  Create empty validator object.

  \param p parent object
*/
QDS_StringValidator::QDS_StringValidator( QObject* p ) 
: QValidator( p ), 
  myLen( -1 ) 
{
}

/*!
  \brief Constructor. 

  Create validator object with filter string \a f.

  \param f filter string
  \param p parent object
*/
QDS_StringValidator::QDS_StringValidator( const QString& f, QObject* p ) 
: QValidator( p ), 
  myFlags( f ), 
  myLen( -1 ) 
{
}

/*!
  \brief Constructor.

  Create validator object with filter string \a ft and format flags \a fg.
  \param ft filter string
  \param fg format string
  \param p parent object
*/
QDS_StringValidator::QDS_StringValidator( const QString& ft, const QString& fg, QObject* p ) 
: QValidator( p ), 
  myLen( -1 ), 
  myFilter( ft ), 
  myFlags( fg ) 
{
}

/*!
  \brief Destructor.
*/
QDS_StringValidator::~QDS_StringValidator() 
{
}

/*!
  \brief Get maximum valid string length.

  If maximum length is not specified, -1 is returned.

  \return maximum string length
*/
int QDS_StringValidator::length() const 
{ 
  return myLen; 
}

/*!
  \brief Set maximum valid string length.

  If l <= 0 is not specified, maximum valid length is reset
  (any string length is valid).

  \param l maximum strin length
*/
void QDS_StringValidator::setLength( const int l )
{
  myLen = l;
}

/*!
  \brief Validate the entered string \a input. 

  Reimplemented from QValidator. 
  
  Convert specified string to the upper/lower case if the format flags contains
  specificator 'u'/'l' correspondingly.  If valid string length is specified,
  then check given string length. If filter is specified, then perform also 
  validation with filter by QRegExpValidator.

  Returns \c QValidator::Invalid if input is invalid according to this validator's rules,
  \c QValidator::Intermediate if it is likely that a little more editing will make the
  input acceptable and \c QValidator::Acceptable if the input is valid.

  \param input string being validated
  \param pos cursor position
  \return validation state (\c QValidator::State)
*/
QValidator::State QDS_StringValidator::validate( QString& input, int& pos ) const
{
  if ( input.isEmpty() )
    return Acceptable;

  QString orig = input;
  if ( myFlags.contains( 'u', Qt::CaseInsensitive ) )
    input = input.toUpper();
  if ( myFlags.contains( 'l', Qt::CaseInsensitive ) )
    input = input.toLower();

  State rgState = Acceptable;
  State svState = orig == input ? Acceptable : Intermediate;

  if ( myLen >= 0 && (int)input.length() > myLen )
      svState = Intermediate;

  if ( !myFilter.isEmpty() )
    rgState = QRegExpValidator( QRegExp( myFilter ), 0 ).validate( input, pos );

  svState = qMin( svState, rgState );

  return svState;
}
