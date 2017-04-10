//-----------------------------------------------------------------------------
// Created on: 23 May 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiAlgo_MeshMerge_h
#define asiAlgo_MeshMerge_h

// A-Situs includes
#include <asiAlgo.h>

// Active Data includes
#include <Mesh.h>

// OCCT includes
#include <NCollection_IndexedMap.hxx>
#include <Poly_CoherentTriangulation.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TopoDS_Shape.hxx>

// Standard includes
#include <vector>

//! Given a shape with initialized tessellations inside, assembles all
//! triangles in a monolithic structure. Use this tool to assemble a single
//! tessellation from series of tessellations distributed by several faces.
//! The boundary information is preserved by means of a dedicated collection.
class asiAlgo_MeshMerge
{
public:

  //! Conversion mode.
  enum Mode
  {
    Mode_PolyCoherentTriangulation,
    Mode_Mesh
  };

  //! Auxiliary unoriented link structure.
  struct t_unoriented_link
  {
    int N1; //!< First node.
    int N2; //!< Second node.

    //! ctor default.
    t_unoriented_link() : N1(0), N2(0) {}

    //! ctor with parameters.
    t_unoriented_link(const int _N1, const int _N2) : N1(_N1), N2(_N2) {}

    //! \return hash code for the arc.
    static int HashCode(const t_unoriented_link& arc, const int upper)
    {
      int key = arc.N1 + arc.N2;
      key += (key << 10);
      key ^= (key >> 6);
      key += (key << 3);
      key ^= (key >> 11);
      return (key & 0x7fffffff) % upper;
    }

    //! \return true if two links are equal.
    static int IsEqual(const t_unoriented_link& arc1, const t_unoriented_link& arc2)
    {
      return arc1.N1 == arc2.N1 && arc1.N2 == arc2.N2 ||
             arc1.N2 == arc2.N1 && arc1.N1 == arc2.N2;
    }
  };

  typedef NCollection_IndexedMap<t_unoriented_link, t_unoriented_link> t_link_set;

public:

  asiAlgo_EXPORT
    asiAlgo_MeshMerge(const TopoDS_Shape& body,
                      const Mode          mode            = Mode_PolyCoherentTriangulation,
                      const bool          collectBoundary = true);

public:

  //! \return result.
  const Handle(Poly_CoherentTriangulation)& GetResultPoly() const
  {
    return m_resultPoly;
  }

  //! \return result.
  const Handle(ActData_Mesh)& GetResultMesh() const
  {
    return m_resultMesh;
  }

  //! \return free links (those corresponding to non-shared edges).
  const t_link_set& GetFreeLinks() const
  {
    return m_freeLinks;
  }

  //! \return manifold links (those corresponding to the shared manifold edges).
  const t_link_set& GetManifoldLinks() const
  {
    return m_manifoldLinks;
  }

  //! \return non-manifold links (those corresponding to the shared non-manifold edges).
  const t_link_set& GetNonManifoldLinks() const
  {
    return m_nonManifoldLinks;
  }

protected:

  void build();
  void addFreeLink(const t_unoriented_link& link);
  void addManifoldLink(const t_unoriented_link& link);
  void addNonManifoldLink(const t_unoriented_link& link);

protected:

  TopoDS_Shape                       m_body;             //!< Working body.
  Mode                               m_mode;             //!< Conversion mode.
  bool                               m_bCollectBnd;      //!< Whether to preserve boundary.
  Handle(Poly_CoherentTriangulation) m_resultPoly;       //!< Result tessellation.
  Handle(ActData_Mesh)               m_resultMesh;       //!< Result mesh.
  t_link_set                         m_freeLinks;        //!< Free links.
  t_link_set                         m_manifoldLinks;    //!< Manifold links.
  t_link_set                         m_nonManifoldLinks; //!< Non-manifold links.

};

#endif
