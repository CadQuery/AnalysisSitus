//-----------------------------------------------------------------------------
// Created on: 02 December 2015
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_PCurveSource.h>

// A-Situs (visualization) includes
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkObjectFactory.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom2dAdaptor_Curve.hxx>

//-----------------------------------------------------------------------------
// Construction
//-----------------------------------------------------------------------------

vtkStandardNewMacro(asiVisu_PCurveSource);

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
    vtkErrorMacro( << "Invalid domain: edge is NULL" );
    return;
  }

  if ( face.IsNull() )
  {
    vtkErrorMacro( << "Invalid domain: face is NULL" );
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

  // Discretize
  Geom2dAdaptor_Curve gac(c2d, f, l);
  GCPnts_QuasiUniformDeflection Defl(gac, 0.0001);
  if ( !Defl.IsDone() )
  {
    vtkErrorMacro( << "Cannot discretize p-curve" );
    return;
  }
  const int nPts = Defl.NbPoints();
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
    gp_Pnt P = Defl.Value(k);
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
