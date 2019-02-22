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

#ifndef asiUI_DatumPathEdit_HeaderFile
#define asiUI_DatumPathEdit_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_Datum.h>
#pragma warning(pop)

// Qtx includes
#pragma warning(push, 0)
#include <Qtx.h>
#pragma warning(pop)

// Forward declaration
class QLineEdit;
class QValidator;

//! Datum for editing the paths.
//! The editor control contains line edit field to
//! input the path and a button, which opens path dialog
//! on click.
//! The path being edited can be one of tree type: 
//! 1) OpenPath - path to open the file (when clicking on button
//!    appropriate dialog will be shown).
//! 2) SavePath - path to save the file (when clicking on button
//!    appropriate dialog will be shown).
//! 3) DirPath - directory selector (when clicking on button
//!    appropriate dialog will be shown).
//! The filter string can be set to filter the allowed file
//! extensions.
//! The custom title for the dialog can be set with GetTitle(),
//! SetTitle() methods.
//! 
//! This datum supports "filter" and "title" widgetParams that
//! can be defined in data dictionary. This params corresponds
//! to filter string and dialog title string.
class asiUI_EXPORT asiUI_DatumPathEdit : public asiUI_Datum
{
  Q_OBJECT

  class Editor;

public:

  enum PathType
  {
    OpenPath,
    SavePath,
    DirPath
  };

  virtual void SetEnabled(const bool theIsEnabled,
                          const int  theWidgets = Control);

public:

  asiUI_DatumPathEdit(const QString& theDicID, 
                       QWidget*       theParent = 0,
                       const int      theDatumFlags = All);

  PathType GetPathType() const;

  QString GetFilter() const;

  QString GetTitle() const;

  void SetPathType(const PathType thePathType);

  void SetFilter(const QString& theFilter);

  void SetTitle(const QString& theTitle);

  bool
    IsProjectLocation();

public slots:

  void SetDirectory(const QString& thePath);

signals:

  //! Signal emitted when the path is selected from the
  //! browse dialog box.
  void PathSelected();

  //! This signal is emitted when the Return/Enter
  //! key is pressed.
  void ReturnPressed();

  //! This signal is emitted when the Return/Enter
  //! key is pressed or the line edit loses
  //! focus.
  void EditingFinished();

public slots:

  virtual void onUnitsUpdated();

protected:

  int toEditorPath(int thePathType) const;

  int fromEditorPath(int theEditorPath) const;

  virtual QDS_Datum* getDatum() const;

  virtual void colorizeText(const QColor& theColor);

private:

  Editor* m_pEditor;
};

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! \ingroup GUI
//!
//! editor control for datum.
class asiUI_DatumPathEdit::Editor : public QDS_Datum
{
  Q_OBJECT

public:
  
  Editor(const QString& theDicID,
         QWidget*       theParent = 0,
         const int      theFlags = All);

  void UpdateUnits();

  //! Get path type.
  //! \return qtx path type
  Qtx::PathType GetPathType() const
  {
    return m_Type;
  }

  void SetPathType(const Qtx::PathType theType)
  {
    m_Type = theType;
    setPathCompleter();
  }

  //! Get file filter.
  //! \return filter string.
  QString GetFilter() const
  {
    return m_Filter;
  }

  //! Get file dialog's title.
  //! \return the title string.
  QString GetTitle() const
  {
    return m_Title;
  }

  //! Set file filter.
  //! Warning: Setting this parameter will
  //! override data dictionary filter definition
  //! for this widget.
  void SetFilter(const QString& theFilter)
  {
    m_Filter = theFilter;
    setPathCompleter();
  }
  
  //! Set dialog's title.
  //! Warning: Setting this parameter will
  //! override data dictionary title definition
  //! for this widget.
  void SetTitle(const QString& theTitle)
  {
    m_Title = theTitle;
  }

  virtual QString getString() const;

  QLineEdit* GetLineEdit() { return lineEdit(); }

  virtual QValidator* validator(const bool theConsiderLimits = false) const;

  bool IsProjectLocation();

  void SetDirectory(const QString& thePath);

signals:

  //! Signal emitted when the path is selected from the
  //! browse dialog box.
  void PathSelected();

  //! This signal is emitted when the Return/Enter
  //! key is pressed.
  void ReturnPressed();

  //! This signal is emitted when the Return/Enter
  //! key is pressed or the line edit loses
  //! focus.
  void EditingFinished();

private slots:

  virtual void onOpenDialog();

  void onTextChanged(const QString& theStr);

protected:

  virtual QWidget* createControl(QWidget* theParent);

  QLineEdit* lineEdit() const;

  virtual void setString(const QString& theString);

  void setPathCompleter();

private:
  
  //! filter string
  QString m_Filter;

  //! dialog's title
  QString m_Title;

  //! file dialog mode
  Qtx::PathType m_Type;

  //! start file dialog directory.
  QString m_StartDir;

  //! indicates whether the start location is project directory.
  bool m_bIsProjectLoc;
};

#endif
