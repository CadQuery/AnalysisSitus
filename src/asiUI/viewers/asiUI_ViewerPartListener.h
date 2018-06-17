//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
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

#ifndef asiUI_ViewerPartListener_h
#define asiUI_ViewerPartListener_h

// asiUI includes
#include <asiUI_Viewer3dListener.h>
#include <asiUI_ViewerDomain.h>
#include <asiUI_ViewerHost.h>
#include <asiUI_ViewerPart.h>

// Active Data includes
#include <ActAPI_IPlotter.h>

//! Default slots for part viewer.
class asiUI_EXPORT asiUI_ViewerPartListener : public asiUI_Viewer3dListener
{
  Q_OBJECT

public:

  asiUI_ViewerPartListener(asiUI_ViewerPart*              wViewerPart,
                           asiUI_ViewerDomain*            wViewerDomain,
                           asiUI_ViewerHost*              wViewerHost,
                           const Handle(asiEngine_Model)& model,
                           ActAPI_ProgressEntry           progress,
                           ActAPI_PlotterEntry            plotter);

  virtual
    ~asiUI_ViewerPartListener();

public:

  virtual void
    Connect();

protected slots:

  void
    onFacePicked(const asiVisu_PickResult& pickRes);

  void
    onEdgePicked(const asiVisu_PickResult& pickRes);

  void
    onVertexPicked(const asiVisu_PickResult& pickRes);

protected:

  virtual void
    populateMenu(QMenu&);

  virtual void
    executeAction(QAction*);

protected:

  asiUI_ViewerDomain* m_wViewerDomain; //!< Domain viewer.
  asiUI_ViewerHost*   m_wViewerHost;   //!< Host viewer.

  //! Custom actions.
  QAction* m_pSaveBREPAction;
  QAction* m_pShowNormsAction;
  QAction* m_pInvertFacesAction;
  QAction* m_pShowOriContour;
  QAction* m_pCopyAsString;

};

#endif
