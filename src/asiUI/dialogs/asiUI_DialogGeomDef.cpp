//-----------------------------------------------------------------------------
// Created on: 19 June 2018
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
#include <asiUI_DialogGeomDef.h>

// asiUI includes
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_JSON.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param[in] model    Data Model instance.
//! \param[in] progress progress notifier.
//! \param[in] plotter  imperative plotter.
//! \param[in] parent   parent widget.
asiUI_DialogGeomDef::asiUI_DialogGeomDef(const Handle(asiEngine_Model)& model,
                                         ActAPI_ProgressEntry           progress,
                                         ActAPI_PlotterEntry            plotter,
                                         QWidget*                       parent)
: QDialog    (parent),
  m_model    (model),
  m_progress (progress),
  m_plotter  (plotter)
{
  // Main layout.
  m_pMainLayout = new QVBoxLayout();

  // Editors.
  m_widgets.pName       = new QLineEdit();
  m_widgets.pDefinition = new asiUI_StyledTextEdit();

  // Group box for name.
  QGroupBox*   pNameGroup = new QGroupBox("Geometric object");
  QGridLayout* pNameGrid  = new QGridLayout(pNameGroup);
  //
  pNameGrid->addWidget(new QLabel("Name:"), 0, 0);
  pNameGrid->addWidget(m_widgets.pName,     0, 1);

  // Group box for parameters.
  QGroupBox* pJSONGroup = new QGroupBox("JSON geometry definition");

  // JSON definition layout.
  QVBoxLayout* pBoxLayout = new QVBoxLayout(pJSONGroup);
  pBoxLayout->addWidget(m_widgets.pDefinition);

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pApply = new QPushButton("Apply");
  m_widgets.pClose = new QPushButton("Close");

  // Layout for buttons.
  QFrame*      bButtonsFrame  = new QFrame;
  QHBoxLayout* pButtonsLayout = new QHBoxLayout(bButtonsFrame);
  //
  pButtonsLayout->addWidget(m_widgets.pApply);
  pButtonsLayout->addWidget(m_widgets.pClose);

  // Sizing.
  m_widgets.pApply->setMaximumWidth(BTN_MIN_WIDTH);
  m_widgets.pClose->setMaximumWidth(BTN_MIN_WIDTH);

  // Reactions.
  connect( m_widgets.pApply, SIGNAL( clicked() ), this, SLOT( onApply() ) );
  connect( m_widgets.pClose, SIGNAL( clicked() ), this, SLOT( onClose() ) );

  //---------------------------------------------------------------------------
  // Layout
  //---------------------------------------------------------------------------

  // Configure main layout.
  m_pMainLayout->addWidget(pNameGroup);
  m_pMainLayout->addWidget(pJSONGroup);
  m_pMainLayout->addWidget(bButtonsFrame);
  //
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::NonModal);
  this->setWindowTitle("Geometry definition");

  // Set good initial size.
  this->setMinimumSize( QSize(300, 400) );
}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_DialogGeomDef::~asiUI_DialogGeomDef()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

//! Fills editor with data.
void asiUI_DialogGeomDef::Initialize(const Handle(ActAPI_INode)& node)
{
  if ( !node->IsKind( STANDARD_TYPE(asiData_IVCurveNode) ) &&
       !node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
    return;

  std::ostringstream buff;

  // Process curve.
  if ( node->IsKind( STANDARD_TYPE(asiData_IVCurveNode) ) )
  {
    Handle(asiData_IVCurveNode)
      curveNode = Handle(asiData_IVCurveNode)::DownCast(node);

    // Get parametric curve.
    Handle(Geom_Curve) curve = curveNode->GetCurve();

    // Serialize geometry.
    asiAlgo_JSON::DumpCurve(curve, buff);
  }

  // Process surface.
  else if ( node->IsKind( STANDARD_TYPE(asiData_IVSurfaceNode) ) )
  {
    Handle(asiData_IVSurfaceNode)
      surfNode = Handle(asiData_IVSurfaceNode)::DownCast(node);

    // Get parametric surface.
    Handle(Geom_Surface) surface = surfNode->GetSurface();

    // Serialize geometry.
    asiAlgo_JSON::DumpSurface(surface, buff);
  }

  // Set name.
  m_widgets.pName->setText( ExtStr2QStr( node->GetName() ) );

  // Set JSON definition.
  m_widgets.pDefinition->setText( buff.str().c_str() );
}

//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

//! Reaction on apply.
void asiUI_DialogGeomDef::onApply()
{
  TCollection_AsciiString varName = QStr2AsciiStr( m_widgets.pName->text() );
  varName.LeftAdjust();
  varName.RightAdjust();
  //
  if ( varName.IsEmpty() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Name of entity is empty.");
    return;
  }

  // Prepare JSON tool on the contents of definition text area.
  asiAlgo_JSON jsonTool( QStr2AsciiStr( m_widgets.pDefinition->toPlainText() ).ToCString() );

  // Create/update curve.
  if ( jsonTool.IsCurve() )
  {
    Handle(Geom_BSplineCurve) curve;
    //
    if ( !jsonTool.ExtractBCurve(curve) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot construct curve: incorrect format.");
      return;
    }

    m_plotter.REDRAW_CURVE(varName, curve, Color_Default);
  }

  // Create/update surface.
  else if ( jsonTool.IsSurface() )
  {
    Handle(Geom_BSplineSurface) surf;
    //
    if ( !jsonTool.ExtractBSurface(surf) )
    {
      m_progress.SendLogMessage(LogErr(Normal) << "Cannot construct surface: incorrect format.");
      return;
    }

    m_plotter.REDRAW_SURFACE(varName, surf, Color_Default);
  }
}

//! Reaction on close.
void asiUI_DialogGeomDef::onClose()
{
  this->close();
}
