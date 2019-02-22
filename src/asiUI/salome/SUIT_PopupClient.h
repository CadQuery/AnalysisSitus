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

#ifndef SUIT_POPUPCLIENT_H
#define SUIT_POPUPCLIENT_H

#include <asiUI.h>

#pragma warning(push, 0)
#include <QObject>
#pragma warning(pop)

class QMenu;
class QContextMenuEvent;

/*!\class SUIT_PopupClient
 * Descr: Base class for instances which creates popup menu on QContextMenuEvent
 */
class asiUI_EXPORT SUIT_PopupClient
{
public:

  class Signal;

  SUIT_PopupClient();
  virtual ~SUIT_PopupClient();

  bool            connectPopupRequest( QObject* reciever, const char* slot );
  bool            disconnectPopupRequest( QObject* reciever, const char* slot );

  virtual QString popupClientType() const = 0;
  virtual void    contextMenuPopup( QMenu* ) {}

protected:
  void    contextMenuRequest( QContextMenuEvent* e );

private:
  Signal* mySignal;
};

/*! Class: SUIT_PopupClient::Signal [internal] \n
 *  Descr: invoke signal which is connected to reciever in SUIT_PopupClient
 */
class SUIT_PopupClient::Signal : public QObject
{
  Q_OBJECT

public:
  Signal();
  virtual ~Signal();

  void    sendSignal( SUIT_PopupClient*, QContextMenuEvent* );

signals:
  void contextMenuRequest( SUIT_PopupClient*, QContextMenuEvent* );
};

#endif
