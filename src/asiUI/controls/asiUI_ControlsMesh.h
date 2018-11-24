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

#ifndef asiUI_ControlsMesh_h
#define asiUI_ControlsMesh_h

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ViewerPart.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#pragma warning(pop)

// OCCT include
#include <TopoDS_Compound.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//! Widget for mesh controls.
class asiUI_ControlsMesh : public QWidget
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ControlsMesh(const Handle(asiEngine_Model)& model,
                       asiUI_ViewerPart*              pPartViewer,
                       asiUI_ObjectBrowser*           pBrowser,
                       ActAPI_ProgressEntry           notifier,
                       ActAPI_PlotterEntry            plotter,
                       QWidget*                       parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_ControlsMesh();

public slots:

  void onLoadStl();
  void onDefineContour();
  void onBuildContourLines();
  void onBuildPatches();
  void onFairContourLines();
  void onUndo();
  void onRedo();

  void onLoadPly();
  void onSaveStl();
  void onSelectFaces();
  void onSelectEdges();

protected:

  void onUndoRedo(const Handle(ActAPI_TxRes)& txRes);

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pLoadStl;
    QPushButton* pDefineContour;
    QPushButton* pBuildContourLines;
    QPushButton* pBuildPatches;
    QPushButton* pFairContourLines;
    QLineEdit*   pLambda;
    QPushButton* pUndo;
    QPushButton* pRedo;

    QPushButton* pLoadPly;     //!< Button for PLY loading.
    QPushButton* pSaveStl;     //!< Button for STL saving.
    QPushButton* pSelectFaces; //!< Button to enable selection of faces.
    QPushButton* pSelectEdges; //!< Button to enable selection of edges.

    t_widgets() : pLoadStl     (NULL),
                  pLoadPly     (NULL),
                  pSaveStl     (NULL),
                  pSelectFaces (NULL),
                  pSelectEdges (NULL)
    {}

    void Release()
    {
      delete pLoadStl;     pLoadStl     = NULL;
      delete pLoadPly;     pLoadPly     = NULL;
      delete pSaveStl;     pSaveStl     = NULL;
      delete pSelectFaces; pSelectFaces = NULL;
      delete pSelectEdges; pSelectEdges = NULL;
    }
  };

  t_widgets               m_widgets;    //!< Involved widgets.
  Handle(asiEngine_Model) m_model;      //!< Data Model instance.
  asiUI_ViewerPart*       m_partViewer; //!< Part viewer.
  asiUI_ObjectBrowser*    m_objectBrowser;
  ActAPI_ProgressEntry    m_notifier;   //!< Progress Notifier.
  ActAPI_PlotterEntry     m_plotter;    //!< Imperative Plotter.

};

#endif
