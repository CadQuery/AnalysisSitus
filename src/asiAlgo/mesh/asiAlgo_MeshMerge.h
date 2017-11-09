//-----------------------------------------------------------------------------
// Created on: 23 May 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
