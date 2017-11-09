//-----------------------------------------------------------------------------
// Created on: 31 March 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiUI_ControlsFeature_h
#define asiUI_ControlsFeature_h

// asiUI includes
#include <asiUI_Common.h>
#include <asiUI_ViewerPart.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

// OCCT include
#include <TopoDS_Compound.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//! Widget for feature controls.
class asiUI_ControlsFeature : public QWidget
{
  Q_OBJECT

public:

  asiUI_EXPORT
    asiUI_ControlsFeature(const Handle(asiEngine_Model)& model,
                          asiUI_ViewerPart*              pPartViewer,
                          ActAPI_ProgressEntry           notifier,
                          ActAPI_PlotterEntry            plotter,
                          QWidget*                       parent = NULL);

  asiUI_EXPORT virtual
    ~asiUI_ControlsFeature();

public slots:

  void onShowTOPOGraph       ();
  void onShowAAG             ();
  void onElimSelected        ();
  //
  void onFreeEdges           ();
  void onNonManifoldEdges    ();
  void onCheckDihedralAngles ();
  void onFindSmoothEdges     ();
  void onFindConvexOnly      ();
  //
  void onDetachSelected      ();
  void onDeleteSelected      ();
  void onDeleteSelectedFull  ();
  void onDivideClosedEdges   ();
  void onDivideAngle         ();
  void onBoundingBox         ();

protected:

  void classifyDihAngle(const TopoDS_Face&          F,
                        const TopoDS_Face&          G,
                        TopTools_IndexedMapOfShape& convexEdges,
                        TopTools_IndexedMapOfShape& concaveEdges,
                        TopTools_IndexedMapOfShape& undefinedEdges,
                        TopTools_IndexedMapOfShape& smoothEdges,
                        TopoDS_Compound&            convexEdgesComp,
                        TopoDS_Compound&            concaveEdgesComp,
                        TopoDS_Compound&            undefinedEdgesComp,
                        TopoDS_Compound&            smoothEdgesComp,
                        const bool                  usePlotter,
                        const bool                  allowSmooth,
                        const double                smoothAngularTol) const;

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pShowTOPOGraph;      //!< Show topology graph.
    QPushButton* pShowAAG;            //!< Show Attributed Adjacency Graph.
    QPushButton* pElimSelected;       //!< Removes convex-only faces from AAG.
    //
    QPushButton* pFreeEdges;          //!< Finds non-manifold edges.
    QPushButton* pNonManifoldEdges;   //!< Finds non-manifold edges.
    QPushButton* pCheckDihAngles;     //!< Checks dihedral angles on convexity / concavity.
    QPushButton* pFindSmoothEdges;    //!< Finds C1 smooth edges.
    QPushButton* pFindConvexOnly;     //!< Finds convex-only faces.
    //
    QPushButton* pDetachSelected;     //!< Detaches selected faces from B-Rep.
    QPushButton* pDeleteSelected;     //!< Deletes selected faces (only).
    QPushButton* pDeleteSelectedFull; //!< Deletes selected faces with all contours.
    QPushButton* pDivideClosedEdges;  //!< Divides closed edges in the model.
    QPushButton* pDivideAngle;        //!< Divides faces by angles.
    QPushButton* pBoundingBox;        //!< Calculates bounding box.

    t_widgets() : pShowTOPOGraph      (NULL),
                  pShowAAG            (NULL),
                  pElimSelected       (NULL),
                  //
                  pFreeEdges          (NULL),
                  pNonManifoldEdges   (NULL),
                  pCheckDihAngles     (NULL),
                  pFindSmoothEdges    (NULL),
                  pFindConvexOnly     (NULL),
                  //
                  pDetachSelected     (NULL),
                  pDeleteSelected     (NULL),
                  pDeleteSelectedFull (NULL),
                  pDivideClosedEdges  (NULL),
                  pDivideAngle        (NULL),
                  pBoundingBox        (NULL)
    {}

    void Release()
    {
      delete pShowTOPOGraph;      pShowTOPOGraph      = NULL;
      delete pShowAAG;            pShowAAG            = NULL;
      delete pElimSelected;       pElimSelected       = NULL;
      //
      delete pFreeEdges;          pFreeEdges          = NULL;
      delete pNonManifoldEdges;   pNonManifoldEdges   = NULL;
      delete pCheckDihAngles;     pCheckDihAngles     = NULL;
      delete pFindSmoothEdges;    pFindSmoothEdges    = NULL;
      delete pFindConvexOnly;     pFindConvexOnly     = NULL;
      //
      delete pDetachSelected;     pDetachSelected     = NULL;
      delete pDeleteSelected;     pDeleteSelected     = NULL;
      delete pDeleteSelectedFull; pDeleteSelectedFull = NULL;
      delete pDivideClosedEdges;  pDivideClosedEdges  = NULL;
      delete pDivideAngle;        pDivideAngle        = NULL;
      delete pBoundingBox;        pBoundingBox        = NULL;
    }
  };

  t_widgets               m_widgets;    //!< Involved widgets.
  Handle(asiEngine_Model) m_model;      //!< Data Model instance.
  asiUI_ViewerPart*       m_partViewer; //!< Part viewer.
  ActAPI_ProgressEntry    m_notifier;   //!< Progress Notifier.
  ActAPI_PlotterEntry     m_plotter;    //!< Imperative Plotter.

};

#endif
