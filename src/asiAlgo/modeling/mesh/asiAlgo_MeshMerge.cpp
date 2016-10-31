//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
    inline bool IsFound() const { return m_bFound; }

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

    inline int GetID() const { return m_iID; }

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

    inline const TopoDS_Face&             Face() const                  { return m_F; }
    inline void                           SetFace(const TopoDS_Face& F) { m_F = F; }
    inline NCollection_DataMap<int, int>& FaceTris()                    { return m_GlobalLocalTris; }
    inline NCollection_DataMap<int, int>& LocalNodes()                  { return m_LocalGlobalNodes; }

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

  void appendNodeInGlobalTri(const int                            LocalId,
                             const int                            GlobalID,
                             const gp_XYZ&                        P,
                             Handle(Poly_CoherentTriangulation)&  GlobalTri,
                             NCollection_CellFilter<InspectNode>& NodeFilter,
                             NCollection_DataMap<int, int>&       DegNodesMap,
                             NCollection_DataMap<int, int>&       LocalNodesMap)
  {
    const double prec = Precision::Confusion();
    InspectNode Inspect(prec, P);
    gp_XYZ XYZ_min = Inspect.Shift( P, -prec );
    gp_XYZ XYZ_max = Inspect.Shift( P,  prec );

    // Attempt to find a spatial cell containing this node
    NodeFilter.Inspect(XYZ_min, XYZ_max, Inspect);
    const bool isFound = Inspect.IsFound();
  
    if ( !isFound )
    {
      TriNode N;
      N.ID    = GlobalID;
      N.Point = P;
      NodeFilter.Add(N, P);
      GlobalTri->SetNode(P, GlobalID);
      LocalNodesMap.Bind(LocalId, GlobalID);
    }
    else
    {
      const int equalID = Inspect.GetID();
      DegNodesMap.Bind(GlobalID, equalID);
      LocalNodesMap.Bind(LocalId, equalID);
    }
  }

};

//-----------------------------------------------------------------------------
// Conglomeration tool
//-----------------------------------------------------------------------------

//! Constructor.
//! \param body [in] CAD model to extract triangulation patches from.
//! \param mode [in] conversion mode.
asiAlgo_MeshMerge::asiAlgo_MeshMerge(const TopoDS_Shape& body,
                                     const Mode          mode)
: m_body(body),
  m_mode(mode)
{
  this->build();
}

//! Assembles many triangulations into a single one.
void asiAlgo_MeshMerge::build()
{
  // Create result
  m_resultPoly = new Poly_CoherentTriangulation;

  // Working tools and variables
  int nNodes = 0, nTriangles = 0;
  NCollection_CellFilter<InspectNode> NodeFilter( Precision::Confusion() );
  NCollection_DataMap<int, int> DegNodesMap;

  // Iterate over the patches
  for ( TopExp_Explorer exp(m_body, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& F = TopoDS::Face( exp.Current() );
    PatchInfo FT(F);
    NCollection_DataMap<int, int>& LocalNodesMap = FT.LocalNodes();

    // Extract triangulation of a patch
    TopLoc_Location Loc;
    Handle(Poly_Triangulation) LocalTri = BRep_Tool::Triangulation(F, Loc);
    //
    const int nLocalNodes     = LocalTri->NbNodes();
    const int nLocalTriangles = LocalTri->NbTriangles();

    // Add nodes
    if ( Loc.IsIdentity() )
    {
      for ( int i = 1; i <= nLocalNodes; ++i )
      {
        const int curID = nNodes + i - 1;
        const gp_XYZ& curP = LocalTri->Nodes()(i).XYZ();

        // Add node
        ::appendNodeInGlobalTri(i, curID, curP, m_resultPoly, NodeFilter, DegNodesMap, LocalNodesMap);
      }
    }
    else
    {
      const gp_Trsf& trsf = Loc.Transformation();
      for ( int i = 1; i <= nLocalNodes; ++i )
      {
        const int curID = nNodes + i - 1;
        const gp_XYZ& curP = LocalTri->Nodes()(i).Transformed(trsf).XYZ();

        // Add node
        ::appendNodeInGlobalTri(i, curID, curP, m_resultPoly, NodeFilter, DegNodesMap, LocalNodesMap);
      }
    }

    // Add triangles
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
      for ( int idx = 0; idx < 3; ++idx )
      {
        if ( DegNodesMap.IsBound(m[idx] + nNodes - 1) )
          m[idx] = DegNodesMap.Find(m[idx] + nNodes - 1) - nNodes + 1;
      }
      if ( m[0] == m[1] || m[0] == m[2] || m[1] == m[2] )
        continue;

      m_resultPoly->AddTriangle(m[0] + nNodes - 1,
                                m[1] + nNodes - 1,
                                m[2] + nNodes - 1);
    }

    nNodes += nLocalNodes;
    nTriangles += nLocalTriangles;
  }

  if ( m_mode == Mode_Mesh )
    m_resultMesh = new Mesh( m_resultPoly->GetTriangulation() );
}
