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
#include <asiUI_DatumLabel.h>

// Qt includes
#pragma warning(push, 0)
#include <QDS_Datum.h>
#include <QLabel>
#pragma warning(pop)

//! Construct datum.
//! \param theDictId [in] dictionary record item for the datum.
//! \param theParent [in] parent object for the datum.
//! \param theFlags [in] datum initialization flags.
asiUI_DatumLabel::asiUI_DatumLabel(const QString& theDictId,
                                   QWidget* theParent,
                                   const int theFlags)
: asiUI_Datum(theParent)
{
  m_pEditor = new Editor(theDictId, theParent, theFlags);

  init(m_pEditor);
}

//! Access label control used as a domain editor of the datum.
//! The method provides access to the lowest level and should
//! be used with advisedly. The general purpose is to fine-tune
//! properties and outlook of the editor widget.
QLabel* asiUI_DatumLabel::AccessLabel() const
{
  return m_pEditor->GetLabel();
}

//! \return the internal datum control.
QDS_Datum* asiUI_DatumLabel::getDatum() const
{
  return m_pEditor;
}

//-----------------------------------------------------------------------------
// Editor control
//-----------------------------------------------------------------------------

//! Label datum internals constructor.
//! \param theDictId [in] the dictionary record identifier.
//! \param theParent [in] the parent object for the datum.
//! \param theFlags [in] the initialization flags.
asiUI_DatumLabel::Editor::Editor(const QString& theDictId,
                                  QWidget* theParent,
                                  const int theFlags)
: asiUI_Datum::DatumImpl<QDS_Datum>(theDictId, theParent, theFlags)
{
}

//! \return label control.
QLabel* asiUI_DatumLabel::Editor::GetLabel() const
{
  return ::qobject_cast<QLabel*>( controlWidget() );
}

//! \return string value of the datum.
QString asiUI_DatumLabel::Editor::getString() const
{
  return (GetLabel() == NULL) ? QString() : GetLabel()->text();
}

//! Set label control string.
//! \param theString [in] the string to set up.
void asiUI_DatumLabel::Editor::setString(const QString& theString)
{
  if ( QLabel* aLabel = GetLabel() )
    aLabel->setText(theString);
}

//! Create label control widget.
//! \param theParent [in] the 
QWidget* asiUI_DatumLabel::Editor::createControl(QWidget* theParent)
{
  return new QLabel(theParent);
}
