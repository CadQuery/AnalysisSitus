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
#include <asiUI_DatumPathEdit.h>

// asiUI includes
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_DictionaryItem.h>

// Qt includes
#pragma warning(push, 0)
#include <QLineEdit>
#include <QCompleter>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QToolButton>
#include <QRegExpValidator>
#pragma warning(pop)

//! Constructor.
//! \param theDicID [in] id of the dictionary item for datum.
//! \param theParent [in] parent widget for subwidget controls.
//! \param theDatumFlags [in] datum subwidget and option flags.
asiUI_DatumPathEdit::asiUI_DatumPathEdit(const QString& theDicID, 
                                           QWidget* theParent,
                                           const int theDatumFlags)
: asiUI_Datum(theParent)
{
  m_pEditor = new Editor(theDicID, theParent, convertFlags(theDatumFlags));

  init(m_pEditor);

  connect(m_pEditor, SIGNAL(PathSelected()),    this, SIGNAL(EditingFinished()));
  connect(m_pEditor, SIGNAL(ReturnPressed()),   this, SIGNAL(EditingFinished()));
  connect(m_pEditor, SIGNAL(EditingFinished()), this, SIGNAL(EditingFinished()));
}

//! Get type of path.
//! \return path type value.
asiUI_DatumPathEdit::PathType asiUI_DatumPathEdit::GetPathType() const
{
  return (PathType)fromEditorPath((int)m_pEditor->GetPathType());
}

//! Get type of path.
//! \return file extension filter string.
QString asiUI_DatumPathEdit::GetFilter() const
{
  return m_pEditor->GetFilter();
}

//! Get file dialog's title.
//! \return title string.
QString asiUI_DatumPathEdit::GetTitle() const
{
  return m_pEditor->GetTitle();
}

//! Set path type for the file path editor.
//! 1) OpenPath - path to open the file (when clicking on button
//!    appropriate dialog will be shown).
//! 2) SavePath - path to save the file (when clicking on button
//!    appropriate dialog will be shown).
//! 3) DirPath - directory selector (when clicking on button
//!    appropriate dialog will be shown).
//! \param thePathType [in] path type
void asiUI_DatumPathEdit::SetPathType(const PathType thePathType)
{
  m_pEditor->SetPathType((Qtx::PathType)toEditorPath(thePathType));
}

//! Set file extension filter string.
//! The filter format should be given in qt style 
//! (ex. "IGES files (*.igs *.iges);;All files (*.*)" )
//! \param theFilter [in] extension filter string.
void asiUI_DatumPathEdit::SetFilter(const QString& theFilter)
{
  m_pEditor->SetFilter(theFilter);
}

//! Set title string for the file dialog.
//! \param theTitle [in] file dialog title.
void asiUI_DatumPathEdit::SetTitle(const QString& theTitle)
{
  m_pEditor->SetTitle(theTitle);
}

//! Convert asiUI_DatumPathEdit path type to suit path type.
//! \param thePathType [in] asiUI_DatumPathEdit path type.
//! \return suit path type.
int asiUI_DatumPathEdit::toEditorPath(int thePathType) const
{
  switch (thePathType)
  {
    case SavePath : return Qtx::PT_SaveFile;
    case DirPath : return Qtx::PT_Directory;
  }

  return Qtx::PT_OpenFile;
}

//! Convert suit path type to asiUI_DatumPathEdit path type.
//! \param theEditorPath [in] suit path type.
//! \return asiUI_DatumPathEdit path type.
int asiUI_DatumPathEdit::fromEditorPath(int theEditorPath) const
{
  switch (theEditorPath)
  {
    case Qtx::PT_SaveFile  : return SavePath;
    case Qtx::PT_Directory : return DirPath;
  }

  return OpenPath;
}

//! Update unit system.
void asiUI_DatumPathEdit::onUnitsUpdated()
{
  m_pEditor->UpdateUnits();
}

void asiUI_DatumPathEdit::SetEnabled(const bool theIsEnabled,
                                     const int  theWidgets)
{
  m_pEditor->GetLineEdit()->setReadOnly (!theIsEnabled);
  asiUI_Datum::SetEnabled( theIsEnabled, theWidgets );
}

//! Get datum.
//! \return suit datum pointer.
QDS_Datum* asiUI_DatumPathEdit::getDatum() const
{
  return m_pEditor;
}

//! Hilight edited string with the specified color.
//! \param theColor [in] the color to hilight.
void asiUI_DatumPathEdit::colorizeText(const QColor& theColor)
{
  QLineEdit* aLineEdit = m_pEditor->GetLineEdit();
  if ( !aLineEdit )
    return;

  QPalette aColorizedPalette = aLineEdit->palette();

  aColorizedPalette.setColor(QPalette::Active, QPalette::Text, theColor);

  aLineEdit->setPalette(aColorizedPalette);
}

//! Set the start directory path.
//! \param thePath [in] the path to start directory.
void asiUI_DatumPathEdit::SetDirectory(const QString& thePath)
{
  m_pEditor->SetDirectory(thePath);
}

//! Return true if the start location of the file dialog is the project directory.
//! \return true if location is project, otherwise - false.
bool asiUI_DatumPathEdit::IsProjectLocation()
{
  return m_pEditor->IsProjectLocation();
}

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

static const char* browse_icon[] = {
"16 16 5 1",
"  c none",
". c #ffff00",
"# c #848200",
"a c #ffffff",
"b c #000000",
"                ",
"          bbb   ",
"         b   b b",
"              bb",
"  bbb        bbb",
" ba.abbbbbbb    ",
" b.a.a.a.a.b    ",
" ba.a.a.a.ab    ",
" b.a.abbbbbbbbbb",
" ba.ab#########b",
" b.ab#########b ",
" bab#########b  ",
" bb#########b   ",
" bbbbbbbbbbb    ",
"                ",
"                "
};

//! Constructor. Initializes controls and dialog mode
asiUI_DatumPathEdit::Editor::Editor(const QString& theDicID,
                                     QWidget* theParent,
                                     const int theFlags)
: QDS_Datum(theDicID, theParent, theFlags),
  m_Type(Qtx::PT_OpenFile)
{
  Handle(asiAlgo_DictionaryItem) anItem = 
    Handle(asiAlgo_DictionaryItem)::DownCast(dicItem());

  if ( anItem.IsNull() )
    return;

  if ( anItem->GetWidgetType() == asiAlgo_DictionaryItem::WT_SaveFilePath )
    m_Type = Qtx::PT_SaveFile;
  else if ( anItem->GetWidgetType() == asiAlgo_DictionaryItem::WT_DirPath )
    m_Type = Qtx::PT_Directory;
  
  // widget parameters
  const asiUI_WidgetParameterMap& aWidgetParams = anItem->GetWidgetParams();

  // load filter string parameter
  TCollection_AsciiString aFilterStr("filter");
  if ( aWidgetParams.IsBound(aFilterStr) )
    SetFilter(asiUI_Common::ToQString(aWidgetParams.Find(aFilterStr)));

  TCollection_AsciiString aTitleStr("title");
  if ( aWidgetParams.IsBound(aTitleStr) )
    SetTitle(asiUI_Common::ToQString(aWidgetParams.Find(aTitleStr)));

  m_bIsProjectLoc = false;
  TCollection_AsciiString aLocateStr("location");
  if ( aWidgetParams.IsBound(aLocateStr) )
  {
    if (aWidgetParams.Find(aLocateStr) == "project")
      m_bIsProjectLoc = true;
  }
}

//! Update system of units.
void asiUI_DatumPathEdit::Editor::UpdateUnits()
{
  unitSystemChanged( QString() );
}

//! set path completer to the editor line
void asiUI_DatumPathEdit::Editor::setPathCompleter()
{
  QLineEdit* aLineEdit = lineEdit();
  if ( aLineEdit != 0 )
    aLineEdit->setCompleter( Qtx::pathCompleter( m_Type, m_Filter ) );
}

//! start open file dialog
void asiUI_DatumPathEdit::Editor::onOpenDialog()
{
  QFileDialog aDialog(widget(QDS::Control));

  switch ( GetPathType() )
  {
    case Qtx::PT_OpenFile:
      aDialog.setAcceptMode(QFileDialog::AcceptOpen);
      aDialog.setFileMode(QFileDialog::AnyFile);
      break;
    case Qtx::PT_SaveFile:
      aDialog.setAcceptMode(QFileDialog::AcceptSave);
      aDialog.setFileMode(QFileDialog::AnyFile);
      break;
    case Qtx::PT_Directory:
      aDialog.setAcceptMode(QFileDialog::AcceptOpen);
      aDialog.setFileMode(QFileDialog::DirectoryOnly);
      break;
  }

  if (!m_StartDir.isEmpty())
    aDialog.setDirectory(m_StartDir);

  aDialog.setNameFilter(GetFilter());
  aDialog.setWindowTitle(GetTitle());

  QLineEdit* aLineEdit = lineEdit();

  if (!aDialog.exec())
  {
    // return focus back and exit
    if ( aLineEdit != 0 )
      aLineEdit->setFocus();

    return;
  }

  // return focus back
  if ( aLineEdit != 0 )
    aLineEdit->setFocus();

  QStringList aFileList = aDialog.selectedFiles();
  if ( aFileList.count() > 0 )
    setValue(QDir::toNativeSeparators ( aFileList.first() ));

  emit PathSelected();
}

//! Notify about text changing in the line edit.
//! \param theStr [in] current text in the line edit widget.
void asiUI_DatumPathEdit::Editor::onTextChanged(const QString& theStr)
{
  asiUI_NotUsed(theStr);

  invalidateCache();

  onParamChanged();
  QString aString = getString();
  emit paramChanged();
  emit paramChanged(aString);
}

//! Create control widget
QWidget* asiUI_DatumPathEdit::Editor::createControl(QWidget* theParent)
{
  QFrame* anEditorFrame = new QFrame(theParent);

  QHBoxLayout* aLay = new QHBoxLayout(anEditorFrame);
  aLay->setMargin(0);
  aLay->setSpacing(5);

  // create editor
  QLineEdit* anEditor = new QLineEdit(anEditorFrame);
  anEditor->setObjectName("LineEditor");

  // create browse button
  QToolButton* aBtnBrowse = new QToolButton(anEditorFrame);
  aBtnBrowse->setIcon( QPixmap( browse_icon ) );

  aLay->addWidget(anEditor);
  aLay->addWidget(aBtnBrowse);

  anEditorFrame->setLayout(aLay);

  connect(anEditor,   SIGNAL( returnPressed() ),   this, SIGNAL( ReturnPressed() ));
  connect(anEditor,   SIGNAL( editingFinished() ), this, SIGNAL( EditingFinished() ));
  connect(aBtnBrowse, SIGNAL( clicked( bool ) ),   this, SLOT( onOpenDialog() ));
  connect(anEditor,   SIGNAL( textChanged( const QString& ) ), 
    this, SLOT( onTextChanged( const QString& ) ) );

  anEditorFrame->setFocusProxy(anEditor);

  setPathCompleter();

  return anEditorFrame;
}

//! Get line edit
QLineEdit* asiUI_DatumPathEdit::Editor::lineEdit() const
{
  QWidget* aWidget = widget(QDS::Control);
  return aWidget->findChild<QLineEdit*>("LineEditor");
}

//! Get string value from path edit
//! \return string value
QString asiUI_DatumPathEdit::Editor::getString() const
{
  QLineEdit* anEditor = lineEdit();
  return ( anEditor != 0 ) ? lineEdit()->text() : QString();
}

//! Set string value to path editor
void asiUI_DatumPathEdit::Editor::setString(const QString& theString)
{
  QLineEdit* anEditor = lineEdit();
  if ( anEditor != 0 )
    anEditor->setText(theString);
}

//! Create validator according to the datum type of value.
//! \param theConsiderLimits defines whether the value limits should 
//! be considered during valiation or not.
QValidator* asiUI_DatumPathEdit::Editor::validator(const bool theConsiderLimits) const
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

//! Set the start directory path.
//! \param thePath [in] the path to start directory.
void asiUI_DatumPathEdit::Editor::SetDirectory(const QString& thePath)
{
  m_StartDir = thePath;
}

//! Return true if the start location of the file dialog is the project directory.
//! \return true if location is project, otherwise - false.
bool asiUI_DatumPathEdit::Editor::IsProjectLocation()
{
  return m_bIsProjectLoc;
}
