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

#include "QDS_Datum.h"

#pragma warning(push, 0)
#include <QLayout>
#include <QVariant>
#include <QTimer>
#include <QEvent>
#include <QMessageBox>
#include <QApplication>
#pragma warning(pop)

#include "QDS_Validator.h"
#include <DDS_Dictionary.h>

#pragma warning(disable : 4714 4996)

/*!
  \class QDS_Datum::Wrapper
  \internal
  \brief Wrapper for sub widgets.
*/

class QDS_Datum::Wrapper : public QWidget
{
public:
  Wrapper( QWidget* = 0 );
  virtual ~Wrapper();

  QWidget* widget() const;
  void     setWidget( QWidget* );

  virtual void  setGeometry( int x, int y, int w, int h );
  virtual void  setSizePolicy( QSizePolicy );

private:
  QWidget* myWid;
};

/*!
  \brief Constructor.
  \param parent parent widget
*/
QDS_Datum::Wrapper::Wrapper( QWidget* parent )
: QWidget( parent ),
  myWid( 0 )
{
  setLayout( new QHBoxLayout() );
  layout()->setSpacing( 0 );
  layout()->setMargin( 0 );
  setFocusPolicy( Qt::StrongFocus );
}

/*!
  \brief Destructor.
*/
QDS_Datum::Wrapper::~Wrapper()
{
}

/*!
  \brief Get widget.
  \return widget
*/
QWidget* QDS_Datum::Wrapper::widget() const
{
  return myWid;
}

/*!
  \brief Set widget.
  \param wid widget
*/
void QDS_Datum::Wrapper::setWidget( QWidget* wid )
{
  if ( myWid == wid || !wid )
    return;

  wid->setParent( this );
  QHBoxLayout* hl = qobject_cast<QHBoxLayout*>( layout() );
  if( myWid )
    hl->removeWidget( myWid );
  hl->addWidget( wid );
  myWid = wid;

  setTabOrder( this, myWid );
  setFocusProxy( myWid );

  myWid->updateGeometry();
  updateGeometry();
}

/*!
  \brief Set size policy of the widget.
  \param sp new size policy
*/
void QDS_Datum::Wrapper::setSizePolicy( QSizePolicy sp )
{
  QWidget::setSizePolicy( sp );

  if ( widget() )
    widget()->setSizePolicy( sp );
}

/*!
  \brief Set widget geometry.
  \param x horizontal position
  \param y vertical position
  \param w widget width
  \param h widget height
*/
void QDS_Datum::Wrapper::setGeometry( int x, int y, int w, int h )
{
  QWidget::setGeometry( x, y, w, h );

  if ( widget() && widget()->size() != size() )
    widget()->setGeometry( 0, 0, width(), height() );
}

/*!
  \class QDS_Datum
  \brief Base class for all controls using the data dictionary. 

  Datum is successor of QObject (not QWidget). This object can have up to three
  sub widgets named as QDS::Label, QDS::Control and QDS::Units. 
  User can prevent creation of any of them by setting corresponding subwidgets IDs
  in the parameter \a flags of the constructor.
  QDS::Label widget displays label of datum, QDS::Control widget allows 
  entering the value and QDS::Units widget displays measure units in the active 
  units system.
  
  These widgets are constructed under the parent widget of the datum. 
  The sub widgets are layouted in the parent widget in the following order: first widget
  is QDS::Label, it is followed by QDS::Control, and in the end is QDS::Units.
  User can add these widgets to layout manually using method addTo().
  Method widget() can be used to retrieve desired widget and place it into layout.

  When QGroupBox is used as parent widget for datum object, all subwidgets are 
  arranged automatically by the group box according to the column and orientation
  properties of the QGroupBox.

  Example:
  \code
  QGroupBox* box = new QGroupBox( 3, Qt::Horizontal, "datum box" );
  QDS_Datum* d1  = new QDS_Datum( "datum_1", box, All );
  QDS_Datum* d2  = new QDS_Datum( "datum_2", box, All );
  QDS_Datum* d3  = new QDS_Datum( "datum_3", box, All );
  \endcode

  In this example we create the QGroupBox with 3 horizontal columns. 
  All the created datum widgets will be placed automatically in 3 rows.
  Datums will be placed from top to bottom one by one and aligned in he grid.

  Datum value is stored as string. User can get/set this value:
  - As string : stringValue()/setStringValue().
  - As integer : integerValue()/setIntegerValue(). Value is converted to/from SI.
  - As double : doubleValue()/setDoubleValue(). Value is converted to/from SI.
  - As variant : value()/setValue().

  User can enable/disable datum subwidgets with setEnabled() method, show/hide them
  with show()/hide() methods, set input focus to datum with setFocus(), set widgets 
  alignment with setAlignment(), etc.
*/

/*!
  \brief Constructor. 

  Create datum object with datum identifier \a id and parent widget \a parent.

  Parameter \a flags defines behaviour of datum and set of created
  subwidgets. Default value of this parameter is QDS::All.

  Parameter \a comp specifies the component name which will be used
  when searching the dictionary item.

  Datum is self-registered in the global list by QDS::insertDatum().

  \param id datum identifier
  \param parent parent widget
  \param flags datum flags
  \param comp component
*/
QDS_Datum::QDS_Datum( const QString& id, QWidget* parent, const int flags, const QString& comp )
: QObject( parent ),
  myId( id ),
  myLabel( 0 ),
  myUnits( 0 ),
  myControl( 0 ),
  myFlags( flags ),
  myInitialised( false ),
  myTr( false )
{
  if ( myFlags & Label )
    myWrapper.insert( Label, new Wrapper( parent ) );
  if ( myFlags & Control )
    myWrapper.insert( Control, new Wrapper( parent ) );
  if ( myFlags & Units )
    myWrapper.insert( Units, new Wrapper( parent ) );

  for ( QMap<int, Wrapper*>::Iterator it = myWrapper.begin(); it != myWrapper.end(); ++it )
    connect( it.value(), SIGNAL( destroyed( QObject* ) ), this, SLOT( onDestroyed( QObject* ) ) );

  Handle(DDS_Dictionary) aDict = DDS_Dictionary::Get();
  if ( aDict.IsNull() )
    return;

  TCollection_AsciiString anId = toAsciiString( id );
  TCollection_AsciiString aComp = toAsciiString( comp );

  if ( aComp.IsEmpty() )
    setDicItem( aDict->GetDicItem( anId ) );
  else
    setDicItem( aDict->GetDicItem( anId, aComp ) );

  QTimer::singleShot( 0, this, SLOT( onInitDatum() ) );

  if ( parent )
    parent->installEventFilter( this );

  insertDatum( this );
}

/*!
  \brief Destructor. 

  Destroy all subwidget. Datum is unregistered from the global list
  by QDS::removeDatum().
*/
QDS_Datum::~QDS_Datum()
{
  removeDatum( this );

  delete myLabel;
  delete myUnits;
  delete myControl;
/*
  for ( QMap<int, Wrapper*>::Iterator it = myWrapper.begin(); it != myWrapper.end(); ++it )
    delete it.data();
*/
}

/*!
  \brief Overloaded operator used to retrieve main subwidget named QDS::Control.
  \return QDS::Control subwidget
*/
QDS_Datum::operator QWidget*() const
{
  return widget( Control );
}

/*!
  \brief Get the datum ID.
  \return datum ID
*/
QString QDS_Datum::id() const
{
  initDatum();

  return myId;
}

/*!
  \brief Get the datum value type.
  \return datum value type
*/
int QDS_Datum::type() const
{
  initDatum();

  int res = DDS_DicItem::Unknown;
  if ( !myDicItem.IsNull() )
    res = myDicItem->GetType();
  return res;
}

/*!
  \brief Return state of custom translation.
  \return true if custom translation is enabled
*/
bool QDS_Datum::isCustomTr() const
{
  return myTr;
}

/*!
  \brief Change state of custom translation.

  Custom translation means that text labels of datums are translated
  with help of standard Qt mechanism of internationalization. In this
  case special records should be placed into *.ts files under context "QDS".
  For example, if label has name "myLabel", the corresponding translation
  should be written:
  <context>
    <name>QDS</name>
    <message>
        <source>myLabel</source>
        <translation>myLabel translation</translation>
    </message>
    ...

  If custom translation mechanism is deactivated, then labels will be shown
  with text got from xml data dictionary file

  By default, the custom translation is deactivated

  \param on - if it is true, custom translation is activated, otherwise, deactivated 
*/
void QDS_Datum::enableCustomTr( const bool on )
{
  myTr = on;
  labelWidget()->setText( label() );
}

/*!
  \brief Get the datum label text.
  \return label text
*/
QString QDS_Datum::label() const
{
  initDatum();

  QString labStr;
  if ( !myDicItem.IsNull() )
  {
    labStr = toQString( myDicItem->GetLabel() );
    if( labStr.isNull() )
      labStr = toQString( myDicItem->GetId() );
  }
  if( myTr && labStr.length()>0 )
  {
    QString dest = QApplication::translate( "QDS", labStr.toLatin1().constData() );
    if( labStr != dest )
      labStr = dest;
  }
  if ( flags() & NotAccel )
    labStr = removeAccel( labStr );
  return labStr;
}

/*!
  \brief Get the datum units text.
  \return units text
*/
QString QDS_Datum::units() const
{
  initDatum();

  QString unitStr;
  if ( !myDicItem.IsNull() )
    unitStr = toQString( myDicItem->GetUnits() );
  return unitStr;
}

/*!
  \brief Get the datum filter string.
  \return filter string
*/
QString QDS_Datum::filter() const
{
  initDatum();

  QString fltr;
  if ( !myDicItem.IsNull() )
    fltr = toQString( myDicItem->GetFilter() );
  return fltr;
}

/*!
  \brief Get the datum format string
  \return format string
*/
QString QDS_Datum::format() const
{
  initDatum();

  QString fmtStr;
  if ( !myDicItem.IsNull() )
    fmtStr = toQString( myDicItem->GetFormat( false ) );
  return fmtStr;
}

/*!
  \brief Get the datum default value
  \return default value
*/
QString QDS_Datum::defaultValue() const
{
  initDatum();

  QString pref = prefix();
  QString suff = suffix();

  QString def;
  if ( !myDicItem.IsNull() )
    def = toQString( myDicItem->GetDefaultValue() );

  QString aDef = def.trimmed();
  if ( !pref.isEmpty() && aDef.left( pref.length() ) == pref )
    aDef = aDef.mid( pref.length() );

  if ( !suff.isEmpty() && aDef.right( suff.length() ) == suff )
    aDef = aDef.mid( 0, aDef.length() - suff.length() );

  return aDef;
}

/*!
  \brief Get the datum minimum value.
  \return minimum value
*/
QString QDS_Datum::minimumValue() const
{
  initDatum();

  QString min;
  if ( !myDicItem.IsNull() && myDicItem->HasData( DDS_DicItem::MinValue ) )
      min = formatValue( (double)myDicItem->GetMinValue() );
  return min;
}

/*!
  \brief Get the datum maximum value.
  \return maximum value
*/
QString QDS_Datum::maximumValue() const
{
  initDatum();

  QString max;
  if ( !myDicItem.IsNull() && myDicItem->HasData( DDS_DicItem::MaxValue ) )
    max = formatValue( myDicItem->GetMaxValue() );
  return max;
}

/*!
  \brief Get the datum long description.
  \return datum long description
*/
QString QDS_Datum::longDescription() const
{
  initDatum();

  QString ldStr;
  if ( !myDicItem.IsNull() )
    ldStr = toQString( myDicItem->GetLongDescription() );
  return ldStr;
}

/*!
  \brief Get the datum short description.
  \return datum short description
*/
QString QDS_Datum::shortDescription() const
{
  initDatum();

  QString sdStr;
  if ( !myDicItem.IsNull() )
    sdStr = toQString( myDicItem->GetLongDescription() );
  return sdStr;
}

/*!
  \brief Get the datum options names.
  \return datum options
*/
QStringList QDS_Datum::options() const
{
  QStringList res;
  if ( !dicItem().IsNull() )
  {
    TColStd_SequenceOfAsciiString lst;
    dicItem()->GetOptionNames( lst );
    for ( int i = 1; i <= lst.Length(); i++ )
      res.append( toQString( lst.Value( i ) ) );
  }
  return res;
}

/*!
  \brief Get datum option specified by \a name as QVariant.
  \return the option value or invalid QVariant if the option does not exist
*/
QVariant QDS_Datum::option( const QString& name ) const
{
  QVariant res;
  if ( !dicItem().IsNull() )
    res = QVariant( toQString( dicItem()->GetOption( toAsciiString( name ) ) ) );
  return res;
}

/*!
  \brief Get datum option specified by \a name as string.
  \return the option value or null QString if the option does not exist
*/
QString QDS_Datum::optionString( const QString& name ) const
{
  QString res;
  if ( !dicItem().IsNull() )
    res = toQString( dicItem()->GetOption( toAsciiString( name ) ) );
  return res;
}

/*!
  \brief Get datum option specified by \a name as double.
  \return the option value or 0 if the option does not exist
*/
double QDS_Datum::optionDouble( const QString& name ) const
{
  double res = 0;
  QVariant opt = option( name );
  if ( opt.isValid() && opt.canConvert( QVariant::Double ) )
    res = opt.toDouble();
  return res;
}

/*!
  \brief Get datum option specified by \a name as integer.
  \return the option value or 0 if the option does not exist
*/
int QDS_Datum::optionInteger( const QString& name ) const
{
  int res = 0;
  QVariant opt = option( name );
  if ( opt.isValid() && opt.canConvert( QVariant::Int ) )
    res = opt.toInt();
  return res;
}

/*!
  \brief Get the datum value as QVariant.
  \return datum value
*/
QVariant QDS_Datum::value() const
{
  QVariant val;
  // trying to return QVariant containing value of correct data type
  if ( dicItem().IsNull() )
  {
    if ( !isEmpty() )
      val = stringValue();
  }
  else
  {
    switch( type() )
    {
      case DDS_DicItem::Float:
        val = doubleValue();
        break;
      case DDS_DicItem::Integer:
      case DDS_DicItem::List:
        val = integerValue();
        break;
      case DDS_DicItem::String:
      case DDS_DicItem::Unknown:
        val = stringValue();
        break;
    }
  }
  return val;
}

/*!
  \brief Get the datum value as string.
  \return datum value converted to string
*/
QString QDS_Datum::stringValue() const
{
  initDatum();

  if ( !myTargetValue.isNull() && myTargetValue == getString() )
    return mySourceValue;
  else
    return getString();
}

/*!
  \brief Get the datum value as double.

  The value is converted according to the measure units in the active units system
  to measure units in "SI" units system.

  \return datum value converted to double
*/
double QDS_Datum::doubleValue() const
{
  initDatum();

  double res = 0;
  if ( !myTargetValue.isNull() && myTargetValue == getString() )
    res = mySourceValue.toDouble();
  else
  {
    res = getString().toDouble();
    if ( !myDicItem.IsNull() && !( flags() & NotConvert ) )
      res = myDicItem->ToSI( res );
  }

  return res;
}

/*!
  \brief Get the datum value as integer.

  The value is converted according to the measure units in the active units system
  to measure units in "SI" units system.

  \return datum value converted to integer
*/
int QDS_Datum::integerValue() const
{
  initDatum();

  int res = 0;
  if ( !myTargetValue.isNull() && myTargetValue == getString() )
    res = mySourceValue.toInt();
  else
  {
    double val = getString().toDouble();
    if ( !myDicItem.IsNull() && !( flags() & NotConvert ) )
      res = (int)myDicItem->ToSI( val );
  }

  return res;
}

/*!
  \brief Get the text data from datum.

  Text consists of label, string value and units.

  \return datum text data
*/
QString QDS_Datum::text() const
{
  initDatum();

  QString aLabel = label();
  QString aData  = stringValue();
  QString aUnits = units();

  QString res = aLabel;
  if ( !res.isEmpty() && !aData.isEmpty() )
    res += QString( ": " );

  res += aData;
  if ( !aUnits.isEmpty() )
    res += QString( " " ) + aUnits;

  return res;
}

/*!
  \brief Check if the datum is empty.
  \return \c false if datum control has value entered
*/
bool QDS_Datum::isEmpty() const
{
  return stringValue().isEmpty();
}

/*!
  \brief Reset datum state and set default value as current.
*/
void QDS_Datum::reset()
{
  initDatum();

  mySourceValue = defaultValue();
  setString( formatValue( mySourceValue ) );
  invalidateCache();

  onParamChanged();
  QString str = getString();
  emit paramChanged();
  emit paramChanged( str );
}

/*!
  \brief Clear the control.
*/
void QDS_Datum::clear()
{
  initDatum();

  if ( !getString().isEmpty() )
  {
    mySourceValue = "";
    setString( mySourceValue );
    invalidateCache();

    onParamChanged();
    QString str = getString();
    emit paramChanged();
    emit paramChanged( str );
  }
}

/*!
  \brief Set datum value from QVariant object.
  \param val new value
*/
void QDS_Datum::setValue( const QVariant& val )
{
  // trying to extract data of correct type from QVariant
  if ( !dicItem().IsNull() )
  {
    bool isOk = false;
    switch( type() )
    {
      case DDS_DicItem::Float:
      {
        double aDblVal = val.toDouble( &isOk );
        if ( isOk )
        {
          setDoubleValue( aDblVal );
          return;
        }
        break;
      }
      case DDS_DicItem::Integer:
      case DDS_DicItem::List:
      {
        int anIntVal = val.toInt( &isOk );
        if ( isOk )
        {
          setIntegerValue( anIntVal );
          return;
        }
        break;
      }
      case DDS_DicItem::String:
      case DDS_DicItem::Unknown:
        break;
    }
  }

  if ( val.isValid() && val.canConvert( QVariant::String ) )
    setStringValue( val.toString() );
  else
    clear();
}

/*!
  \brief Set datum value from string data.
  \param txt new value
*/
void QDS_Datum::setStringValue( const QString& txt )
{
  initDatum();

  mySourceValue = txt;
  QString aStr = formatValue( txt );
  setString( aStr );
  myTargetValue = aStr;

  onParamChanged();
  QString str = getString();
  emit paramChanged();
  emit paramChanged( str );
}

/*!
  \brief Set datum value from double data.

  The value is converted from measure units in "SI" units system
  to the measure units in the active units system.
  Format the value using datum format internal if it is required.
  
  \param num new value
*/
void QDS_Datum::setDoubleValue( const double num )
{
  initDatum();

  mySourceValue = QString().setNum( num, 'g', 16 );
  double val = num;
  if ( !myDicItem.IsNull() && !( flags() & NotConvert ) )
    val = myDicItem->FromSI( val );

  QString aStr = formatValue( val );
  setString( aStr );
  myTargetValue = aStr;

  onParamChanged();
  QString str = getString();
  emit paramChanged();
  emit paramChanged( str );
}

/*!
  \brief Set datum value from integer data.

  The value is converted from measure units in "SI" units system
  to the measure units in the active units system.
  Format the value using datum format if it is required.
  
  \param num new value
*/
void QDS_Datum::setIntegerValue( const int num )
{
  initDatum();

  mySourceValue = QString().setNum( num );
  double val = num;
  if ( !myDicItem.IsNull() && !( flags() & NotConvert ) )
    val = myDicItem->FromSI( val );

  QString aStr = formatValue( val );
  setString( aStr );
  myTargetValue = aStr;

  onParamChanged();
  QString str = getString();
  emit paramChanged();
  emit paramChanged( str );
}

/*!
  \brief Get 'enabled' state of the specified subwidget.
  \param element ORed subwidget flags (QDS::DatumFlags)
  \return \c true if all subwidgets specified by \a element are enabled
  \sa setEnabled()
*/
bool QDS_Datum::isEnabled( const int element ) const
{
  initDatum();

  bool res = true;
  if ( element & Label )
    res = res && labelWidget() && labelWidget()->isEnabled();
  if ( element & Units )
    res = res && unitsWidget() && unitsWidget()->isEnabled();
  if ( element & Control )
    res = res && controlWidget() && controlWidget()->isEnabled();
  return res;
}

/*!
  \brief Enable/disable subwidgets specified by \a element.

  Possible values of \a element: QDS::Label, QDS::Control, QDS::Units
  or their ORed combinations.

  \param on new 'enabled' state
  \param element ORed subwidget flags (QDS::DatumFlags)
  \sa isEnabled()
*/
void QDS_Datum::setEnabled( const bool on, const int element )
{
  initDatum();

  if ( element & Label && labelWidget() )
    labelWidget()->setEnabled( on );
  if ( element & Units && unitsWidget() )
    unitsWidget()->setEnabled( on );
  if ( element & Control && controlWidget() )
    controlWidget()->setEnabled( on );
}

/*!
  \brief Enable/disable main control subwidget (QDS::Control).
  \param on new 'enabled' state
*/
void QDS_Datum::setEnabled( bool on )
{
  setEnabled( on, Control );
}

/*!
  \brief Show/hide subwidgets specified by \a flags.

  Possible values of \a flags: QDS::Label, QDS::Control, QDS::Units
  or their ORed combinations.

  \param visible new 'visibility' state
  \param flags ORed subwidget flags (QDS::DatumFlags)
  \sa show(), hide()
*/
void QDS_Datum::setShown( const bool visible, const int flags )
{
  initDatum();

  uint flag = Units;
  while ( flag )
  {
    if ( flags & flag && widget( flag ) )
      widget( flag )->setVisible( visible );
    flag = flag >> 1;
  }
}

/*!
  \brief Show subwidgets specified by \a element.

  Possible values of \a element: QDS::Label, QDS::Control, QDS::Units
  or their ORed combinations.

  \param element ORed subwidget flags (QDS::DatumFlags)
  \sa hide(), setShown()
*/
void QDS_Datum::show( const int element )
{
  setShown( true, element );
}

/*!
  \brief Hide subwidgets specified by \a element.

  Possible values of \a element: QDS::Label, QDS::Control, QDS::Units
  or their ORed combinations.

  \param element ORed subwidget flags (QDS::DatumFlags)
  \sa show(), setShown()
*/
void QDS_Datum::hide( const int element )
{
  setShown( false, element );
}

/*!
  \brief Get subwidget specified by \a element.

  Possible values of \a element: QDS::Label, QDS::Control, QDS::Units.
  
  \param element subwidget
  \return widget
*/
QWidget* QDS_Datum::widget( const int element ) const
{
  initDatum();
  return wrapper( element );
}

/*!
  \brief Set the input focus to the control widget.
*/
void QDS_Datum::setFocus()
{
  initDatum();

  if ( controlWidget() )
    controlWidget()->setFocus();
}

/*!
  \brief Check if input data is valid.

  If data is invalid and \a msgBox is \c true, the warning message box is shown.

  \param msgBox if \c true, show warning message box if input is invalid
  \param extMsg warning message
  \param extLabel optional name of the variable (if QDS::Label control is not used) 
  \return \c true if input data is valid
*/
bool QDS_Datum::isValid( const bool msgBox, const QString& extMsg, const QString& extLabel ) const
{
  initDatum();

  if ( type() == DDS_DicItem::String && isDoubleFormat( format() ) )
    return true;

  QString req;
  if ( !dicItem().IsNull() )
    req = toQString( dicItem()->GetRequired() );

  bool aState = true;
  QString aStr = getString();

  if ( aStr.isEmpty() )
    aState = !( req == QString( "yes" ) || req == QString( "true" ) || req.toInt() );
  else
    aState = validate( aStr );

  if ( msgBox && !aState )
  {
    QString info;
    if ( !label().isEmpty() )
      info += tr( "DATA_INCORRECT_VALUE" ).arg( label() );
    else if ( !extLabel.isEmpty() )
      info += tr( "DATA_INCORRECT_VALUE" ).arg( extLabel );

    QString typeStr;
    switch ( type() )
    {
    case DDS_DicItem::String:
      typeStr = tr( "DATA_STRING" );
      break;
    case DDS_DicItem::Integer:
      typeStr = tr( "DATA_INTEGER" );
      break;
    case DDS_DicItem::Float:
      typeStr = tr( "DATA_FLOAT" );
      break;
    default:
      typeStr = tr( "DATA_NON_EMPTY" );
      break;
    }
    info += ( info.isEmpty() ? (QString) "" : QString( "\n" ) ) + 
            tr( "DATA_SHOULD_BE_VALUE" ).arg( typeStr );
    QString limit;
    if ( type() == DDS_DicItem::Float || type() == DDS_DicItem::Integer )
    {
      QString aMinValue = minValue();
      QString aMaxValue = maxValue();
      if ( !aMinValue.isEmpty() && !aMaxValue.isEmpty() )
        limit = tr( "DATA_RANGE" ).arg( aMinValue ).arg( aMaxValue );
      else if ( !aMinValue.isEmpty() )
        limit = tr( "DATA_MIN_LIMIT" ).arg( aMinValue );
      else if ( !aMaxValue.isEmpty() )
        limit = tr( "DATA_MAX_LIMIT" ).arg( aMaxValue );
    }
    if ( !limit.isEmpty() )
      info += limit;

    info += QString( ".\n" ) + tr( "DATA_INPUT_VALUE" );

    if ( !extMsg.isEmpty() )
      info += QString( "\n" ) + extMsg;

    QString msg;
    for ( int i = 0; i < info.length(); i++ )
      if ( info.at( i ) == '\n' )
        msg += QString( "<br>" );
      else
        msg += info.at( i );

    info = QString( "<p><nobr>%1</nobr></p>" ).arg( msg );

    QMessageBox::critical( controlWidget() ? controlWidget()->topLevelWidget() : 0,
                           tr( "DATA_ERR_TITLE" ), info, tr( "OK" ) );
    if ( controlWidget() )
      controlWidget()->setFocus();
  }
  return aState;
}

/*!
  \brief Add widgets to the vertical box layout.
  \param l layout
*/
void QDS_Datum::addTo( QVBoxLayout* l )
{
  initDatum();

  if ( !l )
    return;

  if ( wrapper( Label ) )
    l->addWidget( wrapper( Label ) );
  if ( wrapper( Control ) )
    l->addWidget( wrapper( Control ) );
  if ( wrapper( Units ) )
    l->addWidget( wrapper( Units ) );
}

/*!
  \brief Add widgets to the horizontal box layout.
  \param l layout
*/
void QDS_Datum::addTo( QHBoxLayout* l )
{
  initDatum();

  if ( !l )
    return;

  if ( wrapper( Label ) )
    l->addWidget( wrapper( Label ) );
  if ( wrapper( Control ) )
    l->addWidget( wrapper( Control ) );
  if ( wrapper( Units ) )
    l->addWidget( wrapper( Units ) );
}

/*!
  \brief Add widgets to the grid layout.
  \param theLay layout
  \param theRow layout row index
  \param theCol layout column index
  \param vertical if \c true subwidgets are layouted vertically, otherwise - horizontally
*/
void QDS_Datum::addTo( QGridLayout* theLay, const int theRow, const int theCol, bool vertical )
{
  initDatum();

  if ( !theLay )
    return;

  int row = theRow;
  int col = theCol;
  if ( wrapper( Label ) )
  {
    theLay->addWidget( wrapper( Label ), row, col );
    vertical ? row++ : col++;
  }
  if ( wrapper( Control ) )
  {
    theLay->addWidget( wrapper( Control ), row, col );
    vertical ? row++ : col++;
  }
  if ( wrapper( Units ) )
    theLay->addWidget( wrapper( Units ), row, col );
}

/*!
  \brief Set the aligment for QDS::Label and/or QDS::Units subwidgets.
  \param align alignment type (Qt::Alignment)
  \param type ORed subwidget flags
*/
void QDS_Datum::setAlignment( const int align, const int type )
{
  initDatum();

  if ( ( type & Label ) && labelWidget() )
    labelWidget()->setAlignment( Qt::Alignment(align) );
  if ( ( type & Units ) && unitsWidget() )
    unitsWidget()->setAlignment( Qt::Alignment(align) );
}

/*!
  \brief Event filter.

  Perform delayed initialisation. Reimplemented for internal reasons.
  
  \param o event reciever object
  \param e event
  \return \c true if event should be filtered
*/
bool QDS_Datum::eventFilter( QObject* o, QEvent* e )
{
  if ( o == parent() )
  {
    if ( e->type() == QEvent::Show || e->type() == QEvent::ShowToParent ||
         ( e->type() == QEvent::ChildAdded && ((QChildEvent*)e)->child() == this ) )
      initDatum();
  }
  return QObject::eventFilter( o, e );
}

/*!
  \brief Called when datum value is changed.
*/
void QDS_Datum::onParamChanged()
{
}

/*!
  \brief Perform delayed initialization.
*/
void QDS_Datum::onInitDatum()
{
  initDatum();
}

/*!
  \brief Called when child subwidget is destroued.

  Allows avoiding crash of extra calling of the child subwidget destructor.

  \param obj object being destroyed
*/
void QDS_Datum::onDestroyed( QObject* obj )
{
  myWrapper.remove( wrapperType( (Wrapper*)obj ) );
}

/*!
  \brief Get QDS::Label widget.
  \return label widget
*/
QLabel* QDS_Datum::labelWidget() const
{
  initDatum();
  return myLabel;
}

/*!
  \brief Get QDS::Units widget.
  \return units widget
*/
QLabel* QDS_Datum::unitsWidget() const
{
  initDatum();
  return myUnits;
}

/*!
  \brief Get QDS::Control widget.
  \return control widget
*/
QWidget* QDS_Datum::controlWidget() const
{
  initDatum();
  return myControl;
}

/*!
  \brief Get the dictionary item from the datum.
  \return handle to data dictionary item
*/
Handle(DDS_DicItem) QDS_Datum::dicItem() const
{
  return myDicItem;
}

/*!
  \brief Set the dictionary item to the datum.
  \param item handle to data dictionary item
*/
void QDS_Datum::setDicItem( const Handle(DDS_DicItem)& item )
{
  myDicItem = item;
}

/*!
  \brief Create QDS::Label widget.
  \param parent parent widget
  \return label widget
*/
QLabel* QDS_Datum::createLabel( QWidget* parent )
{
  return new QLabel( parent );
}

/*!
  \brief Create QDS::Units widget.
  \param parent parent widget
  \return units widget
*/
QLabel* QDS_Datum::createUnits( QWidget* parent )
{
  return new QLabel( parent );
}

/*!
  \fn QWidget* QDS_Datum::createControl( QWidget* parent );
  \brief Create QDS::Control widget.

  This method should be implemented in the successor classes.

  \param parent parent widget
  \return control widget
*/

/*!
  \fn QString QDS_Datum::getString() const;
  \brief Get string value from datum.
  
  This method should be implemented in the successor classes.
  
  \return datum string value
*/

/*
  \fn void QDS_Datum::setString( const QString& txt );
  \brief Set string value to datum.
  
  This method should be implemented in the successor classes.
  
  \param txt new datum string value
*/

/*!
  \brief Create validator according to the datum type of value.
  \param limits if \c true use minimum and maximum value limits
  \return validator
*/
QValidator* QDS_Datum::validator( const bool limits ) const
{
  QValidator* aValidator = 0;

  QString fltr = filter();

  if ( type() == DDS_DicItem::String )
  {
    QString aFlags;
    QString aFormat = canonicalFormat( format(), aFlags );

    int len = -1;
    int pos = aFormat.indexOf( "." );
    if ( pos != -1 )
    {
      QString numStr = aFormat.mid( pos + 1, aFormat.length() - pos - 2 );
      bool ok;
      int numVal = numStr.toInt( &ok );
      if ( ok )
        len = numVal;
    }

    QDS_StringValidator* aStrVal = new QDS_StringValidator( fltr, aFlags, (QObject*)this );
    aStrVal->setLength( len );

    aValidator = aStrVal;
  }
  else if ( type() == DDS_DicItem::Integer )
  {
    QDS_IntegerValidator* aIntVal = new QDS_IntegerValidator( fltr, (QObject*)this );

    bool ok;
    int limit;
    limit = minValue().toInt( &ok );
    if ( ok && limits )
      aIntVal->setBottom( limit );
    limit = maxValue().toInt( &ok );
    if ( ok && limits )
      aIntVal->setTop( limit );

    aValidator = aIntVal;
  }
  else if ( type() == DDS_DicItem::Float )
  {
    QDS_DoubleValidator* aFloatVal = new QDS_DoubleValidator( fltr, (QObject*)this );

    bool ok;
    double limit;
    limit = minValue().toDouble( &ok );
    if ( ok && limits )
      aFloatVal->setBottom( limit );
    limit = maxValue().toDouble( &ok );
    if ( ok && limits )
      aFloatVal->setTop( limit );

    aValidator = aFloatVal;
  }

  return aValidator;
}

/*!
  \brief Validate the specified string.
  \param txt string to be validated
  \return \c true if string contains valid data
*/
bool QDS_Datum::validate( const QString& txt ) const
{
  if ( type() == DDS_DicItem::Unknown ||
       ( type() == DDS_DicItem::String && isDoubleFormat( format() ) ) )
    return true;

  QValidator* aValidator = validator( true );

  if ( !aValidator )
    return true;

  int pos = 0;
  QString str( txt );
  bool res = aValidator->validate( str, pos ) == QValidator::Acceptable;

  delete aValidator;

  return res;
}

/*!
  \brief Initialize datum.
  
  Retrieve information from data dictionary and create subwidgets
  using virtual mechanism.

  Virtual mechanism doesn't work in the constructor, therefore this method should
  be called outside the constructor.
*/
void QDS_Datum::initialize()
{
  if ( wrapper( Label ) )
    wrapper( Label )->setWidget( myLabel = createLabel( wrapper( Label ) ) );
  if ( wrapper( Control ) )
    wrapper( Control )->setWidget( myControl = createControl( wrapper( Control ) ) );
  if ( wrapper( Units ) )
    wrapper( Units )->setWidget( myUnits = createUnits( wrapper( Units ) ) );

  TCollection_AsciiString comp;
  Handle(DDS_DicItem) item = dicItem();
  if ( !item.IsNull() )
    comp = item->GetComponent();

  QString unitSystem;
  Handle(DDS_Dictionary) dic = DDS_Dictionary::Get();
  if ( !dic.IsNull() )
    unitSystem = toQString( comp.IsEmpty() ? dic->GetActiveUnitSystem() :
                                             dic->GetActiveUnitSystem( comp ) );

  unitSystemChanged( unitSystem );

  QWidget* ctrl = controlWidget();
  if ( ctrl )
  {
    QString lDescr = longDescription();
    QString sDescr = shortDescription();
    if ( !sDescr.isEmpty() )
      ctrl->setToolTip( sDescr );
    if ( !lDescr.isEmpty() )
      ctrl->setWhatsThis( lDescr );
  }

  if ( labelWidget() && ctrl && !( flags() & NotAccel ) )
    labelWidget()->setBuddy( ctrl );
}

/*!
  \brief Process notification about active units system changing.
  
  Update label and units widgets.

  \param unitSystem new active units system
*/
void QDS_Datum::unitSystemChanged(const QString& )
{
  QString labText = label();
  QString unitText = unitsToText( units() );

  if ( flags() & UnitsWithLabel )
  {
    if ( labText.isEmpty() )
      labText = unitText;
    else if ( !unitText.isEmpty() )
      labText = QString( "%1 (%2)" ).arg( labText ).arg( unitText );
    unitText = QString();
  }

  if ( labelWidget() )
    labelWidget()->setText( labText );

  if ( unitsWidget() )
    unitsWidget()->setText( unitText );

  reset();
}

/*!
  \brief Convert units into text presentation.
  \param uni units to be converted
  \return text presentation of units (rich-text)
*/
QString QDS_Datum::unitsToText( const QString& uni )
{
  int pos = -1;
  QString aUnits = uni;
  while ( ( pos = aUnits.indexOf( "**" ) ) != -1 )
  {
    aUnits = aUnits.mid( 0, pos ) + QString( "<tt><font size=+2><sup>" ) +
             aUnits.mid( pos + 2, 1 ) + QString( "</sup></font></tt>" ) +
             aUnits.mid( pos + 3 );
  }
  return aUnits;
}

/*!
  \brief Convert text presentation into internal units format.
  \param txt text presentation of units (rich-text)
  \return units value
*/
QString QDS_Datum::textToUnits( const QString& txt )
{
  int pos = -1;
  QString aUnits = txt;
  while ( ( pos = aUnits.indexOf( "<sup>" ) ) != -1 )
  {
    aUnits.remove( pos, 5 );
    aUnits.insert( pos, "**" );
  }
  while ( ( pos = aUnits.indexOf( "</sup>" ) ) != -1 )
    aUnits.remove( pos, 6 );
  return aUnits;
}

/*!
  \brief Format the specified integer as dictionary item value.
  \param num integer value being formatted
  \param id data dictionary ID
  \param convert if \c true, perform conversion from "SI" units system
  \return formatted value
*/
QString QDS_Datum::format( const int num, const QString& id, const bool convert )
{
  Handle(DDS_DicItem) anItem;
  int aNum = num;
  QString anUnit;
  
  QString aFormat;
  int aType = DDS_DicItem::Unknown;
  Handle(DDS_Dictionary) aDict = DDS_Dictionary::Get();
  if ( !aDict.IsNull() )
  {
    anItem = aDict->GetDicItem( toAsciiString( id ) );
    if ( !anItem.IsNull() )
    {
      aType = anItem->GetType();
      aFormat = toQString( anItem->GetFormat( false ) );
      if ( convert )
        aNum = ( int )anItem->FromSI( aNum );
    }
  }

  return format( aFormat, aType, aNum );
}

/*!
  \brief Format the specified double as dictionary item value.
  \param num double value being formatted
  \param id data dictionary ID
  \param convert if \c true, perform conversion from "SI" units system
  \return formatted value
*/
QString QDS_Datum::format( const double num, const QString& id, const bool convert )
{
  Handle(DDS_DicItem) anItem;
  double aNum = num;
  QString anUnit;
  
  QString aFormat;
  int aType = DDS_DicItem::Unknown;
  Handle(DDS_Dictionary) aDict = DDS_Dictionary::Get();
  if ( !aDict.IsNull() )
  {
    anItem = aDict->GetDicItem( toAsciiString( id ) );
    if ( !anItem.IsNull() )
    {
      aType = anItem->GetType();
      aFormat = toQString( anItem->GetFormat( false ) );
      if ( convert )
        aNum = anItem->FromSI( aNum );
    }
  }

  return format( aFormat, aType, aNum );
}

/*!
  \brief Format the specified string as dictionary item value.
  \param str string value being formatted
  \param id data dictionary ID
  \param convert if \c true, perform conversion from "SI" units system
  \return formatted value
*/
QString QDS_Datum::format( const QString& str, const QString& id, const bool convert )
{
  Handle(DDS_DicItem) anItem;
  QString aStr = str;
  QString anUnit;

  QString aFormat;
  int aType = DDS_DicItem::Unknown;
  Handle(DDS_Dictionary) aDict = DDS_Dictionary::Get();
  if ( !aDict.IsNull() )
  {
    anItem = aDict->GetDicItem( toAsciiString( id ) );
    if ( !anItem.IsNull() )
    {
      aType = anItem->GetType();
      aFormat = toQString( anItem->GetFormat( false ) );
      if ( convert )
        aStr = QString::number( anItem->FromSI( aStr.toDouble() ), 'f', 16 );
    }
  }

  return format( aFormat, aType, aStr );
}

/*!
  \brief Format the given value according to the data dictionary item type.
  \param aFormat format string
  \param aType data dictionary item type
  \param aValue integer value being formatted
  \return formatted value
*/
QString QDS_Datum::format( const QString& aFormat, const int aType, const int aValue )
{
  QString txt;

  if ( !aFormat.isEmpty() )
  {
    switch ( aType )
    {
    case DDS_DicItem::Float:
      txt = sprintf( aFormat, (double)aValue );
      txt = txt.trimmed();
      break;
    case DDS_DicItem::Integer:
      txt = sprintf( aFormat, aValue );
      txt = txt.trimmed();
      break;
    case DDS_DicItem::String:
    default:
      txt = sprintf( aFormat, aValue );
      break;
    }
  }
  else
    txt = QString().setNum( aValue );

  return txt;
}

/*!
  \brief Format the given value according to the data dictionary item type.
  \param aFormat format string
  \param aType data dictionary item type
  \param aValue double value being formatted
  \return formatted value
*/
QString QDS_Datum::format( const QString& aFormat, const int aType, const double aValue )
{
  QString txt;

  if ( !aFormat.isEmpty() )
  {
    switch ( aType )
    {
    case DDS_DicItem::Float:
      txt = QString().sprintf( aFormat.toLatin1().constData(), aValue );
      txt = txt.trimmed();
      break;
    case DDS_DicItem::Integer:
      txt = QString().sprintf( aFormat.toLatin1().constData(), (int)aValue );
      txt = txt.trimmed();
      break;
    case DDS_DicItem::String:
    default:
      txt = QString().sprintf( aFormat.toLatin1().constData(), aValue );
      break;
    }
  }
  else
    txt = QString().setNum( aValue, 'g', 16 );

  return txt;
}

/*!
  \brief Format the given value according to the data dictionary item type.
  \param aFormat format string
  \param aType data dictionary item type
  \param aValue string value being formatted
  \return formatted value
*/
QString QDS_Datum::format( const QString& aFormat, const int aType, const QString& aValue )
{
  QString txt = aValue;

  if ( aType != DDS_DicItem::String )
    txt = txt.trimmed();

  if ( aFormat.isEmpty() || txt.isEmpty() )
    return txt;

  switch ( aType )
  {
  case DDS_DicItem::Float:
    txt = txt.replace( 'd', 'e' ).replace( 'D', 'E' );
    txt = sprintf( aFormat, txt.toDouble() );
    txt = txt.trimmed();
    break;
  case DDS_DicItem::Integer:
    txt = sprintf( aFormat, txt.toInt() );
    txt = txt.trimmed();
    break;
  case DDS_DicItem::String:
    txt = sprintf( aFormat, txt );
    break;
  }

  return txt;
}

/*!
  \brief Wrapper around the standard sprintf() function.

  Process some non standard flags from format string.

  \param fmt string format
  \param val value
  \return formatted value
*/
QString QDS_Datum::sprintf( const QString& fmt, const int val )
{
  return QString().sprintf( canonicalFormat( fmt ).toLatin1().constData(), val );
}

/*!
  \brief Wrapper around the standard sprintf() function.
  \overload

  Process some non standard flags from format string.

  \param fmt string format
  \param val value
  \return formatted value
*/
QString QDS_Datum::sprintf( const QString& fmt, const double val )
{
  return QString().sprintf( canonicalFormat( fmt ).toLatin1().constData(), val );
}

/*!
  \brief Wrapper around the standard sprintf() function.
  \overload

  Process some non standard flags from format string.

  \param fmt string format
  \param val value
  \return formatted value
*/
QString QDS_Datum::sprintf( const QString& fmt, const QString& val )
{
  QString aFlags;
  QString aFormat = canonicalFormat( fmt, aFlags );

  QString txt = val;

  QRegExp rx( "^(%[0-9]*.?[0-9]*s)$" );
  if ( aFormat.indexOf( rx ) != -1 )
  {
    // QString().sprintf() always expects string in UTF8 encoding, so we cannot use it here
    char* buf = new char[txt.length() + 1];
    ::sprintf( buf, aFormat.toLatin1().constData(), (const char*)(txt.toLocal8Bit()) );
    txt = QString::fromLocal8Bit( buf );

    delete[] buf;
  }

  if ( isDoubleFormat( aFormat ) )
  {
    /*bool isOk;
    double aVal = txt.toDouble( &isOk );
    if ( isOk )
    {
      txt = sprintf( aFormat, aVal );
      txt = txt.replace( 'e', 'D' );
    }*/
  }

  if ( aFlags.contains( "u", Qt::CaseInsensitive ) )
    txt = txt.toUpper();
  if ( aFlags.contains( "l", Qt::CaseInsensitive ) )
    txt = txt.toLower();

  return txt;
}

/*!
  \brief Get the canonical sprintf() format.
  \param fmt string format
  \return canonical sprintf() format
*/
QString QDS_Datum::canonicalFormat( const QString& fmt )
{
  QString flags;
  return canonicalFormat( fmt, flags );
}

/*!
  \brief Get the canonical sprintf() format and non standard flags.
  \param fmt string format
  \param flags format flags
  \return canonical sprintf() format
*/
QString QDS_Datum::canonicalFormat( const QString& fmt, QString& flags )
{
  QString newFmt = fmt;
  flags = QString();

  QRegExp rx( "^(%[0-9]*.?[0-9]*)([a-z,A-Z]+)[g|c|d|i|o|u|x|e|f|n|p|s|X|E|G]$" );
  if ( rx.indexIn( newFmt ) >= 0 )
  {
    flags = rx.cap( 2 );
    newFmt.remove( rx.pos( 2 ), flags.length() );
  }
  return newFmt;
}

/*!
  \brief Get displayable units string for specified data dictionary item.
  \param id data dictionary item ID
  \return units string
*/
QString QDS_Datum::units( const QString& id )
{
  QString anUnit;
  Handle(DDS_DicItem) anItem;

  Handle(DDS_Dictionary) aDict = DDS_Dictionary::Get();
  if ( !aDict.IsNull() )
  {
    anItem = aDict->GetDicItem( toAsciiString( id ) );
    if ( !anItem.IsNull() )
      anUnit = unitsToText( toQString( anItem->GetUnits() ) );
  }
  return anUnit;
}

/*!
  \brief Get prefix string from format.
  \return current implementation returns null string
*/
QString QDS_Datum::prefix() const
{
  return QString();
}

/*!
  \brief Get suffix string from format.
  \return current implementation returns null string
*/
QString QDS_Datum::suffix() const
{
  return QString();
}

/*!
  \brief Get minimum value.
  \return minimum value
*/
QString QDS_Datum::minValue() const
{
  QString pref = prefix();
  QString suff = suffix();

  QString aMin = minimumValue().trimmed();

  if ( !pref.isEmpty() && aMin.left( pref.length() ) == pref )
    aMin = aMin.mid( pref.length() );

  if ( !suff.isEmpty() && aMin.right( suff.length() ) == suff )
    aMin = aMin.mid( 0, aMin.length() - suff.length() );

  return aMin;
}

/*!
  \brief Get maximum value.
  \return maximum value
*/
QString QDS_Datum::maxValue() const
{
  QString pref = prefix();
  QString suff = suffix();

  QString aMax = maximumValue().trimmed();

  if ( !pref.isEmpty() && aMax.left( pref.length() ) == pref )
    aMax = aMax.mid( pref.length() );

  if ( !suff.isEmpty() && aMax.right( suff.length() ) == suff )
    aMax = aMax.mid( 0, aMax.length() - suff.length() );

  return aMax;
}

/*!
  \brief Reset the numeric value cache.
*/
void QDS_Datum::invalidateCache()
{
  myTargetValue = QString();
}

/*!
  \brief Remove the acceleartor tags '&' from specified label string \a src.
  \param src processed string
  \return string after removing '&' symbols
*/
QString QDS_Datum::removeAccel( const QString& src )
{
  QString trg = src;

  for ( int i = 0; i < trg.length(); )
  {
    if ( trg.mid( i, 2 ) == QString( "&&" ) )
      i += 2;
    else if ( trg.at( i ) == '&' )
      trg.remove( i, 1 );
    else
      i++;
  }
  return trg;
}

/*!
  \brief Check if given format specified doube value.
  \param theFormat format string 
  \return \c true if \a theFormat has specificator for double values
*/
bool QDS_Datum::isDoubleFormat( const QString& theFormat )
{
  if ( theFormat.length() > 0 )
  {
    QChar c = theFormat[ (int)( theFormat.length() - 1 ) ];
      return c == 'f' || c == 'g' || c == 'e' || c == 'G' || c == 'E';
  }
  else
    return false;
}

/*!
  \brief Get datum flags.
  \return datum flags (QDS::DatumFlags)
*/
int QDS_Datum::flags() const
{
  return myFlags;
}

/*!
  \brief Perform initialization if it needed.
  \internal
*/
void QDS_Datum::initDatum() const
{
  if ( myInitialised )
    return;

  QDS_Datum* that = (QDS_Datum*)this;
  that->myInitialised = true;
  that->initialize();

  if ( parent() )
    parent()->removeEventFilter( (QObject*)this );
}

/*!
  \brief Format integer value according to datum formatting and type.
  \internal
  \param theValue the value being formatted.
  \return formatted string value.
*/
QString QDS_Datum::formatValue( const int theValue ) const
{
  return format( flags() & NotFormat ? "" : format(), type(), theValue );
}

/*!
  \brief Format double value according to datum formatting and type.
  \internal
  \param theValue the value being formatted.
  \return formatted string value.
*/
QString QDS_Datum::formatValue( const double theValue ) const
{
  return format( flags() & NotFormat ? "" : format(), type(), theValue );
}

/*!
  \brief Format string value according to datum formatting and type.
  \internal
  \param theValue the value being formatted.
  \return formatted string value.
*/
QString QDS_Datum::formatValue( const QString& theValue ) const
{
  return format( flags() & NotFormat ? "" : format(), type(), theValue );
}

/*!
  \brief Get wrapper for specified subwidget.
  \internal
  \param wid subwidget
  \return wrapper
*/
QDS_Datum::Wrapper* QDS_Datum::wrapper( QWidget* wid ) const
{
  if ( !wid )
    return 0;

  Wrapper* wrap = 0;
  for ( QMap<int, Wrapper*>::ConstIterator it = myWrapper.begin(); it != myWrapper.end() && !wrap; ++it )
  {
    if ( it.value() && it.value()->widget() == wid )
      wrap = it.value();
  }
  return wrap;
}

/*!
  \brief Get wrapper for specified subwidget.
  \internal
  \param id subwidget ID
  \return wrapper
*/
QDS_Datum::Wrapper* QDS_Datum::wrapper( const int id ) const
{
  Wrapper* wrap = 0;
  if ( myWrapper.contains( id ) )
    wrap = myWrapper[id];
  return wrap;
}

/*!
  \brief Get subwidget type for specified wrapper.
  \internal
  \param wrap wrapper
  \return subwidget ID
*/
int QDS_Datum::wrapperType( QDS_Datum::Wrapper* wrap ) const
{
  int id = -1;
  for ( QMap<int, Wrapper*>::ConstIterator it = myWrapper.begin(); it != myWrapper.end() && id == -1; ++it )
  {
    if ( it.value() == wrap )
      id = it.key();
  }
  return id;
}

/*!
  \fn void QDS_Datum::paramChanged();
  \brief The signal is emitted when the datum value is changed.
*/

/*!
  void QDS_Datum::paramChanged( QString& txt );
  \brief The signal is emitted when the datum value is changed.
  \param txt new datum value
*/

