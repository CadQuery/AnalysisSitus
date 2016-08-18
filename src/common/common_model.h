//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef common_model_h
#define common_model_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (calculus) includes
#include <calculus_design_law_node.h>

// A-Situs (common) includes
#include <common_partition.h>
#include <common_root_node.h>
#include <common_version.h>

// A-Situs (geometry) includes
#include <geom_boundary_edges_node.h>
#include <geom_part_node.h>
#include <geom_re_node.h>
#include <geom_sections_node.h>
#include <geom_section_node.h>
#include <geom_ubend_law_node.h>
#include <geom_ubend_laws_node.h>
#include <geom_ubend_node.h>
#include <geom_volume_node.h>

// A-Situs (mesh) includes
#include <tess_node.h>

// A-Situs (visualization) includes
#include <visu_iv_curve_node.h>
#include <visu_iv_curves_node.h>
#include <visu_iv_node.h>
#include <visu_iv_point_set_2d_node.h>
#include <visu_iv_point_set_node.h>
#include <visu_iv_points_2d_node.h>
#include <visu_iv_points_node.h>
#include <visu_iv_surface_node.h>
#include <visu_iv_surfaces_node.h>
#include <visu_iv_tess_item_node.h>
#include <visu_iv_tess_node.h>
#include <visu_iv_text_item_node.h>
#include <visu_iv_text_node.h>
#include <visu_iv_topo_item_node.h>
#include <visu_iv_topo_node.h>

// Active Data includes
#include <ActData_BaseModel.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(common_model, ActData_BaseModel)

//! Data Model of A-Situs.
class common_model : public ActData_BaseModel
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(common_model, ActData_BaseModel)

public:

  common_model();

//-----------------------------------------------------------------------------
// Populate and clear:
public:

  void Populate();
  void Clear();

//-----------------------------------------------------------------------------
// Accessors to Nodes:
public:

  Handle(tess_node)          GetMeshNode()     const;
  Handle(geom_part_node)     GetPartNode()     const;
  Handle(geom_re_node)       GetRENode()       const;
  Handle(geom_sections_node) GetSectionsNode() const;
  Handle(geom_ubend_node)    GetUBendNode()    const;
  Handle(visu_iv_node)       GetIVNode()       const;

//-----------------------------------------------------------------------------
// Overridden:
public:

  //! Create a cloned instance of Data Model.
  //! \return cloned instance.
  virtual Handle(ActAPI_IModel) Clone() const
  {
    return ActData_BaseModel::CloneInstance<common_model>();
  }

//-----------------------------------------------------------------------------
// Partitions:
public:

  //! Accessor for a Partition instance dedicated to Calculus Design Law Nodes.
  //! \return requested Partition.
  Handle(common_partition<calculus_design_law_node>) GetCalculusDesignLawPartition() const
  {
    return Handle(common_partition<calculus_design_law_node>)::DownCast( this->Partition(Partition_CalculusDesignLaw) );
  }

  //! Accessor for a Partition instance dedicated to Mesh Nodes.
  //! \return requested Partition.
  Handle(common_partition<tess_node>) GetMeshPartition() const
  {
    return Handle(common_partition<tess_node>)::DownCast( this->Partition(Partition_Mesh) );
  }

  //! Accessor for a Partition instance dedicated to root Nodes.
  //! \return requested Partition.
  Handle(common_partition<common_root_node>) GetRootPartition() const
  {
    return Handle(common_partition<common_root_node>)::DownCast( this->Partition(Partition_Root) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Sections Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_sections_node>) GetSectionsPartition() const
  {
    return Handle(common_partition<geom_sections_node>)::DownCast( this->Partition(Partition_Sections) );
  }

  //! Accessor for a Partition instance dedicated to Section Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_section_node>) GetSectionPartition() const
  {
    return Handle(common_partition<geom_section_node>)::DownCast( this->Partition(Partition_Section) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to U-bend Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_ubend_node>) GetUBendPartition() const
  {
    return Handle(common_partition<geom_ubend_node>)::DownCast( this->Partition(Partition_UBend) );
  }

  //! Accessor for a Partition instance dedicated to U-bend Laws Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_ubend_laws_node>) GetUBendLawsPartition() const
  {
    return Handle(common_partition<geom_ubend_laws_node>)::DownCast( this->Partition(Partition_UBendLaws) );
  }

  //! Accessor for a Partition instance dedicated to U-bend Law Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_ubend_law_node>) GetUBendLawPartition() const
  {
    return Handle(common_partition<geom_ubend_law_node>)::DownCast( this->Partition(Partition_UBendLaw) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Geometry Part Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_part_node>) GetPartPartition() const
  {
    return Handle(common_partition<geom_part_node>)::DownCast( this->Partition(Partition_GeomPart) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Face Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_face_node>) GetGeomFacePartition() const
  {
    return Handle(common_partition<geom_face_node>)::DownCast( this->Partition(Partition_GeomFace) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Surface Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_surf_node>) GetGeomSurfacePartition() const
  {
    return Handle(common_partition<geom_surf_node>)::DownCast( this->Partition(Partition_GeomSurface) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Edge Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_edge_node>) GetGeomEdgePartition() const
  {
    return Handle(common_partition<geom_edge_node>)::DownCast( this->Partition(Partition_GeomEdge) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Curve Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_curve_node>) GetGeomCurvePartition() const
  {
    return Handle(common_partition<geom_curve_node>)::DownCast( this->Partition(Partition_GeomCurve) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Boundary Edges Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_boundary_edges_node>) GetGeomBoundaryEdgesPartition() const
  {
    return Handle(common_partition<geom_boundary_edges_node>)::DownCast( this->Partition(Partition_GeomBoundaryEdges) );
  }

  //! Accessor for a Partition instance dedicated to Volume Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_volume_node>) GetGeomVolumePartition() const
  {
    return Handle(common_partition<geom_volume_node>)::DownCast( this->Partition(Partition_GeomVolume) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Reverse Engineering Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_re_node>) GetREPartition() const
  {
    return Handle(common_partition<geom_re_node>)::DownCast( this->Partition(Partition_RE) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Surfaces Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_re_surfaces_node>) GetRESurfacesPartition() const
  {
    return Handle(common_partition<geom_re_surfaces_node>)::DownCast( this->Partition(Partition_RESurfaces) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Surface Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_re_surface_node>) GetRESurfacePartition() const
  {
    return Handle(common_partition<geom_re_surface_node>)::DownCast( this->Partition(Partition_RESurface) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Contours Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_re_contours_node>) GetREContoursPartition() const
  {
    return Handle(common_partition<geom_re_contours_node>)::DownCast( this->Partition(Partition_REContours) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Contour Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_re_contour_node>) GetREContourPartition() const
  {
    return Handle(common_partition<geom_re_contour_node>)::DownCast( this->Partition(Partition_REContour) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Points Nodes.
  //! \return requested Partition.
  Handle(common_partition<geom_re_points_node>) GetREPointsPartition() const
  {
    return Handle(common_partition<geom_re_points_node>)::DownCast( this->Partition(Partition_REPoints) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to IV Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_node>) GetIVPartition() const
  {
    return Handle(common_partition<visu_iv_node>)::DownCast( this->Partition(Partition_IV) );
  }

  //! Accessor for a Partition instance dedicated to IV Points 2D Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_points_2d_node>) GetIVPoints2dPartition() const
  {
    return Handle(common_partition<visu_iv_points_2d_node>)::DownCast( this->Partition(Partition_IV_Points2d) );
  }

  //! Accessor for a Partition instance dedicated to IV Points Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_points_node>) GetIVPointsPartition() const
  {
    return Handle(common_partition<visu_iv_points_node>)::DownCast( this->Partition(Partition_IV_Points) );
  }

  //! Accessor for a Partition instance dedicated to IV Point Set 2D Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_point_set_2d_node>) GetIVPointSet2dPartition() const
  {
    return Handle(common_partition<visu_iv_point_set_2d_node>)::DownCast( this->Partition(Partition_IV_PointSet2d) );
  }

  //! Accessor for a Partition instance dedicated to IV Point Set Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_point_set_node>) GetIVPointSetPartition() const
  {
    return Handle(common_partition<visu_iv_point_set_node>)::DownCast( this->Partition(Partition_IV_PointSet) );
  }

  //! Accessor for a Partition instance dedicated to IV Curves Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_curves_node>) GetIVCurvesPartition() const
  {
    return Handle(common_partition<visu_iv_curves_node>)::DownCast( this->Partition(Partition_IV_Curves) );
  }

  //! Accessor for a Partition instance dedicated to IV Curve Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_curve_node>) GetIVCurvePartition() const
  {
    return Handle(common_partition<visu_iv_curve_node>)::DownCast( this->Partition(Partition_IV_Curve) );
  }

  //! Accessor for a Partition instance dedicated to IV Surfaces Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_surfaces_node>) GetIVSurfacesPartition() const
  {
    return Handle(common_partition<visu_iv_surfaces_node>)::DownCast( this->Partition(Partition_IV_Surfaces) );
  }

  //! Accessor for a Partition instance dedicated to IV Surface Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_surface_node>) GetIVSurfacePartition() const
  {
    return Handle(common_partition<visu_iv_surface_node>)::DownCast( this->Partition(Partition_IV_Surface) );
  }

  //! Accessor for a Partition instance dedicated to IV Topo Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_topo_node>) GetIVTopoPartition() const
  {
    return Handle(common_partition<visu_iv_topo_node>)::DownCast( this->Partition(Partition_IV_Topo) );
  }

  //! Accessor for a Partition instance dedicated to IV Topo Item Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_topo_item_node>) GetIVTopoItemPartition() const
  {
    return Handle(common_partition<visu_iv_topo_item_node>)::DownCast( this->Partition(Partition_IV_TopoItem) );
  }

  //! Accessor for a Partition instance dedicated to IV Tessellation Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_tess_node>) GetIVTessPartition() const
  {
    return Handle(common_partition<visu_iv_tess_node>)::DownCast( this->Partition(Partition_IV_Tess) );
  }

  //! Accessor for a Partition instance dedicated to IV Tessellation Item Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_tess_item_node>) GetIVTessItemPartition() const
  {
    return Handle(common_partition<visu_iv_tess_item_node>)::DownCast( this->Partition(Partition_IV_TessItem) );
  }

  //! Accessor for a Partition instance dedicated to IV Text Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_text_node>) GetIVTextPartition() const
  {
    return Handle(common_partition<visu_iv_text_node>)::DownCast( this->Partition(Partition_IV_Text) );
  }

  //! Accessor for a Partition instance dedicated to IV Text Item Nodes.
  //! \return requested Partition.
  Handle(common_partition<visu_iv_text_item_node>) GetIVTextItemPartition() const
  {
    return Handle(common_partition<visu_iv_text_item_node>)::DownCast( this->Partition(Partition_IV_TextItem) );
  }

private:

  virtual void
    initPartitions();

  virtual void
    initFunctionDrivers();

private:

  virtual Handle(ActAPI_IPartition)
    getVariablePartition(const VariableType& theVarType) const;

  virtual Handle(ActAPI_INode)
    getRootNode() const;

  virtual void
    invariantCopyRefs(ActAPI_FuncGUIDStream&         FuncGUIDs,
                      ActAPI_ParameterLocatorStream& Refs) const;

private:

  virtual int
    actualVersionApp();

  virtual Handle(ActData_CAFConverter)
    converterApp();

private:

  //! IDs of the registered Partitions.
  enum PartitionId
  {
    Partition_RealVar = 1, // Good practice to start with 1 when working with CAF
  //---------------------------------------------------------------------------
    Partition_Root,
  //---------------------------------------------------------------------------
    Partition_GeomPart,
    Partition_GeomFace,
    Partition_GeomSurface,
    Partition_GeomEdge,
    Partition_GeomCurve,
    Partition_GeomBoundaryEdges,
    Partition_GeomVolume,
  //---------------------------------------------------------------------------
    Partition_Mesh,
  //---------------------------------------------------------------------------
    Partition_RE,
    Partition_RESurfaces,
    Partition_RESurface,
    Partition_REContours,
    Partition_REContour,
    Partition_REPoints,
  //---------------------------------------------------------------------------
    Partition_Sections,
    Partition_Section,
  //---------------------------------------------------------------------------
    Partition_CalculusDesignLaw,
  //---------------------------------------------------------------------------
    Partition_UBend,
    Partition_UBendLaws,
    Partition_UBendLaw,
  //---------------------------------------------------------------------------
    Partition_IV,
    Partition_IV_Points2d,
    Partition_IV_Points,
    Partition_IV_PointSet2d,
    Partition_IV_PointSet,
    Partition_IV_Curves,
    Partition_IV_Curve,
    Partition_IV_Surfaces,
    Partition_IV_Surface,
    Partition_IV_Topo,
    Partition_IV_TopoItem,
    Partition_IV_Tess,
    Partition_IV_TessItem,
    Partition_IV_Text,
    Partition_IV_TextItem
  };

};

#endif
