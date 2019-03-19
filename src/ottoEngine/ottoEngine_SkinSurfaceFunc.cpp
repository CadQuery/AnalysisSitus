//-----------------------------------------------------------------------------
// Created on: 19 March 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoEngine_SkinSurfaceFunc.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

// OCCT includes
#include <BRepOffsetAPI_ThruSections.hxx>

//-----------------------------------------------------------------------------

//! \return Tree Function instance.
Handle(ottoEngine_SkinSurfaceFunc) ottoEngine_SkinSurfaceFunc::Instance()
{
  return new ottoEngine_SkinSurfaceFunc();
}

//-----------------------------------------------------------------------------

//! \return GUID.
const char* ottoEngine_SkinSurfaceFunc::GUID()
{
  return "B14A119B-5817-4158-A021-5B966CB51B12";
}

//-----------------------------------------------------------------------------

//! \return GUID.
const char* ottoEngine_SkinSurfaceFunc::GetGUID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------

//! \return human-readable name of Tree Function.
const char* ottoEngine_SkinSurfaceFunc::GetName() const
{
  return "Skin surface";
}

//-----------------------------------------------------------------------------

//! Executes Tree Function.
//! \param[in] argsIn   INPUT Parameters.
//! \param[in] argsOut  OUTPUT Parameters
//! \param[in] userData user data.
//! \param[in] PEntry   progress entry.
//! \return execution status.
int ottoEngine_SkinSurfaceFunc::execute(const Handle(ActAPI_HParameterList)& argsIn,
                                        const Handle(ActAPI_HParameterList)& argsOut,
                                        const Handle(Standard_Transient)&    ActData_NotUsed(userData),
                                        ActAPI_ProgressEntry                 PEntry) const
{
  /* ============================
   *  Interpret INPUT Parameters
   * ============================ */

  TopoDS_Shape
    sectionsShape = ActParamTool::AsShape( argsIn->Value(1) )->GetShape();

  /* =============================
   *  Interpret OUTPUT Parameters
   * ============================= */

  Handle(ActData_ShapeParameter)
    outParam = ActParamTool::AsShape( argsOut->Value(1) );

  /* ===============
   *  Build surface
   * =============== */

  BRepOffsetAPI_ThruSections thruSections;

  for ( TopoDS_Iterator it(sectionsShape); it.More(); it.Next() )
  {
    const TopoDS_Wire& W = TopoDS::Wire( it.Value() );

    thruSections.AddWire(W);
  }

  thruSections.Build();
  //
  if ( !thruSections.IsDone() )
  {
    PEntry.SendLogMessage(LogErr(Normal) << "Cannot skin surface.");
    return 1;
  }

  const TopoDS_Shape& result = thruSections.Shape();

  /* ==================================
   *  Set results to OUTPUT Parameters
   * ================================== */

  // Set output
  outParam->SetShape(result, MT_Impacted);

  // Put logging message
  PEntry.SendLogMessage(LogInfo(Normal) << "Surface was skinned.");

  return 0; // OK
}

//-----------------------------------------------------------------------------

//! Returns accepted INPUT signature for VALIDATION.
//! \return expected INPUT signature.
ActAPI_ParameterTypeStream ottoEngine_SkinSurfaceFunc::inputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Shape;
}

//-----------------------------------------------------------------------------

//! Returns accepted OUTPUT signature for VALIDATION.
//! \return expected OUTPUT signature.
ActAPI_ParameterTypeStream ottoEngine_SkinSurfaceFunc::outputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Shape;
}
