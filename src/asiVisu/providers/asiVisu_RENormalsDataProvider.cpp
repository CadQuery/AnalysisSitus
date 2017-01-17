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
//! \param normalsNode [in] Normals Node.
asiVisu_RENormalsDataProvider::asiVisu_RENormalsDataProvider(const Handle(asiData_RENormalsNode)& normalsNode)
: asiVisu_NormalsDataProvider(normalsNode)
{}

//-----------------------------------------------------------------------------

//! \return points where normals are located.
Handle(asiAlgo_PointCloud<float>) asiVisu_RENormalsDataProvider::GetPointsf() const
{
  Handle(asiData_RENormalsNode)
    normals_n = Handle(asiData_RENormalsNode)::DownCast(m_node);
  //
  if ( normals_n.IsNull() || !normals_n->IsWellFormed() )
    return NULL;

  return normals_n->GetNormalsf();
}

//-----------------------------------------------------------------------------

//! \return normals to visualize.
Handle(asiAlgo_PointCloud<float>) asiVisu_RENormalsDataProvider::GetNormalsf() const
{
  Handle(asiData_RENormalsNode)
    normals_n = Handle(asiData_RENormalsNode)::DownCast(m_node);
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
    normals_n = Handle(asiData_RENormalsNode)::DownCast(m_node);
  //
  if ( normals_n.IsNull() || !normals_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << normals_n->Parameter(asiData_RENormalsNode::PID_Normals);

  return out;
}
