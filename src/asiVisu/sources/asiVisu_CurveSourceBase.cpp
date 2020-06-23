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

// Own include
#include <asiVisu_CurveSourceBase.h>

// asiVisu includes
#include <asiVisu_GeomUtils.h>

// OCCT includes
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomLProp_CLProps.hxx>

// VTK includes
#include <vtkCellData.h>
#include <vtkIdTypeArray.h>

//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_CurveSourceBase)

//-----------------------------------------------------------------------------

asiVisu_CurveSourceBase::asiVisu_CurveSourceBase()
: vtkPolyDataAlgorithm (),
  m_iPedigreeId        (0),
  m_fOriTipSize        (0.0)
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline.
}

//-----------------------------------------------------------------------------

asiVisu_CurveSourceBase::~asiVisu_CurveSourceBase()
{}

//-----------------------------------------------------------------------------

bool asiVisu_CurveSourceBase::FillArrays(const Handle(Geom_Curve)& curve,
                                         const double              first,
                                         const double              last,
                                         Handle(HRealArray)&       xCoords,
                                         Handle(HRealArray)&       yCoords,
                                         Handle(HRealArray)&       zCoords) const
{
  const double f = asiVisu_Utils::TrimInf(first),
               l = asiVisu_Utils::TrimInf(last);

  // Discretize curve.
  GeomAdaptor_Curve gac(curve, f, l);
  //
  std::vector<gp_Pnt> curvePts;
  asiVisu_GeomUtils::DiscretizeCurve(gac, 100, curvePts);

  const int nPts = int ( curvePts.size() );
  if ( !nPts )
    return false;

  // Allocate arrays.
  xCoords = new HRealArray(0, nPts - 1, 0.0),
  yCoords = new HRealArray(0, nPts - 1, 0.0),
  zCoords = new HRealArray(0, nPts - 1, 0.0);

  // Fill with coordinates.
  for ( int k = 0; k < nPts; ++k )
  {
    const gp_Pnt& P = curvePts[k];
    //
    xCoords->ChangeValue(k) = P.X();
    yCoords->ChangeValue(k) = P.Y();
    zCoords->ChangeValue(k) = P.Z();
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiVisu_CurveSourceBase::FillArrays(const Handle(Geom2d_Curve)& curve,
                                         const double                first,
                                         const double                last,
                                         Handle(HRealArray)&         xCoords,
                                         Handle(HRealArray)&         yCoords,
                                         Handle(HRealArray)&         zCoords) const
{
  const double f = asiVisu_Utils::TrimInf(first),
               l = asiVisu_Utils::TrimInf(last);

  // Discretize.
  Geom2dAdaptor_Curve gac(curve, f, l);
  GCPnts_QuasiUniformDeflection Defl(gac, 0.0001);
  if ( !Defl.IsDone() )
    return false;
  //
  const int nPts = Defl.NbPoints();
  if ( !nPts )
    return false;

  // Allocate arrays.
  xCoords = new HRealArray(0, nPts - 1, 0.0),
  yCoords = new HRealArray(0, nPts - 1, 0.0),
  zCoords = new HRealArray(0, nPts - 1, 0.0);

  for ( int k = 1; k <= nPts; ++k )
  {
    gp_Pnt P = Defl.Value(k);
    //
    xCoords->ChangeValue(k - 1) = P.X();
    yCoords->ChangeValue(k - 1) = P.Y();
    //
    // Z coordinate is kept zero as it was initialized.
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiVisu_CurveSourceBase::FillPolydata(vtkPolyData* polyData)
{
  /* =====================================================================
   *  Assuming the input arrays ordered, we build series of line segments
   *  representing our curve in a tabular form. Each segment is
   *  defined as the following pair:
   *
   *   seg = (x[i], y[i], z[i]), (x[i+1], y[i+1], z[i+1])
   *
   *  Take care of degenerated case: single point.
   * ===================================================================== */

  const int nbX = m_XCoords->Length(),
            nbY = m_YCoords->Length();

  if ( nbX == 0 || nbY == 0 )
    return false;

  if ( nbX == 1 && nbY == 1 )
  {
    this->registerVertex(0, polyData);
  }
  else
  {
    for ( int i = 0; i < nbX - 1; i++ )
      this->registerLine(i, polyData);
  }

  /* =====================
   *  Add orientation tip
   * ===================== */

  if ( m_fOriTipSize > RealEpsilon() )
  {
    int pinPointIdx = std::ceil( ( m_XCoords->Lower() + m_XCoords->Upper() )*0.5 );

    // Derive tangent.
    if ( m_oriT.Magnitude() < RealEpsilon() )
      m_oriT = gp_XYZ( m_XCoords->Value( pinPointIdx ),
                       m_YCoords->Value( pinPointIdx ),
                       m_ZCoords->Value( pinPointIdx ) )
             - gp_XYZ( m_XCoords->Value( pinPointIdx - 1 ),
                       m_YCoords->Value( pinPointIdx - 1 ),
                       m_ZCoords->Value( pinPointIdx - 1 ) );

    if ( m_oriT.Magnitude() > RealEpsilon() )
    {
      // Normalize tangent.
      m_oriT.Normalize();

      // Derive normal.
      if ( m_oriN.Magnitude() > RealEpsilon() )
        m_oriN.Normalize();
      else
      {
        bool isNComuted = false;
        if ( !m_curve3d.IsNull() )
        {
          const double midParam = ( m_curve3d->FirstParameter() + m_curve3d->LastParameter() )*0.5;

          GeomLProp_CLProps lProps( m_curve3d,
                                    midParam,
                                    2,
                                    gp::Resolution() );

          if ( Abs( lProps.Curvature() ) > RealEpsilon() )
          {
            gp_Dir norm;

            lProps.Normal(norm);
            m_oriN     = norm;
            isNComuted = true;
          }
        }

        // Last chance.
        if ( !isNComuted )
        {
          double Nx = 0., Ny = 0., Nz = 0.;

          if ( Abs( m_oriT.X() ) > RealEpsilon() )
            Nx = 1.0 / m_oriT.X();

          if ( Abs( m_oriT.Y() ) > RealEpsilon() )
            Ny = 1.0 / m_oriT.Y();

          if ( Abs( m_oriT.Z() ) > RealEpsilon() )
            Nz = 1.0 / m_oriT.Z();

          if ( Nx && Ny )
            m_oriN = gp_Dir(Nx, -Ny, 0.);
          else if ( Nx && Nz )
            m_oriN = gp_Dir(Nx, 0., -Nz);
          else if ( Ny && Nz )
            m_oriN = gp_Dir(0., Ny, -Nz);
          else if ( Nx && !Ny && !Nz )
            m_oriN = gp::DY();
          else if ( !Nx && Ny && !Nz )
            m_oriN = gp::DX();
          else if ( !Nx && !Ny && Nz )
            m_oriN = gp::DX();
          else
            m_oriN = gp::DZ();
        }
      }

      // Build tip.
      if ( m_oriT.Magnitude() > RealEpsilon() &&
           m_oriN.Magnitude() > RealEpsilon() )
      {
        gp_Pnt P1( m_XCoords->Value( pinPointIdx ),
                   m_YCoords->Value( pinPointIdx ),
                   m_ZCoords->Value( pinPointIdx ) );

        const double tip_angle = 3*M_PI/4;
        gp_Pnt P2( P1.XYZ() + m_oriT.XYZ()*m_fOriTipSize*Cos(tip_angle) + m_oriN.XYZ()*m_fOriTipSize*Sin(tip_angle) );
        gp_Pnt P3( P1.XYZ() + m_oriT.XYZ()*m_fOriTipSize*Cos(tip_angle) - m_oriN.XYZ()*m_fOriTipSize*Sin(tip_angle) );

        this->registerLine(P1, P2, polyData);
        this->registerLine(P1, P3, polyData);
      }
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

vtkIdType asiVisu_CurveSourceBase::registerGridPoint(const gp_Pnt& point,
                                                     vtkPolyData*  polyData) const
{
  // Access necessary arrays.
  vtkPoints* points = polyData->GetPoints();

  // Push the point into VTK data set.
  vtkIdType pid = points->InsertNextPoint( point.X(),
                                           point.Y(),
                                           point.Z() );

  return pid;
}

//-----------------------------------------------------------------------------

vtkIdType asiVisu_CurveSourceBase::registerGridPoint(const int    index,
                                                     vtkPolyData* polyData) const
{
  return this->registerGridPoint( gp_Pnt( m_XCoords->Value(index),
                                          m_YCoords->Value(index),
                                          m_ZCoords->Value(index) ),
                                  polyData );

}

//-----------------------------------------------------------------------------

vtkIdType asiVisu_CurveSourceBase::registerLine(const gp_Pnt& pointStart,
                                                const gp_Pnt& pointEnd,
                                                vtkPolyData*  polyData) const
{
  std::vector<vtkIdType> pids;
  pids.push_back( this->registerGridPoint(pointStart, polyData) );
  pids.push_back( this->registerGridPoint(pointEnd,   polyData) );

  vtkIdType cellID =
    polyData->InsertNextCell( VTK_LINE, (int) pids.size(), &pids[0] );

  // Set orientation scalar.
  if ( m_ori > VisuOri_Undefined )
  {
    vtkIntArray* typeArr =
      vtkIntArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_ORIENT_SCALARS) );
    //
    typeArr->InsertNextValue(m_ori);
  }

  // Assign a pedigree ID.
  vtkSmartPointer<vtkIdTypeArray>
    PIDsArray = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetPedigreeIds() );
  //
  if ( PIDsArray.GetPointer() )
    PIDsArray->InsertNextValue(m_iPedigreeId);

  return cellID;
}

//-----------------------------------------------------------------------------

vtkIdType asiVisu_CurveSourceBase::registerLine(const int    index,
                                                vtkPolyData* polyData) const
{
  gp_Pnt P1( m_XCoords->Value(index),
             m_YCoords->Value(index),
             m_ZCoords->Value(index) );
  gp_Pnt P2( m_XCoords->Value(index + 1),
             m_YCoords->Value(index + 1),
             m_ZCoords->Value(index + 1) );

  return this->registerLine(P1, P2, polyData);
}

//-----------------------------------------------------------------------------

vtkIdType asiVisu_CurveSourceBase::registerVertex(const int    index,
                                                  vtkPolyData* polyData) const
{
  std::vector<vtkIdType> pids;
  pids.push_back( this->registerGridPoint(index, polyData) );

  vtkIdType cellID =
    polyData->InsertNextCell( VTK_VERTEX, (int) pids.size(), &pids[0] );

  // Assign a pedigree ID
  vtkSmartPointer<vtkIdTypeArray>
    PIDsArray = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetPedigreeIds() );
  //
  PIDsArray->InsertNextValue(m_iPedigreeId);

  return cellID;
}
