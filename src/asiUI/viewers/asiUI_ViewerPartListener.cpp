//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerPartListener.h>

// asiUI includes
#include <asiUI_Common.h>

// asiAlgo includes
#include <asiAlgo_InvertFaces.h>
#include <asiAlgo_Timer.h>
#include <asiAlgo_Utils.h>

// asiEngine includes
#include <asiEngine_Part.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// Qt includes
#pragma warning(push, 0)
#include <QMenu>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param wViewerPart    [in] part viewer.
//! \param wViewerDomain  [in] domain viewer.
//! \param wViewerSurface [in] surface viewer.
//! \param model          [in] Data Model instance.
//! \param progress       [in] progress notifier.
//! \param plotter        [in] imperative plotter.
asiUI_ViewerPartListener::asiUI_ViewerPartListener(asiUI_ViewerPart*              wViewerPart,
                                                   asiUI_ViewerDomain*            wViewerDomain,
                                                   asiUI_ViewerSurface*           wViewerSurface,
                                                   const Handle(asiEngine_Model)& model,
                                                   ActAPI_ProgressEntry           progress,
                                                   ActAPI_PlotterEntry            plotter)
: QObject          (),
  m_wViewerPart    (wViewerPart),
  m_wViewerDomain  (wViewerDomain),
  m_wViewerSurface (wViewerSurface),
  m_model          (model),
  m_progress       (progress),
  m_plotter        (plotter)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ViewerPartListener::~asiUI_ViewerPartListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the target widget.
void asiUI_ViewerPartListener::Connect()
{
  connect( m_wViewerPart, SIGNAL ( facePicked(const asiVisu_PickResult&) ),
           this,          SLOT   ( onFacePicked(const asiVisu_PickResult&) ) );
  //
  connect( m_wViewerPart, SIGNAL ( edgePicked(const asiVisu_PickResult&) ),
           this,          SLOT   ( onEdgePicked(const asiVisu_PickResult&) ) );
  //
  connect( m_wViewerPart, SIGNAL ( contextMenu(const QPoint&) ),
           this,          SLOT   ( onContextMenu(const QPoint&) ) );
}

//-----------------------------------------------------------------------------

//! Reaction on face picking.
//! \param pickRes [in] pick result.
void asiUI_ViewerPartListener::onFacePicked(const asiVisu_PickResult& pickRes)
{
  asiUI_NotUsed(pickRes);

  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  if ( m_wViewerDomain )
    m_wViewerDomain->PrsMgr()->Actualize(geom_n->GetFaceRepresentation().get(), false, true);
  //
  if ( m_wViewerSurface )
    m_wViewerSurface->PrsMgr()->Actualize(geom_n->GetSurfaceRepresentation().get(), false, true);
}

//-----------------------------------------------------------------------------

//! Reaction on edge picking.
//! \param pickRes [in] pick result.
void asiUI_ViewerPartListener::onEdgePicked(const asiVisu_PickResult& pickRes)
{
  asiUI_NotUsed(pickRes);

  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  if ( m_wViewerDomain )
    m_wViewerDomain->PrsMgr()->Actualize(geom_n->GetEdgeRepresentation().get(), false, true);
  //
  if ( m_wViewerSurface )
    m_wViewerSurface->PrsMgr()->Actualize(geom_n->GetCurveRepresentation().get(), false, true);
}

//-----------------------------------------------------------------------------

//! Reaction on context menu invocation.
//! \param globalPos [in] click position in global coordinates.
void asiUI_ViewerPartListener::onContextMenu(const QPoint& globalPos)
{
  // Get highlighted faces
  TColStd_PackedMapOfInteger faceIndices;
  asiEngine_Part( m_model, m_wViewerPart->PrsMgr() ).GetHighlightedFaces(faceIndices);
  //
  if ( !faceIndices.Extent() )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "No selected faces" );
    return;
  }

  // Get Part Node
  Handle(asiData_PartNode) part_n = m_model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
  {
    m_progress.SendLogMessage( LogErr(Normal) << "Part Node is null or bad-formed" );
    return;
  }

  // Prepare the context menu items
  QMenu menu;
  QAction* pSaveBREPAction    = menu.addAction("Save to BREP...");
  QAction* pShowNormsAction   = menu.addAction("Show normal vectors");
  QAction* pInvertFacesAction = menu.addAction("Invert faces");

  // Execute
  QAction* selectedItem = menu.exec(globalPos);
  //
  if ( selectedItem == pSaveBREPAction )
  {
    // Get highlighted sub-shapes
    TopTools_IndexedMapOfShape selected;
    asiEngine_Part( m_model, m_wViewerPart->PrsMgr() ).GetHighlightedSubShapes(selected);

    // Let user choose a filename
    QString filename = asiUI_Common::selectBRepFile(asiUI_Common::OpenSaveAction_Save);
    //
    if ( filename.isEmpty() )
      return;

    // Prepare a shape to dump
    TopoDS_Shape shape2Save;
    //
    if ( selected.Extent() == 1 )
      shape2Save = selected(1);
    else
    {
      // Put selected shapes to a compound
      TopoDS_Compound comp;
      BRep_Builder().MakeCompound(comp);
      //
      for ( int k = 1; k <= selected.Extent(); ++k )
        BRep_Builder().Add( comp, selected(k) );
      //
      shape2Save = comp;
    }

    // Save shape
    if ( !asiAlgo_Utils::WriteBRep( shape2Save, QStr2AsciiStr(filename) ) )
    {
      m_progress.SendLogMessage( LogErr(Normal) << "Cannot save shape" );
      return;
    }
  }
  else if ( selectedItem == pShowNormsAction )
  {
    TIMER_NEW
    TIMER_GO

    m_wViewerPart->PrsMgr()->Actualize( m_model->GetPartNode()->GetNormsRepresentation() );

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Visualization of normals")
  }
  else if ( selectedItem == pInvertFacesAction )
  {
    TIMER_NEW
    TIMER_GO

    asiAlgo_InvertFaces InvertFaces( part_n->GetAAG() );
    //
    if ( !InvertFaces.Perform(faceIndices) )
    {
      m_progress.SendLogMessage( LogErr(Normal) << "Face inversion failed" );
      return;
    }

    // Update Data Model
    m_model->OpenCommand();
    {
      asiEngine_Part(m_model).Update( InvertFaces.GetResult() );
    }
    m_model->CommitCommand();

    // Actualize
    m_wViewerPart->PrsMgr()->Actualize(part_n);
    m_wViewerPart->PrsMgr()->Actualize( m_model->GetPartNode()->GetNormsRepresentation() );

    TIMER_FINISH
    TIMER_COUT_RESULT_MSG("Invert faces")
  }
  else
  {
    // Nothing was chosen
  }
}
