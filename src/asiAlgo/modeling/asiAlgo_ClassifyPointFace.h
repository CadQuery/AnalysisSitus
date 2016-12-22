//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_ClassifyPointFace_h
#define asiAlgo_ClassifyPointFace_h

// Geometry includes
#include <asiAlgo_Classifier.h>

// OCCT includes
#include <IntTools_FClass2d.hxx>
#include <TopoDS_Face.hxx>

// Active Data includes
#include <ActAPI_IProgressNotifier.h>

//! Point-Face classifier.
class asiAlgo_ClassifyPointFace
{
public:

  asiAlgo_EXPORT
    asiAlgo_ClassifyPointFace(const TopoDS_Face& F,
                              const double       inaccuracy,
                              const double       precision);

public:

  asiAlgo_EXPORT asiAlgo_Membership
    operator()(const gp_Pnt2d&      PonS,
               ActAPI_ProgressEntry Journal);

  asiAlgo_EXPORT asiAlgo_Membership
    operator()(const gp_Pnt&        P,
               const bool           checkGap,
               ActAPI_ProgressEntry Journal);

  asiAlgo_EXPORT asiAlgo_Membership
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
