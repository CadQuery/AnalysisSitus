//-----------------------------------------------------------------------------
// Created on: 28 March 2016
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
//    * Neither the name of the copyright holder(s) nor the
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
