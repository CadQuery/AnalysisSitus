//-----------------------------------------------------------------------------
// Created on: 28 March 2016
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
#include <asiAlgo_RehostFaces.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools_ReShape.hxx>
#include <Geom_Surface.hxx>
#include <Precision.hxx>
#include <ShapeFix_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//! Constructor.
//! \param masterCAD [in] master CAD model.
asiAlgo_RehostFaces::asiAlgo_RehostFaces(const TopoDS_Shape& masterCAD)
{
  m_master = masterCAD;
}

//! Performs re-hosting operation.
//! \param faces [in] faces to re-host.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_RehostFaces::Perform(const TopTools_IndexedMapOfShape& faces)
{
  if ( faces.Extent() < 2 )
    return false;

  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;

  // Take surface of the first face
  const TopoDS_Face&   seed_face    = TopoDS::Face( faces(1) );
  Handle(Geom_Surface) seed_surface = BRep_Tool::Surface(seed_face);
  //
  TopoDS_Face mockUpFace;
  BRep_Builder B;
  B.MakeFace( mockUpFace, seed_surface, Precision::Confusion() );
  //
  this->moveWires(seed_face, mockUpFace);

  // Change host surfaces for the rest of faces
  for ( int f = 2; f <= faces.Extent(); ++f )
  {
    const TopoDS_Face& current_face = TopoDS::Face( faces(f) );
    //
    this->moveWires(current_face, mockUpFace);

    // Post modification
    ReShape->Replace(current_face, mockUpFace);
  }
  //
  ReShape->Replace(seed_face, mockUpFace);

  // Apply modifications
  m_result = ReShape->Apply(m_master);

  return true; // Success
}

//-----------------------------------------------------------------------------

//! Moves wires from source face to the target one.
//! \param source [in]  source face.
//! \param target [out] target face.
void asiAlgo_RehostFaces::moveWires(const TopoDS_Face& source,
                                    TopoDS_Face&       target) const
{
  BRep_Builder B;

  // Fix wires one by one and set them back to the face
  for ( TopExp_Explorer wexp(source, TopAbs_WIRE); wexp.More(); wexp.Next() )
  {
    //Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire( TopoDS::Wire( wexp.Current() ),
    //                                               target,
    //                                               Precision::Confusion() );
    //sfw->FixReorder();
    //const int isReorderFailed = sfw->StatusReorder(ShapeExtend_FAIL);
    //sfw->FixEdgeCurves();
    ////
    //if ( isReorderFailed )
    //  continue;

    //sfw->FixShifted();
    //sfw->FixDegenerated();

    // Add fixed wire back to the face
    TopoDS_Wire resWire = TopoDS::Wire( wexp.Current() );// sfw->Wire();
    if ( source.Orientation() == TopAbs_REVERSED )
      resWire.Reverse();
    B.Add(target, resWire);
  }
}
