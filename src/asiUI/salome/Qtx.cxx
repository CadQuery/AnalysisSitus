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

// File:      Qtx.cxx
// Author:    Sergey TELKOV
//
#include "Qtx.h"

#pragma warning(push, 0)
#include <QDir>
#include <QMenu>
#include <QRegExp>
#include <QBitmap>
#include <QWidget>
#include <QLayout>
#include <QPainter>
#include <QDirModel>
#include <QFileInfo>
#include <QCompleter>
#include <QApplication>
#include <QDesktopWidget>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QConicalGradient>
#include <QtDebug>
#pragma warning(pop)

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <clocale>

#pragma warning(disable : 4996 4714 4127)

#define BICOLOR_CHANGE_HUE

/*!
  \class Qtx
  \brief A set of helpful utility functions.

  The class implements a set of the static functions which can be used
  for the different purposes:
  - specify tab order for the set of widgets: setTabOrder()
  - align one widget to the coordinates of the another one: alignWidget()
  - remove extra separators from the menu or toolbar: simplifySeparators()
  - retrieve directory, file name and extension parts of the path:
  dir(), file(), extension()
  - get the path to the temporary directory: tmpDir()
  - create or remove a directory (recursively): mkDir(), rmDir()
  - convert text file from DOS to UNIX native format: dos2unix()
  - convert a picture to the gray scale: grayscale()
  - other
*/

/*!
  \brief Convert character array (ASCII string) to the QString.
  \param str character array
  \param len array length, if < 0, the array should be zero-terminated
  \return QString object
 */
QString Qtx::toQString( const char* str, const int len )
{
  return toQString( (unsigned char*) str, len );
}

/*!
  \brief Convert integer array (UNICODE string) to the QString.
  \param str integer array
  \param len array length, if < 0, the array should be zero-terminated
  \return QString object
 */
QString Qtx::toQString( const short* str, const int len )
{
  return toQString( (unsigned short*)str, len );
}

/*!
  \brief Convert character array (ASCII string) to the QString.
  \param str character array
  \param len array length, if < 0, the array should be zero-terminated
  \return QString object
 */
QString Qtx::toQString( const unsigned char* str, const int len )
{
  QString res;
  const unsigned char* s = str;
  while ( len < 0 || res.length() < len )
  {
    if ( *s == '\0' )
      break;

    res.append( QChar( *s ) );
    s++;
  }
  return res;
}

/*!
  \brief Convert integer array (UNICODE string) to the QString.
  \param str integer array
  \param len array length, if < 0, the array should be zero-terminated
  \return QString object
 */
QString Qtx::toQString( const unsigned short* str, const int len )
{
  QString res;
  const unsigned short* s = str;
  while ( len < 0 || res.length() < len )
  {
    if ( *s == '\0' )
      break;

    res.append( QChar( *s ) );
    s++;
  }
  return res;
}

/*!
  \brief Set tab order for specified list of widgets.

  The function has arbitrary number of parameters, each should be
  hovewer of QWidget* type. Last parameter should be null pointer.

  \param first first widget in the sequence
*/
void Qtx::setTabOrder( QWidget* first, ... )
{
  va_list wids;
  va_start( wids, first );

  QWidgetList widList;

  QWidget* cur = first;
  while ( cur )
  {
    widList.append( cur );
    cur = va_arg( wids, QWidget* );
  }

  setTabOrder( widList );
}

/*!
  \brief Set tab order for specified list of widgets.
  \param widgets list of widgets
*/
void Qtx::setTabOrder( const QWidgetList& widgets )
{
  if ( widgets.count() < 2 )
    return;

  QWidget* prev = 0;
  for ( QWidgetList::const_iterator it = widgets.begin(); it!= widgets.end(); ++it )
  {
    QWidget* next = *it;
    if ( prev && next )
      QWidget::setTabOrder( prev, next );
    prev = next;
  }
}

/*!
  \brief Align widget \a src relative to widget \a ref acording to the 
         alignment flags \a alignFlags.
  \param src source widget (being aligned)
  \param ref reference widget (source widget being aligned to)
  \param alignFlags alignment flags (Qtx::AlignmentFlags)
*/
void Qtx::alignWidget( QWidget* src, const QWidget* ref, const int alignFlags )
{
  if ( !src || !ref || !alignFlags )
    return;

  QPoint srcOri = src->pos();
  QPoint refOri = ref->pos();
  if ( src->parentWidget() && !src->isTopLevel() )
    srcOri = src->parentWidget()->mapToGlobal( srcOri );
  if ( ref->parentWidget() && !ref->isTopLevel() )
    refOri = ref->parentWidget()->mapToGlobal( refOri );

  int x = srcOri.x(), y = srcOri.y();
  int refWidth = ref->frameGeometry().width(), refHei = ref->frameGeometry().height();
  int srcWidth = src->frameGeometry().width(), srcHei = src->frameGeometry().height();

  if ( srcWidth <= 0 )
    srcWidth = src->sizeHint().width();
  if ( srcHei <= 0 )
    srcHei = src->sizeHint().height();

  int border = 0;
  if ( ref->isTopLevel() && ref->isMaximized() &&
       src->isTopLevel() && !src->isMaximized() )
    border = ( src->frameGeometry().width() - src->width() ) / 2;

  if ( alignFlags & Qtx::AlignLeft )
    x = refOri.x() + border;
  if ( alignFlags & Qtx::AlignOutLeft )
    x = refOri.x() - srcWidth - border;
  if ( alignFlags & Qtx::AlignRight )
    x = refOri.x() + refWidth - srcWidth - border;
  if ( alignFlags & Qtx::AlignOutRight )
    x = refOri.x() + refWidth + border;
  if ( alignFlags & Qtx::AlignTop )
    y = refOri.y() + border;
  if ( alignFlags & Qtx::AlignOutTop )
    y = refOri.y() - srcHei - border;
  if ( alignFlags & Qtx::AlignBottom )
    y = refOri.y() + refHei - srcHei - border;
  if ( alignFlags & Qtx::AlignOutBottom )
    y = refOri.y() + refHei + border;
  if ( alignFlags & Qtx::AlignHCenter )
    x = refOri.x() + ( refWidth - srcWidth ) / 2;
  if ( alignFlags & Qtx::AlignVCenter )
    y = refOri.y() + ( refHei - srcHei ) / 2;

  if ( src->parentWidget() && !src->isTopLevel() )
  {
    QPoint pos = src->parentWidget()->mapFromGlobal( QPoint( x, y ) );
    x = pos.x();
    y = pos.y();
  }

  QWidget* desk = QApplication::desktop();
  if ( desk && x + srcWidth + border > desk->width() )
    x = desk->width() - srcWidth - border;
  if ( desk && y + srcHei + border > desk->height() )
    y = desk->height() - srcHei - border;

  x = qMax( x, 0 );
  y = qMax( y, 0 );

  src->move( x, y );
}

/*!
  \brief Remove (recursively) unnecessary separators from the menu or toolbar.
  \param wid widget, should be of QMenu* or QToolBar* class
*/
void Qtx::simplifySeparators( QWidget* wid, const bool recursive )
{
  if ( !wid )
    return;

  QList<QAction*> items = wid->actions();
  if ( items.isEmpty() )
    return;

  QList<QAction*> toRemove;
  for ( int i = 1; i < items.count(); i++ )
  {
    if ( items[i]->isSeparator() && items[i - 1]->isSeparator() )
      toRemove.append( items[i] );

    if ( recursive && items[i]->menu() )
      simplifySeparators( items[i]->menu(), recursive );
  }

  for ( QList<QAction*>::iterator it = toRemove.begin(); it != toRemove.end(); ++it )
    wid->removeAction( *it );

  items = wid->actions();
  if ( !items.isEmpty() && items[0]->isSeparator() )
    wid->removeAction( items[0] );

  items = wid->actions();
  if ( !items.isEmpty() && items[items.count() - 1]->isSeparator() )
    wid->removeAction( items[items.count() - 1] );
}

/*!
  \brief Return \c true if specified \a parent is a parent object
         of given \a child (in terms of QObject).

  This function works recursively. It means that \a true is also
  returned if \a parent is a grand-father, grand-grand-father, etc
  of \a child. If the same object is given as both \a parent and
  \a child, \c true is also returned.

  \param child child object
  \param parent parent object
  \return \c true if the \a parent is a parent of \a child
*/
bool Qtx::isParent( QObject* child, QObject* parent )
{
  if ( !child || !parent )
    return false;

  bool res = false;
  QObject* obj = child;
  while ( !res && obj )
  {
    res = obj == parent;
    obj = obj->parent();
  }
  return res;
}

/*!
  \brief Find the parent object of class specified by \a className (in terms of QObject).

  \param obj current object
  \param className class name of the parent
  \return parent object or null pointer if the parent not found
*/
QObject* Qtx::findParent( QObject* obj, const char* className )
{
  if ( !obj )
    return 0;

  if ( !className || !strlen( className ) )
    return obj->parent();

  QObject* res = 0;
  QObject* p = obj->parent();
  while ( p && !res )
  {
    if ( p->inherits( className ) )
      res = p;
    p = p->parent();
  }

  return res;
}

/*!
  \brief Return directory part of the file path.

  If the file path does not include directory part (the file is in the
  current directory), null string is returned.

  \param path file path
  \param abs if true (default) \a path parameter is treated as absolute file path
  \return directory part of the file path
*/
QString Qtx::dir( const QString& path, const bool abs )
{
  QDir aDir = QFileInfo( path ).dir();
  QString dirPath = abs ? aDir.absolutePath() : aDir.path();
  if ( dirPath == QString( "." ) )
    dirPath = QString();
  return dirPath;
}

/*!
  \brief Return file name part of the file path.

  \param path file path
  \param withExt if true (default) complete file name (with all
         extension except the last) is returned, otherwise only base name
         is returned
  \return file name part of the file path
*/
QString Qtx::file( const QString& path, bool withExt )
{
  QString fPath = path;
  while ( !fPath.isEmpty() && ( fPath[fPath.length() - 1] == '\\' || fPath[fPath.length() - 1] == '/' ) )
    fPath.remove( fPath.length() - 1, 1 );

  if ( withExt )
    return QFileInfo( fPath ).fileName();
  else
    return QFileInfo( fPath ).baseName();
}

/*!
  \brief Return extension part of the file path.

  \param path file path
  \param full if true complete extension (all extensions, dot separated)
         is returned, otherwise (default) only last extension is returned
  \return extension part of the file path
*/
QString Qtx::extension( const QString& path, const bool full )
{
  return full ? QFileInfo( path ).completeSuffix() : QFileInfo( path ).suffix();
}

/*!
  \brief Convert the given parameter to the platform-specific library name.

  The function appends platform-specific prefix (lib) and suffix (.dll/.so)
  to the library file name.
  For example, if \a str = "mylib", "libmylib.so" is returned for Linux and
  mylib.dll for Windows.

  \param str short library name
  \return full library name
*/
QString Qtx::library( const QString& str )
{
  QString path = dir( str, false );
  QString name = file( str, false );
  QString ext  = extension( str );

#ifndef WIN32
  if ( !name.startsWith( "lib" ) )
    name = QString( "lib" ) + name;
#endif

#ifdef WIN32
  QString libExt( "dll" );
#else
  QString libExt( "so" );
#endif

  if ( ext.toLower() != QString( "so" ) && ext.toLower() != QString( "dll" ) )
  {
    if ( !name.isEmpty() && !ext.isEmpty() )
      name += QString( "." );
    name += ext;
  }

  ext = libExt;

  QString fileName = addSlash( path ) + name + QString( "." ) + ext;

  return fileName;
}

/*!
  \brief Get the temporary directory name.
  \return temporary directory (platform specific)
*/
QString Qtx::tmpDir()
{
  const char* tmpdir = ::getenv( "TEMP" );
  if ( !tmpdir )
    tmpdir = ::getenv ( "TMP" );
  if ( !tmpdir )
  {
#ifdef WIN32
    tmpdir = "C:\\";
#else
    tmpdir = "/tmp";
#endif
  }
  return QString( tmpdir );
}

/*!
  \brief Create directory recursively including all intermediate sub directories.
  \return \c true if the directory is successfully created and \c false otherwise
*/
bool Qtx::mkDir( const QString& dirPath )
{
  return QDir().mkpath( dirPath );
}

/*!
  \brief Remove directory recursively including all subdirectories and files.
  \return \c true if the directory is successfully removed and \c false otherwise
*/
bool Qtx::rmDir( const QString& thePath )
{
  QFileInfo fi( thePath );
  if ( !fi.exists() )
    return true;

  bool stat = true;
  if ( fi.isFile() )
    stat = QFile::remove( thePath );
  else if ( fi.isDir() )
  {
    QDir aDir( thePath );
    QFileInfoList anEntries = aDir.entryInfoList();
    for ( QFileInfoList::iterator it = anEntries.begin(); it != anEntries.end(); ++it )
    {
      QFileInfo inf = *it;
      if ( inf.fileName() == "." || inf.fileName() == ".." )
        continue;
      stat = stat && rmDir( inf.absoluteFilePath() );
    }
    stat = stat && aDir.rmdir( thePath );
  }
  return stat;
}

/*!
  \brief Add a slash (platform-specific) to the end of \a path
         if it is not already there.
  \param path directory path
  \return modified path (with slash added to the end)
*/
QString Qtx::addSlash( const QString& path )
{
  QString res = path;
  if ( !res.isEmpty() && res.at( res.length() - 1 ) != QChar( '/' ) &&
       res.at( res.length() - 1 ) != QChar( '\\' ) )
  res += QDir::separator();
  return res;
}

/*!
  \brief Convert text file from DOS format to UNIX.

  The function replaces "LF/CR" symbols sequence by "LF" symbol.

  \param absName file name
  \return \c true if the file is converted successfully and \c false in
         case of any error
*/
bool Qtx::dos2unix( const QString& absName )
{
  FILE* src = ::fopen( absName.toLatin1(), "rb" );
  if ( !src )
    return false;

  /* we'll use temporary file */
  char temp[512] = { '\0' };
  QString dir = Qtx::dir( absName );
  FILE* tgt = ::fopen( strcpy( temp, ::tempnam( dir.toLatin1(), "__x" ) ), "wb" );
  if ( !tgt )
    return false;

  /* temp -> result of conversion */
  const char CR = 0x0d;
  const char LF = 0x0a;
  bool waitingLF = false;

  while( true )
  {
    int  outcnt = 0;
    char inbuf[512], outbuf[512];

    /* convert buffer */
    int nbread = int( ::fread( inbuf, 1, sizeof( inbuf ), src ) );
    for ( int incnt = 0; incnt < nbread; incnt++  )
    {
      if ( waitingLF )
      {
        waitingLF = false;
        if ( inbuf[incnt] == LF )
          outbuf[outcnt++] = LF;
        else
          outbuf[outcnt++] = CR;
      }
      else if ( inbuf[incnt] == CR )
        waitingLF = true;
      else
        outbuf[outcnt++] = inbuf[incnt];
    }

    /* check last sym in buffer */
    waitingLF = ( inbuf[nbread - 1] == CR );

    /* write converted buffer to temp file */
    int nbwri = int( ::fwrite( outbuf, 1, outcnt, tgt ) );
    if ( nbwri != outcnt )
    {
      ::fclose( src );
                        ::fclose( tgt );
      QFile::remove( QString( temp ) );
      return false;
    }
    if ( nbread != sizeof( inbuf ) )
      break;              /* converted ok */
  }
  ::fclose( src );
  ::fclose( tgt );

  /* rename temp -> src */
  if ( !QFile::remove( absName ) )
    return false;

  return QDir().rename( QString( temp ), absName );
}

/*!
  \brief Create path completer which can be used in the widgets
  to provide auto completions.

  Create an instance of QCompleter class and returns the pointer on it.
  The calling function is responsible to the desstroying of the created 
  completer object.

  The QCompleter class provides completions based on a item model and can be
  used in such as QLineEdit and QComboBox. 
  When the user starts typing a word, QCompleter suggests possible ways of 
  completing the word, based on a word list. 

  \param type path type (Qtx::PathType)
  \param filter file/directory filters (list of wildcards, separated by ";;")
  \return a pointer to the created completer
*/
QCompleter* Qtx::pathCompleter( const PathType type, const QString& filter )
{
  QStringList extList;
  QStringList filterList = filter.split( ";;" );
  for ( QStringList::const_iterator it = filterList.begin(); it != filterList.end(); ++it )
  {
    QRegExp rx( "[\\s\\w,;]*\\(?\\*\\.([\\w]+)\\)?[\\d\\s\\w]*" );
    int index = 0;
    while ( ( index = rx.indexIn( *it, index ) ) != -1 )
    {
      extList.append( QString( "*.%1" ).arg( rx.cap( 1 ) ) );
      index += rx.matchedLength();
    }
  }

  QDir::Filters filters = 0;
  switch ( type )
  {
  case PT_OpenFile:
  case PT_SaveFile:
    filters = QDir::AllEntries | QDir::AllDirs | QDir::NoDotAndDotDot;
    break;
  case PT_Directory:
    filters = QDir::Drives | QDir::Dirs | QDir::NoDotAndDotDot;
    break;
  }

  QDirModel* dm = new QDirModel( extList, filters, QDir::Unsorted );
  QCompleter* cmp = new QCompleter( dm, 0 );
  dm->setParent( cmp );

  return cmp;
}

/*!
  \brief Parse given string to retrieve environment variable.

  Looks through the string for the patterns: ${name} or $(name) or %name%.
  If string contains variable satisfying any pattern, the variable name
  is returned, start index of the variable is returned in the \a start parameter,
  and length of the variable is returned in the \a len parameter.

  \param str string being processed
  \param start if variable is found, this parameter contains its starting 
         position in the \a str
  \param len if variable is found, this parameter contains its length 
  \return first found variable or null QString if there is no ones
*/
QString Qtx::findEnvVar( const QString& str, int& start, int& len )
{
  QString varName;
  len = 0;

  QRegExp rx( "(^\\$\\{|[^\\$]\\$\\{)([a-zA-Z]+[a-zA-Z0-9_]*)(\\})|(^\\$\\(|[^\\$]\\$\\()([a-zA-Z]+[a-zA-Z0-9_]*)(\\))|(^\\$|[^\\$]\\$)([a-zA-Z]+[a-zA-Z0-9_]*)|(^%|[^%]%)([a-zA-Z]+[a-zA-Z0-9_]*)(%[^%]|%$)" );

  int pos = rx.indexIn( str, start );
  if ( pos != -1 )
  {
    int i = 1;
    while ( i <= rx.captureCount() && varName.isEmpty() )
    {
      QString capStr = rx.cap( i );
      if ( !capStr.contains( "%" ) && !capStr.contains( "$" ) )
        varName = capStr;
      i++;
    }

    if ( !varName.isEmpty() )
    {
      int capIdx = i - 1;
      start = rx.pos( capIdx );
      int end = start + varName.length();
      if ( capIdx > 1 && rx.cap( capIdx - 1 ).contains( QRegExp( "\\$|%" ) ) )
        start = rx.pos( capIdx - 1 ) + rx.cap( capIdx - 1 ).indexOf( QRegExp( "\\$|%" ) );
      if ( capIdx < rx.captureCount() && !rx.cap( capIdx - 1 ).isEmpty() )
        end++;
      len = end - start;
    }
  }
  return varName;
}

/*!
  \brief Substitute environment variables by their values.

  Environment variable is substituted by its value.

  \param str string to be processed
  \return processed string (with all substitutions made)
*/
QString Qtx::makeEnvVarSubst( const QString& str, const SubstMode mode )
{
  QString res = str;
  if ( mode != Never )
  {
    QMap<QString, int> ignoreMap;

    int start( 0 ), len( 0 );
    while ( true )
    {
      QString envName = findEnvVar( res, start, len );
      if ( envName.isNull() )
        break;

      QString newStr;
      if ( ::getenv( envName.toLatin1() ) || mode == Always )
        newStr = QString( ::getenv( envName.toLatin1() ) );

      if ( newStr.isNull() )
      {
        if ( ignoreMap.contains( envName ) )
        {
          start += len;
          continue;
        }
        ignoreMap.insert( envName, 0 );
      }
      res.replace( start, len, newStr );
    }

    res.replace( "$$", "$" );
    res.replace( "%%", "%" );
  }

  return res;
}

/*!
  \brief Pack the specified color into integer RGB set.
  \param c unpacked color
  \return packed color
*/
int Qtx::rgbSet( const QColor& c )
{
  return rgbSet( c.red(), c.green(), c.blue() );
}

/*!
  \brief Pack the specified RGB color components into integer RGB set.
  \param r red component
  \param g green component
  \param b blue component
  \return packed color
*/
int Qtx::rgbSet( const int r, const int g, const int b )
{
  return ( ( ( 0xff & r ) << 16 ) + ( ( 0xff & g ) << 8 ) + ( 0xff & b ) );
}

/*!
  \brief Unpack the specified integer RGB set to the color.
  \param rgb packed color
  \return unpacked color (QColor)
*/
QColor Qtx::rgbSet( const int rgb )
{
  int r, g, b;
  rgbSet( rgb, r, g, b );
  return QColor( r, g, b );
}

/*!
  \brief Unpack the specified integer RGB set to the three RGB components.
  \param rgb packed color
  \param r returned unpacked red component
  \param g returned unpacked green component
  \param b returned unpacked blue component
*/
void Qtx::rgbSet( const int rgb, int& r, int& g, int& b )
{
  r = ( rgb >> 16 ) & 0xff;
  g = ( rgb >> 8 ) & 0xff;
  b = rgb & 0xff;
}

/*!
  \brief Return the color specified by the index between min (blue) and max (red).
  \param index color index
  \param min required minimum hue value
  \param max required maximum hue value
  \return resulting color
*/
QColor Qtx::scaleColor( const int index, const int min, const int max )
{
  static const int HUE[10] = {230, 210, 195, 180, 160, 80, 60, 50, 30, 0};

  int hue = HUE[0];

  if ( min != max )
  {
    double aPosition = 9.0 * ( index - min ) / ( max - min );
    if ( aPosition > 0.0 )
    {
      if ( aPosition >= 9.0 )
        hue = HUE[9];
      else
      {
        int idx = (int)aPosition;
        hue = HUE[idx] + int( ( aPosition - idx ) * ( HUE[idx + 1] - HUE[idx] ) );
      }
    }
  }

  return QColor::fromHsv( hue, 255, 255 );
}

/*!
  \brief Generate required number of colors aligned from blue to red.
  \param num required number of colors
  \param lst returned set of colors
*/
void Qtx::scaleColors( const int num, QColorList& lst )
{
  lst.clear();
  for ( int i = 0; i < num; i++ )
    lst.append( scaleColor( i, 0, num - 1 ) );
}

/*!
  \brief Scale the pixmap to the required size.

  If \a h is 0 (default) the value of \a w is used instead (to create
  square pixmap).

  \param icon pixmap to be resized
  \param w required pixmap width
  \param h required pixmap height
  \return scaled pixmap
*/
QPixmap Qtx::scaleIcon( const QPixmap& icon, const unsigned w, const unsigned h )
{
  QPixmap p;
  int aw = w, ah = h <= 0 ? w : h;
  if ( icon.isNull() || aw <= 0 || ah <= 0 || ( aw == icon.width() && ah == icon.height() ) )
    p = icon;
  else
    p = icon.fromImage( icon.toImage().scaled( aw, ah, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
  return p;
}

/*!
  \brief Convert given image to the grayscale format.
  \param img initial image
  \return converted to the grayscale image
*/
QImage Qtx::grayscale( const QImage& img )
{
  QImage res = img;

  int colNum = res.colorCount();
  if ( colNum )
  {
    for ( int i = 0; i < colNum; i++ )
      res.setColor( i, qGray( res.color( i ) ) );
  }
  else
  {
    for ( int y = 0; y < res.height(); y++ )
    {
      for ( int x = 0; x < res.width(); x++ )
      {
        QRgb pix = res.pixel( x, y );
        res.setPixel( x, y, qRgba( qGray( pix ), qGray( pix ), qGray( pix ), qAlpha( pix ) ) );
      }
    }
  }

  return res;
}

/*!
  \brief Convert given pixmap to the grayscale format.
  \param pix initial pixmap
  \return converted to the grayscale pixmap
*/
QPixmap Qtx::grayscale( const QPixmap& pix )
{
  QPixmap res;
  res.fromImage( grayscale( pix.toImage() ) );
  return res;
}

/*!
  \brief Create transparent image.
  \param w required image width
  \param h required image height
  \param d required image depth
  \return generated image
*/
QImage Qtx::transparentImage( const int w, const int h, const int d )
{
  QImage::Format fmt;
  switch ( d )
  {
  case 1:
    fmt = QImage::Format_Mono;
    break;
  case 8:
    fmt = QImage::Format_Indexed8;
    break;
  case 16:
  case 24:
  case 32:
  default:
    fmt = QImage::Format_ARGB32;
    break;
  }

  QImage img( w, h, fmt );
  if ( !img.isNull() )
  {
//    img.setAlphaBuffer( true );
    for ( int i = 0; i < img.height(); i++ )
      for ( int j = 0; j < img.width(); j++ )
        img.setPixel( j, i, qRgba( 0, 0, 0, 0 ) );
  }
  return img;
}

/*!
  \brief Create transparent pixmap.
  \param w required image width
  \param h required pixmap height
  \param d required pixmap depth
  \return generated pixmap
*/
QPixmap Qtx::transparentPixmap( const int w, const int h, const int d )
{
  QPixmap pix;
  QImage img = transparentImage( w, h, d );
  if ( !img.isNull() )
    pix.fromImage( img );
  return pix;
}

/*!
  \brief Create composite pixmap. 

  Pixmap \a pix is drawn over pixmap \a dest with coordinates
  specified relatively to the upper left corner of \a dest.
  If \a dest is not given, the new empty pixmap with appropriate size created instead.

  \param pix source pixmap
  \param x horizontal shift
  \param y vertical shift
  \param dest background pixmap
  \return resulting pixmap
*/
QPixmap Qtx::composite( const QPixmap& pix, const int x, const int y, const QPixmap& dest )
{
  if ( pix.isNull() )
    return dest;

  int width = qMax( pix.width() + x, dest.width() );
  int height = qMax( pix.height() + y, dest.height() );

  QPixmap res( width, height );
  QImage img = transparentImage( width, height, 32 );

  QPainter p;
  p.begin( &res );
  p.fillRect( 0, 0, width, height, QBrush( Qt::white ) );

  if ( !dest.isNull() )
  {
    p.drawPixmap( 0, 0, dest );
    QImage temp = dest.toImage();
    for ( int i = 0; i < temp.width() && i < img.width(); i++ )
    {
      for ( int j = 0; j < temp.height() && j < img.height(); j++ )
      {
        if ( temp.hasAlphaChannel() )
          img.setPixel( i, j, temp.pixel( i, j ) );
        else
        {
          QRgb color = temp.pixel( i, j );
          img.setPixel( i, j, qRgba( qRed( color ), qGreen( color ), qBlue( color ), 255 ) );
        }
      }
    }
  }

  p.drawPixmap( x, y, pix );
  QImage temp = pix.toImage();
  for ( int c = x; c < temp.width() + x && c < img.width(); c++ )
  {
    for ( int r = y; r < temp.height() + y && r < img.height(); r++ )
    {
      if ( qAlpha( temp.pixel( c - x, r - y ) ) > 0 )
        img.setPixel( c, r, temp.pixel( c - x, r - y ) );
    }
  }

  p.end();

  for ( int ai = 0; ai < img.width(); ai++ )
  {
    for ( int aj = 0; aj < img.height(); aj++ )
    {
      if ( qAlpha( img.pixel( ai, aj ) ) < 1 )
        img.setPixel( ai, aj, qRgba( 255, 255, 255, 255 ) );
      else
        img.setPixel( ai, aj, qRgba( 0, 0, 0, 0 ) );
    }
  }

  QBitmap bmp( width, height );
  bmp.fromImage( img, Qt::ColorMode_Mask | Qt::ThresholdDither );
  res.setMask( bmp );

  return res;
}

/*!
  \brief Convert color to the string representation.
  
  The resulting string is in the one of two possible formats
  (\c RR, \c GG, \c BB and \c AA value represent red, green, blue
  and alpha components of the color):
  - if color has alpha channel : "#RR,#GG,#BB,#AA"
  - if color does not have alpha channel : "#RRGGBB" 

  If color is invalid, null string is returned.

  Backward conversion can be done with stringToColor() method.

  \param color color to be converted
  \return string representation of the color

  \sa stringToColor()
*/
QString Qtx::colorToString( const QColor& color )
{
  QString str;
  if ( color.isValid() )
  {
    if ( color.alpha() != 255 )
    {
      QStringList vals;
      vals << QString( "#%1" ).arg( color.red(),   0, 16 );
      vals << QString( "#%1" ).arg( color.green(), 0, 16 );
      vals << QString( "#%1" ).arg( color.blue(),  0, 16 );
      vals << QString( "#%1" ).arg( color.alpha(), 0, 16 );
      str = vals.join( "," );
    }
    else
    {
      str = color.name();
    }
  }
  return str;
}

/*!
  \brief Create color from the string representation.
  
  The parameter \a str must be in the one of following formats
  (\c RR, \c GG, \c BB and \c AA value represent red, green, blue
  and alpha components of the color):
  - "#RR,#GG,#BB[,#AA]" or "#RR #GG #BB[ #AA]" (\c RR, \c GG, \c BB
  and optional \c AA values represent red, green, blue and alpha
  components of the color in hexadecimal form)
  - "RR,GG,BB[,AA]" or "RR GG BB[ AA]" (\c RR, \c GG, \c BB
  and optional \c AA values represent red, green, blue and alpha
  components of the color in decimal form)
  - "#RRGGBB" - (\c RR, \c GG and \c BB values represent red, green and blue
  components of the color in hexadecimal form)
  - an integer value representing packed color components (see rgbSet())
  - a name from the list of colors defined in the list of SVG color keyword names
  provided by the World Wide Web Consortium; for example, "steelblue" or "gainsboro".

  Backward conversion can be done with colorToString() method.

  \param str string representation of the color
  \param color resulting color value
  \return \c true if the conversion is successful and \c false otherwise

  \sa colorToString(), rgbSet()
*/
bool Qtx::stringToColor( const QString& str, QColor& color )
{
  bool res = true;
  QStringList vals = str.split( QRegExp( "[\\s|,]" ), QString::SkipEmptyParts );

  QIntList nums;
  for ( QStringList::const_iterator it = vals.begin(); it != vals.end() && res; ++it )
  {
    int num = 0;
    if ( (*it).startsWith( "#" ) )
      num = (*it).mid( 1 ).toInt( &res, 16 );
    else
      num = (*it).toInt( &res, 10 );
    if ( res )
      nums.append( num );
  }

  res = res && nums.count() >= 3;
  if ( res )
    color.setRgb( nums[0], nums[1], nums[2] );

  if ( !res )
  {
    int pack = str.toInt( &res );
    if ( res )
      color = Qtx::rgbSet( pack );
  }

  if ( !res )
  {
    color = QColor( str );
    res = color.isValid();
  }

  return res;
}

/*!
  \brief Convert bi-color value to the string representation.
  
  Bi-color value is specified as main color and integer delta
  value that is used to calculate secondary color by changing
  paremeters of the main color ("saturation" and "value"
  components in HSV notation).

  The resulting string consists of two sub-strings separated by
  '|' symbol. The first part represents main color
  (see colorToString() for more details), the second part is a
  delta value.

  Backward conversion can be done with stringToBiColor() method.

  \param color color to be converted
  \param delta delta value
  \return string representation of the bi-color value

  \sa stringToBiColor(), stringToColor()
*/
QString Qtx::biColorToString( const QColor& color, const int delta )
{
  return QString("%1|%2").arg( Qtx::colorToString( color ) ).arg( delta );
}

/*!
  \brief Restore bi-color value from the string representation.

  Bi-color value is specified as main color and integer delta
  value that is used to calculate secondary color by changing
  paremeters of the main color ("saturation" and "value"
  components in HSV notation).

  The parameter \a str should consist of two sub-strings separated
  by '|' symbol. The first part represents main color
  (see stringToColor() for more details), the second part is a
  delta value.

  Backward conversion can be done with biColorToString() method.

  \param str string representation of the bi-color value
  \param color resulting color value
  \param delta resulting delta value
  \return \c true if the conversion is successful and \c false otherwise

  \sa biColorToString(), stringToColor(), rgbSet()
*/
bool Qtx::stringToBiColor( const QString& str, QColor& color, int& delta )
{
  QStringList data = str.split( "|", QString::KeepEmptyParts );
  QColor c;
  int d = 0;
  bool ok = data.count() > 0 && Qtx::stringToColor( data[0], c );
  bool dok = false;
  if ( data.count() > 1 ) d = data[1].toInt( &dok );
  ok = ok && dok;
  color = ok ? c : QColor();
  delta = ok ? d : 0;
  return ok;
}

/*!
  \brief Compute secondary color value from specified main color
  and delta.

  Secondary color is calculated by changing paremeters of the main
  color ("saturation" and "value" components in HSV notation) using
  specified delta.

  If main color is invalid, result of the function is also invalid color.

  \param color source main color
  \param delta delta value
  \return resulting secondary color
  
  \sa biColorToString(), stringToBiColor()
*/
QColor Qtx::mainColorToSecondary( const QColor& color, int delta )
{
  QColor cs = color;
  if ( cs.isValid() ) {
    int val = qMin( 255, qMax( cs.value() + delta, 0 ) );
    int sat = qMin( 255, qMax( cs.saturation() + delta-(val-cs.value()), 0 ) );
#ifdef BICOLOR_CHANGE_HUE
    const int BICOLOR_HUE_MAXDELTA = 40;
    int dh = delta-(val-cs.value())-(sat-cs.saturation());
    dh = qMin( BICOLOR_HUE_MAXDELTA, qAbs( dh ) ) * ( dh > 0 ? 1 : -1 );
    //int hue = qMin( 359, qMax( cs.hue() + delta-(val-cs.value())-(sat-cs.saturation()), 0 ) );
    //int hue = qMin( 359, qMax( cs.hue() + delta-(val-cs.value())-ds, 0 ) );
    int hue = cs.hue() + dh;
    if ( hue < 0 ) hue = 360 - hue;
#else
    int hue = cs.hue();
#endif
    cs.setHsv( hue, sat, val );
  }
  return cs;
}

/*!
  \brief Dump linear gradient to the string description.
  \param gradient linear gradient to be converted
  \return string representation of the linear gradient
  \sa stringToLinearGradient()
*/
QString Qtx::gradientToString( const QLinearGradient& gradient )
{
  QStringList data;
  data << "linear";
  data << QString::number( gradient.start().x() );
  data << QString::number( gradient.start().y() );
  data << QString::number( gradient.finalStop().x() );
  data << QString::number( gradient.finalStop().y() );
  switch( gradient.spread() ) 
  {
  case QGradient::PadSpread:
    data << "pad";
    break;
  case QGradient::RepeatSpread:
    data << "repeat";
    break;
  case QGradient::ReflectSpread:
    data << "reflect";
    break;
  default:
    break;
  }
  QGradientStops stops = gradient.stops();
  QGradientStop stop;
  foreach ( stop, stops ) 
  {
    data << QString::number( stop.first );
    data << colorToString( stop.second );
  }
  return data.join( "|" );
}

/*!
  \brief Dump radial gradient to the string description.
  \param gradient radial gradient to be converted
  \return string representation of the radial gradient
  \sa stringToRadialGradient()
*/
QString Qtx::gradientToString( const QRadialGradient& gradient )
{
  QStringList data;
  data << "radial";
  data << QString::number( gradient.center().x() );
  data << QString::number( gradient.center().y() );
  data << QString::number( gradient.focalPoint().x() );
  data << QString::number( gradient.focalPoint().y() );
  data << QString::number( gradient.radius() );
  switch( gradient.spread() ) 
  {
  case QGradient::PadSpread:
    data << "pad";
    break;
  case QGradient::RepeatSpread:
    data << "repeat";
    break;
  case QGradient::ReflectSpread:
    data << "reflect";
    break;
  default:
    break;
  }
  QGradientStops stops = gradient.stops();
  QGradientStop stop;
  foreach ( stop, stops ) 
  {
    data << QString::number( stop.first );
    data << colorToString( stop.second );
  }
  return data.join( "|" );
}

/*!
  \brief Dump conical gradient to the string description.
  \param gradient conical gradient to be converted
  \return string representation of the conical gradient
  \sa stringToConicalGradient()
*/
QString Qtx::gradientToString( const QConicalGradient& gradient )
{
  QStringList data;
  data << "conical";
  data << QString::number( gradient.center().x() );
  data << QString::number( gradient.center().y() );
  data << QString::number( gradient.angle() );
  switch( gradient.spread() ) 
  {
  case QGradient::PadSpread:
    data << "pad";
    break;
  case QGradient::RepeatSpread:
    data << "repeat";
    break;
  case QGradient::ReflectSpread:
    data << "reflect";
    break;
  default:
    break;
  }
  QGradientStops stops = gradient.stops();
  QGradientStop stop;
  foreach ( stop, stops ) 
  {
    data << QString::number( stop.first );
    data << colorToString( stop.second );
  }
  return data.join( "|" );
}

/*!
  \brief Create linear gradient from its string representation.
  \param str string representation of the linear gradient
  \param gradient resulting linear gradient object
  \return \c true if the conversion is successful and \c false otherwise
  \sa gradientToString()
*/
bool Qtx::stringToLinearGradient( const QString& str, QLinearGradient& gradient )
{
  bool success = false;
  QStringList vals = str.split( "|", QString::SkipEmptyParts );
  if ( vals.count() > 4 && ( vals[0] == "linear" || vals[0] == "lg" ) )
  {
    // start and end points 
    double x1, y1, x2, y2;
    bool bOk1, bOk2, bOk3, bOk4;
    x1 = vals[1].toDouble( &bOk1 );
    y1 = vals[2].toDouble( &bOk2 );
    x2 = vals[3].toDouble( &bOk3 );
    y2 = vals[4].toDouble( &bOk4 );
    if ( bOk1 && bOk2 && bOk3 && bOk4 )
    {
      gradient = QLinearGradient( x1, y1, x2, y2 );
      // spread type
      if ( vals.count() > 5 )
      {
        QString spread = vals[ 5 ].trimmed().toLower();
        if ( spread == "pad" || spread == "0" )
          gradient.setSpread( QGradient::PadSpread );
        else if ( spread == "repeat" || spread == "2" )
          gradient.setSpread( QGradient::RepeatSpread );
        else if ( spread == "reflect" || spread == "1" )
          gradient.setSpread( QGradient::ReflectSpread );
      }
      // stop points
      QGradientStops stops;
      for ( int i = 6; i < vals.count(); i+=2 )
      {
        bool bOk5, bOk6 = false;
        QColor c;
        double stop = vals[i].toDouble( &bOk5 );
        if ( i+1 < vals.count() )
          bOk6 = stringToColor( vals[ i+1 ], c );
        if ( bOk5 && stop >= 0.0 && stop <= 1.0 && bOk6 && c.isValid() )
          stops.append( QGradientStop( stop, c ) );
      }
      gradient.setStops( stops );
      success = true;
    }
  }
  return success;
}

/*!
  \brief Create radial gradient from its string representation.
  \param str string representation of the radial gradient
  \param gradient resulting radial gradient object
  \return \c true if the conversion is successful and \c false otherwise
  \sa gradientToString()
*/
bool Qtx::stringToRadialGradient( const QString& str, QRadialGradient& gradient )
{
  bool success = false;
  QStringList vals = str.split( "|", QString::SkipEmptyParts );
  if ( ( vals.count() > 5 && vals[0] == "radial" ) || vals[0] == "rg" ) 
  {
    // center, radius and focal point
    double cx, cy, r, fx, fy;
    bool bOk1, bOk2, bOk3, bOk4, bOk5;
    cx = vals[1].toDouble( &bOk1 );
    cy = vals[2].toDouble( &bOk2 );
    fx = vals[3].toDouble( &bOk4 );
    fy = vals[4].toDouble( &bOk5 );
    r  = vals[5].toDouble( &bOk3 );
    if ( bOk1 && bOk2 && bOk3 && bOk4 && bOk5 )
    {
      gradient = QRadialGradient( cx, cy, r, fx, fy );
      // spread type
      if ( vals.count() > 6 )
      {
        QString spread = vals[ 6 ].trimmed().toLower();
        if ( spread == "pad" || spread == "0" )
          gradient.setSpread( QGradient::PadSpread );
        else if ( spread == "repeat" || spread == "2" )
          gradient.setSpread( QGradient::RepeatSpread );
        else if ( spread == "reflect" || spread == "1" )
          gradient.setSpread( QGradient::ReflectSpread );
      }
      // stop points
      QGradientStops stops;
      for ( int i = 7; i < vals.count(); i+=2 )
      {
        bool bOk7, bOk8 = false;
        QColor c;
        double stop = vals[i].toDouble( &bOk7 );
        if ( i+1 < vals.count() )
          bOk8 = stringToColor( vals[ i+1 ], c );
        if ( bOk7 && stop >= 0.0 && stop <= 1.0 && bOk8 && c.isValid() )
          stops.append( QGradientStop( stop, c ) );
      }
      gradient.setStops( stops );
      success = true;
    }
  }
  return success;
}

/*!
  \brief Create conical gradient from its string representation.
  \param str string representation of the conical gradient
  \param gradient resulting conical gradient object
  \return \c true if the conversion is successful and \c false otherwise
  \sa gradientToString()
*/
bool Qtx::stringToConicalGradient( const QString& str, QConicalGradient& gradient )
{
  bool success = false;
  QStringList vals = str.split( "|", QString::SkipEmptyParts );
  if ( ( vals.count() > 3 && vals[0] == "conical" ) || vals[0] == "cg" ) 
  {
    // center and angle
    double cx, cy, a;
    bool bOk1, bOk2, bOk3;
    cx = vals[1].toDouble( &bOk1 );
    cy = vals[2].toDouble( &bOk2 );
    a = vals[3].toDouble( &bOk3 );
    if ( bOk1 && bOk2 && bOk3 )
    {
      gradient = QConicalGradient( cx, cy, a );
      // spread type
      if ( vals.count() > 4 )
      {
        QString spread = vals[ 4 ].trimmed().toLower();
        if ( spread == "pad" || spread == "0" )
          gradient.setSpread( QGradient::PadSpread );
        else if ( spread == "repeat" || spread == "2" )
          gradient.setSpread( QGradient::RepeatSpread );
        else if ( spread == "reflect" || spread == "1" )
          gradient.setSpread( QGradient::ReflectSpread );
      }
      // stop points
      QGradientStops stops;
      for ( int i = 5; i < vals.count(); i+=2 )
      {
        bool bOk4, bOk5 = false;
        QColor c;
        double stop = vals[i].toDouble( &bOk4 );
        if ( i+1 < vals.count() )
          bOk5 = stringToColor( vals[ i+1 ], c );
        if ( bOk4 && stop >= 0.0 && stop <= 1.0 && bOk5 && c.isValid() )
          stops.append( QGradientStop( stop, c ) );
      }
      gradient.setStops( stops );
      success = true;
    }
  }
  return success;
}

/*!
  \class Qtx::Localizer
  \brief Localization helper

  This helper class can be used to solve the localization problems,
  usually related to the textual files reading/writing, namely when
  floating point values are read / written with API functions.
  The problem relates to such locale specific settings as decimal point
  separator, thousands separator, etc.
  
  To use the Localizer class, just create a local variable in the beginning
  of the code where you need to read / write data from textual file(s).
  The constructor of the class forces setting "C" locale temporariy.
  The destructor switches back to the initial locale.

  \code
  Qtx::Localizer loc;
  readSomething();
  writeSomething();
  \endcode
*/

/*!
  \brief Constructor. Forces "C" locale to be set.
*/
Qtx::Localizer::Localizer()
{
  myCurLocale = setlocale( LC_NUMERIC, 0 );
  setlocale( LC_NUMERIC, "C" );
}

/*!
  \brief Destructor. Reverts back to the initial locale.
*/
Qtx::Localizer::~Localizer()
{
  setlocale( LC_NUMERIC, myCurLocale.toLatin1().constData() );
}

#ifndef WIN32

#include <X11/Xlib.h>
#include <GL/glx.h>

/*!
  \brief Open the default X display and returns pointer to it.
         This method is available on Linux only.
  \return Pointer to X display.
  \sa getVisual()
*/
void* Qtx::getDisplay()
{
  static Display* pDisplay = NULL;
  if ( !pDisplay )
    pDisplay = XOpenDisplay( NULL );
  return pDisplay;
}

/*!
  \brief Returns pointer to X visual suitable for 3D rendering.
         This method is available on Linux only.
  \return Pointer to X visual.
  \sa getDisplay()
*/
Qt::HANDLE Qtx::getVisual()
{
  Qt::HANDLE res = (Qt::HANDLE)NULL;

  Display* pDisplay = (Display*)getDisplay();
  if ( !pDisplay )
    return res;

  int errorBase;
  int eventBase;

  // Make sure OpenGL's GLX extension supported
  if( !glXQueryExtension( pDisplay, &errorBase, &eventBase ) ){
    qCritical( "Could not find glx extension" );
    return res;
  }

  // Find an appropriate visual

  int doubleBufferVisual[]  = {
    GLX_RGBA,           // Needs to support OpenGL
    GLX_DEPTH_SIZE, 16, // Needs to support a 16 bit depth buffer
    GLX_DOUBLEBUFFER,   // Needs to support double-buffering
    None                // end of list
  };

  // Try for the double-bufferd visual first
  XVisualInfo *visualInfo = NULL;
  visualInfo = glXChooseVisual( pDisplay, DefaultScreen(pDisplay), doubleBufferVisual );

  if( visualInfo == NULL ){
    qCritical( "Could not find matching glx visual" );
    return res;
  }

  qDebug() << "Picked visual 0x" << hex << XVisualIDFromVisual( visualInfo->visual );
  res = (Qt::HANDLE)( visualInfo->visual );
 
  return res;
}
#endif // WIN32
