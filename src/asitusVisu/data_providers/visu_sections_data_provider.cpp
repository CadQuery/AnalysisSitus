//-----------------------------------------------------------------------------
// Created on: 09 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_sections_data_provider.h>

// A-Situs (common) includes
#include <common_facilities.h>

// A-Situs (geometry) includes
#include <geom_sections_node.h>
#include <geom_utils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Builder.hxx>

//-----------------------------------------------------------------------------

//! Constructor accepting source ID.
//! \param nodeId [in] ID of the target Data Node.
visu_sections_data_provider::visu_sections_data_provider(const ActAPI_DataObjectId& nodeId)
: visu_data_provider()
{
  m_nodeID = nodeId;
}

//-----------------------------------------------------------------------------

//! Returns ID of the Data Node which is being sourced by the visualization
//! pipeline. This ID is bound to the pipeline's actor in order to have a
//! back-reference from Presentation to Data Object.
//! \return Node ID.
ActAPI_DataObjectId visu_sections_data_provider::GetNodeID() const
{
  return m_nodeID;
}

//! \return transformed sections ready to be visualized.
TopoDS_Compound visu_sections_data_provider::GetTransformedSections() const
{
  Handle(geom_sections_node)
    sections_n = common_facilities::Instance()->Model->GetSectionsNode();

  if ( sections_n.IsNull() || !sections_n->IsWellFormed() )
    return TopoDS_Compound();

  // Prepare facilities to build up a compound
  TopoDS_Compound C;
  BRep_Builder().MakeCompound(C);

  // Iterate over the child Nodes which are the individual sections
  for ( Handle(ActAPI_IChildIterator) cit = sections_n->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(geom_section_node) section_n = Handle(geom_section_node)::DownCast( cit->Value() );
    if ( section_n.IsNull() || !section_n->IsWellFormed() )
      continue;

    // Get stored Transformation coefficients
    double Tx, Ty, Tz, Rx, Ry, Rz;
    section_n->GetTransform(Tx, Ty, Tz, Rx, Ry, Rz);

    // Apply transformation to the geometry
    gp_Trsf T = geom_utils::Transformation(Tx, Ty, Tz, Rx, Ry, Rz);
    TopoDS_Shape section = geom_utils::ApplyTransformation(section_n->GetGeometry(), T, true);

    // Build up compound
    BRep_Builder().Add(C, section);
  }

  return C;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_sections_data_provider::translationSources() const
{
  Handle(geom_sections_node)
    sections_n = common_facilities::Instance()->Model->GetSectionsNode();

  if ( sections_n.IsNull() || !sections_n->IsWellFormed() )
    return NULL;

  // Resulting Parameters
  ActParamStream out;

  // Iterate over the child Nodes which are the individual sections
  for ( Handle(ActAPI_IChildIterator) cit = sections_n->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(geom_section_node) section_n = Handle(geom_section_node)::DownCast( cit->Value() );
    if ( section_n.IsNull() || !section_n->IsWellFormed() )
      continue;

    out << section_n->Parameter(geom_section_node::PID_Geometry)
        << section_n->Parameter(geom_section_node::PID_Tx)
        << section_n->Parameter(geom_section_node::PID_Ty)
        << section_n->Parameter(geom_section_node::PID_Tz)
        << section_n->Parameter(geom_section_node::PID_Rx)
        << section_n->Parameter(geom_section_node::PID_Ry)
        << section_n->Parameter(geom_section_node::PID_Rz);
  }

  return out;
}
