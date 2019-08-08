//-----------------------------------------------------------------------------
// Created on: 13 July 2016
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
#include <asiUI_DialogFindFace.h>

// GUI includes
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// OCCT includes
#include <TopExp.hxx>

// Qt includes
#pragma warning(push, 0)
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>
#pragma warning(pop)

//-----------------------------------------------------------------------------

#define CONTROL_EDIT_WIDTH 100
#define CONTROL_BTN_WIDTH 150

//-----------------------------------------------------------------------------

asiUI_DialogFindFace::asiUI_DialogFindFace(const Handle(asiEngine_Model)&             model,
                                           const vtkSmartPointer<asiVisu_PrsManager>& prsMgr,
                                           asiUI_ViewerPart*                          pViewer,
                                           ActAPI_ProgressEntry                       progress,
                                           ActAPI_PlotterEntry                        plotter,
                                           QWidget*                                   parent)
//
: QDialog    (parent),
  m_model    (model),
  m_prsMgr   (prsMgr),
  m_pViewer  (pViewer),
  m_progress (progress),
  m_plotter  (plotter)
{
  // Main layout.
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters.
  QGroupBox* pGroup = new QGroupBox("Face to find");

  // Editors.
  m_widgets.pFaceIdSel = new QComboBox();
  //
  m_widgets.pFaceIdSel->addItem("Serial index",   FaceIdType_Serial);
  m_widgets.pFaceIdSel->addItem("Memory address", FaceIdType_Transient);
  m_widgets.pFaceIdSel->addItem("Name",           FaceIdType_Persistent);
  //
  m_widgets.pIdSerial    = new asiUI_LineEdit();
  m_widgets.pIdTransient = new asiUI_LineEdit();
  m_widgets.pIdName      = new asiUI_LineEdit();

  // Sizing.
  m_widgets.pIdSerial    ->setMinimumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pIdTransient ->setMinimumWidth(CONTROL_EDIT_WIDTH);
  m_widgets.pIdName      ->setMinimumWidth(CONTROL_EDIT_WIDTH);

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pFind = new QPushButton("Find");

  // Sizing.
  m_widgets.pFind->setMaximumWidth(CONTROL_BTN_WIDTH);

  // Reaction.
  connect( m_widgets.pFaceIdSel, SIGNAL( currentIndexChanged (const int) ), this, SLOT( onChangeIdType (const int) ) );
  connect( m_widgets.pFind,      SIGNAL( clicked             ()          ), this, SLOT( onFind         ()          ) );

  //---------------------------------------------------------------------------
  // Layout
  //---------------------------------------------------------------------------

  // Create layout.
  QGridLayout* pGrid = new QGridLayout(pGroup);
  pGrid->setSpacing(5);
  //
  pGrid->addWidget(new QLabel("Face ID type:"),         0, 0);
  pGrid->addWidget(new QLabel("Face index (1-based):"), 1, 0);
  pGrid->addWidget(new QLabel("Face address:"),         2, 0);
  pGrid->addWidget(new QLabel("Face name:"),            3, 0);
  //
  pGrid->addWidget(m_widgets.pFaceIdSel,   0, 1);
  pGrid->addWidget(m_widgets.pIdSerial,    1, 1);
  pGrid->addWidget(m_widgets.pIdTransient, 2, 1);
  pGrid->addWidget(m_widgets.pIdName,      3, 1);
  //
  pGrid->setColumnStretch(0, 0);
  pGrid->setColumnStretch(1, 1);
  pGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

  //---------------------------------------------------------------------------
  // Main layout
  //---------------------------------------------------------------------------

  // Configure main layout.
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(m_widgets.pFind);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("Find face");

  // Adjust initial state.
  this->onChangeIdType(0);

  // Set initial focus to reduce excessive clicks.
  m_widgets.pFaceIdSel->setFocus();
}

//-----------------------------------------------------------------------------

asiUI_DialogFindFace::~asiUI_DialogFindFace()
{
  delete m_pMainLayout;
}

//-----------------------------------------------------------------------------

void asiUI_DialogFindFace::onChangeIdType(const int idx)
{
  m_widgets.pIdSerial    ->setEnabled ( idx == FaceIdType_Serial );
  m_widgets.pIdTransient ->setEnabled ( idx == FaceIdType_Transient );
  m_widgets.pIdName      ->setEnabled ( idx == FaceIdType_Persistent );
}

//-----------------------------------------------------------------------------

void asiUI_DialogFindFace::onFind()
{
  const int typeIdx = m_widgets.pFaceIdSel->currentIndex();
  //
  Handle(asiData_PartNode) part_n;
  TopoDS_Shape             part;
  //
  if ( !asiUI_Common::PartShape(m_model, part_n, part) ) return;

  const TopTools_IndexedMapOfShape&
    faces = m_model->GetPartNode()->GetAAG()->GetMapOfFaces();
  //
  TopTools_IndexedMapOfShape found;

  /* Serial identification */

  if ( typeIdx == FaceIdType_Serial )
  {
    const int face_id = m_widgets.pIdSerial->text().toInt();

    if ( face_id > 0 && face_id <= faces.Extent() )
    {
      const TopoDS_Shape& face = faces.FindKey(face_id);
      found.Add(face);
    }
    else
    {
      m_progress.SendLogMessage( LogErr(Normal) << "Input index %1 is out of range [1, %2]."
                                                << face_id << faces.Extent() );
    }
  }

  /* Transient identification */

  else if ( typeIdx == FaceIdType_Transient )
  {
    TCollection_AsciiString face_addr = QStr2AsciiStr( m_widgets.pIdTransient->text() );
    face_addr.LowerCase();

    for ( int f = 1; f <= faces.Extent(); ++f )
    {
      TCollection_AsciiString next_addr = asiAlgo_Utils::ShapeAddr( faces(f) ).c_str();
      next_addr.LowerCase();

      if ( next_addr.IsEqual(face_addr) )
      {
        found.Add( faces(f) );
        break;
      }
    }
  }

  /* Persistent identification */

  else if ( typeIdx == FaceIdType_Persistent )
  {
    TCollection_AsciiString face_name = QStr2AsciiStr( m_widgets.pIdName->text() );
    face_name.LowerCase();

    // Get naming service to access the face by its name.
    Handle(asiAlgo_Naming) naming = part_n->GetNaming();
    //
    if ( naming.IsNull() )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Naming service is not available. Use 'init-naming' command.");
    }
    else
    {
      TopoDS_Shape shape = naming->GetShape(face_name);
      //
      if ( shape.ShapeType() != TopAbs_FACE )
      {
        m_progress.SendLogMessage( LogErr(Normal) << "Shape '%1' is not a face, it is of %2 type."
                                                  << face_name << asiAlgo_Utils::ShapeTypeStr(shape).c_str() );
      }
      else
      {
        found.Add(shape);
      }
    }
  }

  if ( !found.IsEmpty() )
  {
    // Highlight.
    asiEngine_Part(m_model, m_prsMgr).HighlightSubShapes(found);
    //
    m_pViewer->onSubShapesPicked(); // TODO: change with event.
  }
  else
  {
    m_progress.SendLogMessage(LogErr(Normal) << "No face found.");
  }

  // Close.
  this->close();
}
