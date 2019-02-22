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

#include "QDS_ComboBox.h"

#include <QtxComboBox.h>

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>

#pragma warning(push, 0)
#include <QLineEdit>
#pragma warning(pop)

/*
  \class QDS_ComboBox
  
  \brief Datum with control corresponding to the combo box. 

  This control is used for datum with enumerable values.
  It can be used for datum which has type of value 'List'. 

  Each item of the combobox is defined by two properties: integer identifier 
  and string name. All operations on items are performed via identifier.
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
QDS_ComboBox::QDS_ComboBox( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags, comp )
{
}

/*!
  \brief Destructor.
*/
QDS_ComboBox::~QDS_ComboBox()
{
}

/*!
  \brief Check if combo box allows text editing.
  \return \c true if combo box is editable
*/
bool QDS_ComboBox::editable() const
{
  if ( comboBox() && comboBox()->lineEdit() )
    return !comboBox()->lineEdit()->isReadOnly();
  else
    return false;
}

/*!
  \brief Enable/disable text editing.
  \param on if \c true, combo box is made editable
*/
void QDS_ComboBox::setEditable( const bool on )
{
  QComboBox* aCombo = comboBox();
  if ( aCombo )
    aCombo->setEditable( on );
  if ( aCombo && aCombo->lineEdit() )
  {
    aCombo->lineEdit()->setReadOnly( !on );
    aCombo->setValidator(0);
    if ( on )
      aCombo->setValidator( validator() );
  }
}

/*!
  \brief Get number of items in the combo box.

  \param total if \c false, only visible items are taken into account, 
        otherwise get total number of items
  \return number of items
*/
int QDS_ComboBox::count( bool total ) const
{
  if ( total )
    return myValue.count();
  else if ( comboBox() )
    return comboBox()->count();
  else
    return 0;
}

/*!
  \brief Get items identifiers.

  \param ids returned list of items IDs
  \param total if \c false, only visible items are taken into account, 
        otherwise get total number of items
*/
void QDS_ComboBox::values( QList<int>& ids, bool total ) const
{
  ids.clear();
  for ( QIntList::const_iterator it = myDataIds.begin(); it != myDataIds.end(); ++it )
    if ( total || ( myState.contains( *it ) && myState[*it] ) )
      ids.append( *it );
}

/*!
  \brief Get the current item ID as integer value.
  \return current item ID converted to integer
*/
int QDS_ComboBox::integerValue() const
{
  QComboBox* cb = comboBox();
  QString cur = getString();
  if ( cb && cb->count() > 0 && cb->currentIndex() >= 0 )
    cur = cb->itemText( cb->currentIndex() );

  if ( cb && cur == getString() )
    return getId( cb->currentIndex() );
  else
    return getId( getString() );
}

/*!
  \brief Get the current item ID as double value.
  \return current item ID converted to double
*/
double QDS_ComboBox::doubleValue() const
{
  initDatum();

  QComboBox* cb = comboBox();
  QString cur = getString();
  if ( cb && cb->count() > 0 && cb->currentIndex() >= 0 )
    cur = cb->itemText( cb->currentIndex() );

  if ( cb && cur == getString() )
    return getId( cb->currentIndex() );
  else
    return getId( getString() );
}

/*!
  \brief Set the current item acording to the specified \a id.
  \param id item ID
*/
void QDS_ComboBox::setIntegerValue( const int id )
{
  initDatum();

  if ( myValue.contains( id ) )
    setString( myValue[id] );
  else 
    setString( "" );
}

/*!
  \brief Set the current item acording to the specified \a id.
  \overload

  Integer part of \a val is used as new current ID.
  
  \param val item ID
*/
void QDS_ComboBox::setDoubleValue( const double val )
{
  initDatum();

  int id = (int)val;
  if ( myValue.contains( id ) )
    setString( myValue[id] );
  else if ( id == -1 )
    setString( "" );
}

/*!
  \brief Get visibility state of the item specified by \a id.
  \param id item ID
  \return item visibility state
*/
bool QDS_ComboBox::state( const int id ) const
{
  bool state = false;
  if ( myState.contains( id ) )
    state = myState[id];
  return state;
}

/*!
  \brief Set the visibility state of the item specified by \a id.

  If \a id is -1 then specified state will be set to all items.

  If \a append is set to \c true, keep current status for other items,
  otherwise status of other items is cleared.

  \param on new visibility state
  \param id item ID
  \param append if \c true, keep original status for other items
*/
void QDS_ComboBox::setState( const bool on, const int id, const bool append )
{
  QList<int> lst;
  if ( id < 0 )
  {
    for ( IdStateMap::Iterator it = myState.begin(); it != myState.end(); ++it )
      lst.append( it.key() );
  }
  else
    lst.append( id );

  setState( on, lst, append );
}

/*!
  \brief Set the visibility state of items specified by \a ids.

  If \a append is set to \c true, keep current status for other items,
  otherwise status of other items is cleared.

  \param on new visibility state
  \param ids items IDs list
  \param append if \c true, keep original status for other items
*/
void QDS_ComboBox::setState( const bool on, const QList<int>& ids, const bool append )
{
  initDatum();

  if ( ids.isEmpty() && append )
    return;

  bool changed = false;

  QMap<int, int> aMap;
  for ( int i = 0; i < ids.count(); i++ )
    aMap.insert( ids.at( i ), 0 );

  for ( IdStateMap::Iterator it = myState.begin(); it != myState.end(); ++it )
  {
    if ( aMap.contains( it.key() ) )
    {
      if ( it.value() != on )
      {
        it.value() = on;
        changed = true;
      }
    }
    else if ( !append && it.value() == on )
    {
      it.value() = !on;
      changed = true;
    }
  }
  if ( changed )
    updateComboBox();
}

/*!
  \brief Set the custom user items into the combo box.

  User items like standard dictionary list items will be added
  into the combobox. This function allows user to override
  items.

  \param ids items IDs
  \param names items names
*/
void QDS_ComboBox::setValues( const QList<int>& ids, const QStringList& names )
{
  initDatum();

  if ( ids.count() != names.count() )
    return;

  myUserIds = ids;
  myUserNames = names;

  //unitSystemChanged( "" );
}

/*!
  \brief Set the custom user items into the combo box.
  \overload

  User items like standard dictionary list items will be added
  into the combobox. This function allows user to override
  items.

  Uses (0, 1, 2 ... ) as items IDs.

  \param names items names
*/
void QDS_ComboBox::setValues( const QStringList& names )
{
  initDatum();

  QList< int > ids;
  for ( int i = 0, n = names.count(); i < n; i++ )
    ids.append( i );
  setValues( ids, names );
}

/*!
  \brief Reset the datum.

  Set the active item as item with default ID. If default ID is not defined
  then the first item is used.
*/
void QDS_ComboBox::reset()
{
  QString aDefValue = defaultValue();
  if ( !aDefValue.isEmpty() )
    setString( aDefValue );
  else
    setIntegerValue( getId( 0 ) );
}

/*!
  \brief Convert string to integer value.
  \param str item
  \return item ID or -1 if not found
*/
int QDS_ComboBox::stringToValue( const QString& str ) const
{
  return getId( str );
}

/*!
  \brief Convert integer to string value.
  \param val item ID 
  \return item value or empty string if \c val is invalid
*/
QString QDS_ComboBox::valueToString( const int val ) const
{
  QString str;
  if ( myValue.contains( val ) )
    str = myValue[val];
  return str;
}

/*!
  \brief Get string from the combo box.
  \return string value
*/
QString QDS_ComboBox::getString() const
{
  QString res;
  QtxComboBox* cb = comboBox();
  if ( cb )
  {
    if ( !cb->isEditable() )
    {
      if ( !cb->isCleared() )
        res = cb->currentText(); 
    }
    else
      res = cb->lineEdit()->text();
  }
  return res;
}

/*!
  \brief Set the string value to the combo box widget.
  \param txt string value
*/
void QDS_ComboBox::setString( const QString& txt )
{
  QtxComboBox* cb = comboBox();
  if ( !cb )
    return;

  bool isClear = cb->isCleared();
  
  int idx = -1;
  for ( int i = 0; i < cb->count() && idx == -1; i++ )
    if ( cb->itemText( i ) == txt )
      idx = i;

  int old = cb->currentIndex();
  if ( idx != -1 )
    cb->setCurrentIndex( idx );
  else if ( txt.isEmpty() )
  {
    if ( !cb->isEditable() )
      cb->setCleared( true );
    else
      cb->lineEdit()->setText( txt );
  }
  if ( isClear != txt.isEmpty() || ( !isClear && old != cb->currentIndex() ) || isClear != cb->isCleared() )
  {
    onParamChanged();
    QString str = getString();
    emit activated( integerValue() );
    emit activated( str );
    emit paramChanged();
    emit paramChanged( str );
  }
}

/*!
  \brief Get combo box widget.
  \return internal combo box widget
*/
QtxComboBox* QDS_ComboBox::comboBox() const
{
  return ::qobject_cast<QtxComboBox*>( controlWidget() );
}

/*!
  \brief Create internal combo box as control widget.
  \param parent parent widget
  \return created combo box widget
*/
QWidget* QDS_ComboBox::createControl( QWidget* parent )
{
  QtxComboBox* cb = new QtxComboBox( parent );
  cb->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
  connect( cb, SIGNAL( editTextChanged( const QString& ) ), this,
           SLOT( onTextChanged( const QString& ) ) );
  connect( cb, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
  return cb;
}

/*!
  \brief Process notification about active units system changing.

  Update combobox contents.
  
  \param system new active units system
*/
void QDS_ComboBox::unitSystemChanged( const QString& system )
{
  QDS_Datum::unitSystemChanged( system );

  Handle(TColStd_HArray1OfInteger) anIds;
  Handle(TColStd_HArray1OfExtendedString) aValues, anIcons;

  Handle(DDS_DicItem) aDicItem = dicItem();
  if ( !aDicItem.IsNull() )
    aDicItem->GetListOfValues( aValues, anIds, anIcons );

  myValue.clear();
  myIcons.clear();
  myDataIds.clear();

  QMap<int, QString> userMap;
  QIntList::iterator iIt = myUserIds.begin();
  QStringList::iterator sIt = myUserNames.begin();
  for ( ; iIt != myUserIds.end() && sIt != myUserNames.end(); ++iIt, ++sIt )
    userMap.insert( *iIt, *sIt );

  if ( !anIds.IsNull() && !aValues.IsNull() &&
       anIds->Length() == aValues->Length() )
  {
    for ( int i = anIds->Lower(); i <= anIds->Upper(); i++ )
    {
      QString aValue;
      QPixmap aPixmap;
      int id = anIds->Value( i );
      if ( userMap.contains( id  ) )
        aValue = userMap[id];
      else
      {
        aValue = toQString( aValues->Value( i ) );
        if ( !anIcons.IsNull() && i <= anIcons->Upper() )
        {
          QString anIconId = toQString( anIcons->Value( i ) );
          if ( anIconId != "" )
            aPixmap = QPixmap( anIconId );
        }
      }

      myDataIds.append( id );
      myValue.insert( id, aValue );
      myState.insert( id, true );
      if ( !aPixmap.isNull() )
        myIcons.insert( id, aPixmap );
    }
  }

  for ( iIt = myUserIds.begin(); iIt != myUserIds.end(); ++iIt )
  {
    int id = *iIt;
    if ( !myValue.contains( id  ) )
    {
      myDataIds.append( id );
      myValue.insert( id, userMap[id] );
    }
  }

  QIntList del, add;
  for ( IdStateMap::Iterator it1 = myState.begin(); it1 != myState.end(); ++it1 )
    if ( !myValue.contains( it1.key() ) )
      del.append( it1.key() );

  for ( IdValueMap::Iterator it2 = myValue.begin(); it2 != myValue.end(); ++it2 )
    if ( !myState.contains( it2.key() ) )
      add.append( it2.key() );

  for ( QIntList::iterator iter1 = del.begin(); iter1 != del.end(); ++iter1 )
    myState.remove( *iter1 );

  for ( QIntList::iterator iter2 = add.begin(); iter2 != add.end(); ++iter2 )
    myState.insert( *iter2, true );

  updateComboBox();
}

/*!
  \brief Called when text in the combo box (editable) is modified by the user.
  \param txt current text in the combo box editor (not used)
*/
void QDS_ComboBox::onTextChanged( const QString& /*txt*/ )
{
  onParamChanged();
  emit paramChanged();
  QString str = getString();
  emit paramChanged( str );
}

/*!
  \brief Called when combo box item is activated.
  \param idx index of teh item being activated
*/
void QDS_ComboBox::onActivated( int idx )
{
  if ( comboBox() )
  {
    int ind = comboBox()->currentIndex();
    comboBox()->setCurrentIndex( -1 );
    comboBox()->setCurrentIndex( ind );
  }

  int id = getId( idx );
  if ( id != -1 )
  {
    onParamChanged();
    QString str = getString();
    emit activated( id );
    emit activated( str );
    emit paramChanged();
    emit paramChanged( str );
  }
}

/*!
  \brief Update combo box.
*/
void QDS_ComboBox::updateComboBox()
{
  QtxComboBox* cb = comboBox();

  int curId = -1;

  bool isClear = false;

  if ( cb )
  {
    isClear = cb->isCleared();

    curId = getId( cb->currentIndex() );
    cb->clear();
  }

  myIndex.clear();

  int idx = 0;
  for ( QIntList::const_iterator it = myDataIds.begin(); it != myDataIds.end(); ++it )
  {
    int id = *it;
    if ( !myValue.contains( id ) || !myState.contains( id ) || !myState[id] )
      continue;

    myIndex.insert( id, idx++ );
    if ( cb )
    {
      if ( myIcons.contains( id ) )
        cb->addItem( QIcon(myIcons[id]), myValue[id] );
      else
        cb->addItem( myValue[id] );
    }
  }

  if ( cb && cb->count() )
  {
    cb->setFont( cb->font() );
    cb->updateGeometry();

    if ( isClear )
      cb->setItemText( cb->currentIndex(), "" );
    else
    {
      if ( getIndex( curId ) != -1 )
        cb->setCurrentIndex( getIndex( curId ) );
      if ( curId != getId( cb->currentIndex() ) )
        onActivated( cb->currentIndex() );
    }
  }
}

/*!
  \brief Get index of the combo box item according to its identifier.
  \param id item ID
  \return item index or -1 if not found
*/
int QDS_ComboBox::getIndex( const int id ) const
{
  int idx = -1;
  if ( myIndex.contains( id ) )
    idx = myIndex[id];
  return idx;
}

/*!
  \brief Get index of the combo box item.
  \param str combo box item
  \return item index or -1 if not found
*/
int QDS_ComboBox::getIndex( const QString& str ) const
{
  int idx = -1;
  QComboBox* cb = comboBox();
  if ( cb )
  {
    for ( int i = 0; i < cb->count() && idx == -1; i++ )
      if ( cb->itemText( i ) == str )
        idx = i;
  }
  return idx;
}

/*!
  \brief Get item identifier according to the specified index.
  \param idx item index
  \return item ID or -1 if index is out of range
*/
int QDS_ComboBox::getId( const int idx ) const
{
  int id = -1;
  IdIndexMap::ConstIterator it = myIndex.begin();
  for (; it != myIndex.end() && id == -1; ++it )
    if ( it.value() == idx )
      id = it.key();
  return id;
}

/*!
  \brief Get item identifier.
  \param str combo box item
  \return item ID or -1 if not found
*/
int QDS_ComboBox::getId( const QString& str ) const
{
  int id = -1;
  int candidate = -1;
  IdValueMap::ConstIterator it = myValue.begin();
  for (; it != myValue.end() && id == -1; ++it )
  {
    if ( it.value() == str )
    {
      if ( state( it.key() ) )
        id = it.key();
      else
        candidate = it.key();
    }
  }
  if ( id == -1 )
    id = candidate;

  return id;
}

/*!
  \fn void QDS_ComboBox::activated( int id );
  \brief Emitted when the current item in the combo box is changed.
  \param id current item ID
*/

/*!
  \fn void QDS_ComboBox::activated( const QString& txt );
  \brief Emitted when the current item in the combo box is changed.
  \param txt current item text
*/
