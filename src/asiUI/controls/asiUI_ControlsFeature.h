//-----------------------------------------------------------------------------
// Created on: 31 March 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
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
  void onNonManifoldEdges    ();
  void onCheckDihedralAngles ();
  void onFindSmoothEdges     ();
  void onFindConvexOnly      ();
  //
  void onDetachSelected      ();
  void onDeleteSelected      ();
  void onDeleteSelectedFull  ();

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
    QPushButton* pNonManifoldEdges;   //!< Finds non-manifold edges.
    QPushButton* pCheckDihAngles;     //!< Checks dihedral angles on convexity / concavity.
    QPushButton* pFindSmoothEdges;    //!< Finds C1 smooth edges.
    QPushButton* pFindConvexOnly;     //!< Finds convex-only faces.
    //
    QPushButton* pDetachSelected;     //!< Detaches selected faces from B-Rep.
    QPushButton* pDeleteSelected;     //!< Deletes selected faces (only).
    QPushButton* pDeleteSelectedFull; //!< Deletes selected faces with all contours.

    t_widgets() : pShowTOPOGraph      (NULL),
                  pShowAAG            (NULL),
                  pElimSelected       (NULL),
                  //
                  pNonManifoldEdges   (NULL),
                  pCheckDihAngles     (NULL),
                  pFindSmoothEdges    (NULL),
                  pFindConvexOnly     (NULL),
                  //
                  pDetachSelected     (NULL),
                  pDeleteSelected     (NULL),
                  pDeleteSelectedFull (NULL)
    {}

    void Release()
    {
      delete pShowTOPOGraph;      pShowTOPOGraph      = NULL;
      delete pShowAAG;            pShowAAG            = NULL;
      delete pElimSelected;       pElimSelected       = NULL;
      //
      delete pNonManifoldEdges;   pNonManifoldEdges   = NULL;
      delete pCheckDihAngles;     pCheckDihAngles     = NULL;
      delete pFindSmoothEdges;    pFindSmoothEdges    = NULL;
      delete pFindConvexOnly;     pFindConvexOnly     = NULL;
      //
      delete pDetachSelected;     pDetachSelected     = NULL;
      delete pDeleteSelected;     pDeleteSelected     = NULL;
      delete pDeleteSelectedFull; pDeleteSelectedFull = NULL;
    }
  };

  t_widgets               m_widgets;    //!< Involved widgets.
  Handle(asiEngine_Model) m_model;      //!< Data Model instance.
  asiUI_ViewerPart*       m_partViewer; //!< Part viewer.
  ActAPI_ProgressEntry    m_notifier;   //!< Progress Notifier.
  ActAPI_PlotterEntry     m_plotter;    //!< Imperative Plotter.

};

#endif
