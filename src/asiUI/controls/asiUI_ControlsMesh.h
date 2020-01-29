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

#ifndef asiUI_ControlsMesh_h
#define asiUI_ControlsMesh_h

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_ViewerPart.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Widget for mesh controls.
class asiUI_ControlsMesh : public QScrollArea
{
  Q_OBJECT

public:

  //! Constructor.
  //! \param[in] model       Data Model instance.
  //! \param[in] pPartViewer part viewer.
  //! \param[in] notifier    progress notifier.
  //! \param[in] plotter     imperative plotter.
  //! \param[in] parent      parent widget.
  asiUI_EXPORT
    asiUI_ControlsMesh(const Handle(asiEngine_Model)& model,
                       asiUI_ViewerPart*              pPartViewer,
                       ActAPI_ProgressEntry           notifier,
                       ActAPI_PlotterEntry            plotter,
                       QWidget*                       parent = nullptr);

public slots:

  void onLoadFromStl();
  void onLoadFromPly();
  void onLoadFromObj();
  //
  void onSaveToStl();
  void onSaveFacetsToPly();
  //
  void onSelectFaces();
  void onSelectEdges();
  void onSelectVertices();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    struct t_load
    {
      //! Ctor.
      t_load() : pFromStl(nullptr), pFromPly(nullptr), pFromObj(nullptr)
      {}

      QPushButton* pFromStl; //!< Button for STL loading.
      QPushButton* pFromPly; //!< Button for PLY loading.
      QPushButton* pFromObj; //!< Button for OBJ loading.
    } Load;

    struct t_save
    {
      //! Ctor.
      t_save() : pToStl(nullptr), pFacetsToPly(nullptr)
      {}

      QPushButton* pToStl;       //!< Button for STL saving.
      QPushButton* pFacetsToPly; //!< Button for saving part's facets to PLY.
    } Save;

    struct t_select
    {
      //! Ctor.
      t_select() : pFaces(nullptr), pEdges(nullptr), pVertices(nullptr)
      {}

      QPushButton* pFaces;    //!< Button to enable selection of faces.
      QPushButton* pEdges;    //!< Button to enable selection of edges.
      QPushButton* pVertices; //!< Button to enable selection of vertices.
    } Select;
  };

  t_widgets               m_widgets;    //!< Involved widgets.
  Handle(asiEngine_Model) m_model;      //!< Data Model instance.
  asiUI_ViewerPart*       m_partViewer; //!< Part viewer.
  ActAPI_ProgressEntry    m_notifier;   //!< Progress Notifier.
  ActAPI_PlotterEntry     m_plotter;    //!< Imperative Plotter.

};

#endif
