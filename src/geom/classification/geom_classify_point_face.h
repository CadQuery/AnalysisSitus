//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_classify_point_face_h
#define geom_classify_point_face_h

// Geometry includes
#include <geom_classifier.h>

// OCCT includes
#include <IntTools_FClass2d.hxx>
#include <TopoDS_Face.hxx>

//! Point-Face classifier.
class geom_classify_point_face
{
public:

  ASitus_EXPORT
    geom_classify_point_face(const TopoDS_Face& F,
                             const double       inaccuracy,
                             const double       precision);

public:

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt2d&      PonS,
               ActAPI_ProgressEntry Journal);

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt&        P,
               const bool           checkGap,
               ActAPI_ProgressEntry Journal);

  ASitus_EXPORT geom_membership
    operator()(const gp_Pnt&        P,
               const bool           checkGap,
               gp_Pnt2d&            UV,
               ActAPI_ProgressEntry Journal);

protected:

  TopoDS_Face       m_F;           //!< Working face.
  double            m_fPrecision;  //!< Precision of optimization method.
  double            m_fInaccuracy; //!< Inaccuracy in the input geometry.
  IntTools_FClass2d m_fclass;      //!< Internal classifier.

};

#endif
