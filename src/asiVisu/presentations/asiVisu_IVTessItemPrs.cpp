//-----------------------------------------------------------------------------
// Created on: 23 May 2016
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

// Own include
#include <asiVisu_IVTessItemPrs.h>

// A-Situs (visualization) includes
#include <asiVisu_MeshContourPipeline.h>
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param theNode [in] Node to create a Presentation for.
asiVisu_IVTessItemPrs::asiVisu_IVTessItemPrs(const Handle(ActAPI_INode)& theNode)
: asiVisu_IVPrs(theNode)
{
  // Create Data Provider
  Handle(asiVisu_MeshDataProvider)
    DP = new asiVisu_MeshDataProvider( theNode->GetId(),
                                      ActParamStream() << theNode->Parameter(asiData_IVTessItemNode::PID_Mesh) );

  // Pipeline for contours
  this->addPipeline        ( Pipeline_Main, new asiVisu_MeshPipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

  // Pipeline for mesh contour
  this->addPipeline(Pipeline_MeshContour, new asiVisu_MeshContourPipeline);
  this->assignDataProvider(Pipeline_MeshContour, DP);

  // We use CONTOUR mesh pipeline along with an ordinary one. Thus it is
  // really necessary to resolve coincident primitives to avoid blinking
  // on mesh edges
  vtkMapper::SetResolveCoincidentTopology(1);
}

//! Factory method for Presentation.
//! \param theNode [in] Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVTessItemPrs::Instance(const Handle(ActAPI_INode)& theNode)
{
  return new asiVisu_IVTessItemPrs(theNode);
}
