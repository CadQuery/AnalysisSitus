//-----------------------------------------------------------------------------
// Created on: 10 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_ubend_law_data_provider.h>

// A-Situs (geometry) includes
#include <geom_ubend.h>

//! ctor accepting the law Node.
//! \param N [in] law Node.
visu_ubend_law_data_provider::visu_ubend_law_data_provider(const Handle(geom_ubend_law_node)& N)
//
: visu_curve_data_provider (),
  m_node                   (N)
{}

//! \return curve type.
Handle(Standard_Type) visu_ubend_law_data_provider::GetCurveType() const
{
  return STANDARD_TYPE(Geom2d_BSplineCurve);
}

//! Extracts X or Y evolution law from a persistent object.
//! \return X or Y evolution law.
Handle(Geom2d_Curve) visu_ubend_law_data_provider::GetCurve2d(double&, double&) const
{
  Handle(calculus_design_law) xLaw = m_node->GetLaw_X()->AsLaw();
  Handle(calculus_design_law) yLaw = m_node->GetLaw_Y()->AsLaw();
  //
  return geom_ubend::CreateParametricLaw(xLaw, yLaw);
}

//! Not used.
Handle(Geom_Curve) visu_ubend_law_data_provider::GetCurve(double&, double&) const
{
  return NULL;
}

//! \return persistent Node ID.
ActAPI_DataObjectId visu_ubend_law_data_provider::GetNodeID() const
{
  return m_node->GetId();
}

//! \return sensitive Parameters,
Handle(ActAPI_HParameterList) visu_ubend_law_data_provider::translationSources() const
{
  // Access coordinate laws
  Handle(calculus_design_law_node) x_law_n = m_node->GetLaw_X();
  Handle(calculus_design_law_node) y_law_n = m_node->GetLaw_Y();
  //
  return ActParamStream() << x_law_n->Parameter(calculus_design_law_node::PID_Degree)
                          << x_law_n->Parameter(calculus_design_law_node::PID_Knots)
                          << x_law_n->Parameter(calculus_design_law_node::PID_Mults)
                          << x_law_n->Parameter(calculus_design_law_node::PID_Poles)
                          << y_law_n->Parameter(calculus_design_law_node::PID_Degree)
                          << y_law_n->Parameter(calculus_design_law_node::PID_Knots)
                          << y_law_n->Parameter(calculus_design_law_node::PID_Mults)
                          << y_law_n->Parameter(calculus_design_law_node::PID_Poles);
}
