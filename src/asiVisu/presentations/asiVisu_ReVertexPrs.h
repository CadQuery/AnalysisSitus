//-----------------------------------------------------------------------------
// Created on: 02 November 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiVisu_ReVertexPrs_h
#define asiVisu_ReVertexPrs_h

// asiVisu includes
#include <asiVisu_DefaultPrs.h>

// asiData includes
#include <asiData_ReVertexNode.h>

// VTK includes
#include <vtkTextWidget.h>

//! Presentation class for a single vertex in a wireframe model of
//! reverse engineering workflow.
class asiVisu_ReVertexPrs : public asiVisu_DefaultPrs
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ReVertexPrs, asiVisu_DefaultPrs)

  // Allows to register this Presentation class
  DEFINE_PRESENTATION_FACTORY(asiData_ReVertexNode, Instance)

public:

  //! Pipelines.
  enum PipelineId
  {
    Pipeline_Main = 1
  };

public:

  //! Factory method for Presentation.
  //! \param[in] N Node to create a Presentation for.
  //! \return new Presentation instance.
  asiVisu_EXPORT static Handle(asiVisu_Prs)
    Instance(const Handle(ActAPI_INode)& N);

protected:

  //! Reaction to highlighting. For the case of Vertex Node, the reaction
  //! is simply changing the actor's color.
  //! \param[in] renderer  instance of renderer.
  //! \param[in] pickRes   picking result.
  //! \param[in] selNature selection nature.
  virtual void
    highlight(vtkRenderer*                        renderer,
              const Handle(asiVisu_PickerResult)& pickRes,
              const asiVisu_SelectionNature       selNature) const;

  //! Reaction to unhighlighting. For the case of Vertex Node, the reaction
  //! is simply changing the actor's color.
  //! \param[in] renderer  instance of renderer.
  //! \param[in] selNature selection nature.
  virtual void
    unHighlight(vtkRenderer*                  renderer,
                const asiVisu_SelectionNature selNature) const;

private:

  //! \brief Creates a Presentation object for the passed Node.
  //! Allocation is allowed only via Instance() method.
  //! \param[in] N Node to create a Presentation for.
  asiVisu_ReVertexPrs(const Handle(ActAPI_INode)& N);

protected:

  // Convenience structure for color in RGB format.
  struct t_colorRGBf
  {
    double r, g, b;
    t_colorRGBf() : r(0.), g(0.), b(0.) {}

  };

  t_colorRGBf m_color;     //!< Default color of a vertex.
  t_colorRGBf m_hiliColor; //!< Color on highlighting.

};

#endif
