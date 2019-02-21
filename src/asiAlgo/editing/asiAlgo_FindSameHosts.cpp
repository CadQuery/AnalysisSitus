//-----------------------------------------------------------------------------
// Created on: 05 April 2016
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
#include <asiAlgo_FindSameHosts.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

//! Constructor accepting the faces to analyze.
//! \param faces [in] faces to analyze.
asiAlgo_FindSameHosts::asiAlgo_FindSameHosts(const TopTools_IndexedMapOfShape& faces) : m_faces(faces)
{}

//! Finds unique host surfaces.
//! \param groups [in] identified groups.
void asiAlgo_FindSameHosts::operator()(t_groups& groups) const
{
  TColStd_PackedMapOfInteger traversed;

  // First loop to iterate all faces
  for ( int i1 = 1; i1 <= m_faces.Extent(); ++i1 )
  {
    if ( traversed.Contains(i1) )
      continue;

    // Find identical
    TColStd_PackedMapOfInteger sameAs_i1;
    this->findSame(i1, sameAs_i1);
    //
    traversed.Unite(sameAs_i1);

    // Add result
    sameAs_i1.Add(i1); // Add seed face as well
    groups.Append(sameAs_i1);
  }
}

//! Searches for faces hosted similarly to the reference one.
//! \param reference [in]  index of the reference face.
//! \param found     [out] list of similarly hosted faces.
//! \return true in case of successful search, false -- otherwise.
bool asiAlgo_FindSameHosts::findSame(const int                   reference,
                                     TColStd_PackedMapOfInteger& found) const
{
  const TopoDS_Face& F1 = TopoDS::Face( m_faces(reference) );
  Handle(Geom_Plane) P1 = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface(F1) );
  //
  if ( P1.IsNull() )
    return false;

  // Get surface properties
  const gp_Ax3& Ax_P1   = P1->Position();
  const gp_Dir& xDir_P1 = Ax_P1.XDirection();
  const gp_Dir& yDir_P1 = Ax_P1.YDirection();
  gp_Pln        pln_P1  = P1->Pln();

  // Working precisions
  const double anglePrec = Precision::Angular();
  const double distPrec  = Precision::Confusion();

  // Look for identical faces
  for ( int i2 = 1; i2 <= m_faces.Extent(); ++i2 )
  {
    if ( i2 == reference )
      continue;

    const TopoDS_Face& F2 = TopoDS::Face( m_faces(i2) );
    Handle(Geom_Plane) P2 = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface(F2) );
    //
    if ( P2.IsNull() )
      continue;

    // Get surface properties
    const gp_Ax3& Ax_P2   = P2->Position();
    const gp_Dir& xDir_P2 = Ax_P2.XDirection();
    const gp_Dir& yDir_P2 = Ax_P2.YDirection();
    gp_Pln        pln_P2  = P2->Pln();

    if ( !xDir_P1.IsParallel(xDir_P2, anglePrec) )
      continue;
    //
    if ( !yDir_P1.IsParallel(yDir_P2, anglePrec) )
      continue;
    //
    if ( pln_P1.Distance(pln_P2) > distPrec )
      continue;

  //--------------------------------------------------//
    found.Add(i2);
  //--------------------------------------------------//
  }
  return found.Extent() > 0;
}
