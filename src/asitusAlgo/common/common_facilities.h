//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef common_facilities_h
#define common_facilities_h

// A-Situs (common) includes
#include <common_model.h>

// A-Situs (XDE) includes
#include <xde_model.h>

// A-Situs (journaling) includes
#include <journal_iv.h>
#include <journal_notifier.h>

// A-Situs (GUI) includes
#include <gui_decomposition_browser.h>
#include <gui_object_browser.h>
#include <gui_object_browser_xde.h>
#include <gui_viewer_dmu.h>
#include <gui_viewer_domain.h>
#include <gui_viewer_part.h>

// A-Situs (visualization) includes
#include <visu_prs_manager.h>

// OCCT includes
#pragma warning(push, 0)
#include <AIS_Shape.hxx>
#pragma warning(pop)

// Qr includes
#include <QrCore.h>

DEFINE_STANDARD_HANDLE(common_facilities, Standard_Transient)

//! Common tools and objects for A-Situs software.
class common_facilities : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(common_facilities, Standard_Transient)

public:

  Handle(common_model)             Model;             //!< Active Data Model instance.
  Handle(xde_model)                Model_XDE;         //!< XDE Data Model instance.
  //
  gui_decomposition_browser*       DecompBrowser;     //!< Volume decomposition browser.
  gui_object_browser*              ObjectBrowser;     //!< Object browser.
  gui_object_browser_xde*          ObjectBrowser_XDE; //!< Object browser for XDE.
  Handle(ActAPI_INode)             CurrentNode;       //!< Currently selected Node.
  //
  gui_viewer_part*                 ViewerPart;        //!< Viewer for part.
  gui_viewer_domain*               ViewerDomain;      //!< Viewer for face parametric domain.
  gui_viewer_dmu*                  ViewerDMU;         //!< OpenCascade-driven viewer for dMU.
  //
  Handle(ActAPI_IProgressNotifier) Notifier;          //!< Algorithmic notifier.
  Handle(ActAPI_IPlotter)          Plotter;           //!< Algorithmic plotter.

  //! Visualization facilities.
  struct t_prs
  {
  //---------------------------------------------------------------------------
    vtkSmartPointer<visu_prs_manager> Mesh;     //!< Mesh.
    vtkSmartPointer<visu_prs_manager> Part;     //!< Part.
    vtkSmartPointer<visu_prs_manager> Domain;   //!< Face domain.
    vtkSmartPointer<visu_prs_manager> Host;     //!< Host geometry.
  //---------------------------------------------------------------------------
    vtkSmartPointer<visu_prs_manager> Skinner;  //!< Skinner.
    vtkSmartPointer<visu_prs_manager> Section;  //!< Section.
  //---------------------------------------------------------------------------
    vtkSmartPointer<visu_prs_manager> UBend;    //!< U-bend.
    vtkSmartPointer<visu_prs_manager> UBend_Xt; //!< X(t) for U-bend.
    vtkSmartPointer<visu_prs_manager> UBend_Yt; //!< Y(t) for U-bend.
    vtkSmartPointer<visu_prs_manager> UBend_XY; //!< Section law for U-bend.
  //---------------------------------------------------------------------------

    inline void ActualizeAll()
    {
      if ( Part )     Part     ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      if ( Domain )   Domain   ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      if ( Host )     Host     ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      //
      if ( Skinner )  Skinner  ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      if ( Section )  Section  ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      //
      if ( UBend )    UBend    ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      if ( UBend_Xt ) UBend_Xt ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      if ( UBend_Yt ) UBend_Yt ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      if ( UBend_XY ) UBend_XY ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
    }

    inline void DeleteAll()
    {
      if ( Part )    Part    ->DeleteAllPresentations();
      if ( Domain )  Domain  ->DeleteAllPresentations();
      if ( Host )    Host    ->DeleteAllPresentations();
      //
      if ( Skinner ) Skinner ->DeleteAllPresentations();
      if ( Section ) Section ->DeleteAllPresentations();
      //
      if ( UBend )   UBend   ->DeleteAllPresentations();
    }
  } Prs;

public:

  //! \return single instance of facilities.
  static Handle(common_facilities) Instance()
  {
    if ( m_ref.IsNull() )
      m_ref = new common_facilities;

    return m_ref;
  }

public:

  static std::string ADDR(void* _transient)
  {
    std::string addr_str;
    std::ostringstream ost;
    ost << _transient;
    addr_str = ost.str();

    size_t pos = 0;
    while ( addr_str[pos] == '0' )
      pos++;

    if ( pos )
      addr_str = QrCore::substr( addr_str, (int) pos, (int) (addr_str.size() - pos) );

    return addr_str;
  }

private:

  inline common_facilities()
  //
    : DecompBrowser     (NULL),
      ObjectBrowser     (NULL),
      ObjectBrowser_XDE (NULL),
      ViewerPart        (NULL),
      ViewerDomain      (NULL),
      ViewerDMU         (NULL)
  {
    Notifier = new journal_notifier;
    Plotter  = new journal_iv;
  }

private:

  static Handle(common_facilities) m_ref; //!< Single instance of facilities.

};

#endif
