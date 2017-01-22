//-----------------------------------------------------------------------------
// Created on: 09 December 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_RENormalsDataProvider.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param pointsNode  [in] points Node.
//! \param normalsNode [in] normals Node.
asiVisu_RENormalsDataProvider::asiVisu_RENormalsDataProvider(const Handle(asiData_REPointsNode)&  pointsNode,
                                                             const Handle(asiData_RENormalsNode)& normalsNode)
: asiVisu_NormalsDataProvider(pointsNode, normalsNode)
{}

//-----------------------------------------------------------------------------

//! \return points where normals are located.
Handle(asiAlgo_PointCloud<float>) asiVisu_RENormalsDataProvider::GetPointsf() const
{
  Handle(asiData_REPointsNode)
    points_n = Handle(asiData_REPointsNode)::DownCast(m_points);
  //
  if ( points_n.IsNull() || !points_n->IsWellFormed() )
    return NULL;

  return points_n->GetPointsf();
}

//-----------------------------------------------------------------------------

//! \return normals to visualize.
Handle(asiAlgo_PointCloud<float>) asiVisu_RENormalsDataProvider::GetNormalsf() const
{
  Handle(asiData_RENormalsNode)
    normals_n = Handle(asiData_RENormalsNode)::DownCast(m_normals);
  //
  if ( normals_n.IsNull() || !normals_n->IsWellFormed() )
    return NULL;

  return normals_n->GetNormalsf();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_RENormalsDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_RENormalsNode)
    normals_n = Handle(asiData_RENormalsNode)::DownCast(m_normals);
  //
  if ( normals_n.IsNull() || !normals_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << normals_n->Parameter(asiData_RENormalsNode::PID_Normals);

  return out;
}

//-----------------------------------------------------------------------------

//! \return max modulus for a normal.
double asiVisu_RENormalsDataProvider::GetMaxNormModulus() const
{
  return 10.0;
}
