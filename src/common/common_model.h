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

  Handle(tess_node)          Mesh_Node()     const;
  Handle(geom_part_node)     PartNode()     const;
  Handle(geom_re_node)       RENode()       const;
  Handle(geom_sections_node) SectionsNode() const;
  Handle(geom_ubend_node)    UBendNode()    const;
  Handle(visu_iv_node)       IVNode()       const;

//-----------------------------------------------------------------------------
// Overridden:
public:

  //! Create a cloned instance of Data Model.
  //! \return cloned instance.
  inline virtual Handle(ActAPI_IModel) Clone() const
  {
    return ActData_BaseModel::CloneInstance<common_model>();
  }

//-----------------------------------------------------------------------------
// Partitions:
public:

  //! Accessor for a Partition instance dedicated to Calculus Design Law Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<calculus_design_law_node>) CalculusDesignLawPartition() const
  {
    return Handle(common_partition<calculus_design_law_node>)::DownCast( this->Partition(Partition_CalculusDesignLaw) );
  }

  //! Accessor for a Partition instance dedicated to Mesh Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<tess_node>) MeshPartition() const
  {
    return Handle(common_partition<tess_node>)::DownCast( this->Partition(Partition_Mesh) );
  }

  //! Accessor for a Partition instance dedicated to root Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<common_root_node>) RootPartition() const
  {
    return Handle(common_partition<common_root_node>)::DownCast( this->Partition(Partition_Root) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Sections Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_sections_node>) SectionsPartition() const
  {
    return Handle(common_partition<geom_sections_node>)::DownCast( this->Partition(Partition_Sections) );
  }

  //! Accessor for a Partition instance dedicated to Section Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_section_node>) SectionPartition() const
  {
    return Handle(common_partition<geom_section_node>)::DownCast( this->Partition(Partition_Section) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to U-bend Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_ubend_node>) UBendPartition() const
  {
    return Handle(common_partition<geom_ubend_node>)::DownCast( this->Partition(Partition_UBend) );
  }

  //! Accessor for a Partition instance dedicated to U-bend Laws Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_ubend_laws_node>) UBendLawsPartition() const
  {
    return Handle(common_partition<geom_ubend_laws_node>)::DownCast( this->Partition(Partition_UBendLaws) );
  }

  //! Accessor for a Partition instance dedicated to U-bend Law Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_ubend_law_node>) UBendLawPartition() const
  {
    return Handle(common_partition<geom_ubend_law_node>)::DownCast( this->Partition(Partition_UBendLaw) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Geometry Part Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_part_node>) PartPartition() const
  {
    return Handle(common_partition<geom_part_node>)::DownCast( this->Partition(Partition_GeomPart) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Face Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_face_node>) GeomFacePartition() const
  {
    return Handle(common_partition<geom_face_node>)::DownCast( this->Partition(Partition_GeomFace) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Surface Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_surf_node>) GeomSurfacePartition() const
  {
    return Handle(common_partition<geom_surf_node>)::DownCast( this->Partition(Partition_GeomSurface) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Boundary Edges Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_boundary_edges_node>) GeomBoundaryEdgesPartition() const
  {
    return Handle(common_partition<geom_boundary_edges_node>)::DownCast( this->Partition(Partition_GeomBoundaryEdges) );
  }

  //! Accessor for a Partition instance dedicated to Volume Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_volume_node>) GeomVolumePartition() const
  {
    return Handle(common_partition<geom_volume_node>)::DownCast( this->Partition(Partition_GeomVolume) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Reverse Engineering Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_re_node>) REPartition() const
  {
    return Handle(common_partition<geom_re_node>)::DownCast( this->Partition(Partition_RE) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Surfaces Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_re_surfaces_node>) RESurfacesPartition() const
  {
    return Handle(common_partition<geom_re_surfaces_node>)::DownCast( this->Partition(Partition_RESurfaces) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Surface Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_re_surface_node>) RESurfacePartition() const
  {
    return Handle(common_partition<geom_re_surface_node>)::DownCast( this->Partition(Partition_RESurface) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Contours Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_re_contours_node>) REContoursPartition() const
  {
    return Handle(common_partition<geom_re_contours_node>)::DownCast( this->Partition(Partition_REContours) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Contour Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_re_contour_node>) REContourPartition() const
  {
    return Handle(common_partition<geom_re_contour_node>)::DownCast( this->Partition(Partition_REContour) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Points Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<geom_re_points_node>) REPointsPartition() const
  {
    return Handle(common_partition<geom_re_points_node>)::DownCast( this->Partition(Partition_REPoints) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to IV Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_node>) IVPartition() const
  {
    return Handle(common_partition<visu_iv_node>)::DownCast( this->Partition(Partition_IV) );
  }

  //! Accessor for a Partition instance dedicated to IV Points 2D Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_points_2d_node>) IVPoints2dPartition() const
  {
    return Handle(common_partition<visu_iv_points_2d_node>)::DownCast( this->Partition(Partition_IV_Points2d) );
  }

  //! Accessor for a Partition instance dedicated to IV Points Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_points_node>) IVPointsPartition() const
  {
    return Handle(common_partition<visu_iv_points_node>)::DownCast( this->Partition(Partition_IV_Points) );
  }

  //! Accessor for a Partition instance dedicated to IV Point Set 2D Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_point_set_2d_node>) IVPointSet2dPartition() const
  {
    return Handle(common_partition<visu_iv_point_set_2d_node>)::DownCast( this->Partition(Partition_IV_PointSet2d) );
  }

  //! Accessor for a Partition instance dedicated to IV Point Set Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_point_set_node>) IVPointSetPartition() const
  {
    return Handle(common_partition<visu_iv_point_set_node>)::DownCast( this->Partition(Partition_IV_PointSet) );
  }

  //! Accessor for a Partition instance dedicated to IV Curves Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_curves_node>) IVCurvesPartition() const
  {
    return Handle(common_partition<visu_iv_curves_node>)::DownCast( this->Partition(Partition_IV_Curves) );
  }

  //! Accessor for a Partition instance dedicated to IV Curve Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_curve_node>) IVCurvePartition() const
  {
    return Handle(common_partition<visu_iv_curve_node>)::DownCast( this->Partition(Partition_IV_Curve) );
  }

  //! Accessor for a Partition instance dedicated to IV Surfaces Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_surfaces_node>) IVSurfacesPartition() const
  {
    return Handle(common_partition<visu_iv_surfaces_node>)::DownCast( this->Partition(Partition_IV_Surfaces) );
  }

  //! Accessor for a Partition instance dedicated to IV Surface Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_surface_node>) IVSurfacePartition() const
  {
    return Handle(common_partition<visu_iv_surface_node>)::DownCast( this->Partition(Partition_IV_Surface) );
  }

  //! Accessor for a Partition instance dedicated to IV Topo Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_topo_node>) IVTopoPartition() const
  {
    return Handle(common_partition<visu_iv_topo_node>)::DownCast( this->Partition(Partition_IV_Topo) );
  }

  //! Accessor for a Partition instance dedicated to IV Topo Item Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_topo_item_node>) IVTopoItemPartition() const
  {
    return Handle(common_partition<visu_iv_topo_item_node>)::DownCast( this->Partition(Partition_IV_TopoItem) );
  }

  //! Accessor for a Partition instance dedicated to IV Tessellation Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_tess_node>) IVTessPartition() const
  {
    return Handle(common_partition<visu_iv_tess_node>)::DownCast( this->Partition(Partition_IV_Tess) );
  }

  //! Accessor for a Partition instance dedicated to IV Tessellation Item Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_tess_item_node>) IVTessItemPartition() const
  {
    return Handle(common_partition<visu_iv_tess_item_node>)::DownCast( this->Partition(Partition_IV_TessItem) );
  }

  //! Accessor for a Partition instance dedicated to IV Text Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_text_node>) IVTextPartition() const
  {
    return Handle(common_partition<visu_iv_text_node>)::DownCast( this->Partition(Partition_IV_Text) );
  }

  //! Accessor for a Partition instance dedicated to IV Text Item Nodes.
  //! \return requested Partition.
  inline Handle(common_partition<visu_iv_text_item_node>) IVTextItemPartition() const
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
