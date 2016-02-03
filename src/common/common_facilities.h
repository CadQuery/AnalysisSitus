//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef common_facilities_h
#define common_facilities_h

// A-Situs (common) includes
#include <common_model.h>

// A-Situs (GUI) includes
#include <gui_object_browser.h>
#include <gui_object_browser_xde.h>

// A-Situs (visualization) includes
#include <visu_prs_manager.h>

DEFINE_STANDARD_HANDLE(common_facilities, Standard_Transient)

//! Common tools and objects for A-Situs software.
class common_facilities : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(common_facilities, Standard_Transient)

public:

  Handle(common_model)    Model;             //!< Data Model instance.
  gui_object_browser*     ObjectBrowser;     //!< Object Browser.
  gui_object_browser_xde* ObjectBrowser_XDE; //!< Object Browser for XDE.
  Handle(ActAPI_INode)    CurrentNode;       //!< Currently selected Node.

  //! Visualization facilities.
  struct t_prs
  {
  //---------------------------------------------------------------------------
    vtkSmartPointer<visu_prs_manager> DMU;     //!< DMU.
    vtkSmartPointer<visu_prs_manager> Mesh;    //!< Mesh.
    vtkSmartPointer<visu_prs_manager> Part;    //!< Part.
    vtkSmartPointer<visu_prs_manager> Domain;  //!< Face domain.
    vtkSmartPointer<visu_prs_manager> Surface; //!< Host surface.
  //---------------------------------------------------------------------------
    vtkSmartPointer<visu_prs_manager> Skinner; //!< Skinner.
    vtkSmartPointer<visu_prs_manager> Section; //!< Section.
  //---------------------------------------------------------------------------
    vtkSmartPointer<visu_prs_manager> UBend;   //!< U-bend.

    inline void ActualizeAll()
    {
      if ( DMU )     DMU     ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      if ( Part )    Part    ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      if ( Domain )  Domain  ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      if ( Surface ) Surface ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      //
      if ( Skinner ) Skinner ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      if ( Section ) Section ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
      //
      if ( UBend )   UBend   ->Actualize(common_facilities::Instance()->Model->GetRootNode(), true);
    }

    inline void DeRenderAll()
    {
      if ( DMU )     DMU     ->DeRenderAllPresentations();
      if ( Part )    Part    ->DeRenderAllPresentations();
      if ( Domain )  Domain  ->DeRenderAllPresentations();
      if ( Surface ) Surface ->DeRenderAllPresentations();
      //
      if ( Skinner ) Skinner ->DeRenderAllPresentations();
      if ( Section ) Section ->DeRenderAllPresentations();
      //
      if ( UBend )   UBend   ->DeRenderAllPresentations();
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

private:

  inline common_facilities() : ObjectBrowser(NULL), ObjectBrowser_XDE(NULL) {} //!< Not available for client code.

private:

  static Handle(common_facilities) m_ref; //!< Single instance of facilities.

};

#endif
