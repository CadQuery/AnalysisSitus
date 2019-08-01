//-----------------------------------------------------------------------------
// Created on: 01 August 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

#ifndef asiVisu_CurveSourceBase_h
#define asiVisu_CurveSourceBase_h

// asiVisu includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//! Base class for polygonal data sources representing curves.
class asiVisu_CurveSourceBase : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_CurveSourceBase, vtkPolyDataAlgorithm);

  asiVisu_EXPORT static asiVisu_CurveSourceBase*
    New();

public:

  //! Prepares raw arrays for coordinates of discretization points along
  //! the passed curve.
  //! \param[in]  curve   curve to discretize.
  //! \param[in]  first   first trimming parameter on the curve.
  //! \param[in]  last    last trimming parameter on the curve.
  //! \param[out] xCoords X coordinates of the computed discretization points.
  //! \param[out] yCoords Y coordinates of the computed discretization points.
  //! \param[out] zCoords Z coordinates of the computed discretization points.
  //! \return true in case of successful discretization, false -- otherwise.
  asiVisu_EXPORT bool
    FillArrays(const Handle(Geom_Curve)& curve,
               const double              first,
               const double              last,
               Handle(HRealArray)&       xCoords,
               Handle(HRealArray)&       yCoords,
               Handle(HRealArray)&       zCoords) const;

  //! Prepares raw arrays for coordinates of discretization points along
  //! the passed two-dimensional curve.
  //! \param[in]  curve   two-dimensional curve to discretize.
  //! \param[in]  first   first trimming parameter on the curve.
  //! \param[in]  last    last trimming parameter on the curve.
  //! \param[out] xCoords X coordinates of the computed discretization points.
  //! \param[out] yCoords Y coordinates of the computed discretization points.
  //! \param[out] zCoords all zeroes.
  //! \return true in case of successful discretization, false -- otherwise.
  asiVisu_EXPORT bool
    FillArrays(const Handle(Geom2d_Curve)& curve,
               const double                first,
               const double                last,
               Handle(HRealArray)&         xCoords,
               Handle(HRealArray)&         yCoords,
               Handle(HRealArray)&         zCoords) const;

  //! Populates polydata from the coordinate arrays.
  //! \param[in,out] polyData polydata being populated.
  //! \return true in case of success, false -- otherwise.
  asiVisu_EXPORT bool
    FillPolydata(vtkPolyData* polyData);

public:

  void          SetTipSize    (const double size)       { m_fOriTipSize = size; }
  double        GetTipSize    ()                  const { return m_fOriTipSize; }

  void          SetTipTangent (const gp_Vec& vec)       { m_oriT = vec; }
  const gp_Vec& GetTipTangent ()                  const { return m_oriT; }

  void          SetTipNorm    (const gp_Vec& vec)       { m_oriN = vec; }
  const gp_Vec& GetTipNorm    ()                  const { return m_oriN; }

  void          SetPedigreeId (const int pid)           { m_iPedigreeId = pid; }
  int           GetPedigreeId ()                  const { return m_iPedigreeId; }

protected:

  //! Adds the given point to the passed polygonal data set.
  //! \param[in]     point    point to add.
  //! \param[in,out] polyData polygonal data set being populated.
  //! \return ID of the just added VTK point.
  asiVisu_EXPORT vtkIdType
    registerGridPoint(const gp_Pnt& point,
                      vtkPolyData*  polyData) const;

  //! Adds the grid point with the given index to the passed polygonal data set.
  //! \param[in]     index    index of (X, Y, Z) coordinate triple.
  //! \param[in,out] polyData polygonal data set being populated.
  //! \return ID of the just added VTK point.
  asiVisu_EXPORT vtkIdType
    registerGridPoint(const int    index,
                      vtkPolyData* polyData) const;

  //! Adds a line cell into the polygonal data set.
  //! \param[in]      pointStart first point.
  //! \param[in]      pointEnd   second point.
  //! \param[in, out] polyData   polygonal data set being populated.
  //! \return ID of the just added VTK cell.
  asiVisu_EXPORT vtkIdType
    registerLine(const gp_Pnt& pointStart,
                 const gp_Pnt& pointEnd,
                 vtkPolyData*  polyData) const;

  //! Adds a line cell into the polygonal data set.
  //! \param[in]     index    index of (X, Y, Z) coordinate triple.
  //! \param[in,out] polyData polygonal data set being populated.
  //! \return ID of the just added VTK cell.
  asiVisu_EXPORT vtkIdType
    registerLine(const int    index,
                 vtkPolyData* polyData) const;

  //! Adds a vertex cell into the polygonal data set.
  //! \param[in]     index    index of (X, Y, Z) coordinate triple.
  //! \param[in,out] polyData polygonal data set being populated.
  //! \return ID of the just added VTK cell.
  asiVisu_EXPORT vtkIdType
    registerVertex(const int    index,
                   vtkPolyData* polyData) const;

protected:

  //! Default ctor.
  asiVisu_EXPORT
    asiVisu_CurveSourceBase();

  //! Dtor.
  asiVisu_EXPORT
    ~asiVisu_CurveSourceBase();

private:

  asiVisu_CurveSourceBase(const asiVisu_CurveSourceBase&) = delete;
  asiVisu_CurveSourceBase& operator=(const asiVisu_CurveSourceBase&) = delete;

protected:

  Handle(Geom_Curve) m_curve3d;     //!< 3D curve (if specified).
  Handle(HRealArray) m_XCoords;     //!< X coordinates.
  Handle(HRealArray) m_YCoords;     //!< Y coordinates.
  Handle(HRealArray) m_ZCoords;     //!< Z coordinates.
  int                m_iPedigreeId; //!< Pedigree ID.

// Orientation markers:
protected:

  asiVisu_Orientation m_ori;         //!< Curve orientation.
  double              m_fOriTipSize; //!< Size of orientation tip (calculated externally).
  gp_Vec              m_oriT;        //!< Orientation vector at the end point.
  gp_Vec              m_oriN;        //!< Normal to the curve at the end point.

};

#endif
