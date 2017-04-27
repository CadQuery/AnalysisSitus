//-----------------------------------------------------------------------------
// Created on: 27 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_REGaussMapRefinedAxisDataProvider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param gaussNode [in] Gauss map Node.
asiVisu_REGaussMapRefinedAxisDataProvider::asiVisu_REGaussMapRefinedAxisDataProvider(const Handle(asiData_REGaussMapNode)& gaussNode)
: asiVisu_VectorsDataProvider(gaussNode)
{}

//-----------------------------------------------------------------------------

//! \return points where normals are located.
Handle(asiAlgo_BaseCloud<float>) asiVisu_REGaussMapRefinedAxisDataProvider::GetPointsf()
{
  Handle(asiData_REGaussMapNode)
    gauss_n = Handle(asiData_REGaussMapNode)::DownCast(m_source);
  //
  if ( gauss_n.IsNull() || !gauss_n->IsWellFormed() )
    return NULL;

  Handle(asiAlgo_BaseCloud<float>) points = new asiAlgo_BaseCloud<float>();
  //
  points->AddElement(0, 0, 0);

  return points;
}

//-----------------------------------------------------------------------------

//! \return vectors to visualize.
Handle(asiAlgo_BaseCloud<float>) asiVisu_REGaussMapRefinedAxisDataProvider::GetVectorsf()
{
  Handle(asiData_REGaussMapNode)
    gauss_n = Handle(asiData_REGaussMapNode)::DownCast(m_source);
  //
  if ( gauss_n.IsNull() || !gauss_n->IsWellFormed() )
    return NULL;

  double ax, ay, az;
  gauss_n->GetRefinedAxis(ax, ay, az);

  Handle(asiAlgo_BaseCloud<float>) vectors = new asiAlgo_BaseCloud<float>();
  //
  vectors->AddElement( (float) ax, (float) ay, (float) az );

  return vectors;
}

//-----------------------------------------------------------------------------

//! \return max modulus for a vector.
double asiVisu_REGaussMapRefinedAxisDataProvider::GetMaxVectorModulus() const
{
  return 10.0;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_REGaussMapRefinedAxisDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_REGaussMapNode)
    gauss_n = Handle(asiData_REGaussMapNode)::DownCast(m_source);
  //
  if ( gauss_n.IsNull() || !gauss_n->IsWellFormed() )
    return out;

  // Register Parameters
  out << gauss_n->Parameter(asiData_REGaussMapNode::PID_RefinedAxisX)
      << gauss_n->Parameter(asiData_REGaussMapNode::PID_RefinedAxisY)
      << gauss_n->Parameter(asiData_REGaussMapNode::PID_RefinedAxisZ);

  return out;
}
