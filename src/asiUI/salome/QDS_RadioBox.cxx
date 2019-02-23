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

#include "QDS_RadioBox.h"

#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>

#pragma warning(push, 0)
#include <QButtonGroup>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QRadioButton>
#pragma warning(pop)

/*
  \class QDS_RadioBox
  \brief Datum with control corresponding to button group with set of exclusive radio buttons.

  This control used for datum with enumerable values. It can be used for datum which has
  type of value 'List'. Each radio button of group box is defined by two properties:
  integer identifier and string name. All operations on radio buttons are performed via identifier.

  If datum label text is specified, then it is displayed in the group box title.
*/

/*!
  \brief Constructor. 

  Create radio button box datum object with datum identifier \a id and parent widget \a parent.
  
  Parameter \a flags defines behaviour of datum and set of created
  subwidgets. Default value of this parameter is QDS::All. 
  
  Parameter \a comp specifies the component name which will be used
  when searching the dictionary item.

  \param id datum identifier
  \param parent parent widget
  \param flags datum flags
  \param comp component
*/
QDS_RadioBox::QDS_RadioBox( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QDS_Datum( id, parent, flags & ~( Label | Units ), comp ),
  myButtonGroup( 0 )
{
}

/*!
  \brief Destructor.
*/
QDS_RadioBox::~QDS_RadioBox()
{
}

/*!
  \brief Get number of buttons in radio box.

  If \a total is \c false, only visible buttons are taken into account; 
  otherwise total number of buttons is returned

  \param total get number of visible buttons if \c true and total number of buttons if \c false
  \return requested number of items
*/
int QDS_RadioBox::count( bool total ) const
{
  if ( total )
    return myValue.count();
  else
  {
    QList<QRadioButton*> bList;
    buttons( bList );
    return bList.count();
  }
}

/*!
  \brief Get buttons identifiers.
  \param ids returned list of buttons IDs
  \param total take into account only visible buttons if \c true and all buttons if \c false
*/
void QDS_RadioBox::values( QList<int>& ids, bool total ) const
{
  ids.clear();
  for ( QIntList::const_iterator it = myDataIds.begin(); it != myDataIds.end(); ++it )
    if ( total || ( myState.contains( *it ) && myState[*it] ) )
      ids.append( *it );
}

/*!
  \brief Get visibility state of the button specified by \a id.
  \param id button ID
  \return item visibility state
*/
bool QDS_RadioBox::state( const int id ) const
{
  bool state = false;
  if ( myState.contains( id ) )
    state = myState[id];
  return state;
}

/*!
  \brief Set the visibility state of the button specified by \a id.

  If \a id is -1 then specified state will be set to all buttons.

  If \a append is set to \c true, keep current status for other buttons,
  otherwise status of other buttons is cleared.

  \param on new visibility state
  \param id button ID
  \param append if \c true, keep original status for other buttons
*/
void QDS_RadioBox::setState( const bool on, const int id, const bool append )
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
  \brief Set the visibility state of buttons specified by \a ids.

  If \a append is set to \c true, keep the current status for other buttons,
  otherwise status of other buttons is cleared.

  \param on new visibility state
  \param ids buttons IDs list
  \param append if \c true, keep original status for other buttons
*/
void QDS_RadioBox::setState( const bool on, const QList<int>& ids, const bool append )
{
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
    updateRadioBox();
}

/*!
  \brief Set the custom user buttons into the radio box.

  User items like standard dictionary buttons will be added
  into the radio box. This function allows user to customize
  buttons.

  \param ids buttons IDs
  \param names buttons names
*/
void QDS_RadioBox::setValues( const QList<int>& ids, const QStringList& names )
{
  if ( ids.count() != names.count() )
    return;

  myUserIds = ids;
  myUserNames = names;
}

/*!
  \brief Set the custom user buttons into the radio box.
  \overload

  User buttons like standard dictionary buttons will be added
  into the radio box. This function allows user to customize
  buttons.

  Uses (0, 1, 2 ... ) as buttons IDs.

  \param names buttons names
*/
void QDS_RadioBox::setValues( const QStringList& names )
{
  QList< int > ids;
  for ( int i = 0, n = names.count(); i < n; i++ )
    ids.append( i );
  setValues( ids, names );
}

/*!
  \brief Get string from the radio box.

  String which contains identifier of the currently selected button is returned.
  \return identifier of the current button converted to string
*/
QString QDS_RadioBox::getString() const
{
  QString res;
  QButtonGroup* bg = buttonGroup();
  if ( bg )
  {
    int id = bg->checkedId();
    if ( id != -1 )
      res = QString::number( id );
  }
  return res;
}

/*!
  \brief Set the string value to the radio box widget.

  Button with the identifier from specified string \a txt becomes selected in the radio box.

  \param txt string value
*/
void QDS_RadioBox::setString( const QString& txt )
{
  QButtonGroup* bg = buttonGroup();
  if ( !bg )
    return;

  int oldId = bg->checkedId();

  if ( txt.isEmpty() )
  {
    QList<QRadioButton*> bList;
    buttons( bList );
    QListIterator<QRadioButton*> it( bList );
    while ( it.hasNext() )
      it.next()->setChecked( false );
  }
  else
  {
    bool ok;
    int id = txt.toInt( &ok );
    if ( !ok )
      id = -1;

    bool block = signalsBlocked();
    blockSignals( true );
    bg->button(id)->setChecked(true);
    blockSignals( block );
  }

  int newId = bg->checkedId();

  if ( oldId != newId )
  {
    onParamChanged();
    QString str = getString();
    emit activated( newId );
    emit paramChanged();
    emit paramChanged( str );
  }
}

/*!
  \brief Get internal button group.
  \return pointer to the QButtonGroup object
*/
QButtonGroup* QDS_RadioBox::buttonGroup() const
{
  return myButtonGroup;
}

/*!
  \brief Get internal group box widget.
  \return pointer to the QGroupBox widget
*/
QGroupBox* QDS_RadioBox::groupBox() const
{
  return ::qobject_cast<QGroupBox*>( controlWidget() );
}

/*!
  \brief Get radio button group box widget.
  \return internal group box widget
*/
QWidget* QDS_RadioBox::createControl( QWidget* parent )
{
  myButtonGroup = new QButtonGroup( parent );
  myButtonGroup->setExclusive( true );

  QGroupBox *gb = new QGroupBox( "", parent );
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addStretch(1);
  gb->setLayout(vbox);
  return gb;
}

/*!
  \brief Process notification about active units system changing.

  Update radio box contents.
  
  \param system new active units system
*/
void QDS_RadioBox::unitSystemChanged( const QString& system )
{
  QDS_Datum::unitSystemChanged( system );

  Handle(TColStd_HArray1OfInteger) anIds;
  Handle(TColStd_HArray1OfExtendedString) aValues, anIcons;

  Handle(DDS_DicItem) aDicItem = dicItem();
  if ( !aDicItem.IsNull() )
    aDicItem->GetListOfValues( aValues, anIds, anIcons );

  myValue.clear();
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
      int id = anIds->Value( i );
      if ( userMap.contains( id  ) )
        aValue = userMap[id];
      else
        aValue = toQString( aValues->Value( i ) );

      myDataIds.append( id );
      myValue.insert( id, aValue );
      myState.insert( id, true );
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

  QGroupBox* gb = groupBox();
  if ( gb ) gb->setTitle( label() );

  updateRadioBox();
}

/*!
  \brief Called when user toggles any radio button.
  \param on new radio button state
*/
void QDS_RadioBox::onToggled( bool on )
{
  if ( !on )
    return;

  onParamChanged();
  emit paramChanged();
  QString str = getString();
  emit paramChanged( str );
}

/*!
  \brief Update radio box.
*/
void QDS_RadioBox::updateRadioBox()
{
  QButtonGroup* bg = buttonGroup();
  if ( !bg )
    return;

  QGroupBox* gb = groupBox();
  if ( !gb )
    return;

  int curId = bg->checkedId();

  QList<QRadioButton*> bList1;
  buttons( bList1 );
  QListIterator<QRadioButton*> itr( bList1 );
  while ( itr.hasNext() ) {
    QRadioButton* aButton = itr.next();
    if ( gb->layout() ) gb->layout()->removeWidget(aButton);
    delete aButton;
  }

  for ( QIntList::const_iterator it = myDataIds.begin(); it != myDataIds.end(); ++it )
  {
    int id = *it;
    if ( !myValue.contains( id ) || !myState.contains( id ) || !myState[id] )
      continue;

    QRadioButton* rb = new QRadioButton( myValue[id] );
    ((QObject*)rb)->setParent( bg );
    bg->addButton( rb, id );
    if ( gb->layout() ) gb->layout()->addWidget(rb);

    connect( rb, SIGNAL( toggled( bool ) ), this, SLOT( onToggled( bool ) ) );
  }

  if ( curId != -1 )
  {
    int id = curId;
    if ( !bg->button( id ) )
    {
      QList<QRadioButton*> bList;
      buttons( bList );
      if ( !bList.isEmpty() )
        id = bg->id( bList.empty() ? 0 : bList.first() );
    }

    bool block = signalsBlocked();
    blockSignals( true );
    bg->button(id)->setChecked(true);
    blockSignals( block );
  }

  if ( curId != bg->checkedId() )
  {
    onParamChanged();
    QString str = getString();
    emit paramChanged();
    emit paramChanged( str );
  }
}

/*!
  \brief Get all the radio buttons from the radio box.
  \param lst returned list of radio buttons
*/
void QDS_RadioBox::buttons( QList<QRadioButton*>& lst ) const
{
  lst.clear();

  QButtonGroup* bg = buttonGroup();
  if ( !bg )
    return;

  QList<QRadioButton*> objs = bg->findChildren<QRadioButton*>();
  QListIterator<QRadioButton*> it( objs );
  while ( it.hasNext() )
    lst.append( it.next() );
}

/*!
  \fn void QDS_RadioBox::activated( int id );
  \brief The signal is emitted when any radio button is toggled.
  \param id button ID
*/
