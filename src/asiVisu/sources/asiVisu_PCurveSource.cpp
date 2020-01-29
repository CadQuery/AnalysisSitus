//-----------------------------------------------------------------------------
// Created on: 02 December 2015
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
#include <asiVisu_PCurveSource.h>

// asiVisu includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkObjectFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <Geom2dAdaptor_Curve.hxx>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_PCurveSource)

//! Default constructor.
asiVisu_PCurveSource::asiVisu_PCurveSource() : asiVisu_CurveSource()
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_PCurveSource::~asiVisu_PCurveSource()
{
}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Sets input geometry.
//! \param edge [in] edge to retrieve the p-curve from.
//! \param face [in] host face.
void asiVisu_PCurveSource::SetEdgeOnFace(const TopoDS_Edge& edge,
                                         const TopoDS_Face& face)
{
  if ( edge.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: edge is null" );
    return;
  }

  if ( face.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: face is null" );
    return;
  }

  m_edge = edge;
  m_face = face;

  /* ============================================
   *  Prepare Cartesian arrays for visualization
   * ============================================ */

  // Access p-curve
  double f, l;
  Handle(Geom2d_Curve) c2d = BRep_Tool::CurveOnSurface(m_edge, m_face, f, l);
  //
  if ( c2d.IsNull() )
  {
    vtkWarningMacro( << "nullptr p-curve" );
    return;
  }

  // Discretize with different methods (if onw fails, try another).
  std::vector<gp_Pnt> points;
  int nPts = 0;
  //
  try
  {
    const bool revOrder = (f > l ? true : false);
    //
    Geom2dAdaptor_Curve gac(c2d, revOrder ? l : f, revOrder ? f : l);
    GCPnts_QuasiUniformDeflection QUDefl(gac, 1e-4);
    //
    if ( !QUDefl.IsDone() )
    {
      std::cout << "Warning: quasi-uniform deflection failed." << std::endl;

      GCPnts_TangentialDeflection TDefl(gac, 1.0, 1e-4);
      //
      if ( !TDefl.NbPoints() )
      {
        vtkErrorMacro( << "Cannot discretize p-curve" );
        return;
      }
      else
      {
        nPts = TDefl.NbPoints();

        for ( int p = 1; p <= nPts; ++p )
        {
          gp_Pnt point = TDefl.Value(p);
          points.push_back(point);
        }
      }
    }
    else
    {
      nPts = QUDefl.NbPoints();

      if ( revOrder )
        for ( int p = nPts; p >= 1; --p )
          points.push_back( QUDefl.Value(p) );
      else
        for ( int p = 1; p <= nPts; ++p )
          points.push_back( QUDefl.Value(p) );

    }
  }
  catch ( ... )
  {
    vtkErrorMacro( << "Failed (exception) to discretize curve" );
    return;
  }

  if ( !nPts )
  {
    vtkErrorMacro( << "No points in discretization" );
    return;
  }

  // Allocate arrays
  Handle(HRealArray) xCoords = new HRealArray(0, nPts - 1, 0.0),
                     yCoords = new HRealArray(0, nPts - 1, 0.0),
                     zCoords = new HRealArray(0, nPts - 1, 0.0);

  for ( int k = 1; k <= nPts; ++k )
  {
    const gp_Pnt& P = points[k-1];
    //
    xCoords->ChangeValue(k - 1) = P.X();
    yCoords->ChangeValue(k - 1) = P.Y();
    zCoords->ChangeValue(k - 1) = P.Z();
  }

  /* ================================
   *  Resolve scalar for orientation
   * ================================ */

  asiVisu_Orientation ori;
  if ( m_edge.Orientation() == TopAbs_FORWARD )
    ori = VisuOri_Forward;
  else if ( m_edge.Orientation() == TopAbs_REVERSED )
    ori = VisuOri_Reversed;
  else if ( m_edge.Orientation() == TopAbs_INTERNAL )
    ori = VisuOri_Internal;
  else if ( m_edge.Orientation() == TopAbs_EXTERNAL )
    ori = VisuOri_External;
  else
    ori = VisuOri_Undefined;

  /* ===================
   *  Initialize source
   * =================== */

  // Perform basic initialization
  asiVisu_CurveSource::SetInputArrays(xCoords, yCoords, zCoords, ori);
}
