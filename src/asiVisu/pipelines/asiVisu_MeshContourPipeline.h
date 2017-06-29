//-----------------------------------------------------------------------------
// Created on: 26 November 2015
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

#ifndef asiVisu_MeshContourPipeline_h
#define asiVisu_MeshContourPipeline_h

// Visualization includes
#include <asiVisu_MeshPipeline.h>

// OCCT includes
#include <NCollection_DataMap.hxx>

DEFINE_STANDARD_HANDLE(asiVisu_MeshContourPipeline, asiVisu_MeshPipeline)

//! Visualization pipeline for a tessellation represented with contour.
class asiVisu_MeshContourPipeline : public asiVisu_MeshPipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshContourPipeline, asiVisu_MeshPipeline)

public:

  asiVisu_EXPORT
    asiVisu_MeshContourPipeline(const bool doUseDefaultColor = true);

private:

  //! Copying prohibited.
  asiVisu_MeshContourPipeline(const asiVisu_MeshContourPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshContourPipeline& operator=(const asiVisu_MeshContourPipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_ExtractEdges = 1 //!< Filter to extract polygonal contour.
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

private:

  virtual void callback_add_to_renderer      (vtkRenderer* theRenderer);
  virtual void callback_remove_from_renderer (vtkRenderer* theRenderer);
  virtual void callback_update               ();

private:

  //! Map of internally used filters.
  FilterMap m_filterMap;

  //! Contour color.
  double m_fColor[3];

  //! Contour opacity.
  double m_fOpacity;

  //! Contour line width.
  double m_fLineWidth;

};

#endif
