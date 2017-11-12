//-----------------------------------------------------------------------------
// Created on: 13 November 2015
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

#ifndef asiVisu_MeshEScalarPipeline_h
#define asiVisu_MeshEScalarPipeline_h

// Visualization includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshEScalarDataProvider, asiVisu_MeshDataProvider)

//! Provides data necessary for visualization of mesh with elemental scalars.
class asiVisu_MeshEScalarDataProvider : public asiVisu_MeshDataProvider
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshEScalarDataProvider, asiVisu_MeshDataProvider)

public:

  virtual Handle(HIntArray)
    GetElementIDs() const = 0;

  virtual Handle(HRealArray)
    GetElementScalars() const = 0;

};

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshEScalarPipeline, asiVisu_Pipeline)

//! Visualization pipeline for meshes with elemental scalars.
class asiVisu_MeshEScalarPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshEScalarPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_MeshEScalarPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dataProvider);

private:

  virtual void addToRendererCallback      (vtkRenderer* renderer);
  virtual void removeFromRendererCallback (vtkRenderer* renderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  asiVisu_MeshEScalarPipeline(const asiVisu_MeshEScalarPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshEScalarPipeline& operator=(const asiVisu_MeshEScalarPipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_EScalar = 1, //!< Filter for populating cell scalar array.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

};

#endif
