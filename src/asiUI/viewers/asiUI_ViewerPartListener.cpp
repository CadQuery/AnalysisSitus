//-----------------------------------------------------------------------------
// Created on: 07 November 2016 (99 years of October Revolution)
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_ViewerPartListener.h>

//-----------------------------------------------------------------------------

//! Constructor accepting all necessary facilities.
//! \param wViewerPart    [in] part viewer.
//! \param wViewerDomain  [in] domain viewer.
//! \param wViewerSurface [in] surface viewer.
//! \param model          [in] Data Model instance.
asiUI_ViewerPartListener::asiUI_ViewerPartListener(asiUI_ViewerPart*              wViewerPart,
                                                   asiUI_ViewerDomain*            wViewerDomain,
                                                   asiUI_ViewerSurface*           wViewerSurface,
                                                   const Handle(asiEngine_Model)& model)
: QObject          (),
  m_wViewerPart    (wViewerPart),
  m_wViewerDomain  (wViewerDomain),
  m_wViewerSurface (wViewerSurface),
  m_model          (model)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiUI_ViewerPartListener::~asiUI_ViewerPartListener()
{}

//-----------------------------------------------------------------------------

//! Connects this listener to the target widget.
void asiUI_ViewerPartListener::Connect()
{
  connect( m_wViewerPart, SIGNAL ( facePicked() ),
           this,          SLOT   ( onFacePicked() ) );
  //
  connect( m_wViewerPart, SIGNAL ( edgePicked() ),
           this,          SLOT   ( onEdgePicked() ) );
}

//-----------------------------------------------------------------------------

//! Reaction on face picking.
void asiUI_ViewerPartListener::onFacePicked()
{
  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  m_wViewerDomain->PrsMgr()  -> Actualize(geom_n->GetFaceRepresentation().get(), false, true);
  m_wViewerSurface->PrsMgr() -> Actualize(geom_n->GetSurfaceRepresentation().get(), false, true);
}

//-----------------------------------------------------------------------------

//! Reaction on edge picking.
void asiUI_ViewerPartListener::onEdgePicked()
{
  Handle(asiData_PartNode) geom_n = m_model->GetPartNode();
  //
  m_wViewerDomain->PrsMgr()  -> Actualize(geom_n->GetEdgeRepresentation().get(), false, true);
  m_wViewerSurface->PrsMgr() -> Actualize(geom_n->GetCurveRepresentation().get(), false, true);
}
