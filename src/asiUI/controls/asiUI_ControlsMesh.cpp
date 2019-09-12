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
#include <asiUI_ControlsMesh.h>

// asiVisu includes
#include <asiVisu_TriangulationPrs.h>

// asiAlgo includes
#include <asiAlgo_MeshConvert.h>
#include <asiAlgo_PLY.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// Qt includes
#pragma warning(push, 0)
#include <QGroupBox>
#pragma warning(pop)

//-----------------------------------------------------------------------------

asiUI_ControlsMesh::asiUI_ControlsMesh(const Handle(asiEngine_Model)& model,
                                       asiUI_ViewerPart*              pPartViewer,
                                       ActAPI_ProgressEntry           notifier,
                                       ActAPI_PlotterEntry            plotter,
                                       QWidget*                       parent)
: QScrollArea  (parent),
  m_model      (model),
  m_partViewer (pPartViewer),
  m_notifier   (notifier),
  m_plotter    (plotter)
{
  // Main layout.
  m_pMainLayout = new QVBoxLayout;

  // Buttons.
  m_widgets.Load.pFromStl = new QPushButton("From STL");
  m_widgets.Load.pFromPly = new QPushButton("From PLY");
  m_widgets.Load.pFromObj = new QPushButton("From OBJ");
  //
  m_widgets.Save.pToStl       = new QPushButton("To STL");
  m_widgets.Save.pFacetsToPly = new QPushButton("B-Rep facets to PLY");
  //
  m_widgets.Select.pFaces    = new QPushButton("Triangles");
  m_widgets.Select.pEdges    = new QPushButton("Links");
  m_widgets.Select.pVertices = new QPushButton("Nodes");
  //
  m_widgets.Select.pFaces    ->setCheckable(true);
  m_widgets.Select.pEdges    ->setCheckable(true);
  m_widgets.Select.pVertices ->setCheckable(true);

  // Group box for loading mesh.
  QGroupBox*   pLoadGroup = new QGroupBox("Load");
  QVBoxLayout* pLoadLay   = new QVBoxLayout(pLoadGroup);
  //
  pLoadLay->addWidget(m_widgets.Load.pFromStl);
  pLoadLay->addWidget(m_widgets.Load.pFromPly);
  pLoadLay->addWidget(m_widgets.Load.pFromObj);

  // Group box for saving mesh.
  QGroupBox*   pSaveGroup = new QGroupBox("Save");
  QVBoxLayout* pSaveLay   = new QVBoxLayout(pSaveGroup);
  //
  pSaveLay->addWidget(m_widgets.Save.pToStl);
  pSaveLay->addWidget(m_widgets.Save.pFacetsToPly);

  // Group box for interactive selection.
  QGroupBox*   pSelectionGroup = new QGroupBox("Select");
  QVBoxLayout* pSelectionLay   = new QVBoxLayout(pSelectionGroup);
  //
  pSelectionLay->addWidget(m_widgets.Select.pFaces);
  pSelectionLay->addWidget(m_widgets.Select.pEdges);
  pSelectionLay->addWidget(m_widgets.Select.pVertices);

  // Set layout
  m_pMainLayout->addWidget(pLoadGroup);
  m_pMainLayout->addWidget(pSaveGroup);
  m_pMainLayout->addWidget(pSelectionGroup);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop);
  //
  QWidget* pMainWidget = new QWidget(this);
  pMainWidget->setLayout(m_pMainLayout);
  //
  this->setWidgetResizable(true);
  this->setFrameStyle(QFrame::NoFrame);
  this->setWidget(pMainWidget);

  // Connect signals to slots
  connect( m_widgets.Load.pFromStl,     SIGNAL( clicked() ), SLOT( onLoadFromStl     () ) );
  connect( m_widgets.Load.pFromPly,     SIGNAL( clicked() ), SLOT( onLoadFromPly     () ) );
  connect( m_widgets.Load.pFromObj,     SIGNAL( clicked() ), SLOT( onLoadFromObj     () ) );
  connect( m_widgets.Save.pToStl,       SIGNAL( clicked() ), SLOT( onSaveToStl       () ) );
  connect( m_widgets.Save.pFacetsToPly, SIGNAL( clicked() ), SLOT( onSaveFacetsToPly () ) );
  connect( m_widgets.Select.pFaces,     SIGNAL( clicked() ), SLOT( onSelectFaces     () ) );
  connect( m_widgets.Select.pEdges,     SIGNAL( clicked() ), SLOT( onSelectEdges     () ) );
  connect( m_widgets.Select.pVertices,  SIGNAL( clicked() ), SLOT( onSelectVertices  () ) );
}

//-----------------------------------------------------------------------------

//! On STL loading.
void asiUI_ControlsMesh::onLoadFromStl()
{
  // Select filename.
  QString filename = asiUI_Common::selectSTLFile(asiUI_Common::OpenSaveAction_Open);

  TIMER_NEW
  TIMER_GO

  // Load mesh.
  Handle(Poly_Triangulation) triangulation;
  //
  if ( !asiAlgo_Utils::ReadStl(QStr2AsciiStr(filename), triangulation, m_notifier) )
  {
    m_notifier.SendLogMessage( LogErr(Normal) << "Cannot read STL file." );
    return;
  }
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Loaded mesh from %1." << QStr2AsciiStr(filename) );

  TIMER_FINISH
  TIMER_COUT_RESULT_NOTIFIER(m_notifier, "Load STL")

  // Dump the number of loaded triangles.
  m_notifier.SendLogMessage( LogInfo(Normal) << "Loaded %1 triangles."
                                             << triangulation->Triangles().Length() );

  //---------------------------------------------------------------------------
  // Initialize Triangulation Node
  //---------------------------------------------------------------------------

  // Set mesh.
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
void asiUI_ControlsMesh::onLoadFromPly()
{
  // Select filename.
  QString filename = asiUI_Common::selectPLYFile(asiUI_Common::OpenSaveAction_Open);

  // Load mesh.
  Handle(ActData_Mesh) mesh;
  //
  if ( !asiAlgo_Utils::ReadPly(QStr2AsciiStr(filename), mesh, m_notifier) )
  {
    m_notifier.SendLogMessage( LogErr(Normal) << "Cannot read PLY file." );
    return;
  }
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Loaded mesh from %1." << QStr2AsciiStr(filename) );

  //---------------------------------------------------------------------------
  // Initialize Tessellation Node
  //---------------------------------------------------------------------------

  // Set mesh.
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

//! On OBJ loading.
void asiUI_ControlsMesh::onLoadFromObj()
{
  // Select filename.
  QString filename = asiUI_Common::selectOBJFile(asiUI_Common::OpenSaveAction_Open);

  // Load mesh.
  Handle(ActData_Mesh) mesh;
  //
  if ( !asiAlgo_Utils::ReadObj(QStr2AsciiStr(filename), mesh, m_notifier) )
  {
    m_notifier.SendLogMessage( LogErr(Normal) << "Cannot read OBJ file." );
    return;
  }
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Loaded mesh from %1." << QStr2AsciiStr(filename) );

  //---------------------------------------------------------------------------
  // Initialize Tessellation Node
  //---------------------------------------------------------------------------

  // Set mesh.
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
void asiUI_ControlsMesh::onSaveToStl()
{
  // Select filename.
  QString filename = asiUI_Common::selectSTLFile(asiUI_Common::OpenSaveAction_Save);

  // Get mesh
  Handle(asiData_TriangulationNode) triangulation_n = m_model->GetTriangulationNode();
  Handle(Poly_Triangulation)        triangulation   = triangulation_n->GetTriangulation();

  // Save
  if ( !asiAlgo_Utils::WriteStl( triangulation, QStr2AsciiStr(filename) ) )
  {
    m_notifier.SendLogMessage( LogErr(Normal) << "Cannot write STL file." );
    return;
  }
  //
  m_notifier.SendLogMessage( LogInfo(Normal) << "Mesh was saved to %1." << QStr2AsciiStr(filename) );
}

//-----------------------------------------------------------------------------

//! Saves facets to PLY file.
void asiUI_ControlsMesh::onSaveFacetsToPly()
{
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;
  //
  QString filename = asiUI_Common::selectPLYFile(asiUI_Common::OpenSaveAction_Save);

  // Convert shape's inherent mesh to a storable mesh.
  Handle(ActData_Mesh) storedMesh;
  //
  if ( !asiAlgo_MeshConvert::ToPersistent(part, storedMesh) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot convert mesh to persistent form.");
    return;
  }

  // Save mesh to ply file.
  if ( !asiAlgo_PLY(m_notifier).Write( storedMesh, QStr2AsciiStr(filename) ) )
  {
    m_notifier.SendLogMessage(LogErr(Normal) << "Cannot save mesh to PLY file.");
    return;
  }
}

//-----------------------------------------------------------------------------

//! On selection of faces.
void asiUI_ControlsMesh::onSelectFaces()
{
  m_widgets.Select.pEdges->setChecked(false);
  m_widgets.Select.pVertices->setChecked(false);

  m_partViewer->PrsMgr()->SetSelectionMode(SelectionMode_Face);

  // Get presentation for Triangulation Node.
  Handle(asiVisu_TriangulationPrs)
    prs = Handle(asiVisu_TriangulationPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation( m_model->GetTriangulationNode() ) );
  //
  if ( prs.IsNull() )
    return;

  prs->MainActor()->SetPickable( m_widgets.Select.pFaces->isChecked() );
  prs->ContourActor()->SetPickable(0);
  prs->NodesActor()->SetPickable(0);
}

//-----------------------------------------------------------------------------

//! On selection of edges.
void asiUI_ControlsMesh::onSelectEdges()
{
  m_widgets.Select.pFaces->setChecked(false);
  m_widgets.Select.pVertices->setChecked(false);

  m_partViewer->PrsMgr()->SetSelectionMode(SelectionMode_Edge);

  // Get presentation for Triangulation Node.
  Handle(asiVisu_TriangulationPrs)
    prs = Handle(asiVisu_TriangulationPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation( m_model->GetTriangulationNode() ) );
  //
  if ( prs.IsNull() )
    return;

  prs->MainActor()->SetPickable(0);
  prs->ContourActor()->SetPickable( m_widgets.Select.pEdges->isChecked() );
  prs->NodesActor()->SetPickable(0);
}

//-----------------------------------------------------------------------------

//! On selection of vertices.
void asiUI_ControlsMesh::onSelectVertices()
{
  m_widgets.Select.pFaces->setChecked(false);
  m_widgets.Select.pEdges->setChecked(false);

  m_partViewer->PrsMgr()->SetSelectionMode(SelectionMode_Vertex);

  // Get presentation for Triangulation Node.
  Handle(asiVisu_TriangulationPrs)
    prs = Handle(asiVisu_TriangulationPrs)::DownCast( m_partViewer->PrsMgr()->GetPresentation( m_model->GetTriangulationNode() ) );
  //
  if ( prs.IsNull() )
    return;

  prs->MainActor()->SetPickable(0);
  prs->ContourActor()->SetPickable(0);
  prs->NodesActor()->SetPickable( m_widgets.Select.pVertices->isChecked() );
}
