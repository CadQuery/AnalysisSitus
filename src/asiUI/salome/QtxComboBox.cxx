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

// File:      QtxComboBox.cxx
// Author:    Sergey TELKOV
//
#include "QtxComboBox.h"

#pragma warning(push, 0)
#include <QStandardItemModel>
#include <QLineEdit>
#include <QEvent>
#include <QApplication>
#pragma warning(pop)

/*!
  \class QtxComboBox::Model
  \brief Internal view model, used to process 'cleared' state of the combo box.
  \internal
*/
class QtxComboBox::Model : public QStandardItemModel
{
public:
  Model( QObject* parent = 0 );
  ~Model();
  void setCleared( const bool );

  QVariant data( const QModelIndex&, int = Qt::DisplayRole ) const;

private:
  bool   myCleared;
};

/*!
  \brief Constructor
  \internal
  \param parent parent object
*/
QtxComboBox::Model::Model( QObject* parent )
  : QStandardItemModel( 0, 1, parent ),
    myCleared( false )
{
}

/*!
  \brief Destructor
  \internal
*/
QtxComboBox::Model::~Model()
{
}

/*!
  \brief Set 'cleared' state
  \param isClear new 'cleared' state
  \internal
*/
void QtxComboBox::Model::setCleared( const bool isClear )
{
  if ( myCleared == isClear )
    return;
  
  myCleared = isClear;
}

/*!
  \brief Get model data.
  \param index model index
  \param role data role
  \return data of role \a role for the \a index
  \internal
*/
QVariant QtxComboBox::Model::data( const QModelIndex& index, int role ) const
{
  return ( myCleared && ( role == Qt::DisplayRole || role == Qt::DecorationRole ) ) ?
    QVariant() : QStandardItemModel::data( index, role );
}

/*!
  \class QtxComboBox::ClearEvent
  \brief Custom event, used to process 'cleared' state of the combo box
  in the editable mode.
  \internal
*/

#define CLEAR_EVENT QEvent::Type( QEvent::User + 123 )

class QtxComboBox::ClearEvent : public QEvent
{
public:
  ClearEvent();
};

/*!
  \brief Constructor
  \internal
*/
QtxComboBox::ClearEvent::ClearEvent() : QEvent( CLEAR_EVENT )
{
}

/*!
  \class QtxComboBox
  \brief Enhanced version of Qt combo box class.

  In addition to the QComboBox class, QtxComboBox supports 
  adding/removing the items with the associated unique identifiers.
  It also provides a way to set "cleared" state to the combo box -
  when no item is selected.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxComboBox::QtxComboBox( QWidget* parent )
: QComboBox( parent ),
  myCleared( false )
{
  connect( this, SIGNAL( currentIndexChanged( int ) ),  this, SLOT( onCurrentChanged( int ) ) );
  setModel( new Model( this ) );
}

/*!
  \brief Destructor.

  Does nothing currently.
*/
QtxComboBox::~QtxComboBox()
{
}

/*!
  \brief Check if the combo box is in the "cleared" state.
  \return \c true if combobox is in the "cleared" state
*/
bool QtxComboBox::isCleared() const
{
  return myCleared;
}

/*!
  \brief Set "cleared" state.
  \param isClear new "cleared" state
*/
void QtxComboBox::setCleared( const bool isClear )
{
  if ( myCleared == isClear )
    return;
    
  myCleared = isClear;

  if ( lineEdit() )
    lineEdit()->setText( myCleared ? QString( "" ) : itemText( currentIndex() ) );

  update();
}

/*!
  \brief Get current item ID.
  \return item id
*/
int QtxComboBox::currentId() const
{
  return id( currentIndex() );
}

/*!
  \brief Set current item by ID.
  \param num item ID
*/
void QtxComboBox::setCurrentId( int num )
{
  setCurrentIndex( index( num ) );
}

/*!
  \brief Set the identifier to specified item.
  \param index - index of the item
  \param id - identifier of the item
*/
void QtxComboBox::setId( const int index, const int id )
{
  setItemData( index, QVariant( id ), (Qt::ItemDataRole)IdRole );
}

/*!
  \brief Customize paint event.
  \param e paint event
*/
void QtxComboBox::paintEvent( QPaintEvent* e )
{
  Model* m = dynamic_cast<Model*>( model() );
  if ( m )
    m->setCleared( myCleared );
  QComboBox::paintEvent( e );
  if ( m )
    m->setCleared( false );
}

/*!
  \brief Customize child addition/removal event
  \param e child event
*/
void QtxComboBox::childEvent( QChildEvent* e )
{
  if ( ( e->added() || e->polished() ) && qobject_cast<QLineEdit*>( e->child() ) )
    QApplication::postEvent( this, new ClearEvent() );
}

/*!
  \brief Process custom events
  \param e custom event
*/
void QtxComboBox::customEvent( QEvent* e )
{
  if ( e->type() == CLEAR_EVENT && lineEdit() && myCleared )
    lineEdit()->setText( "" );
}

/*!
  \brief Called when current item is chaned (by the user or programmatically).
  \param idx item being set current
*/
void QtxComboBox::onCurrentChanged( int idx )
{
  if ( idx != -1 )
  {
    resetClear();
    emit activatedId( id( idx ) );
  }
}

/*!
  \brief Reset "cleared" state and update the combo box.
*/
void QtxComboBox::resetClear()
{
  if ( !myCleared )
    return;
  
  myCleared = false;
  update();
}

/*!
  \brief Get item ID by the index.
  \param idx item index
  \return item ID or -1 if index is invalid.
*/
int QtxComboBox::id( const int idx ) const
{
  int id = -1;
  QVariant v = itemData( idx, (Qt::ItemDataRole)IdRole );
  if ( v.canConvert( QVariant::Int ) )
    id = v.toInt();
  return id;
}

/*!
  \brief Get item index by the ID.
  \param id item ID
  \return item index or -1 if ID is invalid.
*/
int QtxComboBox::index( const int ident ) const
{
  int idx = -1;
  for ( int i = 0; i < (int)count() && idx == -1; i++ )
  {
    if ( id( i ) == ident )
      idx = i;
  }
  return idx;
}

/*!
  \brief Returns true if the item with index has ID.
  \param idx item index
*/
bool QtxComboBox::hasId( const int idx ) const
{
  QVariant v = itemData( idx, (Qt::ItemDataRole)IdRole );
  return v.canConvert( QVariant::Int );
}

/*!
  \fn void QtxComboBox::activatedId( int id )
  \brief Emitted when the item with identificator \a id is activated.
  \param id item ID
*/
