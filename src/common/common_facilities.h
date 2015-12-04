//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef common_facilities_h
#define common_facilities_h

// A-Situs (common) includes
#include <common_model.h>

// A-Situs (GUI) includes
#include <gui_object_browser.h>

// A-Situs (visualization) includes
#include <visu_prs_manager.h>

DEFINE_STANDARD_HANDLE(common_facilities, Standard_Transient)

//! Common tools and objects for A-Situs software.
class common_facilities : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI(common_facilities, Standard_Transient)

public:

  Handle(common_model) Model;         //!< Data Model instance.
  gui_object_browser*  ObjectBrowser; //!< Object Browser.

  //! Visualization facilities.
  struct t_prs
  {
    vtkSmartPointer<visu_prs_manager> Part;    //!< Part.
    vtkSmartPointer<visu_prs_manager> Domain;  //!< Face domain.
    vtkSmartPointer<visu_prs_manager> Surface; //!< Host surface.

    inline void DeRenderAll()
    {
      Part    ->DeRenderAllPresentations();
      Domain  ->DeRenderAllPresentations();
      Surface ->DeRenderAllPresentations();
    }
  } Prs;

public:

  static Handle(common_facilities) Instance()
  {
    if ( m_ref.IsNull() )
      m_ref = new common_facilities;

    return m_ref;
  }

private:

  common_facilities() {}

private:

  static Handle(common_facilities) m_ref; //!< Single instance of facilities.

};

#endif
