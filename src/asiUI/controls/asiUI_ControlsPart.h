//-----------------------------------------------------------------------------
// Created on: 25 November 2015
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

#ifndef asiUI_ControlsPart_h
#define asiUI_ControlsPart_h

// asiUI includes
#include <asiUI_ViewerPart.h>

// asiEngine includes
#include <asiEngine_Model.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

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

//! Controls in 'Part' tab.
class asiUI_ControlsPart : public QScrollArea
{
  Q_OBJECT

public:

  //! Constructor.
  //! \param[in] model       Data Model instance.
  //! \param[in] pPartViewer Part viewer.
  //! \param[in] notifier    progress notifier.
  //! \param[in] plotter     imperative plotter.
  //! \param[in] parent      parent widget.
  asiUI_EXPORT
    asiUI_ControlsPart(const Handle(asiEngine_Model)& model,
                       asiUI_ViewerPart*              pPartViewer,
                       ActAPI_ProgressEntry           notifier,
                       ActAPI_PlotterEntry            plotter,
                       QWidget*                       parent = NULL);

public slots:

  /* Load */
  void onLoadFromSTEP       ();
  void onLoadFromBRep       ();
  void onLoadFromBRepAppend ();

  /* Save */
  void onSaveToSTEP();
  void onSaveToBRep();

  /* Visualization */
  void onSelectFaces    ();
  void onSelectEdges    ();
  void onSelectVertices ();

signals:

  void partLoaded          (const QString&);
  void partAdded           (const QString&);
  void partSaved           ();
  void partModified        ();
  void selectionFacesOn    ();
  void selectionEdgesOn    ();
  void selectionVerticesOn ();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    struct t_load
    {
      t_load() : pSTEP(NULL), pBRep(NULL), pBRepAppend(NULL) {} //!< Ctor.

      QPushButton* pSTEP;       //!< Button for STEP loading.
      QPushButton* pBRep;       //!< Button for BREP loading.
      QPushButton* pBRepAppend; //!< Button for adding BREP.
    } Load;

    struct t_save
    {
      t_save() : pSTEP(NULL), pBRep(NULL) {} //!< Ctor.

      QPushButton* pSTEP; //!< Save shape to STEP.
      QPushButton* pBRep; //!< Save shape to BREP.
    } Save;

    struct t_select
    {
      t_select() : pFaces(NULL), pEdges(NULL), pVertices(NULL) {} //!< Ctor.

      QPushButton* pFaces;    //!< Enables selection by faces.
      QPushButton* pEdges;    //!< Enables selection by edges.
      QPushButton* pVertices; //!< Enables selection by vertices.
    } Select;
  };

  t_widgets               m_widgets;      //!< Involved widgets.
  Handle(asiEngine_Model) m_model;        //!< Data Model instance.
  asiUI_ViewerPart*       m_partViewer;   //!< Part viewer.
  ActAPI_ProgressEntry    m_notifier;     //!< Progress Notifier.
  ActAPI_PlotterEntry     m_plotter;      //!< Imperative Plotter.
  int                     m_iPrevSelMode; //!< Previous selection mode.

};

#endif
