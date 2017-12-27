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

// Qt includes
#pragma warning(push, 0)
#include <QApplication>
#include <QHeaderView>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor.
//! \param prs      [in] presentation in question.
//! \param prsMgr   [in] presentation manager.
//! \param notifier [in] progress notifier.
//! \param parent   [in] parent widget.
asiUI_DialogPipelines::asiUI_DialogPipelines(const Handle(asiVisu_Prs)&                 prs,
                                             const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                                             ActAPI_ProgressEntry                       notifier,
                                             QWidget*                                   parent)
: QDialog    (parent),
  m_prs      (prs),
  m_prsMgr   (prsMgr),
  m_notifier (notifier)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Widgets
  m_widgets.pPipelines = new QTableWidget();

  // Configure table with options
  QStringList headers;
  headers.append("Pipeline");
  headers.append("Visible");
  //
  m_widgets.pPipelines->setColumnCount( headers.size() );
  m_widgets.pPipelines->setHorizontalHeaderLabels( headers );
  m_widgets.pPipelines->horizontalHeader()->setStretchLastSection( false );
  m_widgets.pPipelines->verticalHeader()->setVisible( false );
  m_widgets.pPipelines->setSelectionMode( QAbstractItemView::SingleSelection );

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

  // Connect signals to slots
  connect( m_widgets.pPipelines, SIGNAL ( itemChanged(QTableWidgetItem*) ),
           this,                 SLOT   ( onStateChanged(QTableWidgetItem*) ) );

  TCollection_AsciiString title("Pipelines of "); title += m_prs->DynamicType()->Name();
  this->setWindowTitle( title.ToCString() );
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_DialogPipelines::~asiUI_DialogPipelines()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! On changing variable value.
//! \param pItem [in] table item being changed.
void asiUI_DialogPipelines::onStateChanged(QTableWidgetItem* pItem)
{
  const int pipelineIndex = pItem->data(Qt::UserRole).toInt();

  // Get pipeline by its index in the Presentation.
  Handle(asiVisu_Pipeline) pl = m_prs->GetPipeline(pipelineIndex);
  //
  this->toggleVisibility(pl);
  //
  m_prsMgr->GetQVTKWidget()->repaint();
}

//-----------------------------------------------------------------------------

//! Initializes table of commands.
void asiUI_DialogPipelines::initialize()
{
  // Get all available pipelines.
  Handle(asiVisu_HPipelineList) pipelines = m_prs->GetPipelineList();
  //
  if ( pipelines.IsNull() )
    return;

  // Prepare properties to access an item.
  const Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

  // Populate table.
  int current_row = 0;
  for ( asiVisu_HPipelineList::Iterator pit(*pipelines); pit.More(); pit.Next() )
  {
    const Handle(asiVisu_Pipeline)& pipeline = pit.Value();

    // Insert table row
    m_widgets.pPipelines->insertRow(current_row);

    // Table item for name
    QTableWidgetItem* pNameItem = new QTableWidgetItem( pipeline->DynamicType()->Name() );
    pNameItem->setFlags(flags);
    m_widgets.pPipelines->setItem(current_row, 0, pNameItem);

    // Check if the current actor is visible.
    const bool isPipelineVisible = this->isPipelineVisible(pipeline);

    // Table widget item for value with the flag for editing.
    QTableWidgetItem* pValueItem = new QTableWidgetItem;
    pValueItem->setCheckState(isPipelineVisible ? Qt::Checked : Qt::Unchecked);
    pValueItem->setData(Qt::UserRole, current_row + 1); // Store index of the pipeline
    m_widgets.pPipelines->setItem(current_row, 1, pValueItem);

    // Switch to the next row
    current_row++;
  }

  // Choose good ratio of column sizes
  m_widgets.pPipelines->resizeColumnsToContents();
}

//---------------------------------------------------------------------------

bool asiUI_DialogPipelines::isPipelineVisible(const Handle(asiVisu_Pipeline)& pl) const
{
  // Get actor.
  vtkActor* pActor = pl->Actor();

  // Get active renderer.
  vtkRenderer* pRenderer = m_prsMgr->GetRenderer();

  // Loop over the props.
  vtkPropCollection* pProps = pRenderer->GetViewProps();
  pProps->InitTraversal();
  //
  bool isFound = false;
  for ( int i = 0; i < pProps->GetNumberOfItems(); ++i )
  {
    vtkProp* pNextProp = pProps->GetNextProp();

    if ( pNextProp == pActor )
    {
      isFound = true;
      break;
    }
  }

  // If the actor is not in the renderer, it is invisible. However, if the
  // actor is in the renderer, it can be invisible.
  bool isVisible;
  if ( isFound )
    isVisible = (pActor->GetVisibility() > 0);
  else
    isVisible = false;

  return isVisible;
}

//-----------------------------------------------------------------------------

void asiUI_DialogPipelines::toggleVisibility(const Handle(asiVisu_Pipeline)& pl) const
{
  // Get actor.
  vtkActor* pActor = pl->Actor();

  // Get active renderer.
  vtkRenderer* pRenderer = m_prsMgr->GetRenderer();

  // Loop over the props.
  vtkPropCollection* pProps = pRenderer->GetViewProps();
  pProps->InitTraversal();
  //
  bool isFound = false;
  for ( int i = 0; i < pProps->GetNumberOfItems(); ++i )
  {
    vtkProp* pNextProp = pProps->GetNextProp();

    if ( pNextProp == pActor )
    {
      isFound = true;
      break;
    }
  }

  if ( isFound )
    pActor->SetVisibility( !pActor->GetVisibility() );
  else
  {
    pRenderer->AddActor(pActor);
    pActor->SetVisibility(1);
  }
}
