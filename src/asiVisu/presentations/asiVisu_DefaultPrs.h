//-----------------------------------------------------------------------------
// Created on: 11 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016, Sergey Slyadnev
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

#ifndef asiVisu_IVPrs_h
#define asiVisu_IVPrs_h

// asiVisu includes
#include <asiVisu_Prs.h>
#include <asiVisu_Utils.h>

//! Base class for default non-interactive presentations.
class asiVisu_DefaultPrs : public asiVisu_Prs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_DefaultPrs, asiVisu_Prs)

public:

  virtual bool IsVisible() const
  {
    return true;
  }

// Callbacks:
private:

  virtual void
    beforeInitPipelines();

  virtual void
    afterInitPipelines();

  virtual void
    beforeUpdatePipelines() const;

  virtual void
    afterUpdatePipelines() const;

  virtual void
    highlight(vtkRenderer*                        renderer,
              const Handle(asiVisu_PickerResult)& pickRes,
              const asiVisu_SelectionNature       selNature) const;

  virtual void
    unHighlight(vtkRenderer*                  renderer,
                const asiVisu_SelectionNature selNature) const;

  virtual void
    renderPipelines(vtkRenderer* renderer) const;

  virtual void
    deRenderPipelines(vtkRenderer* renderer) const;

protected:

  asiVisu_EXPORT
    asiVisu_DefaultPrs(const Handle(ActAPI_INode)& node) : asiVisu_Prs(node) {}

};

#endif
