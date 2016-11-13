//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef exeAsBuilt_RE_h
#define exeAsBuilt_RE_h

// exeAsBuilt includes
#include <exeAsBuilt_Model.h>

// asiVisu includes
#include <asiVisu_PrsManager.h>

// OCCT includes
#include <Geom_Surface.hxx>
#include <TopoDS_Wire.hxx>

//! Data Model API for reverse engineering.
class exeAsBuilt_RE
{
public:

  //! ctor.
  //! \param model [in] Data Model instance.
  exeAsBuilt_RE(const Handle(exeAsBuilt_Model)& model)
  //
  : m_model(model) {}

public:

  Handle(asiData_RENode)
    Create_RE();

//---------------------------------------------------------------------------//

  Handle(asiData_RESurfaceNode)
    Create_Surface(const Handle(Geom_Surface)& surface,
                   const double                uLimit,
                   const double                vLimit);

  void
    Clean_Surfaces();

//---------------------------------------------------------------------------//

  Handle(asiData_REContourNode)
    Create_Contour(const TopoDS_Wire& contour);

  void
    Clean_Contours();

protected:

  Handle(exeAsBuilt_Model) m_model; //!< Data Model instance.

};

#endif
