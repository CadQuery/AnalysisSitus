//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_DesignLaw_h
#define asiAlgo_DesignLaw_h

// A-Situs (calculus) includes
#include <asiAlgo_FuncUnivariate.h>

// OCCT includes
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiAlgo_DesignLaw, asiAlgo_FuncUnivariate)

//! Evolution law for a scalar design variable.
class asiAlgo_DesignLaw : public asiAlgo_FuncUnivariate
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_DesignLaw, asiAlgo_FuncUnivariate)

public:

  asiAlgo_EXPORT
    asiAlgo_DesignLaw(const Handle(TColStd_HArray1OfReal)& poles,
                      const double                         last);

public:

  asiAlgo_EXPORT virtual double
    operator() (const double x);

  asiAlgo_EXPORT virtual double
    Min_X() const;

  asiAlgo_EXPORT virtual double
    Max_X() const;

public:

  Handle(TColStd_HArray1OfReal)&    ChangePoles() { return m_poles; }
  Handle(TColStd_HArray1OfReal)&    ChangeKnots() { return m_knots; }
  Handle(TColStd_HArray1OfInteger)& ChangeMults() { return m_mults; }

protected:

  Handle(TColStd_HArray1OfReal)    m_poles; //!< Poles.
  Handle(TColStd_HArray1OfReal)    m_knots; //!< Knots.
  Handle(TColStd_HArray1OfInteger) m_mults; //!< Multiplicities.

};

#endif
