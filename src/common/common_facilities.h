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

  Handle(common_model)              Model;        //!< Data Model instance.
  vtkSmartPointer<visu_prs_manager> PrsManager;   //!< Presentation Manager.
  vtkSmartPointer<visu_prs_manager> PrsManager2d; //!< Presentation Manager in 2D.

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

  static Handle(common_facilities) m_ref;

};

#endif
