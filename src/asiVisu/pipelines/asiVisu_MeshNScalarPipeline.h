//-----------------------------------------------------------------------------
// Created on: 13 November 2015
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

#ifndef asiVisu_MeshNScalarPipeline_h
#define asiVisu_MeshNScalarPipeline_h

// Visualization includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshNScalarDataProvider, asiVisu_MeshDataProvider)

//! Data source for the corresponding pipeline. Specifies all data necessary
//! for visualization of mesh with nodal scalars.
class asiVisu_MeshNScalarDataProvider : public asiVisu_MeshDataProvider
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshNScalarDataProvider, asiVisu_MeshDataProvider)

public:

  virtual Handle(HIntArray)
    GetNodeIDs() const = 0;

  virtual Handle(HRealArray)
    GetNodeScalars() const = 0;

};

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshNScalarPipeline, asiVisu_Pipeline)

//! Visualization pipeline for meshes with nodal scalars.
class asiVisu_MeshNScalarPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshNScalarPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_MeshNScalarPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& theDataProvider);

private:

  virtual void addToRendererCallback      (vtkRenderer* theRenderer);
  virtual void removeFromRendererCallback (vtkRenderer* theRenderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  asiVisu_MeshNScalarPipeline(const asiVisu_MeshNScalarPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshNScalarPipeline& operator=(const asiVisu_MeshNScalarPipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_NScalar = 1, //!< Filter for populating point scalar array.
    Filter_Normals,     //!< Filter for calculation of normals.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

};

#endif
