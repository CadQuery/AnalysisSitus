//-----------------------------------------------------------------------------
// Created on: 02 March 2017
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
#include <asiVisu_FaceNormalsDataProvider.h>

// asiAlgo includes
#include <asiAlgo_ClassifyPointFace.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <TopoDS.hxx>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param faceNode [in] face Node.
asiVisu_FaceNormalsDataProvider::asiVisu_FaceNormalsDataProvider(const Handle(asiData_FaceNormsNode)& faceNode)
: asiVisu_VectorsDataProvider(faceNode)
{
  // Access owning geometry
  m_partNode = Handle(asiData_PartNode)::DownCast( m_source->GetParentNode() );
}

//-----------------------------------------------------------------------------

//! \return points where normals are located.
Handle(asiAlgo_BaseCloud<float>) asiVisu_FaceNormalsDataProvider::GetPointsf()
{
  TopoDS_Face face = this->GetFace();
  //
  if ( face.IsNull() )
    return NULL;

  // Prepare a point cloud as a result
  m_points  = new asiAlgo_BaseCloud<float>;
  m_vectors = new asiAlgo_BaseCloud<float>;

  // Take surface
  Handle(Geom_Surface) surf = BRep_Tool::Surface(face);

  // Take face domain
  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);
  //
  const double uStep = (uMax - uMin)*0.05;
  const double vStep = (vMax - vMin)*0.05;

  // Prepare classifier
  asiAlgo_ClassifyPointFace classifier(face, BRep_Tool::Tolerance(face), 0.01);

  // Sample points
  double u = uMin;
  bool uStop = false;
  while ( !uStop )
  {
    if ( u > uMax )
    {
      u     = uMax;
      uStop = true;
    }

    double v = vMin;
    bool vStop = false;
    while ( !vStop )
    {
      if ( v > vMax )
      {
        v     = vMax;
        vStop = true;
      }

      // Perform point membership classification
      asiAlgo_Membership pmc = classifier(gp_Pnt2d(u, v), NULL);
      //
      if ( pmc & Membership_InOn )
      {
        gp_Pnt P;
        gp_Vec D1U, D1V;
        surf->D1(u, v, P, D1U, D1V);
        gp_Vec N = (D1U^D1V).Normalized();
        //
        m_points->AddElement( (float) P.X(), (float) P.Y(), (float) P.Z() );
        m_vectors->AddElement( (float) N.X(), (float) N.Y(), (float) N.Z() );
      }

      v += vStep;
    }

    u += uStep;
  }

  // Return
  return m_points;
}

//-----------------------------------------------------------------------------

//! \return normals to visualize.
Handle(asiAlgo_BaseCloud<float>) asiVisu_FaceNormalsDataProvider::GetVectorsf()
{
  // Return cached vectors available after GetPointsf() invocation
  return m_vectors;
}

//-----------------------------------------------------------------------------

//! \return max modulus for a normal.
double asiVisu_FaceNormalsDataProvider::GetMaxVectorModulus() const
{
  TopoDS_Face face = this->GetFace();

  if ( face.IsNull() )
    return 1.0;

  Bnd_Box aabb;
  BRepBndLib::Add(face, aabb);
  const double size = ( aabb.CornerMax().XYZ() - aabb.CornerMin().XYZ() ).Modulus()*0.05;

  return size;
}

//-----------------------------------------------------------------------------

//! \return working face.
TopoDS_Face asiVisu_FaceNormalsDataProvider::GetFace() const
{
  const int globalId = Handle(asiData_FaceNormsNode)::DownCast(m_source)->GetSelectedFace();
  if ( !globalId )
    return TopoDS_Face();

  const TopTools_IndexedMapOfShape&
    subShapes = m_partNode->GetAAG()->GetMapOfSubShapes();

  const TopoDS_Shape& shape = subShapes.FindKey(globalId);
  //
  if ( shape.ShapeType() != TopAbs_FACE )
    return TopoDS_Face();

  // Access face by the stored index
  const TopoDS_Face& F = TopoDS::Face(shape);
  return F;
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_FaceNormalsDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_FaceNormsNode)
    face_n = Handle(asiData_FaceNormsNode)::DownCast(m_source);
  //
  if ( face_n.IsNull() || !face_n->IsWellFormed() )
    return out;

  // Register Parameter as sensitive
  out << face_n->Parameter(asiData_FaceNormsNode::PID_SelectedFace)
      << m_partNode->Parameter(asiData_PartNode::PID_Geometry)
      << m_partNode->Parameter(asiData_PartNode::PID_AAG);

  return out;
}
