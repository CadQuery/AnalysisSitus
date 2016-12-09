//-----------------------------------------------------------------------------
// Created on: 09 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
