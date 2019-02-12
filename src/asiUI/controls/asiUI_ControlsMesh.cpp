//-----------------------------------------------------------------------------
// Created on: 14 August 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
#include <asiUI_ControlsMesh.h>

// asiVisu includes
#include <asiVisu_TriangulationPrs.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Qt includes
#include <QGroupBox>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] model       Data Model instance.
//! \param[in] pPartViewer part viewer.
//! \param[in] notifier    progress notifier.
//! \param[in] plotter     imperative plotter.
//! \param[in] parent      parent widget.
asiUI_ControlsMesh::asiUI_ControlsMesh(const Handle(asiEngine_Model)& model,
                                       asiUI_ViewerPart*              pPartViewer,
                                       ActAPI_ProgressEntry           notifier,
                                       ActAPI_PlotterEntry            plotter,
                                       QWidget*                       parent)
: QWidget      (parent),
  m_model      (model),
  m_partViewer (pPartViewer),
  m_notifier   (notifier),
  m_plotter    (plotter)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Buttons
  m_widgets.pLoadStl        = new QPushButton("Load STL");
  m_widgets.pLoadPly        = new QPushButton("Load PLY");
  m_widgets.pSaveStl        = new QPushButton("Save STL");
  m_widgets.pShowVertices   = new QPushButton("Show/hide vertices");
  m_widgets.pSelectFaces    = new QPushButton("Select faces");
  m_widgets.pSelectEdges    = new QPushButton("Select edges");
  m_widgets.pSelectVertices = new QPushButton("Select vertices");
  //
  m_widgets.pLoadStl        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pLoadPly        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSaveStl        -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pShowVertices   -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSelectFaces    -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSelectEdges    -> setMinimumWidth(BTN_MIN_WIDTH);
  m_widgets.pSelectVertices -> setMinimumWidth(BTN_MIN_WIDTH);

  m_widgets.pSelectFaces    ->setCheckable(true);
  m_widgets.pSelectEdges    ->setCheckable(true);
  m_widgets.pSelectVertices ->setCheckable(true);

  // Group box for data interoperability
  QGroupBox*   pExchangeGroup = new QGroupBox("Data Exchange");
  QVBoxLayout* pExchangeLay   = new QVBoxLayout(pExchangeGroup);
  //
  pExchangeLay->addWidget(m_widgets.pLoadStl);
  pExchangeLay->addWidget(m_widgets.pLoadPly);
  pExchangeLay->addWidget(m_widgets.pSaveStl);

  // Group box for interactive selection
  QGroupBox*   pSelectionGroup = new QGroupBox("Selection");
  QVBoxLayout* pSelectionLay   = new QVBoxLayout(pSelectionGroup);
  //
  pSelectionLay->addWidget(m_widgets.pShowVertices);
  pSelectionLay->addWidget(m_widgets.pSelectFaces);
  pSelectionLay->addWidget(m_widgets.pSelectEdges);
  pSelectionLay->addWidget(m_widgets.pSelectVertices);

  // Set layout
  m_pMainLayout->addWidget(pExchangeGroup);
  m_pMainLayout->addWidget(pSelectionGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  this->setLayout(m_pMainLayout);

  // Connect signals to slots
  connect( m_widgets.pLoadStl,        SIGNAL( clicked() ), SLOT( onLoadStl        () ) );
  connect( m_widgets.pLoadPly,        SIGNAL( clicked() ), SLOT( onLoadPly        () ) );
  connect( m_widgets.pSaveStl,        SIGNAL( clicked() ), SLOT( onSaveStl        () ) );
  connect( m_widgets.pShowVertices,   SIGNAL( clicked() ), SLOT( onShowVertices   () ) );
  connect( m_widgets.pSelectFaces,    SIGNAL( clicked() ), SLOT( onSelectFaces    () ) );
  connect( m_widgets.pSelectEdges,    SIGNAL( clicked() ), SLOT( onSelectEdges    () ) );
  connect( m_widgets.pSelectVertices, SIGNAL( clicked() ), SLOT( onSelectVertices () ) );
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ControlsMesh::~asiUI_ControlsMesh()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------

//! On STL loading.
void asiUI_ControlsMesh::onLoadStl()
{
  // Select filename
  QString filename = asiUI_Common::selectSTLFile(asiUI_Common::OpenSaveAction_Open);

  // Load mesh
  Handle(Poly_Triangulation) triangulation;
  //
  if ( !asiAlgo_Utils::ReadStl(QStr2AsciiStr(filename), triangulation, m_notifier) )
  {
    m_notifier.SendLogMessage( LogErr(Normal) << "Cannot read STL file" );
    return;
  }
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Loaded mesh from %1" << QStr2AsciiStr(filename) );

  //---------------------------------------------------------------------------
  // Initialize Triangulation Node
  //---------------------------------------------------------------------------

  // Set mesh
  Handle(asiData_TriangulationNode) triangulation_n = m_model->GetTriangulationNode();
  //
  m_model->OpenCommand(); // tx start
  {
    triangulation_n->SetTriangulation(triangulation);
  }
  m_model->CommitCommand(); // tx commit

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  m_partViewer->PrsMgr()->Actualize(triangulation_n.get(), false, true);
}

//-----------------------------------------------------------------------------

//! On PLY loading.
void asiUI_ControlsMesh::onLoadPly()
{
  // Select filename
  QString filename = asiUI_Common::selectPLYFile(asiUI_Common::OpenSaveAction_Open);

  // Load mesh
  Handle(ActData_Mesh) mesh;
  //
  if ( !asiAlgo_Utils::ReadPly(QStr2AsciiStr(filename), mesh, m_notifier) )
  {
    m_notifier.SendLogMessage( LogErr(Normal) << "Cannot read PLY file" );
    return;
  }
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Loaded mesh from %1" << QStr2AsciiStr(filename) );

  //---------------------------------------------------------------------------
  // Initialize Triangulation Node
  //---------------------------------------------------------------------------

  // Set mesh
  Handle(asiData_TessNode) tess_n = m_model->GetTessellationNode();
  //
  m_model->OpenCommand(); // tx start
  {
    tess_n->SetMesh(mesh);
  }
  m_model->CommitCommand(); // tx commit

  //---------------------------------------------------------------------------
  // Update UI
  //---------------------------------------------------------------------------

  m_partViewer->PrsMgr()->Actualize(tess_n.get(), false, true);
}

//-----------------------------------------------------------------------------

//! On STL saving.
void asiUI_ControlsMesh::onSaveStl()
{
  // Select filename
  QString filename = asiUI_Common::selectSTLFile(asiUI_Common::OpenSaveAction_Save);

  // Get mesh
  Handle(asiData_TriangulationNode) triangulation_n = m_model->GetTriangulationNode();
  Handle(Poly_Triangulation)        triangulation   = triangulation_n->GetTriangulation();

  // Save
  if ( !asiAlgo_Utils::WriteStl( triangulation, QStr2AsciiStr(filename) ) )
  {
    m_notifier.SendLogMessage( LogErr(Normal) << "Cannot write STL file" );
    return;
  }
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Mesh was saved to %1." << QStr2AsciiStr(filename) );
}

//-----------------------------------------------------------------------------

//! Switches visualization of vertices.
void asiUI_ControlsMesh::onShowVertices()
{
  Handle(asiData_TriangulationNode)
    triangulation_n = m_model->GetTriangulationNode();

  const bool isOn = triangulation_n->HasVertices();

  // Modify data
  m_model->OpenCommand();
  {
    triangulation_n->SetHasVertices(!isOn);
  }
  m_model->CommitCommand();

  // Notify
  isOn ? emit verticesOff() : emit verticesOn();
}

//-----------------------------------------------------------------------------

//! On selection of faces.
void asiUI_ControlsMesh::onSelectFaces()
{
  m_widgets.pSelectEdges->setChecked(false);
  m_widgets.pSelectVertices->setChecked(false);

  m_partViewer->PrsMgr()->SetSelectionMode(SelectionMode_Face);

  // Get presentation for Mesh Node
  Handle(asiVisu_TriangulationPrs)
    prs = Handle(asiVisu_TriangulationPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation( m_model->GetTriangulationNode() ) );
  //
  if ( prs.IsNull() )
    return;

  prs->MainActor()->SetPickable( m_widgets.pSelectFaces->isChecked() );
  prs->ContourActor()->SetPickable(0);
  prs->NodesActor()->SetPickable(0);
}

//-----------------------------------------------------------------------------

//! On selection of edges.
void asiUI_ControlsMesh::onSelectEdges()
{
  m_widgets.pSelectFaces->setChecked(false);
  m_widgets.pSelectVertices->setChecked(false);

  m_partViewer->PrsMgr()->SetSelectionMode(SelectionMode_Edge);

  // Get presentation for Mesh Node
  Handle(asiVisu_TriangulationPrs)
    prs = Handle(asiVisu_TriangulationPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation( m_model->GetTriangulationNode() ) );
  //
  if ( prs.IsNull() )
    return;

  prs->MainActor()->SetPickable(0);
  prs->ContourActor()->SetPickable( m_widgets.pSelectEdges->isChecked() );
  prs->NodesActor()->SetPickable(0);
}

//-----------------------------------------------------------------------------

//! On selection of vertices.
void asiUI_ControlsMesh::onSelectVertices()
{
  m_widgets.pSelectFaces->setChecked(false);
  m_widgets.pSelectEdges->setChecked(false);

  m_partViewer->PrsMgr()->SetSelectionMode(SelectionMode_Vertex);

  // Get presentation for Mesh Node
  Handle(asiVisu_TriangulationPrs)
    prs = Handle(asiVisu_TriangulationPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation( m_model->GetTriangulationNode() ) );
  //
  if ( prs.IsNull() )
    return;

  prs->MainActor()->SetPickable(0);
  prs->ContourActor()->SetPickable(0);
  prs->NodesActor()->SetPickable( m_widgets.pSelectVertices->isChecked() );
}
