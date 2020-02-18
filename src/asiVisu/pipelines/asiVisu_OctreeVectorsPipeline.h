//-----------------------------------------------------------------------------
// Created on: 18 February 2020
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

#ifndef asiVisu_OctreeVectorsPipeline_h
#define asiVisu_OctreeVectorsPipeline_h

// asiVisu includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_OctreeSource.h>
#include <asiVisu_Pipeline.h>

//-----------------------------------------------------------------------------

//! Visualization pipeline for vectors with octree as a source. This pipeline
//! is quite specific. It was developed to visualize the normal field in the
//! points sampled on the boundary voxelization and reprojected to the initial
//! surface.
class asiVisu_OctreeVectorsPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_OctreeVectorsPipeline, asiVisu_Pipeline)

public:

  //! Ctor initializing the pipeline with an external source.
  asiVisu_EXPORT
    asiVisu_OctreeVectorsPipeline(const vtkSmartPointer<asiVisu_OctreeSource>& source);

public:

  //! Sets input data provider. This method does the job.
  //! \param[in] dataProvider data provider.
  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

public:

  //! \return octree source.
  const vtkSmartPointer<asiVisu_OctreeSource>& GetSource() const
  {
    return m_source;
  }

private:

  virtual void callback_add_to_renderer      (vtkRenderer* renderer);
  virtual void callback_remove_from_renderer (vtkRenderer* renderer);
  virtual void callback_update               ();

private:

  //! Copying prohibited.
  asiVisu_OctreeVectorsPipeline(const asiVisu_OctreeVectorsPipeline&) = delete;

  //! Assignment prohibited.
  asiVisu_OctreeVectorsPipeline& operator=(const asiVisu_OctreeVectorsPipeline&) = delete;

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_Vector = 1, //!< Filter for populating cell vector array.
    Filter_Trsf,       //!< Transformation filter to prepare for glyphs.
    Filter_Glyph3D,    //!< Filter for generating glyphs.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Octree source.
  vtkSmartPointer<asiVisu_OctreeSource> m_source;

  //! Map of internally used filters.
  FilterMap m_filterMap;

};

#endif
