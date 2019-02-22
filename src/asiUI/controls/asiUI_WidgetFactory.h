//-----------------------------------------------------------------------------
// Created on: 22 February 2019
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

#ifndef asiUI_WidgetFactoy_HeaderFile
#define asiUI_WidgetFactoy_HeaderFile

// asiUI includes
#include <asiUI_Datum.h>
#include <asiUI_UnitManager.h>

// Qt includes
#pragma warning(push, 0)
#include <QPointer>
#pragma warning(pop)

// Forward declaration.
class QWidget;
class QString;
class asiUI_CommonFacilities;

//-----------------------------------------------------------------------------

//! This class creates appropriate editors for dictionary items.
//! The editor type is selected by analyzing the item's data
//! type (Integer, Float, List, String, Bool) and widget type
//! defined by user in "ui" section of datum.
//!
//! The factory provides connection between unit manager and
//! datum editors, so the editors are updated when unit system
//! changes. However, if datums instantiated outside the widget
//! factory (by calling its constructors), the datum's content
//! should be refreshed by calling appropriate methods.
//!
//! If the ui widgetType, defined by user doesn't correspond
//! to data type, the default editor for this data type will be
//! created. Moreover, error message will be printed in log if
//! inappropriate editor specified.
class asiUI_WidgetFactory : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_WidgetFactory,Standard_Transient)

public:

  asiUI_EXPORT
    asiUI_WidgetFactory(asiUI_CommonFacilities* cf);

// editor factory
public:

  asiUI_EXPORT
    asiUI_Datum* CreateEditor(const QString& theDataDicID,
                              QWidget*       theParent = 0,
                              int            theDatumFlags = asiUI_Datum::Control);

  //asiUI_EXPORT
  //  asiUI_DatumTable* CreateDatumTable(const int theRowNumber = 0,
  //                                     const int theColumnNumber = 0,
  //                                     QWidget*  theParent = 0);

  //asiUI_EXPORT
  //  asiUI_DatumTree* CreateDatumTree(QWidget* theParent = 0);

private:

  asiUI_Datum* editor(const QString& theDataDicID,
                       QWidget*       theParent,
                       int            theDatumFlags);

private:

  asiUI_CommonFacilities* m_pCommonFacilities; //!< Common facilities.
};

#endif
