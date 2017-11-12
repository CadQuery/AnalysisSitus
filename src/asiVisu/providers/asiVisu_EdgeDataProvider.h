//-----------------------------------------------------------------------------
// Created on: 18 August 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of Sergey Slyadnev nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_EdgeDataProvider_h
#define asiVisu_EdgeDataProvider_h

// asiVisu includes
#include <asiVisu_CurveDataProvider.h>

// asiData includes
#include <asiData_PartNode.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//! Data provider for edge geometry.
class asiVisu_EdgeDataProvider : public asiVisu_CurveDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_EdgeDataProvider, asiVisu_CurveDataProvider)

public:

  asiVisu_EXPORT
    asiVisu_EdgeDataProvider(const Handle(asiData_EdgeNode)& edge_n);

  asiVisu_EXPORT
    asiVisu_EdgeDataProvider(const Handle(asiData_CurveNode)& curve_n);

public:

  asiVisu_EXPORT ActAPI_DataObjectId
    GetNodeID() const;

  asiVisu_EXPORT int
    GetEdgeIndexAmongSubshapes() const;

  asiVisu_EXPORT int
    GetEdgeIndexAmongEdges() const;

  asiVisu_EXPORT TopoDS_Edge
    ExtractEdge() const;

public:

  asiVisu_EXPORT virtual Handle(Standard_Type)
    GetCurveType() const;

  asiVisu_EXPORT virtual Handle(Geom2d_Curve)
    GetCurve2d(double& f, double& l) const;

  asiVisu_EXPORT virtual Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

public:

  asiVisu_EXPORT Handle(asiVisu_EdgeDataProvider)
    Clone() const;

protected:

  asiVisu_EXPORT void
    init(const Handle(ActAPI_INode)& subNode);

private:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

private:

  //! Source Node.
  Handle(ActAPI_INode) m_node;

  //! Working part Node.
  Handle(asiData_PartNode) m_partNode;

};

#endif
