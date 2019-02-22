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

// File:      QtxIntSpinBox.h
// Author:    Sergey TELKOV
//
#ifndef QTXINTSPINBOX_H
#define QTXINTSPINBOX_H

#include "Qtx.h"

#pragma warning(push, 0)
#include <QSpinBox>
#pragma warning(pop)

class asiUI_EXPORT QtxIntSpinBox : public QSpinBox
{
  Q_OBJECT

public:
  QtxIntSpinBox( QWidget* = 0 );
  QtxIntSpinBox( int, int, int = 1, QWidget* = 0 );
  virtual ~QtxIntSpinBox();
    
  bool            isCleared() const;
  virtual void    setCleared( const bool );
    
  virtual void    stepBy( int );
  
protected slots:
  virtual void    onTextChanged( const QString& );
  
protected:
  virtual QString textFromValue( int ) const;
    
private:
  bool            myCleared;
};

#endif
