//-----------------------------------------------------------------------------
// Created on: 16 March 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoEngine_BuildOBBFunc.h>

// asiAlgo includes
#include <asiAlgo_MeshOBB.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! \return Tree Function instance.
Handle(ottoEngine_BuildOBBFunc) ottoEngine_BuildOBBFunc::Instance()
{
  return new ottoEngine_BuildOBBFunc();
}

//-----------------------------------------------------------------------------

//! \return GUID.
const char* ottoEngine_BuildOBBFunc::GUID()
{
  return "A14A119B-5817-4158-A020-5B966CB51B12";
}

//-----------------------------------------------------------------------------

//! \return GUID.
const char* ottoEngine_BuildOBBFunc::GetGUID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------

//! \return human-readable name of Tree Function.
const char* ottoEngine_BuildOBBFunc::GetName() const
{
  return "Build OBB";
}

//-----------------------------------------------------------------------------

//! Executes Tree Function.
//! \param[in] argsIn   INPUT Parameters.
//! \param[in] argsOut  OUTPUT Parameters
//! \param[in] userData user data.
//! \param[in] PEntry   progress entry.
//! \return execution status.
int ottoEngine_BuildOBBFunc::execute(const Handle(ActAPI_HParameterList)& argsIn,
                                     const Handle(ActAPI_HParameterList)& argsOut,
                                     const Handle(Standard_Transient)&    ActData_NotUsed(userData),
                                     ActAPI_ProgressEntry                 PEntry) const
{
  /* ============================
   *  Interpret INPUT Parameters
   * ============================ */

  Handle(Poly_Triangulation)
    triangulation = ActParamTool::AsTriangulation( argsIn->Value(1) )->GetTriangulation();

  /* =============================
   *  Interpret OUTPUT Parameters
   * ============================= */

  Handle(ActData_ShapeParameter)
    outParamShape = ActParamTool::AsShape( argsOut->Value(1) );

  Handle(ActData_RealArrayParameter)
    outParamPropsArr = ActParamTool::AsRealArray( argsOut->Value(2) );

  /* ===========
   *  Build OBB
   * =========== */

  asiAlgo_MeshOBB buildOBB(triangulation, PEntry);
  //
  if ( !buildOBB.Perform() )
  {
    PEntry.SendLogMessage(LogErr(Normal) << "Cannot build OBB on mesh.");
    return 1;
  }

  // Get OBB as a B-Rep shape.
  TopoDS_Shape obbShape = buildOBB.GetResultBox();

  // Get OBB analytically.
  const asiAlgo_OBB& obb = buildOBB.GetResult();
  //
  const gp_Pnt& pos = obb.Placement.Location();
  const gp_Dir& OZ  = obb.Placement.Direction();
  const gp_Dir& OX  = obb.Placement.XDirection();
  //
  const gp_Pnt& MinCorner = obb.LocalCornerMin;
  const gp_Pnt& MaxCorner = obb.LocalCornerMax;

  /* ==================================
   *  Set results to OUTPUT Parameters
   * ================================== */

  // Set output shape.
  outParamShape->SetShape(obbShape, MT_Impacted);

  // Set output array with properties.
  Handle(HRealArray) obbProps = new HRealArray(0, 14);
  //
  obbProps->ChangeValue(0)  = pos.X();
  obbProps->ChangeValue(1)  = pos.Y();
  obbProps->ChangeValue(2)  = pos.Z();
  obbProps->ChangeValue(3)  = OZ.X();
  obbProps->ChangeValue(4)  = OZ.Y();
  obbProps->ChangeValue(5)  = OZ.Z();
  obbProps->ChangeValue(6)  = OX.X();
  obbProps->ChangeValue(7)  = OX.Y();
  obbProps->ChangeValue(8)  = OX.Z();
  obbProps->ChangeValue(9)  = MinCorner.X();
  obbProps->ChangeValue(10) = MinCorner.Y();
  obbProps->ChangeValue(11) = MinCorner.Z();
  obbProps->ChangeValue(12) = MaxCorner.X();
  obbProps->ChangeValue(13) = MaxCorner.Y();
  obbProps->ChangeValue(14) = MaxCorner.Z();
  //
  outParamPropsArr->SetArray(obbProps);

  // Put logging message.
  PEntry.SendLogMessage(LogInfo(Normal) << "OBB was constructed.");

  return 0; // OK
}

//-----------------------------------------------------------------------------

//! Returns accepted INPUT signature for VALIDATION.
//! \return expected INPUT signature.
ActAPI_ParameterTypeStream ottoEngine_BuildOBBFunc::inputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Triangulation;
}

//-----------------------------------------------------------------------------

//! Returns accepted OUTPUT signature for VALIDATION.
//! \return expected OUTPUT signature.
ActAPI_ParameterTypeStream ottoEngine_BuildOBBFunc::outputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Shape
                                      << Parameter_RealArray;
}
