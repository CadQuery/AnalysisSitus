//-----------------------------------------------------------------------------
// Created on: 02 March 2017
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
#include <asiVisu_FaceNormalsDataProvider.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

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

  if ( !asiAlgo_Utils::CalculateFaceNormals(face, m_points, m_vectors) )
    return NULL;

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
    subShapes = m_partNode->GetAAG()->RequestMapOfSubShapes();

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
