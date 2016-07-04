//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_classify_point_solid_h
#define geom_classify_point_solid_h

// Geometry includes
#include <geom_classifier.h>

// OCCT includes
#include <BRepClass3d_SolidClassifier.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Solid.hxx>

//! Point-Solid classifier.
class geom_classify_point_solid
{
public:

  ASitus_EXPORT
    geom_classify_point_solid(const TopoDS_Solid& S,
                              const bool          isOpenCascade);

public:

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt&        P,
               ActAPI_ProgressEntry Journal);

protected:

  TopoDS_Solid m_S; //!< Working solid.
  //
  bool                        m_bOcctMode;
  BRepClass3d_SolidClassifier m_occtClassifier;

};

#endif
