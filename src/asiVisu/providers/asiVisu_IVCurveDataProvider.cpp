//-----------------------------------------------------------------------------
// Created on: 11 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_iv_curve_data_provider.h>

// Visualization includes
#include <asiData_IVCurveNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param N [in] source Node.
visu_iv_curve_data_provider::visu_iv_curve_data_provider(const Handle(ActAPI_INode)& N)
: visu_curve_data_provider(),
  m_node(N)
{
}

//-----------------------------------------------------------------------------

//! \return curve type.
Handle(Standard_Type) visu_iv_curve_data_provider::GetCurveType() const
{
  double f, l;
  return this->GetCurve(f, l)->DynamicType();
}

//! Not used.
Handle(Geom2d_Curve) visu_iv_curve_data_provider::GetCurve2d(double&, double&) const
{
  return NULL;
}

//! Accessor for curve.
//! \param f [out] first parameter.
//! \param l [out] last parameter.
//! \return curve.
Handle(Geom_Curve) visu_iv_curve_data_provider::GetCurve(double& f, double& l) const
{
  Handle(asiData_IVCurveNode)
    curve_n = Handle(asiData_IVCurveNode)::DownCast(m_node);
  //
  if ( curve_n.IsNull() || !curve_n->IsWellFormed() )
    return NULL;

  return curve_n->GetCurve(f, l);
}

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId visu_iv_curve_data_provider::GetNodeID() const
{
  return m_node->GetId();
}

//-----------------------------------------------------------------------------

//! Creates a copy of the Data Provider.
//! \return copy.
Handle(visu_iv_curve_data_provider) visu_iv_curve_data_provider::Clone() const
{
  return new visu_iv_curve_data_provider(m_node);
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_iv_curve_data_provider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_IVCurveNode)
    curve_n = Handle(asiData_IVCurveNode)::DownCast(m_node);
  //
  if ( curve_n.IsNull() || !curve_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << curve_n->Parameter(asiData_IVCurveNode::PID_Geometry);

  return out;
}
