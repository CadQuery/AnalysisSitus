//-----------------------------------------------------------------------------
// Created on: 21 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Anton Poletaev, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DatumPathItem.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_OverrideCursor.h>

// asiAlgo includes
#include <asiAlgo_DictionaryItem.h>
#include <asiAlgo_Dictionary.h>

// Salome includes
#include <Qtx.h>

// Qt includes
#pragma warning(push, 0)
#include <QEvent>
#include <QPainter>
#include <QLineEdit>
#include <QCompleter>
#include <QFileDialog>
#include <QToolButton>
#include <QRegExpValidator>
#include <QStyleOptionViewItem>
#pragma warning(pop)

//! Constructor.
//! \param theDictId [in] id of the dictionary item for datum.
//! \param theParent [in] parent widget for subwidget controls.
//! \param theFlags [in] datum subwidget and option flags.
asiUI_DatumPathItem::asiUI_DatumPathItem(const QString& theDictId,
                                           QWidget* theParent,
                                           const int theFlags)
: asiUI_Datum(theParent),
  m_PathType(OpenPath)
{
  m_pDatumImpl = new PathEditImpl(theDictId, theParent, convertFlags(theFlags) );
  init(m_pDatumImpl);

  connect( m_pDatumImpl, SIGNAL( ReturnPressed() ), this, SIGNAL( ReturnPressed() ) );
  connect( m_pDatumImpl, SIGNAL( EditingFinished() ), this, SIGNAL( EditingFinished() ) );

  Handle(asiAlgo_DictionaryItem) anItem = 
    asiAlgo_Dictionary::GetDictionaryItem( QStr2AsciiStr(theDictId) );

  if ( anItem.IsNull() )
    return;

  switch ( anItem->GetWidgetType() )
  {
    case asiAlgo_DictionaryItem::WT_OpenFilePath : SetPathType(OpenPath); break;
    case asiAlgo_DictionaryItem::WT_SaveFilePath : SetPathType(SavePath); break;
    case asiAlgo_DictionaryItem::WT_DirPath : SetPathType(DirPath); break;
  };

  // widget parameters
  const asiUI_WidgetParameterMap& aWidgetParams = anItem->GetWidgetParams();

  // load filter string parameter
  TCollection_AsciiString aFilterStr("filter");
  if ( aWidgetParams.IsBound(aFilterStr) )
    SetFilter( AsciiStr2QStr( aWidgetParams.Find(aFilterStr) ) );

  TCollection_AsciiString aTitleStr("title");
  if ( aWidgetParams.IsBound(aTitleStr) )
    SetTitle( AsciiStr2QStr( aWidgetParams.Find(aTitleStr) ) );

  m_bIsProjectLoc = false;
  TCollection_AsciiString aLocateStr("location");
  if ( aWidgetParams.IsBound(aLocateStr) )
  {
    if (aWidgetParams.Find(aLocateStr) == "project")
      m_bIsProjectLoc = true;
  }

  Qtx::PathType aQtxType = Qtx::PT_OpenFile;
  switch ( GetPathType() )
  {
    case OpenPath : aQtxType = Qtx::PT_OpenFile; break;
    case SavePath : aQtxType = Qtx::PT_SaveFile; break;
    case DirPath  : aQtxType = Qtx::PT_Directory; break;
  }

  m_pDatumImpl->SetPathCompleter( Qtx::pathCompleter(aQtxType, m_Filter) ); 
}

//! Get type of path.
//! \return path type value.
asiUI_DatumPathItem::PathType asiUI_DatumPathItem::GetPathType() const
{
  return m_PathType;
}

//! Get type of path.
//! \return file extension filter string.
QString asiUI_DatumPathItem::GetFilter() const
{
  return m_Filter;
}

//! Get file dialog's title.
//! \return title string.
QString asiUI_DatumPathItem::GetTitle() const
{
  return m_Title;
}

//! Set path type for the file path editor.
//! 1) OpenPath - path to open the file (when clicking on button
//!    appropriate dialog will be shown).
//! 2) SavePath - path to save the file (when clicking on button
//!    appropriate dialog will be shown).
//! 3) DirPath - directory selector (when clicking on button
//!    appropriate dialog will be shown).
//! \param thePathType [in] path type
void asiUI_DatumPathItem::SetPathType(const PathType thePathType)
{
  m_PathType = thePathType;
}

//! Set file extension filter string.
//! The filter format should be given in qt style 
//! (ex. "IGES files (*.igs *.iges);;All files (*.*)" )
//! \param theFilter [in] extension filter string.
void asiUI_DatumPathItem::SetFilter(const QString& theFilter)
{
  m_Filter = theFilter;
}

//! Set title string for the file dialog.
//! \param theTitle [in] file dialog title.
void asiUI_DatumPathItem::SetTitle(const QString& theTitle)
{
  m_Title = theTitle;
}

//! Start browsing.
//! \return empty or path string is click is successfull, invalid string
//! is event is dropped.
QString asiUI_DatumPathItem::Browse()
{
  asiUI_OverrideCursor aWaitCursor;

  QFileDialog aDialog;

  switch ( GetPathType() )
  {
    case OpenPath :
      aDialog.setAcceptMode(QFileDialog::AcceptOpen);
      aDialog.setFileMode(QFileDialog::AnyFile);
      break;
    case SavePath :
      aDialog.setAcceptMode(QFileDialog::AcceptSave);
      aDialog.setFileMode(QFileDialog::AnyFile);
      break;
    case DirPath :
      aDialog.setAcceptMode(QFileDialog::AcceptOpen);
      aDialog.setFileMode(QFileDialog::DirectoryOnly);
      break;
  }

  if (!m_StartDir.isEmpty())
    aDialog.setDirectory(m_StartDir);

  aDialog.setNameFilter( GetFilter() );
  aDialog.setWindowTitle( GetTitle() );

  if ( aDialog.exec() != QFileDialog::Accepted )
    return QString();

  QString aFile = QString();
  QStringList aFileList = aDialog.selectedFiles();
  if ( aFileList.count() > 0 )
  {
    aFile = aFileList.first();
    SetValue(QDir::toNativeSeparators (aFile));
  }

  emit PathSelected();

  return aFile;
}

//! Set the start directory path.
//! \param thePath [in] the path to start directory.
void asiUI_DatumPathItem::SetDirectory(const QString& thePath)
{
  m_StartDir = thePath;
}

//! Return true if the start location of the file dialog is the project directory.
//! \return true if location is project, otherwise - false.
bool asiUI_DatumPathItem::IsProjectLocation()
{
  return m_bIsProjectLoc;
}

//! Get datum.
//! \return suit datum pointer.
QDS_Datum* asiUI_DatumPathItem::getDatum() const
{
  return m_pDatumImpl;
}

// ============================================================================
//                         Extended path string editor
// ============================================================================

//! Default constructor.
//! Is simply void.
asiUI_DatumPathItem::PathEditImpl::PathEditImpl(const QString& theDictId,
                                                 QWidget* theParent,
                                                 const int theFlags)
: asiUI_Datum::DatumImpl<QDS_Datum>(theDictId, theParent, theFlags),
  m_PathCompleter(NULL)
{
  //
}

//! Get string value from path edit
//! \return string value
QString asiUI_DatumPathItem::PathEditImpl::getString() const
{
  QLineEdit* aLineEdit = lineEdit();
  return aLineEdit ? aLineEdit->text() : QString();
}

//! Create validator according to the datum type of value.
//! \param theConsiderLimits defines whether the value limits should 
//! be considered during valiation or not.
QValidator* asiUI_DatumPathItem::PathEditImpl::validator(const bool theConsiderLimits) const
{
  if ( type() != DDS_DicItem::String )
  {
    return QDS_Datum::validator(theConsiderLimits);
  }

  return new asiUI_Datum::ExactRegExpValidator(
    QRegExp( "^([^:;<>\\\\/\\*\\?\\\"\\|]{1}|"
             "([A-Z]:[\\\\/]|[\\\\/]{2})[^:;\\*\\?\\\"<>\\|]*|"
             "[^:;\\*\\?\\\"<>\\|]{2,})$|"
             "^$" ), (QObject*)this );
}

//! Set instance of path completer.
//! used for input control.
void asiUI_DatumPathItem::PathEditImpl::SetPathCompleter(QCompleter* theCompleter)
{
  m_PathCompleter = theCompleter;

  QLineEdit* aLineEdit = lineEdit();
  if ( aLineEdit && m_PathCompleter )
    aLineEdit->setCompleter(m_PathCompleter);
}

//! Notify about text changing in the line edit.
//! \param theStr [in] current text in the line edit widget.
void asiUI_DatumPathItem::PathEditImpl::onTextChanged(const QString& theStr)
{
  asiUI_NotUsed(theStr);

  invalidateCache();

  onParamChanged();
  QString aString = getString();
  emit paramChanged();
  emit paramChanged(aString);
}

//! Create control widget.
//! \param theParent [in] the parent widget.
//! \return editor control.
QWidget* asiUI_DatumPathItem::PathEditImpl::createControl(QWidget* theParent)
{
  // create editor
  QLineEdit* anEditor = new QLineEdit(theParent);
  anEditor->setObjectName("LineEditor");

  connect( anEditor, SIGNAL( returnPressed() ), this, SIGNAL( ReturnPressed() ) );
  connect( anEditor, SIGNAL( editingFinished() ), this, SIGNAL( EditingFinished() ) );
  connect( anEditor, SIGNAL( textChanged( const QString& ) ), 
    this, SLOT( onTextChanged( const QString& ) ) );

  if ( m_PathCompleter )
    anEditor->setCompleter(m_PathCompleter);

  return anEditor;
}

//! \return path line editor control.
QLineEdit* asiUI_DatumPathItem::PathEditImpl::lineEdit() const
{
  QWidget* aWidget = widget(QDS::Control);
  return aWidget->findChild<QLineEdit*>("LineEditor");
}

//! Set string value to path editor
//! \param theString [in] the string to set.
void asiUI_DatumPathItem::PathEditImpl::setString(const QString& theString)
{
  QLineEdit* anEditor = lineEdit();
  if ( anEditor )
    anEditor->setText(theString);
}
