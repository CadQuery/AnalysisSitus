//-----------------------------------------------------------------------------
// Created on: 16 March 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoEngine_BuildSectionsFunc.h>

// asiAlgo includes
#include <asiAlgo_MeshInterPlane.h>
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS_Edge.hxx>

//-----------------------------------------------------------------------------

//! \return Tree Function instance.
Handle(ottoEngine_BuildSectionsFunc) ottoEngine_BuildSectionsFunc::Instance()
{
  return new ottoEngine_BuildSectionsFunc();
}

//-----------------------------------------------------------------------------

//! \return GUID.
const char* ottoEngine_BuildSectionsFunc::GUID()
{
  return "B14A119B-5817-4158-A020-5B966CB51B12";
}

//-----------------------------------------------------------------------------

//! \return GUID.
const char* ottoEngine_BuildSectionsFunc::GetGUID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------

//! \return human-readable name of Tree Function.
const char* ottoEngine_BuildSectionsFunc::GetName() const
{
  return "Build sections";
}

//-----------------------------------------------------------------------------

//! Executes Tree Function.
//! \param[in] argsIn   INPUT Parameters.
//! \param[in] argsOut  OUTPUT Parameters
//! \param[in] userData user data.
//! \param[in] PEntry   progress entry.
//! \return execution status.
int ottoEngine_BuildSectionsFunc::execute(const Handle(ActAPI_HParameterList)& argsIn,
                                          const Handle(ActAPI_HParameterList)& argsOut,
                                          const Handle(Standard_Transient)&    ActData_NotUsed(userData),
                                          ActAPI_ProgressEntry                 PEntry) const
{
  /* ============================
   *  Interpret INPUT Parameters
   * ============================ */

  Handle(Poly_Triangulation)
    triangulation = ActParamTool::AsTriangulation( argsIn->Value(1) )->GetTriangulation();

  Handle(HRealArray)
    obbProps = ActParamTool::AsRealArray( argsIn->Value(2) )->GetArray();

  const int
    numSections = ActParamTool::AsInt( argsIn->Value(3) )->GetValue();

  /* =============================
   *  Interpret OUTPUT Parameters
   * ============================= */

  Handle(ActData_ShapeParameter)
    outParam = ActParamTool::AsShape( argsOut->Value(1) );

  /* ================
   *  Build sections
   * ================ */

  // Restore section plane.
  const double pos_X = obbProps->Value(0);
  const double pos_Y = obbProps->Value(1);
  const double pos_Z = obbProps->Value(2);
  const double OZ_X  = obbProps->Value(3);
  const double OZ_Y  = obbProps->Value(4);
  const double OZ_Z  = obbProps->Value(5);
  const double OX_X  = obbProps->Value(6);
  const double OX_Y  = obbProps->Value(7);
  const double OX_Z  = obbProps->Value(8);
  const double min_X = obbProps->ChangeValue(9);
  const double min_Y = obbProps->ChangeValue(10);
  const double min_Z = obbProps->ChangeValue(11);
  const double max_X = obbProps->ChangeValue(12);
  const double max_Y = obbProps->ChangeValue(13);
  const double max_Z = obbProps->ChangeValue(14);
  //
  gp_Ax3 placement( gp_Pnt(pos_X, pos_Y, pos_Z), gp_Dir(OZ_X, OZ_Y, OZ_Z), gp_Dir(OX_X, OX_Y, OX_Z) );
  //
  gp_Trsf T;
  T.SetTransformation(placement);
  T.Invert();
  //
  gp_XYZ Pmin(min_X, min_Y, min_Z);
  gp_XYZ Pmax(max_X, max_Y, max_Z);
  gp_Pnt Pstart;
  //
  const double deltaX = Abs(max_X - min_X);
  const double deltaY = Abs(max_Y - min_Y);
  const double deltaZ = Abs(max_Z - min_Z);
  //
  gp_Vec DU, DV;
  double height = 0.0;
  //
  if ( deltaX < deltaY && deltaX < deltaZ ) // DX is the min
  {
    DU = gp_XYZ(deltaX, 0., 0.);
    //
    if ( deltaY < deltaZ )
    {
      DV = gp_XYZ(0., deltaY, 0.);
      height = deltaZ;
    }
    else
    {
      DV = gp_XYZ(0., 0., deltaZ);
      height = deltaY;
    }
  }
  if ( deltaY < deltaX && deltaY < deltaZ ) // DY is the min
  {
    DU = gp_XYZ(0., deltaY, 0.);
    //
    if ( deltaX < deltaZ )
    {
      DV = gp_XYZ(deltaX, 0., 0.);
      height = deltaZ;
    }
    else
    {
      DV = gp_XYZ(0., 0., deltaZ);
      height = deltaX;
    }
  }
  if ( deltaZ < deltaX && deltaZ < deltaY ) // DZ is the min
  {
    DU = gp_XYZ(0., 0., deltaZ);
    //
    if ( deltaX < deltaY )
    {
      DV = gp_XYZ(deltaX, 0., 0.);
      height = deltaY;
    }
    else
    {
      DV = gp_XYZ(0., deltaY, 0.);
      height = deltaX;
    }
  }
  //
  gp_Dir DN = DU^DV;
  //
  Pstart = DU.XYZ()*0.5 + DV.XYZ()*0.5 - DN.XYZ()*height*0.45;

  const int numPlanes = numSections;
  const double hstep = height / numPlanes;

  TopoDS_Compound compEdges;
  BRep_Builder().MakeCompound(compEdges);

  for ( int i = 0; i < numPlanes; ++i )
  {
    gp_Pnt transformedPstart = Pstart;
    gp_Vec transformedDU = DU;
    gp_Vec transformedDV = DV;
    gp_Vec transformedDN = DN;

    transformedPstart = transformedPstart.XYZ() + i*hstep*DN.XYZ();
    //
    transformedDN.Transform(T);
    transformedDU.Transform(T);
    transformedDV.Transform(T);
    transformedPstart.Transform(T);

    // Create section plane
    gp_Ax3 ax3( transformedPstart, transformedDN, transformedDU );
    //
    Handle(Geom_Plane) sectionPlane = new Geom_Plane(ax3);

    // Intersect with plane.
    asiAlgo_MeshInterPlane algo(triangulation, PEntry, NULL);
    //
    if ( !algo.Perform(sectionPlane, true) )
    {
      PEntry.SendLogMessage(LogErr(Normal) << "Failed to cut mesh with plane.");
      return 1;
    }

    const Handle(asiAlgo_BaseCloud<double>)& ptsCloud = algo.GetResult();

    std::vector<gp_XYZ> pts;
    for ( int k = 0; k < ptsCloud->GetNumberOfElements(); ++k )
      pts.push_back( ptsCloud->GetElement(k) );
    //
    pts.push_back(pts[0]);

    // Run approximation algorithm.
    Handle(Geom_BSplineCurve) resCurve;
    if ( !asiAlgo_Utils::ApproximatePoints(pts, 3, 3, 1., resCurve) )
    {
      PEntry.SendLogMessage(LogErr(Normal) << "Approximation failed.");
      return 1;
    }

    // Make edge.
    TopoDS_Edge resEdge = BRepBuilderAPI_MakeEdge(resCurve);

    // Make wire.
    TopoDS_Wire resWire = BRepBuilderAPI_MakeWire(resEdge);

    BRep_Builder().Add(compEdges, resWire);
  }

  /* ==================================
   *  Set results to OUTPUT Parameters
   * ================================== */

  // Set output
  outParam->SetShape(compEdges, MT_Impacted);

  // Put logging message
  PEntry.SendLogMessage(LogInfo(Normal) << "Sections were built.");

  return 0; // OK
}

//-----------------------------------------------------------------------------

//! Returns accepted INPUT signature for VALIDATION.
//! \return expected INPUT signature.
ActAPI_ParameterTypeStream ottoEngine_BuildSectionsFunc::inputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Triangulation
                                      << Parameter_RealArray
                                      << Parameter_Int;
}

//-----------------------------------------------------------------------------

//! Returns accepted OUTPUT signature for VALIDATION.
//! \return expected OUTPUT signature.
ActAPI_ParameterTypeStream ottoEngine_BuildSectionsFunc::outputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Shape;
}
