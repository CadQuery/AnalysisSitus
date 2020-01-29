//-----------------------------------------------------------------------------
// Created on: 30 November 2016
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

#ifndef asiVisu_ShapeRobustTessellator_h
#define asiVisu_ShapeRobustTessellator_h

// asiVisu includes
#include <asiVisu_ShapeColorSource.h>
#include <asiVisu_ShapeData.h>

// asiAlgo includes
#include <asiAlgo_AAG.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// OCCT includes
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

// VTK includes
#pragma warning(push, 0)
#include <vtkType.h>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Utility which is designed to offer a high level of sustainability of
//! visualization facets. Why would you want to have a non-standard
//! faceter for your B-Rep model? Such need arises in two major contexts:
//!
//! - Your CAD model is broken. Local topology anomalies may cause failures
//!   in standard triangulation algorithms, which will lead to a situation
//!   when you see just nothing on your screen. However, you may want to
//!   observe the topology anomalies and have at least an idea on how
//!   shape looks like.
//!
//! - Your CAD model is in temporary "not-so-valid" state. This usually happens
//!   when you apply Euler Operators which keep the topological integrity
//!   of your model but generally break its geometric consistency. A broken
//!   geometric integrity is a severe issue for faceters, so you end up,
//!   again, with a "black screen". However, the inability to see the
//!   intermediate results of Euler Ops contradicts to our intuition as
//!   the result of every single operator can be crafted by hand (well,
//!   involving some abstract shapes due to unknown geometries).
//!
//! This tessellation tool attempts to build a detailed polygonal
//! representation of a shape regardless of any invalidities. Of course,
//! such representation cannot be unambiguous since PMC is not defined for
//! "not-so-valid" models. However, the advantage of such visualization over
//! the standard one is its capacity to give you "at least something" to
//! look at. The latter enables you to debug your geometric stuff while
//! having some visual feedback from your prototyping system.
class asiVisu_ShapeRobustTessellator : public vtkObject
{
public:

  //! Generator of scalars for custom colors associated with boundary
  //! elements of a shape.
  class t_colorScalarGenerator
  {
  public:

    //! Ctor accepting the last non-occupied scalar value. This value will be
    //! the value returned on first invocation of GetScalar() method.
    //! \param[in] firstUnusedScalar first unused scalar.
    t_colorScalarGenerator(const int firstUnusedScalar)
    {
      m_iUnusedScalar = firstUnusedScalar;
    }

  public:

    //! Returns the scalar associated with the passed color.
    //! \param[in] color integer representation of the color in question.
    //! \return scalar to use.
    int GetScalar(const int color)
    {
      const int* pScalar = m_map.Seek(color);
      if ( pScalar )
        return *pScalar;

      int result = m_iUnusedScalar++;
      m_map.Bind(color, result);
      return result;
    }

    //! \return unused scalar.
    int GetUnusedScalar() const
    {
      return m_iUnusedScalar;
    }

    //! \return scalar mapping.
    const NCollection_DataMap<int, int>& GetScalarMap() const
    {
      return m_map;
    }

  private:

    int                           m_iUnusedScalar; //!< Current unused scalar.
    NCollection_DataMap<int, int> m_map;           //!< Color-scalar map.
  };

public:

  //! Class-level function to supply faces with scalars.
  //! \param[in] colorSource abstract color source.
  //! \param[in] pScalarGen  scalar generator.
  //! \param[in] faceId      ID of the face of interest.
  //! \param[in] defaultType default scalar corresponding to face type.
  //! \return scalar value.
  asiVisu_EXPORT static int
    FaceScalar(const Handle(asiVisu_ShapeColorSource)& colorSource,
               t_colorScalarGenerator*                 pScalarGen,
               const int                               faceId,
               const asiVisu_ShapePrimitive            defaultType);

// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_ShapeRobustTessellator, vtkObject);

  asiVisu_EXPORT static asiVisu_ShapeRobustTessellator*
    New();

  asiVisu_EXPORT
    asiVisu_ShapeRobustTessellator();

  asiVisu_EXPORT
    ~asiVisu_ShapeRobustTessellator();

public:

  asiVisu_EXPORT void
    Initialize(const Handle(asiAlgo_AAG)&              aag,
               const double                            linearDeflection = 0,
               const double                            angularDeflection_deg = 0,
               const Handle(asiVisu_ShapeColorSource)& colorSource = nullptr,
               ActAPI_ProgressEntry                    progress = nullptr,
               ActAPI_PlotterEntry                     plotter  = nullptr);

  asiVisu_EXPORT void
    Initialize(const TopoDS_Shape&                     shape,
               const double                            linearDeflection = 0,
               const double                            angularDeflection_deg = 0,
               const Handle(asiVisu_ShapeColorSource)& colorSource = nullptr,
               ActAPI_ProgressEntry                    progress = nullptr,
               ActAPI_PlotterEntry                     plotter  = nullptr);

public:

  //! \return AAG instance.
  const Handle(asiAlgo_AAG)& GetAAG() const
  {
    return m_aag;
  }

  //! \return master shape.
  const TopoDS_Shape& GetShape() const
  {
    return m_shape;
  }

  //! \return result shape data.
  const Handle(asiVisu_ShapeData)& GetResult() const
  {
    return m_data;
  }

  //! \return last unused scalar for boundary element coloring.
  int GetLastUnusedScalar() const
  {
    return m_pScalarGen->GetUnusedScalar();
  }

  //! Returns the map containing scalar values for custom colors.
  //! \param[out] extraScalars map of extra scalars used for custom coloring
  //!                          of boundary elements.
  void GetExtraColorsScalars(NCollection_DataMap<int, int>& extraScalars) const
  {
    extraScalars = m_pScalarGen->GetScalarMap();
  }

  //! \return scalar generator.
  t_colorScalarGenerator* GetScalarGenerator() const
  {
    return m_pScalarGen;
  }

public:

  asiVisu_EXPORT void
    Build();

protected:

  asiVisu_EXPORT void
    internalInit(const double                            linearDeflection,
                 const double                            angularDeflection_deg,
                 const Handle(asiVisu_ShapeColorSource)& colorSource,
                 ActAPI_ProgressEntry                    progress,
                 ActAPI_PlotterEntry                     plotter);

  asiVisu_EXPORT virtual void
    internalBuild();

  asiVisu_EXPORT bool
    isValidFace(const TopoDS_Face& face) const;

  asiVisu_EXPORT int
    getFaceScalar(const int                    faceId,
                  const asiVisu_ShapePrimitive defaultType) const;

protected:

  asiVisu_EXPORT void
    addVertex(const TopoDS_Vertex&         vertex,
              const vtkIdType              shapeId,
              const asiVisu_ShapePrimitive scType);

  asiVisu_EXPORT void
    addEdge(const TopoDS_Edge&           edge,
            const vtkIdType              shapeId,
            const int                    edgeId,
            const asiVisu_ShapePrimitive scType);

  asiVisu_EXPORT void
    addFace(const TopoDS_Face& face,
            const vtkIdType    shapeId,
            const int          faceId,
            const bool         isValid);

  asiVisu_EXPORT void
    addCurve(const Handle(Geom_Curve)&    curve,
             const double                 tMin,
             const double                 tMax,
             const vtkIdType              shapeId,
             const asiVisu_ShapePrimitive scType);

  asiVisu_EXPORT void
    addBrokenEdgeCurve(const TopoDS_Edge&           edge,
                       const vtkIdType              shapeId,
                       const int                    edgeId,
                       const asiVisu_ShapePrimitive scType);

protected:

  double                           m_fGlobalBndDiag;    //!< Diagonal of AABB for calibration.
  Handle(asiAlgo_AAG)              m_aag;               //!< AAG to access the topological elements.
  TopoDS_Shape                     m_shape;             //!< Input shape.
  Handle(asiVisu_ShapeData)        m_data;              //!< Visualization data.
  double                           m_fLinDeflection;    //!< Linear deflection.
  double                           m_fAngDeflectionDeg; //!< Angular deflection.
  Handle(asiVisu_ShapeColorSource) m_colorSource;       //!< Interface to access colors of sub-shapes.
  ActAPI_ProgressEntry             m_progress;          //!< Progress notifier.
  ActAPI_PlotterEntry              m_plotter;           //!< Imperative plotter.

  //! Scalars generator for coloring.
  t_colorScalarGenerator* m_pScalarGen;

};

#endif
