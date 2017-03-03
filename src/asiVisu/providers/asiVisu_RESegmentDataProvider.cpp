//-----------------------------------------------------------------------------
// Created on: 01 March 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_RESegmentDataProvider.h>

// asiData includes
#include <asiData_REPointsNode.h>

// OCCT includes
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param segment_n [in] Segment Node.
asiVisu_RESegmentDataProvider::asiVisu_RESegmentDataProvider(const Handle(asiData_RESegmentNode)& segment_n)
: asiVisu_PointsDataProvider(segment_n)
{}

//-----------------------------------------------------------------------------

//! \return point cloud to visualize.
Handle(asiAlgo_BaseCloud<double>) asiVisu_RESegmentDataProvider::GetPoints() const
{
  Handle(asiData_RESegmentNode)
    segment_n = Handle(asiData_RESegmentNode)::DownCast(m_node);
  //
  if ( segment_n.IsNull() || !segment_n->IsWellFormed() )
    return NULL;

  // Get indices of the points in a segment
  Handle(TColStd_HPackedMapOfInteger) hindices = segment_n->GetIndices();
  //
  if ( hindices.IsNull() )
    return NULL;

  // Get the entire point cloud
  Handle(ActAPI_INode)              points_base = segment_n->GetParentNode()  // Segments
                                                           ->GetParentNode(); // Points
  Handle(asiData_REPointsNode)      points_n    = Handle(asiData_REPointsNode)::DownCast(points_base);
  Handle(asiAlgo_BaseCloud<double>) universum   = points_n->GetPoints();

  // Points to visualize
  Handle(asiAlgo_BaseCloud<double>) result  = new asiAlgo_BaseCloud<double>;
  const TColStd_PackedMapOfInteger& indices = hindices->Map();
  //
  for ( TColStd_MapIteratorOfPackedMapOfInteger it(indices); it.More(); it.Next() )
  {
    const int pidx = it.Key();
    double x, y, z;
    universum->GetElement(pidx, x, y, z);
    result->AddElement(x, y, z);
  }

  return result;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_RESegmentDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_RESegmentNode)
    segment_n = Handle(asiData_RESegmentNode)::DownCast(m_node);
  //
  if ( segment_n.IsNull() || !segment_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << segment_n->Parameter(asiData_RESegmentNode::PID_Indices);

  return out;
}
