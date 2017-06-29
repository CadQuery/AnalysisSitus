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

#ifndef asiVisu_MeshEVectorPipeline_h
#define asiVisu_MeshEVectorPipeline_h

// Visualization includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>

//-----------------------------------------------------------------------------
// Data Provider
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshEVectorDataProvider, asiVisu_MeshDataProvider)

//! Data source for the corresponding pipeline. Specifies all data necessary
//! for visualization of mesh with elemental vectors.
class asiVisu_MeshEVectorDataProvider : public asiVisu_MeshDataProvider
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshEVectorDataProvider,asiVisu_MeshDataProvider)

public:

  virtual Handle(HIntArray)
    GetElementIDs() const = 0;

  virtual Handle(HRealArray)
    GetElementVectors() const = 0;

  virtual double
    GetMaxModulus() const = 0;

};

//-----------------------------------------------------------------------------
// Pipeline
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiVisu_MeshEVectorPipeline, asiVisu_Pipeline)

//! Visualization pipeline for Mesh meshes with elemental vectors.
class asiVisu_MeshEVectorPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshEVectorPipeline, asiVisu_Pipeline)

public:

  asiVisu_EXPORT
    asiVisu_MeshEVectorPipeline();

public:

  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& theDataProvider);

private:

  virtual void addToRendererCallback      (vtkRenderer* theRenderer);
  virtual void removeFromRendererCallback (vtkRenderer* theRenderer);
  virtual void updateCallback             ();

private:

  //! Copying prohibited.
  asiVisu_MeshEVectorPipeline(const asiVisu_MeshEVectorPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshEVectorPipeline& operator=(const asiVisu_MeshEVectorPipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_EVector = 1, //!< Filter for populating cell vector array.
    Filter_Trsf,        //!< Transformation filter.
    Filter_Glyph3D,     //!< Filter for glyphing.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

};

#endif
