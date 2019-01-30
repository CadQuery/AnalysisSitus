//-----------------------------------------------------------------------------
// Created on: 28 March 2016
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

#ifndef asiVisu_SurfaceSource_h
#define asiVisu_SurfaceSource_h

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// OCCT includes
#include <Geom_Surface.hxx>

//-----------------------------------------------------------------------------

//! Source of polygonal data representing a parametric surface.
class asiVisu_SurfaceSource : public vtkPolyDataAlgorithm
{
public:

  //! This enumeration offers different scalar type which you may want
  //! to have associated with your surface.
  enum NodeScalars
  {
    Scalars_NoScalars = 0,     //!< No scalars associated with surface points.
    Scalars_GaussianCurvature, //!< Gaussian curvature measured in sample points.
    Scalars_MeanCurvature,     //!< Mean curvature measured in sample points.
    Scalars_Deviation          //!< Deviation from reference mesh measured in sample points.
  };

// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_SurfaceSource, vtkPolyDataAlgorithm);
  static asiVisu_SurfaceSource* New();

// Kernel methods:
public:

  void SetReferenceMesh (const Handle(asiAlgo_BVHFacets)& mesh);
  void SetInputSurface  (const Handle(Geom_Surface)& surface);
  void SetNumberOfSteps (const int nSteps);
  void SetScalars       (const NodeScalars scalars);
  void SetTrimValues    (const double uLimit, const double vLimit);
  //
  double      GetMinScalar  () const { return m_fMinScalar; }
  double      GetMaxScalar  () const { return m_fMaxScalar; }
  bool        HasScalars    () const { return (m_scalars != Scalars_NoScalars); }
  NodeScalars GetScalarType () const { return m_scalars; }

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  vtkIdType
    registerGridPoint(const gp_Pnt& point,
                      vtkPolyData*  polyData);

  vtkIdType
    registerTriangle(const vtkIdType n1,
                     const vtkIdType n2,
                     const vtkIdType n3,
                     vtkPolyData*    polyData);

  double
    computeScalar(const double u, const double v);

protected:

  asiVisu_SurfaceSource();
  ~asiVisu_SurfaceSource();

private:

  asiVisu_SurfaceSource(const asiVisu_SurfaceSource&);
  asiVisu_SurfaceSource& operator=(const asiVisu_SurfaceSource&);

private:

  Handle(asiAlgo_BVHFacets) m_mesh;       //!< Reference mesh to compute deviation map.
  Handle(Geom_Surface)      m_surf;       //!< Parametric surface to render.
  int                       m_iSteps;     //!< Number of steps for sampling UV space.
  NodeScalars               m_scalars;    //!< Type of scalars to associate.
  double                    m_fMinScalar; //!< Min scalar value.
  double                    m_fMaxScalar; //!< Max scalar value.
  double                    m_fTrimU;     //!< Trimming value to limit infinite U.
  double                    m_fTrimV;     //!< Trimming value to limit infinite V.
};

#endif
