//-----------------------------------------------------------------------------
// Created on: 26 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef asiData_Model_h
#define asiData_Model_h

// A-Situs includes
#include <asiData.h>

// A-Situs (calculus) includes
#include <asiData_DesignLawNode.h>

// A-Situs (common) includes
#include <asiData_Partition.h>
#include <asiData_RootNode.h>
#include <common_version.h>

// A-Situs (geometry) includes
#include <asiData_BoundaryEdgesNode.h>
#include <asiData_PartNode.h>
#include <asiData_RENode.h>
#include <geom_sections_node.h>
#include <geom_section_node.h>
#include <geom_ubend_law_node.h>
#include <geom_ubend_laws_node.h>
#include <geom_ubend_node.h>
#include <geom_volume_node.h>

// A-Situs (mesh) includes
#include <asiData_TessNode.h>

// A-Situs (visualization) includes
#include <asiData_IVCurveNode.h>
#include <asiData_IVCurvesNode.h>
#include <asiData_IVNode.h>
#include <asiData_IVPointSet2dNode.h>
#include <asiData_IVPointSetNode.h>
#include <asiData_IVPoints2dNode.h>
#include <asiData_IVPointsNode.h>
#include <asiData_IVSurfaceNode.h>
#include <asiData_IVSurfacesNode.h>
#include <asiData_IVTessItemNode.h>
#include <asiData_IVTessNode.h>
#include <asiData_IVTextItemNode.h>
#include <asiData_IVTextNode.h>
#include <asiData_IVTopoItemNode.h>
#include <asiData_IVTopoNode.h>

// Active Data includes
#include <ActData_BaseModel.h>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_Model, ActData_BaseModel)

//! Data Model of A-Situs.
class asiData_Model : public ActData_BaseModel
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_Model, ActData_BaseModel)

public:

  asiData_Model();

//-----------------------------------------------------------------------------
// Populate and clear:
public:

  void Populate();
  void Clear();

//-----------------------------------------------------------------------------
// Accessors to Nodes:
public:

  Handle(asiData_TessNode)          GetMeshNode()     const;
  Handle(asiData_PartNode)     GetPartNode()     const;
  Handle(asiData_RENode)       GetRENode()       const;
  Handle(geom_sections_node) GetSectionsNode() const;
  Handle(geom_ubend_node)    GetUBendNode()    const;
  Handle(asiData_IVNode)       GetIVNode()       const;

//-----------------------------------------------------------------------------
// Overridden:
public:

  //! Create a cloned instance of Data Model.
  //! \return cloned instance.
  virtual Handle(ActAPI_IModel) Clone() const
  {
    return ActData_BaseModel::CloneInstance<asiData_Model>();
  }

//-----------------------------------------------------------------------------
// Partitions:
public:

  //! Accessor for a Partition instance dedicated to Calculus Design Law Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_DesignLawNode>) GetCalculusDesignLawPartition() const
  {
    return Handle(asiData_Partition<asiData_DesignLawNode>)::DownCast( this->Partition(Partition_CalculusDesignLaw) );
  }

  //! Accessor for a Partition instance dedicated to Mesh Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_TessNode>) GetMeshPartition() const
  {
    return Handle(asiData_Partition<asiData_TessNode>)::DownCast( this->Partition(Partition_Mesh) );
  }

  //! Accessor for a Partition instance dedicated to root Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RootNode>) GetRootPartition() const
  {
    return Handle(asiData_Partition<asiData_RootNode>)::DownCast( this->Partition(Partition_Root) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Sections Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<geom_sections_node>) GetSectionsPartition() const
  {
    return Handle(asiData_Partition<geom_sections_node>)::DownCast( this->Partition(Partition_Sections) );
  }

  //! Accessor for a Partition instance dedicated to Section Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<geom_section_node>) GetSectionPartition() const
  {
    return Handle(asiData_Partition<geom_section_node>)::DownCast( this->Partition(Partition_Section) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to U-bend Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<geom_ubend_node>) GetUBendPartition() const
  {
    return Handle(asiData_Partition<geom_ubend_node>)::DownCast( this->Partition(Partition_UBend) );
  }

  //! Accessor for a Partition instance dedicated to U-bend Laws Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<geom_ubend_laws_node>) GetUBendLawsPartition() const
  {
    return Handle(asiData_Partition<geom_ubend_laws_node>)::DownCast( this->Partition(Partition_UBendLaws) );
  }

  //! Accessor for a Partition instance dedicated to U-bend Law Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<geom_ubend_law_node>) GetUBendLawPartition() const
  {
    return Handle(asiData_Partition<geom_ubend_law_node>)::DownCast( this->Partition(Partition_UBendLaw) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Geometry Part Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_PartNode>) GetPartPartition() const
  {
    return Handle(asiData_Partition<asiData_PartNode>)::DownCast( this->Partition(Partition_GeomPart) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Face Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_FaceNode>) GetGeomFacePartition() const
  {
    return Handle(asiData_Partition<asiData_FaceNode>)::DownCast( this->Partition(Partition_GeomFace) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Surface Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_SurfNode>) GetGeomSurfacePartition() const
  {
    return Handle(asiData_Partition<asiData_SurfNode>)::DownCast( this->Partition(Partition_GeomSurface) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Edge Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_EdgeNode>) GetGeomEdgePartition() const
  {
    return Handle(asiData_Partition<asiData_EdgeNode>)::DownCast( this->Partition(Partition_GeomEdge) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Curve Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_CurveNode>) GetGeomCurvePartition() const
  {
    return Handle(asiData_Partition<asiData_CurveNode>)::DownCast( this->Partition(Partition_GeomCurve) );
  }

  //! Accessor for a Partition instance dedicated to Geometry Boundary Edges Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_BoundaryEdgesNode>) GetGeomBoundaryEdgesPartition() const
  {
    return Handle(asiData_Partition<asiData_BoundaryEdgesNode>)::DownCast( this->Partition(Partition_GeomBoundaryEdges) );
  }

  //! Accessor for a Partition instance dedicated to Volume Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<geom_volume_node>) GetGeomVolumePartition() const
  {
    return Handle(asiData_Partition<geom_volume_node>)::DownCast( this->Partition(Partition_GeomVolume) );
  }

  //! Accessor for a Partition instance dedicated to Contour Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_ContourNode>) GetGeomContourPartition() const
  {
    return Handle(asiData_Partition<asiData_ContourNode>)::DownCast( this->Partition(Partition_GeomContour) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to Reverse Engineering Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RENode>) GetREPartition() const
  {
    return Handle(asiData_Partition<asiData_RENode>)::DownCast( this->Partition(Partition_RE) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Surfaces Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RESurfacesNode>) GetRESurfacesPartition() const
  {
    return Handle(asiData_Partition<asiData_RESurfacesNode>)::DownCast( this->Partition(Partition_RESurfaces) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Surface Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_RESurfaceNode>) GetRESurfacePartition() const
  {
    return Handle(asiData_Partition<asiData_RESurfaceNode>)::DownCast( this->Partition(Partition_RESurface) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Contours Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_REContoursNode>) GetREContoursPartition() const
  {
    return Handle(asiData_Partition<asiData_REContoursNode>)::DownCast( this->Partition(Partition_REContours) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Contour Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_REContourNode>) GetREContourPartition() const
  {
    return Handle(asiData_Partition<asiData_REContourNode>)::DownCast( this->Partition(Partition_REContour) );
  }

  //! Accessor for a Partition instance dedicated to Reverse Engineering
  //! Points Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_REPointsNode>) GetREPointsPartition() const
  {
    return Handle(asiData_Partition<asiData_REPointsNode>)::DownCast( this->Partition(Partition_REPoints) );
  }

//-----------------------------------------------------------------------------

  //! Accessor for a Partition instance dedicated to IV Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVNode>) GetIVPartition() const
  {
    return Handle(asiData_Partition<asiData_IVNode>)::DownCast( this->Partition(Partition_IV) );
  }

  //! Accessor for a Partition instance dedicated to IV Points 2D Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVPoints2dNode>) GetIVPoints2dPartition() const
  {
    return Handle(asiData_Partition<asiData_IVPoints2dNode>)::DownCast( this->Partition(Partition_IV_Points2d) );
  }

  //! Accessor for a Partition instance dedicated to IV Points Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVPointsNode>) GetIVPointsPartition() const
  {
    return Handle(asiData_Partition<asiData_IVPointsNode>)::DownCast( this->Partition(Partition_IV_Points) );
  }

  //! Accessor for a Partition instance dedicated to IV Point Set 2D Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVPointSet2dNode>) GetIVPointSet2dPartition() const
  {
    return Handle(asiData_Partition<asiData_IVPointSet2dNode>)::DownCast( this->Partition(Partition_IV_PointSet2d) );
  }

  //! Accessor for a Partition instance dedicated to IV Point Set Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVPointSetNode>) GetIVPointSetPartition() const
  {
    return Handle(asiData_Partition<asiData_IVPointSetNode>)::DownCast( this->Partition(Partition_IV_PointSet) );
  }

  //! Accessor for a Partition instance dedicated to IV Curves Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVCurvesNode>) GetIVCurvesPartition() const
  {
    return Handle(asiData_Partition<asiData_IVCurvesNode>)::DownCast( this->Partition(Partition_IV_Curves) );
  }

  //! Accessor for a Partition instance dedicated to IV Curve Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVCurveNode>) GetIVCurvePartition() const
  {
    return Handle(asiData_Partition<asiData_IVCurveNode>)::DownCast( this->Partition(Partition_IV_Curve) );
  }

  //! Accessor for a Partition instance dedicated to IV Surfaces Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVSurfacesNode>) GetIVSurfacesPartition() const
  {
    return Handle(asiData_Partition<asiData_IVSurfacesNode>)::DownCast( this->Partition(Partition_IV_Surfaces) );
  }

  //! Accessor for a Partition instance dedicated to IV Surface Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVSurfaceNode>) GetIVSurfacePartition() const
  {
    return Handle(asiData_Partition<asiData_IVSurfaceNode>)::DownCast( this->Partition(Partition_IV_Surface) );
  }

  //! Accessor for a Partition instance dedicated to IV Topo Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTopoNode>) GetIVTopoPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTopoNode>)::DownCast( this->Partition(Partition_IV_Topo) );
  }

  //! Accessor for a Partition instance dedicated to IV Topo Item Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTopoItemNode>) GetIVTopoItemPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTopoItemNode>)::DownCast( this->Partition(Partition_IV_TopoItem) );
  }

  //! Accessor for a Partition instance dedicated to IV Tessellation Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTessNode>) GetIVTessPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTessNode>)::DownCast( this->Partition(Partition_IV_Tess) );
  }

  //! Accessor for a Partition instance dedicated to IV Tessellation Item Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTessItemNode>) GetIVTessItemPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTessItemNode>)::DownCast( this->Partition(Partition_IV_TessItem) );
  }

  //! Accessor for a Partition instance dedicated to IV Text Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTextNode>) GetIVTextPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTextNode>)::DownCast( this->Partition(Partition_IV_Text) );
  }

  //! Accessor for a Partition instance dedicated to IV Text Item Nodes.
  //! \return requested Partition.
  Handle(asiData_Partition<asiData_IVTextItemNode>) GetIVTextItemPartition() const
  {
    return Handle(asiData_Partition<asiData_IVTextItemNode>)::DownCast( this->Partition(Partition_IV_TextItem) );
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
    Partition_GeomContour,
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
