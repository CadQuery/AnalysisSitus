//-----------------------------------------------------------------------------
// Created on: 23 May 2016
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
