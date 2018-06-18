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

// Own include
#include <asiVisu_CurveSource.h>

// VTK includes
#include <vtkCellData.h>
#include <vtkDataObject.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax1.hxx>

//-----------------------------------------------------------------------------

static void PlotCurve(const Adaptor3d_Curve& C,
                      double&                f,
                      const double           h,
                      const gp_Pnt&          Pf,
                      const gp_Pnt&          Pl,
                      std::vector<gp_Pnt>&   result)
{
  double IsoRatio = 1.001;
  gp_Pnt Pm = C.Value(f + h);

  double dfLength = Pf.Distance(Pl);

  if ( dfLength < Precision::Confusion() || 
       Pm.Distance(Pf) + Pm.Distance(Pl) <= IsoRatio*dfLength )
  {
    result.push_back(Pl);
  }
  else
  {
    PlotCurve(C, f, h/2., Pf, Pm, result);
    double t = f + h;
    PlotCurve(C, t, h/2., Pm, Pl, result);
  }
}

//-----------------------------------------------------------------------------

static void PlotCurve(const Adaptor3d_Curve& C,
                      const int              discrete,
                      std::vector<gp_Pnt>&   result)
{
  const int numIntervals = C.NbIntervals(GeomAbs_CN);
  TColStd_Array1OfReal TI(1, numIntervals + 1);
  C.Intervals(TI, GeomAbs_CN);

  gp_Pnt P = C.Value( C.FirstParameter() );
  gp_Pnt firstP = P, nextP;

  // Add first point.
  result.push_back(P);

  // Process intervals of constant continuity.
  for ( int intrv = 1; intrv <= numIntervals; ++intrv )
  {
    double       t     =  TI(intrv);
    const double step  = (TI(intrv + 1) - t) / discrete;
    const int    nIter =  discrete / 2;

    for ( int j = 1; j < nIter; ++j )
    {
      const double t1 = t + step*2.;
      nextP = C.Value(t1);
      PlotCurve(C, t, step, firstP, nextP, result);
      firstP = nextP;
      t = t1;
    }
  }

  // Add last point.
  result.push_back( C.Value( C.LastParameter() ) );
}

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_CurveSource);

//! Default constructor.
asiVisu_CurveSource::asiVisu_CurveSource()
  : vtkPolyDataAlgorithm (),
    m_fOriTipSize        (0.0),
    m_iPedigreeId        (0)
{
  this->SetNumberOfInputPorts(0); // Connected directly to our own Data Provider
                                  // which has nothing to do with VTK pipeline
}

//! Destructor.
asiVisu_CurveSource::~asiVisu_CurveSource()
{}

//-----------------------------------------------------------------------------
// Kernel methods
//-----------------------------------------------------------------------------

//! Initialize data source from a topological edge.
//! \param edge [in] edge to extract the spatial curve from.
//! \return true if poly data has been produced.
bool asiVisu_CurveSource::SetInputEdge(const TopoDS_Edge& edge)
{
  if ( BRep_Tool::Degenerated(edge) )
    return false;

  double f, l;
  Handle(Geom_Curve) crv = BRep_Tool::Curve(edge, f, l);
  //
  if ( crv.IsNull() )
    return false;

  // Build a trimmed curve in order to put trimming parameters of a curve
  // right into it
  Handle(Geom_TrimmedCurve) tcrv = new Geom_TrimmedCurve(crv, f, l);

  // Resolve orientation
  asiVisu_Orientation ori = VisuOri_Undefined;
  switch ( edge.Orientation() )
  {
    case TopAbs_FORWARD  : ori = VisuOri_Forward;  break;
    case TopAbs_REVERSED : ori = VisuOri_Reversed; break;
    case TopAbs_INTERNAL : ori = VisuOri_Internal; break;
    case TopAbs_EXTERNAL : ori = VisuOri_External; break;
    default: break;
  }

  // Initialize with a curve
  return this->SetInputCurve(tcrv, f, l, ori);
}

//! Initialize data source from a parametric curve.
//! \param curve [in] curve to discretize.
//! \param first [in] first parameter.
//! \param last  [in] last parameter.
//! \param ori   [in] orientation.
//! \return true if poly data has been produced.
bool asiVisu_CurveSource::SetInputCurve(const Handle(Geom_Curve)& curve,
                                        const double              first,
                                        const double              last,
                                        const asiVisu_Orientation ori)
{
  const double f = asiVisu_Utils::TrimInf(first),
               l = asiVisu_Utils::TrimInf(last);

  // Discretize
  GeomAdaptor_Curve gac(curve, f, l);
  //
  std::vector<gp_Pnt> curvePts;
  PlotCurve(gac, 100, curvePts);

  const int nPts = int ( curvePts.size() );
  if ( !nPts )
    return false;

  // Allocate arrays
  Handle(HRealArray) xCoords = new HRealArray(0, nPts - 1, 0.0),
                     yCoords = new HRealArray(0, nPts - 1, 0.0),
                     zCoords = new HRealArray(0, nPts - 1, 0.0);

  for ( int k = 0; k < nPts; ++k )
  {
    const gp_Pnt& P = curvePts[k];
    //
    xCoords->ChangeValue(k) = P.X();
    yCoords->ChangeValue(k) = P.Y();
    zCoords->ChangeValue(k) = P.Z();
  }

  // Pass arrays as inputs
  this->SetInputArrays(xCoords, yCoords, zCoords, ori);
  return true;
}

//! Initialize data source from a two-dimensional parametric curve.
//! \param curve [in] curve to discretize.
//! \param first [in] first parameter.
//! \param last  [in] last parameter.
//! \param ori   [in] orientation.
//! \return true if poly data has been produced.
bool asiVisu_CurveSource::SetInputCurve2d(const Handle(Geom2d_Curve)& curve,
                                          const double                first,
                                          const double                last,
                                          const asiVisu_Orientation    ori)
{
  const double f = asiVisu_Utils::TrimInf(first),
               l = asiVisu_Utils::TrimInf(last);

  // Discretize
  Geom2dAdaptor_Curve gac(curve, f, l);
  GCPnts_QuasiUniformDeflection Defl(gac, 0.0001);
  if ( !Defl.IsDone() )
  {
    vtkErrorMacro( << "Cannot discretize curve" );
    return false;
  }
  const int nPts = Defl.NbPoints();
  if ( !nPts )
    return false;

  // Allocate arrays
  Handle(HRealArray) xCoords = new HRealArray(0, nPts - 1, 0.0),
                     yCoords = new HRealArray(0, nPts - 1, 0.0),
                     zCoords = new HRealArray(0, nPts - 1, 0.0);

  for ( int k = 1; k <= nPts; ++k )
  {
    gp_Pnt P = Defl.Value(k);
    //
    xCoords->ChangeValue(k - 1) = P.X();
    yCoords->ChangeValue(k - 1) = P.Y();
    //
    // Z coordinate is kept zero as it was initialized
  }

  // Pass arrays as inputs
  this->SetInputArrays(xCoords, yCoords, zCoords, ori);
  return true;
}

//! Sets the Data Source input arrays. These arrays represent a curve
//! explicitly as series of points. Of course, to have these points you need
//! to sample your curve beforehand.
//!
//! \param xCoords [in] X coordinates.
//! \param yCoords [in] Y coordinates.
//! \param zCoords [in] Z coordinates.
//! \param ori     [in] orientation.
void asiVisu_CurveSource::SetInputArrays(const Handle(HRealArray)& xCoords,
                                         const Handle(HRealArray)& yCoords,
                                         const Handle(HRealArray)& zCoords,
                                         const asiVisu_Orientation ori)
{
  if ( xCoords->Length() != zCoords->Length() )
  {
    vtkErrorMacro( << "Invalid domain: faulty number of X coordinates" );
    return;
  }

  if ( yCoords->Length() != zCoords->Length() )
  {
    vtkErrorMacro( << "Invalid domain: faulty number of Y coordinates" );
  }

  m_XCoords = xCoords;
  m_YCoords = yCoords;
  m_ZCoords = zCoords;
  m_ori     = ori;
}

//! Returns input arrays.
//! \param xCoords [out] x coordinates.
//! \param yCoords [out] y coordinates.
//! \param zCoords [out] z coordinates.
//! \param ori     [out] orientation.
void asiVisu_CurveSource::GetInputArrays(Handle(HRealArray)&  xCoords,
                                         Handle(HRealArray)&  yCoords,
                                         Handle(HRealArray)&  zCoords,
                                         asiVisu_Orientation& ori) const
{
  xCoords = m_XCoords;
  yCoords = m_YCoords;
  zCoords = m_ZCoords;
  ori     = m_ori;
}

//! This method (called by superclass) performs conversion of OCCT
//! data structures to VTK polygonal representation.
//!
//! \param request      [in]  describes "what" algorithm should do. This is
//!                           typically just one key such as REQUEST_INFORMATION.
//! \param inputVector  [in]  inputs of the algorithm.
//! \param outputVector [out] outputs of the algorithm.
int asiVisu_CurveSource::RequestData(vtkInformation*        request,
                                     vtkInformationVector** inputVector,
                                     vtkInformationVector*  outputVector)
{
  if ( m_XCoords.IsNull() || m_YCoords.IsNull() || m_ZCoords.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: NULL coordinate arrays" );
    return 0;
  }

  /* ==============================
   *  Prepare involved collections
   * ============================== */

  vtkPolyData* aPolyOutput = vtkPolyData::GetData(outputVector);
  aPolyOutput->Allocate();

  vtkSmartPointer<vtkPoints> aPoints = vtkSmartPointer<vtkPoints>::New();
  aPolyOutput->SetPoints(aPoints);

  // Access cell data
  vtkCellData* aCD = aPolyOutput->GetCellData();

  // Array for orientation types
  vtkSmartPointer<vtkIntArray> aTypeArr = asiVisu_Utils::InitIntArray(ARRNAME_ORIENT_SCALARS);
  aCD->AddArray(aTypeArr);

  // Pedigree IDs for selection
  vtkSmartPointer<vtkIdTypeArray> PIDsArray = vtkSmartPointer<vtkIdTypeArray>::New();
  PIDsArray->SetNumberOfComponents(1);
  aCD->SetPedigreeIds(PIDsArray);

  /* =====================================================================
   *  Assuming the input arrays ordered, we build series of line segments
   *  representing our curve in a tabular form. Each segment is
   *  defined as the following pair:
   *
   *   seg = (x[i], y[i], z[i]), (x[i+1], y[i+1], z[i+1])
   *
   *  Take care of degenerated case: single point
   * ===================================================================== */

  const int nbX = m_XCoords->Length(),
            nbY = m_YCoords->Length();

  if ( nbX == 0 || nbY == 0 )
    return 0;

  if ( nbX == 1 && nbY == 1 )
  {
    this->registerVertex(0, aPolyOutput);
  }
  else
  {
    for ( int i = 0; i < nbX - 1; i++ )
      this->registerLine(i, aPolyOutput);
  }

  /* =====================
   *  Add orientation tip
   * ===================== */

  if ( m_fOriTipSize > RealEpsilon() )
  {
    // Derive tangent
    if ( m_oriT.Magnitude() < RealEpsilon() )
      m_oriT = gp_XYZ( m_XCoords->Value( m_XCoords->Upper() ),
                       m_YCoords->Value( m_YCoords->Upper() ),
                       m_ZCoords->Value( m_ZCoords->Upper() ) )
             - gp_XYZ( m_XCoords->Value( m_XCoords->Upper() - 1 ),
                       m_YCoords->Value( m_YCoords->Upper() - 1 ),
                       m_ZCoords->Value( m_ZCoords->Upper() - 1 ) );

    if ( m_oriT.Magnitude() > RealEpsilon() )
    {
      // Normalize tangent
      m_oriT.Normalize();

      // Derive normal
      if ( m_oriN.Magnitude() > RealEpsilon() )
        m_oriN.Normalize();
      else
        m_oriN = m_oriT.Rotated( gp_Ax1( gp::Origin(), gp::DZ() ), M_PI/2); // Lets take any orthogonal vector

      // Build tip
      if ( m_oriT.Magnitude() > RealEpsilon() &&
           m_oriN.Magnitude() > RealEpsilon() )
      {
        gp_Pnt P1( m_XCoords->Value( m_XCoords->Upper() ),
                   m_YCoords->Value( m_YCoords->Upper() ),
                   m_ZCoords->Value( m_ZCoords->Upper() ) );

        const double tip_angle = 3*M_PI/4;
        gp_Pnt P2( P1.XYZ() + m_oriT.XYZ()*m_fOriTipSize*Cos(tip_angle) + m_oriN.XYZ()*m_fOriTipSize*Sin(tip_angle) );
        gp_Pnt P3( P1.XYZ() + m_oriT.XYZ()*m_fOriTipSize*Cos(tip_angle) - m_oriN.XYZ()*m_fOriTipSize*Sin(tip_angle) );

        this->registerLine(P1, P2, aPolyOutput);
        this->registerLine(P1, P3, aPolyOutput);
      }
    }
  }

  return Superclass::RequestData(request, inputVector, outputVector);
}

//! Adds the given point to the passed polygonal data set.
//! \param point    [in]     point to add.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType asiVisu_CurveSource::registerGridPoint(const gp_Pnt& point,
                                                 vtkPolyData*  polyData)
{
  // Access necessary arrays
  vtkPoints* aPoints = polyData->GetPoints();

  // Push the point into VTK data set
  vtkIdType aPid = aPoints->InsertNextPoint( point.X(),
                                             point.Y(),
                                             point.Z() );

  return aPid;
}

//! Adds the grid point with the given index to the passed polygonal data set.
//! \param index    [in]     index of (X, Y, Z) coordinate triple.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK point.
vtkIdType asiVisu_CurveSource::registerGridPoint(const int    index,
                                                 vtkPolyData* polyData)
{
  return this->registerGridPoint( gp_Pnt( m_XCoords->Value(index),
                                          m_YCoords->Value(index),
                                          m_ZCoords->Value(index) ),
                                  polyData );

}

//! Adds a line cell into the polygonal data set.
//! \param pointStart [in]     first point.
//! \param pointEnd   [in]     second point.
//! \param polyData   [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_CurveSource::registerLine(const gp_Pnt& pointStart,
                                            const gp_Pnt& pointEnd,
                                            vtkPolyData*  polyData)
{
  std::vector<vtkIdType> aPids;
  aPids.push_back( this->registerGridPoint(pointStart, polyData) );
  aPids.push_back( this->registerGridPoint(pointEnd,   polyData) );

  vtkIdType aCellID =
    polyData->InsertNextCell( VTK_LINE, (int) aPids.size(), &aPids[0] );

  // Set orientation scalar
  if ( m_ori > VisuOri_Undefined )
  {
    vtkIntArray* aTypeArr =
      vtkIntArray::SafeDownCast( polyData->GetCellData()->GetArray(ARRNAME_ORIENT_SCALARS) );
    aTypeArr->InsertNextValue(m_ori);
  }

  // Assign a pedigree ID
  vtkSmartPointer<vtkIdTypeArray>
    PIDsArray = vtkIdTypeArray::SafeDownCast( polyData->GetCellData()->GetPedigreeIds() );
  //
  PIDsArray->InsertNextValue(m_iPedigreeId);

  return aCellID;
}

//! Adds a line cell into the polygonal data set.
//! \param index    [in]     index of (X, Y, Z) coordinate triple.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_CurveSource::registerLine(const int    index,
                                            vtkPolyData* polyData)
{
  gp_Pnt P1( m_XCoords->Value(index),
             m_YCoords->Value(index),
             m_ZCoords->Value(index) );
  gp_Pnt P2( m_XCoords->Value(index + 1),
             m_YCoords->Value(index + 1),
             m_ZCoords->Value(index + 1) );

  return this->registerLine(P1, P2, polyData);
}

//! Adds a vertex cell into the polygonal data set.
//! \param index    [in]     index of (X, Y, Z) coordinate triple.
//! \param polyData [in/out] polygonal data set being populated.
//! \return ID of the just added VTK cell.
vtkIdType asiVisu_CurveSource::registerVertex(const int    index,
                                              vtkPolyData* polyData)
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
