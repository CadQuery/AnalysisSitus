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

#ifndef asiAlgo_MeshMerge_h
#define asiAlgo_MeshMerge_h

// asiAlgo includes
#include <asiAlgo_MeshLink.h>

// Active Data includes
#include <Mesh.h>

// OCCT includes
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

public:

  asiAlgo_EXPORT
    asiAlgo_MeshMerge(const TopoDS_Shape& body,
                      const Mode          mode            = Mode_PolyCoherentTriangulation,
                      const bool          collectBoundary = true);

  asiAlgo_EXPORT
    asiAlgo_MeshMerge(const std::vector<Handle(Poly_Triangulation)>& triangulations,
                      const Mode                                     mode = Mode_PolyCoherentTriangulation);

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
  const asiAlgo_MeshLinkSet& GetFreeLinks() const
  {
    return m_freeLinks;
  }

  //! \return manifold links (those corresponding to the shared manifold edges).
  const asiAlgo_MeshLinkSet& GetManifoldLinks() const
  {
    return m_manifoldLinks;
  }

  //! \return non-manifold links (those corresponding to the shared non-manifold edges).
  const asiAlgo_MeshLinkSet& GetNonManifoldLinks() const
  {
    return m_nonManifoldLinks;
  }

protected:

  void build(const TopoDS_Shape& body,
             const Mode          mode,
             const bool          collectBoundary);
  //
  void build(const std::vector<Handle(Poly_Triangulation)>& triangulations,
             const Mode                                     mode);
  //
  void addFreeLink(const asiAlgo_MeshLink& link);
  void addManifoldLink(const asiAlgo_MeshLink& link);
  void addNonManifoldLink(const asiAlgo_MeshLink& link);

// OUTPUTS:
protected:

  Handle(Poly_CoherentTriangulation) m_resultPoly;       //!< Result tessellation.
  Handle(ActData_Mesh)               m_resultMesh;       //!< Result mesh.
  asiAlgo_MeshLinkSet                m_freeLinks;        //!< Free links.
  asiAlgo_MeshLinkSet                m_manifoldLinks;    //!< Manifold links.
  asiAlgo_MeshLinkSet                m_nonManifoldLinks; //!< Non-manifold links.

};

#endif
