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

#ifndef asiUI_DatumPathItem_HeaderFile
#define asiUI_DatumPathItem_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>

// QDS includes
#pragma warning(push, 0)
#include <QDS_Datum.h>
#pragma warning(pop)

// Forward declaration
class QEvent;
class QLineEdit;
class QValidator;
class QCompleter;
class QModelIndex;
class QStyleOptionViewItem;

class asiUI_EXPORT asiUI_DatumPathItem : public asiUI_Datum
{
  Q_OBJECT

  class PathEditImpl;

public:

  enum PathType
  {
    OpenPath,
    SavePath,
    DirPath
  };

public:

  asiUI_DatumPathItem(const QString& theDictId,
                       QWidget* theParent = 0,
                       const int theFlags = All);

  PathType GetPathType() const;

  QString GetFilter() const;

  QString GetTitle() const;

  void SetPathType(const PathType thePathType);

  void SetFilter(const QString& theFilter);

  void SetTitle(const QString& theTitle);

  virtual QString
    Browse();

  bool IsProjectLocation();

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

protected:

  virtual QDS_Datum* getDatum() const;

private:

  QString m_Title;
  QString m_Filter;
  QString m_StartDir;
  bool m_bIsProjectLoc;
  PathType m_PathType;
  PathEditImpl* m_pDatumImpl;
};

// ============================================================================
//                         Extended path string editor
// ============================================================================

//! \ingroup GUI
//!
//! \todo provide comment here
class asiUI_DatumPathItem::PathEditImpl : public asiUI_Datum::DatumImpl<QDS_Datum>
{
  Q_OBJECT

public:

  PathEditImpl(const QString& theDictId,
               QWidget* theParent = 0,
               const int theFlags = All);

  virtual QString getString() const;

  QLineEdit* GetLineEdit() { return lineEdit(); }

  virtual QValidator* validator(const bool theConsiderLimits = false) const;

  void SetPathCompleter(QCompleter* theCompleter);

signals:

  //! This signal is emitted when the Return/Enter
  //! key is pressed.
  void ReturnPressed();

  //! This signal is emitted when the Return/Enter
  //! key is pressed or the line edit loses
  //! focus.
  void EditingFinished();

private slots:

  void onTextChanged(const QString& theStr);

protected:

  virtual QWidget* createControl(QWidget* theParent);

  QLineEdit* lineEdit() const;

  virtual void setString(const QString& theString);

private:

  QCompleter* m_PathCompleter;
};

#endif

