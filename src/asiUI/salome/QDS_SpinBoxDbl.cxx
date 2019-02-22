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

#include "QDS_SpinBoxDbl.h"

#include <QtxDoubleSpinBox.h>

/*
  \class QDS_SpinBoxDbl
  \brief Datum with control corresponding to spin box.
  
  This control is suitable for double numbers. User can input data directly in the spin box
  or can modify current value by clicking arrow (+/-) buttons.
*/

/*!
  \brief Constructor. 

  Create spin box datum object with datum identifier \a id and parent widget \a parent.

  Parameter \a flags defines behaviour of datum and set of created
  subwidgets. Default value of this parameter is QDS::All.

  Parameter \a comp specifies the component name which will be used
  when searching the dictionary item.

  \param id datum identifier
  \param parent parent widget
  \param flags datum flags
  \param comp component
*/
QDS_SpinBoxDbl::QDS_SpinBoxDbl( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  \brief Destructor.
*/
QDS_SpinBoxDbl::~QDS_SpinBoxDbl()
{
}

/*!
  \brief Get string from the spin box.
  \return string value
*/
QString QDS_SpinBoxDbl::getString() const
{
  QString res;
  QtxDoubleSpinBox* sb = spinBox();
  if ( sb && !sb->isCleared() )
  {
    /*bool hasFocus = sb->hasFocus();
    if ( hasFocus )
      sb->clearFocus();*/
    
    res = sb->text();
    if ( !sb->suffix().isEmpty() )
      res.remove( res.indexOf( sb->suffix() ), sb->suffix().length() );
    if ( !sb->prefix().isEmpty() )
      res.remove( res.indexOf( sb->prefix() ), sb->prefix().length() );
    
    /*if ( hasFocus )
      sb->setFocus();*/
  }

  return res;
}

/*!
  \brief Set the string value to the spin box widget.
  \param txt string value
*/
void QDS_SpinBoxDbl::setString( const QString& txt )
{
  if ( !spinBox() )
    return;

  spinBox()->setCleared( txt.isEmpty() );
  if ( !txt.isEmpty() )
    spinBox()->setValue( txt.toDouble() );
}

/*!
  \brief Get spin box widget.
  \return internal spin box widget.
*/
QtxDoubleSpinBox* QDS_SpinBoxDbl::spinBox() const
{
  return ::qobject_cast<QtxDoubleSpinBox*>( controlWidget() );
}

/*!
  \brief Create internal spin box as control widget.
  \param parent parent widget
  \return created spin box widget
*/
QWidget* QDS_SpinBoxDbl::createControl( QWidget* parent )
{
  QtxDoubleSpinBox* aSpinBox = new QtxDoubleSpinBox( parent );
  aSpinBox->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( aSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( onValueChanged( double ) ) );
  return aSpinBox;
}

/*!
  \brief Called when value in the spin box is changed.

  Emit signals to notify  about value changing in the spin box.

  \param val current spin box value
*/
void QDS_SpinBoxDbl::onValueChanged( double )
{
  onParamChanged();
  QString str = getString();

  emit paramChanged();
  emit paramChanged( str );
}

/*!
  \brief Get the spin box increment value.
  \return current increment value
*/
double QDS_SpinBoxDbl::step() const
{
  double s = 0;
  if ( spinBox() )
    s = spinBox()->singleStep();
  return s;
}

/*!
  \brief Set the spin box increment value.
  \param step new increment value
*/
void QDS_SpinBoxDbl::setStep( const double step )
{
  if ( spinBox() )
    spinBox()->setSingleStep( step );
}

/*!
  \brief Process notification about active units system changing.

  Update spin box contents according to the data dictionary item properties: suffix, prefix, minimum, maximum
  
  \param system new active units system
*/
void QDS_SpinBoxDbl::unitSystemChanged( const QString& system )
{
  QDS_Datum::unitSystemChanged( system );

  QtxDoubleSpinBox* sb = spinBox();
  if ( !sb )
    return;

  // not porting this code to qt4, only commented, since from the task context
  // the new setted validator accepts any double
  //delete sb->validator();
  //QValidator* valid = validator();
  //sb->setValidator( valid );

  sb->setSuffix( suffix() );
  sb->setPrefix( prefix() );

  int aPreci = 1;
  Handle(DDS_DicItem) aDicItem = dicItem();
  if ( !aDicItem.IsNull() )
    aPreci = aDicItem->GetPrecision();

  sb->setDecimals( aPreci );

  sb->setSingleStep( .1 );
  sb->setMinimum( minValue().isEmpty() ? -1.0e10 : minValue().toDouble() );
  sb->setMaximum( maxValue().isEmpty() ? 1.0e10 : maxValue().toDouble() );
}
