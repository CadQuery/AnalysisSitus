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

#ifndef asiUI_ControlsAnalysis_h
#define asiUI_ControlsAnalysis_h

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

//! Widget for analysis controls.
class asiUI_ControlsAnalysis : public QScrollArea
{
  Q_OBJECT

public:

  //! Constructor.
  //! \param[in] model       Data Model instance.
  //! \param[in] pPartViewer part viewer.
  //! \param[in] pBrowser    object browser.
  //! \param[in] notifier    progress notifier.
  //! \param[in] plotter     imperative plotter.
  //! \param[in] parent      parent widget.
  asiUI_EXPORT
    asiUI_ControlsAnalysis(const Handle(asiEngine_Model)& model,
                           asiUI_ViewerPart*              pPartViewer,
                           asiUI_ObjectBrowser*           pBrowser,
                           ActAPI_ProgressEntry           notifier,
                           ActAPI_PlotterEntry            plotter,
                           QWidget*                       parent = nullptr);

public slots:

  void onDiagnose             ();
  void onCheckToler           ();
  void onCheckEuler           ();
  void onCheckContours        ();
  void onFindOpenEdges        ();
  void onFindNonManifoldEdges ();
  //
  void onShowTopoGraph   ();
  void onShowAAG         ();
  void onShowAAGWoSel    ();
  void onCheckDihAngles  ();
  void onFindSmoothEdges ();
  void onFindConvexOnly  ();
  //
  void onEdgeCurvature ();
  void onEdgeLength    ();
  void onEdgeStrain    ();
  void onFaceArea      ();
  void onFaceEnergy    ();
  void onAABB          ();

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
                        const bool                  verboseOutput,
                        const bool                  allowSmooth,
                        const double                smoothAngularTol) const;

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    struct t_validity
    {
      //! Ctor.
      t_validity() : pDiagnose             (nullptr),
                     pCheckToler           (nullptr),
                     pCheckEuler           (nullptr),
                     pCheckContours        (nullptr),
                     pFindOpenEdges        (nullptr),
                     pFindNonManifoldEdges (nullptr)
      {}

      QPushButton* pDiagnose;             //!< To diagnose part.
      QPushButton* pCheckToler;           //!< To check geometric tolerances.
      QPushButton* pCheckEuler;           //!< To check Euler-Poincare property.
      QPushButton* pCheckContours;        //!< To check contours.
      QPushButton* pFindOpenEdges;        //!< To find open (naked) edges.
      QPushButton* pFindNonManifoldEdges; //!< To find non-manifold edges.
    } Validity;

    struct t_features
    {
      //! Ctor.
      t_features() : pShowTopoGraph   (nullptr),
                     pShowAAG         (nullptr),
                     pShowAAGWoSel    (nullptr),
                     pCheckDihAngles  (nullptr),
                     pFindSmoothEdges (nullptr),
                     pFindConvexOnly  (nullptr)
      {}

      QPushButton* pShowTopoGraph;   //!< To show topology graph.
      QPushButton* pShowAAG;         //!< To show attributed adjacency graph (AAG).
      QPushButton* pShowAAGWoSel;    //!< To show AAG without selected faces.
      QPushButton* pCheckDihAngles;  //!< To classify dihedral angles.
      QPushButton* pFindSmoothEdges; //!< To find smooth edges.
      QPushButton* pFindConvexOnly;  //!< To find convex-only faces.
    } Features;

    struct t_properties
    {
      //! Ctor.
      t_properties() : pEdgeCurvature (nullptr),
                       pEdgeLength    (nullptr),
                       pEdgeStrain    (nullptr),
                       pFaceArea      (nullptr),
                       pFaceEnergy    (nullptr),
                       pAABB          (nullptr)
      {}

      QPushButton* pEdgeCurvature; //!< To check edge curvature.
      QPushButton* pEdgeLength;    //!< To check edge length.
      QPushButton* pEdgeStrain;    //!< To check strain energy.
      QPushButton* pFaceArea;      //!< To check face area.
      QPushButton* pFaceEnergy;    //!< To check face bending energy.
      QPushButton* pAABB;          //!< To check axis-aligned bounding box (AABB).
    } Properties;
  };

  t_widgets               m_widgets;    //!< Involved widgets.
  Handle(asiEngine_Model) m_model;      //!< Data Model instance.
  asiUI_ViewerPart*       m_partViewer; //!< Part viewer.
  asiUI_ObjectBrowser*    m_browser;    //!< Object browser.
  ActAPI_ProgressEntry    m_notifier;   //!< Progress Notifier.
  ActAPI_PlotterEntry     m_plotter;    //!< Imperative Plotter.

};

#endif
