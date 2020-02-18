//-----------------------------------------------------------------------------
// Created on: 23 January 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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
#include <asiVisu_OctreePrs.h>

// asiVisu includes
#include <asiVisu_OctreeDataProvider.h>
#include <asiVisu_OctreePipeline.h>
#include <asiVisu_OctreeVectorsPipeline.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------

asiVisu_OctreePrs::asiVisu_OctreePrs(const Handle(ActAPI_INode)& N) : asiVisu_DefaultPrs(N)
{
  /* =====================
   *  Pipeline for octree.
   * ===================== */

  Handle(asiData_OctreeNode)
    octreeNode = Handle(asiData_OctreeNode)::DownCast(N);

  // Create data provider.
  Handle(asiVisu_OctreeDataProvider)
    octree_dp = new asiVisu_OctreeDataProvider(octreeNode);

  // Create pipeline for octree.
  Handle(asiVisu_OctreePipeline)
    octree_pl = new asiVisu_OctreePipeline;
  //
  this->addPipeline        ( Pipeline_Main, octree_pl );
  this->assignDataProvider ( Pipeline_Main, octree_dp );

  octree_pl->Actor()->GetProperty()->SetLineWidth(1.5f);

  /* ======================
   *  Pipeline for vectors.
   * ====================== */

  // Create pipeline for vectors extracted from octree.
  Handle(asiVisu_OctreeVectorsPipeline)
    vectors_pl = new asiVisu_OctreeVectorsPipeline( octree_pl->GetSource() );
  //
  this->addPipeline        ( Pipeline_Vectors, vectors_pl );
  this->assignDataProvider ( Pipeline_Vectors, octree_dp );

  vectors_pl->Actor()->GetProperty()->SetColor(1., 1., 1.);
  vectors_pl->Mapper()->ScalarVisibilityOff();
}

//-----------------------------------------------------------------------------

Handle(asiVisu_Prs) asiVisu_OctreePrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_OctreePrs(N);
}
