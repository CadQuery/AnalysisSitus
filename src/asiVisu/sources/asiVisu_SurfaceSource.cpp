//-----------------------------------------------------------------------------
// Created on: 25 March 2016
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

// Own include
#include <asiVisu_SurfaceSource.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_ProjectPointOnMesh.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// OCCT includes
#include <GeomLProp_SLProps.hxx>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_SurfaceSource);

//! Default constructor.
asiVisu_SurfaceSource::asiVisu_SurfaceSource()
//
  : vtkPolyDataAlgorithm (),
    m_iSteps             (0),
    m_scalars            (Scalars_NoScalars),
    m_fMinScalar         (0.0),
    m_fMaxScalar         (0.0),
    m_fTrimU             (100.0),
    m_fTrimV             (100.0)
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_SurfaceSource::~asiVisu_SurfaceSource()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Sets reference mesh to compute distance for deriving deviation maps.
//! \param[in] mesh mesh to set as a reference.
void asiVisu_SurfaceSource::SetReferenceMesh(const Handle(asiAlgo_BVHFacets)& mesh)
{
  m_mesh = mesh;
  //
  this->Modified();
}

//! Initialize data source with a parametric surface.
//! \param surf [in] surface to visualize.
void asiVisu_SurfaceSource::SetInputSurface(const Handle(Geom_Surface)& surf)
{
  m_surf = surf;
  //
  this->Modified();
}

//! Sets the number of sampling steps for surface discretization.
//! \param nSteps [in] number of steps to set.
void asiVisu_SurfaceSource::SetNumberOfSteps(const int nSteps)
{
  m_iSteps = nSteps;
  //
  this->Modified();
}

//! Sets type of scalars to associate with nodes.
//! \param scalars [in] scalars.
void asiVisu_SurfaceSource::SetScalars(const NodeScalars scalars)
{
  m_scalars = scalars;
  //
  this->Modified();
}

//! Sets trimming values for infinite surface domains.
//! \param uLimit [in] trimming value for U.
//! \param vLimit [in] trimming value for V.
void asiVisu_SurfaceSource::SetTrimValues(const double uLimit,
                                          const double vLimit)
{
  m_fTrimU = uLimit;
  m_fTrimV = vLimit;
  //
  this->Modified();
}

//-----------------------------------------------------------------------------

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param request      [in]  describes "what" algorithm should do. This is
//!                           typically just one key such as REQUEST_INFORMATION.
//! \param inputVector  [in]  inputs of the algorithm.
//! \param outputVector [out] outputs of the algorithm.
int asiVisu_SurfaceSource::RequestData(vtkInformation*        request,
                                       vtkInformationVector** inputVector,
                                       vtkInformationVector*  outputVector)
{
  if ( m_surf.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL surface" );
    return 0;
  }

  if ( m_iSteps <= 0 )
  {
    vtkErrorMacro( << "Invalid domain: number of sampling steps must be positive" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* polyOutput = vtkPolyData::GetData(outputVector);
  polyOutput->Allocate();

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  polyOutput->SetPoints(points);

  // Array for scalars
  vtkSmartPointer<vtkDoubleArray> curvature;
  //
  if ( m_scalars == Scalars_GaussianCurvature ||
       m_scalars == Scalars_MeanCurvature ||
       m_scalars == Scalars_Deviation )
  {
    vtkPointData* PD = polyOutput->GetPointData();
    curvature = asiVisu_Utils::InitDoubleArray(ARRNAME_SURF_SCALARS);
    PD->SetScalars(curvature);
  }

  //---------------------------------------------------------------------------

  double uMin, uMax, vMin, vMax;
  m_surf->Bounds(uMin, uMax, vMin, vMax);
  //
  uMin = asiVisu_Utils::TrimInf(uMin, m_fTrimU);
  uMax = asiVisu_Utils::TrimInf(uMax, m_fTrimU);
  vMin = asiVisu_Utils::TrimInf(vMin, m_fTrimV);
  vMax = asiVisu_Utils::TrimInf(vMax, m_fTrimV);

  const double uStep = (uMax - uMin) / m_iSteps;
  const double vStep = (vMax - vMin) / m_iSteps;

  // Choose u values
  std::vector<double> U;
  double              u     = uMin;
  bool                uStop = false;
  //
  while ( !uStop )
  {
    if ( u > uMax )
    {
      u     = uMax;
      uStop = true;
    }

    U.push_back(u);
    u += uStep;
  }

  // Choose v values
  std::vector<double> V;
  double              v     = vMin;
  bool                vStop = false;
  //
  while ( !vStop )
  {
    if ( v > vMax )
    {
      v     = vMax;
      vStop = true;
    }

    V.push_back(v);
    v += vStep;
  }

  m_fMinScalar =  RealLast();
  m_fMaxScalar = -RealLast();

  // Register geometry (points)
  std::vector< std::vector<vtkIdType> > UV_ids;
  for ( size_t i = 0; i < U.size(); ++i )
  {
    std::vector<vtkIdType> uIso_ids;
    for ( size_t j = 0; j < V.size(); ++j )
    {
      const gp_Pnt P = m_surf->Value(U[i], V[j]);
      //
      uIso_ids.push_back( this->registerGridPoint(P, polyOutput) );

      // Associate scalars
      if ( m_scalars != Scalars_NoScalars )
      {
        const double scalar = this->computeScalar(U[i], V[j]);
        //
        curvature->InsertNextValue(scalar);
      }
    }
    UV_ids.push_back(uIso_ids);
  }

  // Register topology (triangles)
  for ( size_t i = 1; i < U.size(); ++i )
  {
    for ( size_t j = 1; j < V.size(); ++j )
    {
      // First triangle
      {
        const vtkIdType n1 = UV_ids[i]    [j];
        const vtkIdType n2 = UV_ids[i]    [j - 1];
        const vtkIdType n3 = UV_ids[i - 1][j];
        //
        this->registerTriangle(n1, n2, n3, polyOutput);
      }

      // Opposite triangle
      {
        const vtkIdType n1 = UV_ids[i]    [j - 1];
        const vtkIdType n2 = UV_ids[i - 1][j - 1];
        const vtkIdType n3 = UV_ids[i - 1][j];
        //
        this->registerTriangle(n1, n2, n3, polyOutput);
      }
    }
  }

  if ( this->HasScalars() )
    if ( Abs(m_fMaxScalar - m_fMinScalar) < 1.0e-6 )
      m_fMinScalar = m_fMaxScalar; // Good for visualization

  //---------------------------------------------------------------------------

  return Superclass::RequestData(request, inputVector, outputVector);
}

//! Adds the given point to the passed polygonal data set.
//! \param point    [in]     point to add.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType asiVisu_SurfaceSource::registerGridPoint(const gp_Pnt& point,
                                                   vtkPolyData*  polyData)
{
  // Access necessary arrays
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType pid = points->InsertNextPoint( point.X(),
                                           point.Y(),
                                           point.Z() );

  return pid;
}

//! Adds a triangle cell into the polygonal data set.
//! \param n1       [in]     index of the first node.
//! \param n2       [in]     index of the second node.
//! \param n3       [in]     index of the third node.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_SurfaceSource::registerTriangle(const vtkIdType n1,
                                                  const vtkIdType n2,
                                                  const vtkIdType n3,
                                                  vtkPolyData*    polyData)
{
  std::vector<vtkIdType> nodes;
  nodes.push_back(n1);
  nodes.push_back(n2);
  nodes.push_back(n3);
  //
  vtkIdType cellID = polyData->InsertNextCell(VTK_TRIANGLE, 3, &nodes[0]);
  //
  return cellID;
}

//! Computes scalar value for the passed point on surface. This function
//! also updates the stored min and max scalar values which are then used
//! to specify the range.
//! \param[in] u U parameter value.
//! \param[in] v V parameter value.
//! \return computed scalar.
double asiVisu_SurfaceSource::computeScalar(const double u, const double v)
{
  if ( m_scalars == Scalars_GaussianCurvature || m_scalars == Scalars_MeanCurvature )
  {
    GeomLProp_SLProps lProps( m_surf, u, v, 2, gp::Resolution() );
    double k = 0.0;
    //
    if ( lProps.IsCurvatureDefined() )
    {
      if ( m_scalars == Scalars_GaussianCurvature )
        k = lProps.GaussianCurvature();
      if ( m_scalars == Scalars_MeanCurvature )
        k = lProps.MeanCurvature();
      //
      if ( k < m_fMinScalar ) m_fMinScalar = k;
      if ( k > m_fMaxScalar ) m_fMaxScalar = k;
    }

    return k;
  }

  if ( m_scalars == Scalars_Deviation )
  {
    // Compute deviation by projecting the sample point.
    asiAlgo_ProjectPointOnMesh projectTool(m_mesh);
    //
    gp_Pnt       P     = m_surf->Value(u, v);
    gp_Pnt       Pproj = projectTool.Perform(P);
    const double dist  = P.Distance(Pproj);
    //
    if ( dist < m_fMinScalar ) m_fMinScalar = dist;
    if ( dist > m_fMaxScalar ) m_fMaxScalar = dist;

    return dist;
  }

  return 0.0; // No scalar defined.
}
