//-----------------------------------------------------------------------------
// Created on: 18 August 2016
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

#ifndef asiVisu_EdgeDataProvider_h
#define asiVisu_EdgeDataProvider_h

// A-Situs visualization includes
#include <asiVisu_CurveDataProvider.h>

// asiData includes
#include <asiData_PartNode.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_EdgeDataProvider, asiVisu_CurveDataProvider)

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
