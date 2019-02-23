//-----------------------------------------------------------------------------
// Created on: 12 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiUI_ControlsMeshListener.h>

// asiVisu includes
#include <asiVisu_TriangulationPrs.h>

// Qt includes
#pragma warning(push, 0)
#include <QMainWindow>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param wControls [in] controls.
//! \param model     [in] Data Model instance.
//! \param cf        [in] commonf facilities.
//! \param notifier  [in] progress notifier.
asiUI_ControlsMeshListener::asiUI_ControlsMeshListener(asiUI_ControlsMesh*                   wControls,
                                                       const Handle(asiEngine_Model)&        model,
                                                       const Handle(asiUI_CommonFacilities)& cf,
                                                       ActAPI_ProgressEntry                  notifier)
: QObject     (),
  m_wControls (wControls),
  m_model     (model),
  m_cf        (cf),
  m_notifier  (notifier)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ControlsMeshListener::~asiUI_ControlsMeshListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the controls widget.
void asiUI_ControlsMeshListener::Connect()
{
  connect( m_wControls, SIGNAL ( verticesOn() ),
           this,        SLOT   ( onVerticesOn() ) );
  //
  connect( m_wControls, SIGNAL ( verticesOff() ),
           this,        SLOT   ( onVerticesOff() ) );
}

//-----------------------------------------------------------------------------

//! Reaction on enabling visualization of vertices.
void asiUI_ControlsMeshListener::onVerticesOn()
{
  Handle(asiVisu_TriangulationPrs)
    prs = Handle(asiVisu_TriangulationPrs)::DownCast( m_cf->ViewerPart->PrsMgr()->GetPresentation( m_model->GetTriangulationNode() ) );
  //
  prs->VerticesOn();

  // Re-initialize pickers as the data set has changed
  m_cf->ViewerPart->PrsMgr()->InitializePickers( Handle(ActAPI_INode)() );
  m_cf->ViewerPart->Repaint();
}

//-----------------------------------------------------------------------------

//! Reaction on disabling visualization of vertices.
void asiUI_ControlsMeshListener::onVerticesOff()
{
  Handle(asiVisu_TriangulationPrs)
    prs = Handle(asiVisu_TriangulationPrs)::DownCast( m_cf->ViewerPart->PrsMgr()->GetPresentation( m_model->GetTriangulationNode() ) );
  //
  prs->VerticesOff();

  // Re-initialize pickers as the data set has changed
  m_cf->ViewerPart->PrsMgr()->InitializePickers( Handle(ActAPI_INode)() );
  m_cf->ViewerPart->Repaint();
}
