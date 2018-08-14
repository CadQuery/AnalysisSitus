//-----------------------------------------------------------------------------
// Created on: 11 April 2016
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
#include <asiVisu_IVSurfacePrs.h>

// asiVisu includes
#include <asiVisu_BSurfKnotsPipeline.h>
#include <asiVisu_BSurfPolesPipeline.h>
#include <asiVisu_IVSurfacePipeline.h>
#include <asiVisu_IVSurfaceDataProvider.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkMapper.h>
#include <vtkProperty.h>

//! Creates a Presentation object for the passed Node.
//! \param[in] N Node to create a Presentation for.
asiVisu_IVSurfacePrs::asiVisu_IVSurfacePrs(const Handle(ActAPI_INode)& N)
: asiVisu_DefaultPrs(N)
{
  // Create Data Provider
  Handle(asiVisu_IVSurfaceDataProvider) DP = new asiVisu_IVSurfaceDataProvider(N);

  //---------------------------------------------------------------------------
  // Main pipeline
  //---------------------------------------------------------------------------

  // Pipeline for shaded surface
  this->addPipeline        ( Pipeline_Main, new asiVisu_IVSurfacePipeline );
  this->assignDataProvider ( Pipeline_Main, DP );

    //---------------------------------------------------------------------------
  // Control network for B-surfaces
  //---------------------------------------------------------------------------

  // Pipeline for control net
  this->addPipeline        ( Pipeline_BPoles, new asiVisu_BSurfPolesPipeline );
  this->assignDataProvider ( Pipeline_BPoles, DP );
  //
  this->GetPipeline(Pipeline_BPoles)->Actor()->SetVisibility(0);

  //---------------------------------------------------------------------------
  // Knots isos for B-surfaces
  //---------------------------------------------------------------------------

  // Pipeline for surface isolines corresponding to knots
  this->addPipeline        ( Pipeline_BKnotsIsos, new asiVisu_BSurfKnotsPipeline );
  this->assignDataProvider ( Pipeline_BKnotsIsos, DP );
  //
  this->GetPipeline(Pipeline_BKnotsIsos)->Actor()->SetVisibility(0);
}

//! Factory method for Presentation.
//! \param[in] N Node to create a Presentation for.
//! \return new Presentation instance.
Handle(asiVisu_Prs) asiVisu_IVSurfacePrs::Instance(const Handle(ActAPI_INode)& N)
{
  return new asiVisu_IVSurfacePrs(N);
}
