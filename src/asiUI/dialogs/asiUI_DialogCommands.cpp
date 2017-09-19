//-----------------------------------------------------------------------------
// Created on: 19 September 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DialogCommands.h>

// asiEngine includes
#include <asiEngine_Part.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_DialogCommandsDelegate.h>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QHeaderView>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor.
//! \param interp   [in] Tcl interpreter.
//! \param notifier [in] progress notifier.
//! \param parent   [in] parent widget.
asiUI_DialogCommands::asiUI_DialogCommands(const Handle(asiTcl_Interp)& interp,
                                           ActAPI_ProgressEntry         notifier,
                                           QWidget*                     parent)
: QDialog    (parent),
  m_interp   (interp),
  m_notifier (notifier)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Widgets
  m_widgets.pCommands = new QTableWidget();

  // Configure table with options
  QStringList headers;
  headers.append("Name");
  headers.append("Help string");
  //
  m_widgets.pCommands->setColumnCount( headers.size() );
  m_widgets.pCommands->setHorizontalHeaderLabels( headers );
  m_widgets.pCommands->horizontalHeader()->setStretchLastSection( true );
  m_widgets.pCommands->verticalHeader()->setVisible( false );
  m_widgets.pCommands->setSelectionMode( QAbstractItemView::SingleSelection );
  m_widgets.pCommands->setSelectionBehavior( QAbstractItemView::SelectRows );
  m_widgets.pCommands->setItemDelegateForColumn( 1, new asiUI_DialogCommandsDelegate(this) );

  // Set layout
  m_pMainLayout->setSpacing(10);
  //
  m_pMainLayout->addWidget(m_widgets.pCommands);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);
  //
  this->setLayout(m_pMainLayout);

  // Initialize table
  this->initialize();

  // Set good initial size
  this->setMinimumSize( QSize(400, 600) );
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_DialogCommands::~asiUI_DialogCommands()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Initializes table of commands.
void asiUI_DialogCommands::initialize()
{
  // Collect variables
  NCollection_Sequence<asiAlgo_Variable> vars;

  // Prepare properties to access the item
  const Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

  // Populate table
  int current_row = 0;
  for ( int v = 1; v <= vars.Length(); ++v )
  {
    const asiAlgo_Variable& var = vars(v);

    // Insert table row
    m_widgets.pCommands->insertRow(current_row);

    // Table item for name
    QTableWidgetItem* pNameItem = new QTableWidgetItem( AsciiStr2QStr(var.Name) );
    pNameItem->setFlags(flags);
    pNameItem->setToolTip( AsciiStr2QStr(var.Description) );
    m_widgets.pCommands->setItem(current_row, 0, pNameItem);

    // Get variable value from statics if it is available there
    QString valueStr;
    Standard_CString cname = var.Name.ToCString();
    //
    valueStr = "UNDEFINED";

    // Table widget item for value with the flag for editing
    QTableWidgetItem *pValueItem = new QTableWidgetItem(valueStr);
    pValueItem->setData( asiUI_DialogCommandsDelegate::WidgetRole, QVariant(var.Type) );
    pValueItem->setFlags( flags | Qt::ItemIsEditable );
    m_widgets.pCommands->setItem(current_row, 1, pValueItem);

    // Switch to the next row
    current_row++;
  }

  // Choose good ratio of column sizes
  m_widgets.pCommands->resizeColumnsToContents();
}
