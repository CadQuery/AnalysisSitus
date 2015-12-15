//-----------------------------------------------------------------------------
// Created on: 14 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

// Own include
#include <mesh_convert.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <NCollection_CellFilter.hxx>
#include <Poly_CoherentTriangulation.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

// VTK includes
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------

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

    inline const TopoDS_Face&             Face()const                   { return m_F; }
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

//! Converts internal mesh of the given shape to a persistent form.
//! \param source [in]  source shape (mesh keeper).
//! \param result [out] mesh in a persistent form.
//! \return true in case of success, false -- otherwise.
bool mesh_convert::ToPersistent(const TopoDS_Shape& source,
                                Handle(OMFDS_Mesh)& result)
{
  // Working tools and variables
  int nNodes = 0, nTriangles = 0;
  NCollection_CellFilter<InspectNode> NodeFilter( Precision::Confusion() );
  Handle(Poly_CoherentTriangulation) GlobalTri = new Poly_CoherentTriangulation;
  NCollection_DataMap<int, int> DegNodesMap;

  // Iterate over the patches
  for ( TopExp_Explorer exp(source, TopAbs_FACE); exp.More(); exp.Next() )
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
        ::appendNodeInGlobalTri(i, curID, curP, GlobalTri, NodeFilter, DegNodesMap, LocalNodesMap);
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
        ::appendNodeInGlobalTri(i, curID, curP, GlobalTri, NodeFilter, DegNodesMap, LocalNodesMap);
      }
    }

    // Create elements
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

      GlobalTri->AddTriangle(m[0] + nNodes - 1,
                             m[1] + nNodes - 1,
                             m[2] + nNodes - 1);
    }

    nNodes += nLocalNodes;
    nTriangles += nLocalTriangles;
  }

  // Set result to classic triangulation (not coherent)
  result = new OMFDS_Mesh( GlobalTri->GetTriangulation() );
  return true;
}

//! Converts VTK polygonal data set to the persistent one.
//! \param source [in]  VTK polygonal data set.
//! \param result [out] persistent mesh.
//! \return true in case of success, false -- otherwise.
bool mesh_convert::ToPersistent(vtkPolyData*        source,
                                Handle(OMFDS_Mesh)& result)
{
  // Elements
  const vtkIdType numCells = source->GetNumberOfCells();
  if ( numCells )
    result = new OMFDS_Mesh;

  // Mapping between VTK nodes against stored nodes
  NCollection_DataMap<vtkIdType, int> NodeMap;

  // Points
  const vtkIdType numPts = source->GetNumberOfPoints();
  for ( vtkIdType ptId = 0; ptId < numPts; ++ptId )
  {
    double coord[3] = {0.0, 0.0, 0.0};
    source->GetPoint(ptId, coord);
    const int storedNodeId = result->AddNode(coord[0], coord[1], coord[2]);
    //
    NodeMap.Bind(ptId, storedNodeId);
  }

  // Loop over the VTK polygonal cells
  vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();
  ptIds->Allocate(VTK_CELL_SIZE);
  //
  for ( vtkIdType cellId = 0; cellId < numCells; ++cellId )
  {
    const int cellType = source->GetCellType(cellId);
    if ( cellType != VTK_TRIANGLE )
    {
      std::cout << "Warning: non-triangular elements are not supported" << std::endl;
      continue;
    }

    // Get the list of points for this cell
    source->GetCellPoints(cellId, ptIds);

    // Add face
    const int id0 = NodeMap.Find( ptIds->GetId(0) );
    const int id1 = NodeMap.Find( ptIds->GetId(1) );
    const int id2 = NodeMap.Find( ptIds->GetId(2) );
    //
    result->AddFace(id0, id1, id2);
  }
  return true;
}
