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
#include <asiVisu_IVPrs.h>

//-----------------------------------------------------------------------------

//! Callback for initialization of Presentation pipelines.
void asiVisu_IVPrs::beforeInitPipelines()
{
  // Do nothing...
}

//! Callback for initialization of Presentation pipelines.
void asiVisu_IVPrs::afterInitPipelines()
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked before the
//! kernel update routine starts.
void asiVisu_IVPrs::beforeUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for updating of Presentation pipelines invoked after the
//! kernel update routine completes.
void asiVisu_IVPrs::afterUpdatePipelines() const
{
  // Do nothing...
}

//! Callback for highlighting.
void asiVisu_IVPrs::highlight(vtkRenderer*                  theRenderer,
                              const asiVisu_PickResult&     thePickRes,
                              const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(thePickRes);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for highlighting reset.
void asiVisu_IVPrs::unHighlight(vtkRenderer*                  theRenderer,
                                const asiVisu_SelectionNature theSelNature) const
{
  asiVisu_NotUsed(theRenderer);
  asiVisu_NotUsed(theSelNature);
}

//! Callback for rendering.
void asiVisu_IVPrs::renderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}

//! Callback for de-rendering.
void asiVisu_IVPrs::deRenderPipelines(vtkRenderer* theRenderer) const
{
  asiVisu_NotUsed(theRenderer);
}
