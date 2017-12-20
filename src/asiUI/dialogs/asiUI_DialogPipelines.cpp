//-----------------------------------------------------------------------------
// Created on: 18 December 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiUI_DialogPipelines.h>

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_DialogPipelinesDelegate.h>

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QHeaderView>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor.
//! \param prs      [in] presentation in question.
//! \param notifier [in] progress notifier.
//! \param parent   [in] parent widget.
asiUI_DialogPipelines::asiUI_DialogPipelines(const Handle(asiVisu_Prs)& prs,
                                             ActAPI_ProgressEntry       notifier,
                                             QWidget*                   parent)
: QDialog    (parent),
  m_prs      (prs),
  m_notifier (notifier)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Widgets
  m_widgets.pPipelines = new QTableWidget();

  // Configure table with options
  QStringList headers;
  headers.append("Name");
  headers.append("Visible");
  //
  m_widgets.pPipelines->setColumnCount( headers.size() );
  m_widgets.pPipelines->setHorizontalHeaderLabels( headers );
  m_widgets.pPipelines->horizontalHeader()->setStretchLastSection( true );
  m_widgets.pPipelines->verticalHeader()->setVisible( false );
  m_widgets.pPipelines->setSelectionMode( QAbstractItemView::SingleSelection );
  m_widgets.pPipelines->setItemDelegateForColumn( 1, new asiUI_DialogPipelinesDelegate(this) );

  // Set layout
  m_pMainLayout->setSpacing(10);
  //
  m_pMainLayout->addWidget(m_widgets.pPipelines);
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
asiUI_DialogPipelines::~asiUI_DialogPipelines()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! Initializes table of commands.
void asiUI_DialogPipelines::initialize()
{
  //// Collect variables
  //std::vector<asiTcl_CommandInfo> commands;
  //m_interp->GetAvailableCommands(commands);

  //// Prepare properties to access the item
  //const Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

  //// Populate table
  //int current_row = 0;
  //for ( int c = 0; c < (int) commands.size(); ++c )
  //{
  //  const asiTcl_CommandInfo& cmd = commands[c];

  //  // Insert table row
  //  m_widgets.pCommands->insertRow(current_row);

  //  // Table item for name
  //  QTableWidgetItem* pNameItem = new QTableWidgetItem( cmd.Name.c_str() );
  //  pNameItem->setFlags(flags);
  //  pNameItem->setToolTip( cmd.Help.c_str() );
  //  m_widgets.pCommands->setItem(current_row, 0, pNameItem);

  //  // Table item for module
  //  QTableWidgetItem* pModuleItem = new QTableWidgetItem( cmd.Group.c_str() );
  //  pModuleItem->setFlags(flags);
  //  m_widgets.pCommands->setItem(current_row, 1, pModuleItem);

  //  // Table item for source
  //  QTableWidgetItem* pSourceItem = new QTableWidgetItem( cmd.Filename.c_str() );
  //  pSourceItem->setFlags(flags);
  //  m_widgets.pCommands->setItem(current_row, 2, pSourceItem);

  //  // Switch to the next row
  //  current_row++;
  //}

  // Choose good ratio of column sizes
  m_widgets.pPipelines->resizeColumnsToContents();
}
