//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_MeshMerge.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <NCollection_CellFilter.hxx>
#include <Poly_CoherentTriangulation.hxx>
#include <Precision.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

namespace
{
  //---------------------------------------------------------------------------
  // Inspectors for spatial nodes
  //---------------------------------------------------------------------------

  //! Spatial point enriched with numeric identifier.
  struct TriNode
  {
    gp_XYZ Point; //!< Geometric representation.
    int    ID;    //!< Associated ID.
  };

  //! Auxiliary class to search for coincident spatial points.
  class InspectXYZ : public NCollection_CellFilter_InspectorXYZ
  {
  public:

    typedef gp_XYZ Target;

    //! Constructor accepting resolution distance and point.
    InspectXYZ(const double tol, const gp_XYZ& P) : m_fTol(tol), m_bFound(false), m_P(P) {}

    //! \return true/false depending on whether the node was found or not.
    bool IsFound() const { return m_bFound; }

    //! Implementation of inspection method.
    NCollection_CellFilter_Action Inspect(const gp_XYZ& Target)
    {
      m_bFound = ( (m_P - Target).SquareModulus() <= Square(m_fTol) );
      return CellFilter_Keep;
    }

  private:

    gp_XYZ m_P;      //!< Source point.
    bool   m_bFound; //!< Whether two points are coincident or not.
    double m_fTol;   //!< Resolution to check for coincidence.

  };

  //! Auxiliary class to search for coincident tessellation nodes.
  class InspectNode : public InspectXYZ
  {
  public:

    typedef TriNode Target;

    //! Constructor accepting resolution distance and point.
    InspectNode(const double tol, const gp_XYZ& P) : InspectXYZ(tol, P), m_iID(-1) {}

    int GetID() const { return m_iID; }

    //! Implementation of inspection method.
    NCollection_CellFilter_Action Inspect(const TriNode& Target)
    {
      InspectXYZ::Inspect(Target.Point);

      if ( InspectXYZ::IsFound() )
        m_iID = Target.ID;

      return CellFilter_Keep;
    }

  private:

    int m_iID; //!< Found target ID.

  };

  //---------------------------------------------------------------------------
  // Patch tessellation info
  //---------------------------------------------------------------------------

  //! Stores correspondences between IDs of triangles and nodes with respect
  //! to patch local triangulation and global triangulation.
  class PatchInfo
  {
  public:

    PatchInfo(const TopoDS_Face& F) : m_F(F) {}

    const TopoDS_Face&             GetFace() const               { return m_F; }
    void                           SetFace(const TopoDS_Face& F) { m_F = F; }
    NCollection_DataMap<int, int>& FaceTris()                    { return m_GlobalLocalTris; }
    NCollection_DataMap<int, int>& LocalNodes()                  { return m_LocalGlobalNodes; }

  private:

    //! Initial patch.
    TopoDS_Face m_F;

    //! Correspondence between the global and local triangle indices.
    NCollection_DataMap<int, int> m_GlobalLocalTris;

    //! Correspondence between local and global node indices.
    NCollection_DataMap<int, int> m_LocalGlobalNodes;
  };

  //---------------------------------------------------------------------------
  // Auxiliary functions
  //---------------------------------------------------------------------------

  void appendNodeInGlobalTri(const int                            localNodeId,
                             int&                                 globalNodeId,
                             const gp_XYZ&                        xyz,
                             Handle(Poly_CoherentTriangulation)&  GlobalTri,
                             NCollection_CellFilter<InspectNode>& NodeFilter,
                             NCollection_DataMap<int, int>&       LocGlobMap)
  {
    const double prec = Precision::Confusion();
    InspectNode Inspect(prec, xyz);
    gp_XYZ XYZ_min = Inspect.Shift( xyz, -prec );
    gp_XYZ XYZ_max = Inspect.Shift( xyz,  prec );

    // Coincidence test
    NodeFilter.Inspect(XYZ_min, XYZ_max, Inspect);
    const bool isFound = Inspect.IsFound();
    //
    if ( !isFound )
    {
      TriNode N;
      N.ID    = globalNodeId;
      N.Point = xyz;
      //
      NodeFilter.Add(N, xyz);
      GlobalTri->SetNode(xyz, globalNodeId);
      LocGlobMap.Bind(localNodeId, globalNodeId);

      // (!!!) Increment global node ID
      ++globalNodeId;
    }
    else
    {
      const int equalID = Inspect.GetID();
      LocGlobMap.Bind(localNodeId, equalID);
    }
  }

};

//-----------------------------------------------------------------------------
// Conglomeration tool
//-----------------------------------------------------------------------------

//! Constructor.
//! \param body            [in] CAD model to extract triangulation patches from.
//! \param mode            [in] conversion mode.
//! \param collectBoundary [in] indicates whether to preserve boundary links.
asiAlgo_MeshMerge::asiAlgo_MeshMerge(const TopoDS_Shape& body,
                                     const Mode          mode,
                                     const bool          collectBoundary)
: m_body        (body),
  m_mode        (mode),
  m_bCollectBnd (collectBoundary)
{
  this->build();
}

//-----------------------------------------------------------------------------

//! Assembles many triangulations into a single one.
void asiAlgo_MeshMerge::build()
{
  // Create result as coherent triangulation
  m_resultPoly = new Poly_CoherentTriangulation;

  // Working tools and variables
  int globalNodeId = 0;
  NCollection_CellFilter<InspectNode> NodeFilter( Precision::Confusion() );

  //###########################################################################
  // [BEGIN] Iterate over the faces
  for ( TopExp_Explorer exp(m_body, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& F = TopoDS::Face( exp.Current() );
    NCollection_DataMap<int, int> FaceNodeIds_ToGlobalNodeIds;

    // Extract triangulation of a patch
    TopLoc_Location Loc;
    const Handle(Poly_Triangulation)& LocalTri = BRep_Tool::Triangulation(F, Loc);
    //
    if ( LocalTri.IsNull() )
      continue;
    //
    const int nLocalNodes     = LocalTri->NbNodes();
    const int nLocalTriangles = LocalTri->NbTriangles();

    // Add nodes with coincidence test
    for ( int localNodeId = 1; localNodeId <= nLocalNodes; ++localNodeId )
    {
      gp_XYZ xyz;
      if ( Loc.IsIdentity() )
        xyz = LocalTri->Nodes()(localNodeId).XYZ();
      else
        xyz = LocalTri->Nodes()(localNodeId).Transformed( Loc.Transformation() ).XYZ();

      // Add node to the conglomerate after coincidence test
      ::appendNodeInGlobalTri(localNodeId,                  // [in]     local node ID in a face
                              globalNodeId,                 // [in,out] global node ID in the conglomerate mesh
                              xyz,                          // [in]     coordinates for coincidence test
                              m_resultPoly,                 // [in,out] result conglomerate mesh
                              NodeFilter,                   // [in]     cell filter
                              FaceNodeIds_ToGlobalNodeIds); // [in,out] face-conglomerate map of node IDs
    }

    // Collect local indices of the boundary nodes
    if ( m_bCollectBnd )
    {
      for ( TopExp_Explorer eexp(F, TopAbs_EDGE); eexp.More(); eexp.Next() )
      {
        const TopoDS_Edge& E = TopoDS::Edge( eexp.Current() );

        // Get discrete analogue of p-curve
        TopLoc_Location ELoc;
        const Handle(Poly_PolygonOnTriangulation)&
          polygonOnTri = BRep_Tool::PolygonOnTriangulation(E, LocalTri, ELoc);
        //
        if ( polygonOnTri.IsNull() )
          continue;

        // Add node indices to the collection of boundary nodes
        const TColStd_Array1OfInteger& polygonOnTriNodes = polygonOnTri->Nodes();
        int prevBndNode = 0;
        //
        for ( int k = polygonOnTriNodes.Lower(); k <= polygonOnTriNodes.Upper(); ++k )
        {
          const int localBndIndex  = polygonOnTriNodes(k);
          const int globalBndIndex = FaceNodeIds_ToGlobalNodeIds(localBndIndex);

          if ( k > polygonOnTriNodes.Lower() )
          {
            t_unoriented_link bndLink(prevBndNode, globalBndIndex);
            this->addBoundaryLink(bndLink);
          }
          //
          prevBndNode = globalBndIndex;
        }
      }
    }

    // Add triangles taking into account face orientations
    for ( int i = 1; i <= nLocalTriangles; ++i )
    {
      int n1, n2, n3;
      LocalTri->Triangles()(i).Get(n1, n2, n3);
      int m[3] = {n1, n2, n3};

      if ( F.Orientation() == TopAbs_REVERSED )
      {
        m[1] = n3;
        m[2] = n2;
      }

      m[0] = FaceNodeIds_ToGlobalNodeIds.Find(m[0]);
      m[1] = FaceNodeIds_ToGlobalNodeIds.Find(m[1]);
      m[2] = FaceNodeIds_ToGlobalNodeIds.Find(m[2]);

      if ( m[0] == m[1] || m[0] == m[2] || m[1] == m[2] )
        continue;

      m_resultPoly->AddTriangle(m[0], m[1], m[2]);
    }
  }
  // [END] Iterate over the faces
  //###########################################################################

  if ( m_mode == Mode_Mesh )
    if ( !m_resultPoly->GetTriangulation().IsNull() )
      m_resultMesh = new Mesh( m_resultPoly->GetTriangulation() );
}

//-----------------------------------------------------------------------------

void asiAlgo_MeshMerge::addBoundaryLink(const t_unoriented_link& bndLink)
{
  m_boundaryLinks.Add(bndLink);
}
