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

#ifndef QDS_DATUM_H
#define QDS_DATUM_H

#include "QDS.h"

#pragma warning(push, 0)
#include <QPointer>
#include <QMap>
#include <QLabel>
#pragma warning(pop)

class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QValidator;

#include <DDS_DicItem.h>

class QWidget;

class asiUI_EXPORT QDS_Datum : public QObject, public QDS
{
  Q_OBJECT

  class Wrapper;

public:
  QDS_Datum( const QString&, QWidget* = 0, const int = All, const QString& = QString() );
  virtual ~QDS_Datum();

  QString                   id() const;
  int                       type() const;
  QString                   label() const;
  QString                   units() const;
  QString                   filter() const;
  QString                   format() const;
  QString                   longDescription() const;
  QString                   shortDescription() const;

  QString                   defaultValue() const;
  QString                   minimumValue() const;
  QString                   maximumValue() const;

  QStringList               options() const;
  QVariant                  option( const QString& ) const;
  QString                   optionString( const QString& ) const;
  double                    optionDouble( const QString& ) const;
  int                       optionInteger( const QString& ) const;

  virtual QVariant          value() const;

  virtual QString           stringValue() const;
  virtual double            doubleValue() const;
  virtual int               integerValue() const;

  QString                   text() const;

  virtual bool              isEmpty() const;

  virtual void              reset();
  virtual void              clear();

  virtual void              setValue( const QVariant& );

  virtual void              setStringValue( const QString& );
  virtual void              setDoubleValue( const double );
  virtual void              setIntegerValue( const int );

  virtual bool              isEnabled( const int = Control ) const;
  virtual void              setEnabled( const bool, const int );

  void                      show( const int = All );
  void                      hide( const int = All );
  virtual void              setShown( const bool, const int = All );

  QWidget*                  widget( const int ) const;
  void                      setFocus();

  virtual bool              isValid( const bool = true, 
                                     const QString& = QString(),
                                     const QString& = QString() ) const;
  virtual QValidator*       validator( const bool = false ) const;

  void                      addTo( QVBoxLayout* );
  void                      addTo( QHBoxLayout* );
  void                      addTo( QGridLayout*, const int, const int, const bool = false );

  virtual void              setAlignment( const int, const int = Label );

  static QString            unitsToText( const QString& );
  static QString            textToUnits( const QString& );

  static QString            format( const QString&, const QString&, const bool = false );
  static QString            format( const int, const QString&, const bool = false );
  static QString            format( const double, const QString&, const bool = false );
  static QString            units( const QString& );

  virtual QString           getString() const = 0;

  virtual bool              eventFilter( QObject*, QEvent* );

  operator QWidget*() const;

  bool isCustomTr() const;
  void enableCustomTr( const bool );

signals:
  void                      paramChanged();
  void                      paramChanged( QString& );

public slots:
  void                      setEnabled( bool );

protected slots:
  virtual void              onParamChanged();

private slots:
  void                      onInitDatum();
  void                      onDestroyed( QObject* );

protected:
  QLabel*                   labelWidget() const;
  QLabel*                   unitsWidget() const;
  QWidget*                  controlWidget() const;

  Handle(DDS_DicItem)       dicItem() const;
  void                      setDicItem( const Handle(DDS_DicItem)& );

  int                       flags() const;

  void                      invalidateCache();

  virtual QLabel*           createLabel( QWidget* );
  virtual QLabel*           createUnits( QWidget* );
  virtual QWidget*          createControl( QWidget* ) = 0;

  virtual void              setString( const QString& ) = 0;

  virtual bool              validate( const QString& ) const;

  QString                   prefix() const;
  QString                   suffix() const;
  virtual QString           minValue() const;
  virtual QString           maxValue() const;

  static QString            format( const QString&, const int, const int );
  static QString            format( const QString&, const int, const double );
  static QString            format( const QString&, const int, const QString& );

  static QString            sprintf( const QString&, const int );
  static QString            sprintf( const QString&, const double );
  static QString            sprintf( const QString&, const QString& );

  virtual void              unitSystemChanged( const QString& );

  void                      initDatum() const;

  virtual QString           formatValue( const int ) const;
  virtual QString           formatValue( const double ) const;
  virtual QString           formatValue( const QString& ) const;

private:
  void                      initialize();

  Wrapper*                  wrapper( QWidget* ) const;
  Wrapper*                  wrapper( const int ) const;
  int                       wrapperType( Wrapper* ) const;

  static QString            removeAccel( const QString& );
  static bool               isDoubleFormat( const QString& );
  static QString            canonicalFormat( const QString& );
  static QString            canonicalFormat( const QString&, QString& );

private:
  typedef QPointer<QLabel>  GuardedLabel;
  typedef QPointer<QWidget> GuardedWidget;

private:
  QString                   myId;
  int                       myFlags;
  Handle(DDS_DicItem)       myDicItem;
  QMap<int, Wrapper*>       myWrapper;

  GuardedLabel              myLabel;
  GuardedLabel              myUnits;
  GuardedWidget             myControl;

  QString                   mySourceValue;
  QString                   myTargetValue;

  bool                      myInitialised, myTr;

  friend class QDS;
};

#endif 
