//-----------------------------------------------------------------------------
// Created on: 31 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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

#ifndef asiUI_ControlsModeling_h
#define asiUI_ControlsModeling_h

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

// OCCT include
#include <TopoDS_Compound.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Widget for modeling controls.
class asiUI_ControlsModeling : public QScrollArea
{
  Q_OBJECT

public:

  //! Ctor.
  //! \param[in] model       Data Model instance.
  //! \param[in] pPartViewer Part viewer.
  //! \param[in] notifier    progress notifier.
  //! \param[in] plotter     imperative plotter.
  //! \param[in] parent      parent widget.
  asiUI_EXPORT
    asiUI_ControlsModeling(const Handle(asiEngine_Model)& model,
                           asiUI_ViewerPart*              pPartViewer,
                           ActAPI_ProgressEntry           notifier,
                           ActAPI_PlotterEntry            plotter,
                           QWidget*                       parent = NULL);

public slots:

  void onDeleteFaces    (); //!< On face removal (gap will emerge).
  void onSuppressFaces  (); //!< On face suppression (useful for isolated features).
  void onDefeatureFaces (); //!< On "smart" face removal.
  void onDetachFaces    (); //!< On face detaching (breaking sharing).
  //
  void onSew            (); //!< On sewing.
  void onAutoRepair     (); //!< On automatic repair.
  void onMaximizeFaces  (); //!< On face maximization.
  void onHealSmallEdges (); //!< On merging "small" edges.
  void onFillGap        (); //!< On gap filling.

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    struct t_edit
    {
      t_edit() : pDeleteFaces    (NULL),
                 pSuppressFaces  (NULL),
                 pDefeatureFaces (NULL),
                 pDetachFaces    (NULL)
      {}

      QPushButton* pDeleteFaces;    //!< Deletes selected faces (only).
      QPushButton* pSuppressFaces;  //!< Deletes selected faces with all contours.
      QPushButton* pDefeatureFaces; //!< Deletes selected faces and stitches neighbors.
      QPushButton* pDetachFaces;    //!< Detaches selected faces from B-Rep.
    } Edit;

    struct t_healing
    {
      t_healing() : pSew            (NULL),
                    pAutoRepair     (NULL),
                    pMaximizeFaces  (NULL),
                    pHealSmallEdges (NULL),
                    pFillGap        (NULL)
      {}

      QPushButton* pSew;            //!< Sewing.
      QPushButton* pAutoRepair;     //!< Automatic repair.
      QPushButton* pMaximizeFaces;  //!< Maximizes faces.
      QPushButton* pHealSmallEdges; //!< Heals small edges.
      QPushButton* pFillGap;        //!< Fills gaps.
    } Healing;
  };

  t_widgets               m_widgets;    //!< Involved widgets.
  Handle(asiEngine_Model) m_model;      //!< Data Model instance.
  asiUI_ViewerPart*       m_partViewer; //!< Part viewer.
  ActAPI_ProgressEntry    m_notifier;   //!< Progress Notifier.
  ActAPI_PlotterEntry     m_plotter;    //!< Imperative Plotter.

};

#endif
