//-----------------------------------------------------------------------------
// Created on: 23 May 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_MeshMerge.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <NCollection_CellFilter.hxx>
#include <Poly_CoherentTriangulation.hxx>
#include <Precision.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

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
{
  this->build(body, mode, collectBoundary);
}

//-----------------------------------------------------------------------------

//! Constructor.
//! \param triangulations [in] list of triangulations to merge into one.
//! \param mode           [in] conversion mode.
asiAlgo_MeshMerge::asiAlgo_MeshMerge(const std::vector<Handle(Poly_Triangulation)>& triangulations,
                                     const Mode                                     mode)
{
  this->build(triangulations, mode);
}

//-----------------------------------------------------------------------------

//! Assembles many triangulations into a single one.
//! \param body            [in] CAD model to extract triangulation patches from.
//! \param mode            [in] conversion mode.
//! \param collectBoundary [in] indicates whether to preserve boundary links.
void asiAlgo_MeshMerge::build(const TopoDS_Shape& body,
                              const Mode          mode,
                              const bool          collectBoundary)
{
  // Create result as coherent triangulation
  m_resultPoly = new Poly_CoherentTriangulation;

  // Prepare map of edges and their parent faces
  TopTools_IndexedDataMapOfShapeListOfShape edgesOnFaces;
  if ( collectBoundary )
    TopExp::MapShapesAndAncestors(body, TopAbs_EDGE, TopAbs_FACE, edgesOnFaces);

  // Working tools and variables
  int globalNodeId = 0;
  NCollection_CellFilter<InspectNode> NodeFilter( Precision::Confusion() );

  //###########################################################################
  // [BEGIN] Iterate over the faces
  for ( TopExp_Explorer exp(body, TopAbs_FACE); exp.More(); exp.Next() )
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
    if ( collectBoundary )
    {
      for ( TopExp_Explorer eexp(F, TopAbs_EDGE); eexp.More(); eexp.Next() )
      {
        const TopoDS_Edge& E = TopoDS::Edge( eexp.Current() );

        // Get discrete analogue of p-curve
        const Handle(Poly_PolygonOnTriangulation)&
          polygonOnTri = BRep_Tool::PolygonOnTriangulation(E, LocalTri, Loc);
        //
        if ( polygonOnTri.IsNull() )
          continue;

        // Get owner faces to decide the type of sharing
        const TopTools_ListOfShape& ownerFaces = edgesOnFaces.FindFromKey(E);
        const int nFaces = ownerFaces.Extent();

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
            asiAlgo_MeshLink bndLink(prevBndNode, globalBndIndex);

            // Depending on the sharing type choose one or another collection
            if ( nFaces == 1 )
              this->addFreeLink(bndLink);
            else if ( nFaces > 2 )
              this->addNonManifoldLink(bndLink);
            else
              this->addManifoldLink(bndLink);
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

  if ( mode == Mode_Mesh )
    if ( !m_resultPoly->GetTriangulation().IsNull() )
      m_resultMesh = new ActData_Mesh( m_resultPoly->GetTriangulation() );
}

//-----------------------------------------------------------------------------

void asiAlgo_MeshMerge::build(const std::vector<Handle(Poly_Triangulation)>& triangulations,
                              const Mode                                     mode)
{
  // Create result as coherent triangulation
  m_resultPoly = new Poly_CoherentTriangulation;

  // Working tools and variables
  int globalNodeId = 0;
  NCollection_CellFilter<InspectNode> NodeFilter( Precision::Confusion() );

  //###########################################################################
  // [BEGIN] Iterate over the faces
  for ( size_t tidx = 0; tidx < triangulations.size(); ++tidx )
  {
    const Handle(Poly_Triangulation)& LocalTri = triangulations[tidx];
    //
    if ( LocalTri.IsNull() )
      continue;
    //
    const int nLocalNodes     = LocalTri->NbNodes();
    const int nLocalTriangles = LocalTri->NbTriangles();
    //
    NCollection_DataMap<int, int> FaceNodeIds_ToGlobalNodeIds;

    // Add nodes with coincidence test
    for ( int localNodeId = 1; localNodeId <= nLocalNodes; ++localNodeId )
    {
      gp_XYZ xyz = LocalTri->Nodes()(localNodeId).XYZ();

      // Add node to the conglomerate after coincidence test
      ::appendNodeInGlobalTri(localNodeId,                  // [in]     local node ID in a face
                              globalNodeId,                 // [in,out] global node ID in the conglomerate mesh
                              xyz,                          // [in]     coordinates for coincidence test
                              m_resultPoly,                 // [in,out] result conglomerate mesh
                              NodeFilter,                   // [in]     cell filter
                              FaceNodeIds_ToGlobalNodeIds); // [in,out] face-conglomerate map of node IDs
    }

    // Add triangles to the result
    for ( int i = 1; i <= nLocalTriangles; ++i )
    {
      int n1, n2, n3;
      LocalTri->Triangles()(i).Get(n1, n2, n3);
      int m[3] = {n1, n2, n3};

      m[0] = FaceNodeIds_ToGlobalNodeIds.Find(m[0]);
      m[1] = FaceNodeIds_ToGlobalNodeIds.Find(m[1]);
      m[2] = FaceNodeIds_ToGlobalNodeIds.Find(m[2]);

      if ( m[0] == m[1] || m[0] == m[2] || m[1] == m[2] )
        continue;

      m_resultPoly->AddTriangle(m[0], m[1], m[2]);
    }
  }
  // [END] Iterate over the patches
  //###########################################################################

  if ( mode == Mode_Mesh )
    if ( !m_resultPoly->GetTriangulation().IsNull() )
      m_resultMesh = new ActData_Mesh( m_resultPoly->GetTriangulation() );
}

//-----------------------------------------------------------------------------

void asiAlgo_MeshMerge::addFreeLink(const asiAlgo_MeshLink& link)
{
  m_freeLinks.Add(link);
}

//-----------------------------------------------------------------------------

void asiAlgo_MeshMerge::addManifoldLink(const asiAlgo_MeshLink& link)
{
  m_manifoldLinks.Add(link);
}

//-----------------------------------------------------------------------------

void asiAlgo_MeshMerge::addNonManifoldLink(const asiAlgo_MeshLink& link)
{
  m_nonManifoldLinks.Add(link);
}