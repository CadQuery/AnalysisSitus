//-----------------------------------------------------------------------------
// Created on: 25 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef visu_surface_source_h
#define visu_surface_source_h

// Visualization includes
#include <visu_utils.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

// OCCT includes
#include <Geom_Surface.hxx>

//! Source of polygonal data representing a parametric surface.
class visu_surface_source : public vtkPolyDataAlgorithm
{
public:

  //! This enumeration offers different scalar type which you may want
  //! to have associated with your surface.
  enum NodeScalars
  {
    Scalars_NoScalars = 0,
    Scalars_GaussianCurvature,
    Scalars_MeanCurvature
  };

// RTTI and construction:
public:

  vtkTypeMacro(visu_surface_source, vtkPolyDataAlgorithm);
  static visu_surface_source* New();

// Kernel methods:
public:

  void SetInputSurface  (const Handle(Geom_Surface)& surface);
  void SetNumberOfSteps (const int nSteps);
  void SetScalars       (const NodeScalars scalars);
  void SetTrimValues    (const double uLimit, const double vLimit);
  //
  inline double      GetMinScalar  () const { return m_fMinScalar; }
  inline double      GetMaxScalar  () const { return m_fMaxScalar; }
  inline bool        HasScalars    () const { return (m_scalars != Scalars_NoScalars); }
  inline NodeScalars GetScalarType () const { return m_scalars; }

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

protected:

  visu_surface_source();
  ~visu_surface_source();

private:

  visu_surface_source(const visu_surface_source&);
  visu_surface_source& operator=(const visu_surface_source&);

private:

  Handle(Geom_Surface) m_surf;       //!< Parametric surface to render.
  int                  m_iSteps;     //!< Number of steps for sampling UV space.
  NodeScalars          m_scalars;    //!< Type of scalars to associate.
  double               m_fMinScalar; //!< Min scalar value.
  double               m_fMaxScalar; //!< Max scalar value.
  double               m_fTrimU;     //!< Trimming value to limit infinite U.
  double               m_fTrimV;     //!< Trimming value to limit infinite V.
};

#endif
