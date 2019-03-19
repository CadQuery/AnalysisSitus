//-----------------------------------------------------------------------------
// Created on: 15 March 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <ottoEngine_LoadMeshFunc.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! \return Tree Function instance.
Handle(ottoEngine_LoadMeshFunc) ottoEngine_LoadMeshFunc::Instance()
{
  return new ottoEngine_LoadMeshFunc();
}

//-----------------------------------------------------------------------------

//! \return GUID.
const char* ottoEngine_LoadMeshFunc::GUID()
{
  return "A14A119B-5817-4158-A020-5B966CB51A12";
}

//-----------------------------------------------------------------------------

//! \return GUID.
const char* ottoEngine_LoadMeshFunc::GetGUID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------

//! \return human-readable name of Tree Function.
const char* ottoEngine_LoadMeshFunc::GetName() const
{
  return "Load STL";
}

//-----------------------------------------------------------------------------

//! Executes Tree Function.
//! \param[in] argsIn   INPUT Parameters.
//! \param[in] argsOut  OUTPUT Parameters
//! \param[in] userData user data.
//! \param[in] PEntry   progress entry.
//! \return execution status.
int ottoEngine_LoadMeshFunc::execute(const Handle(ActAPI_HParameterList)& argsIn,
                                     const Handle(ActAPI_HParameterList)& argsOut,
                                     const Handle(Standard_Transient)&    ActData_NotUsed(userData),
                                     ActAPI_ProgressEntry                 PEntry) const
{
  /* ============================
   *  Interpret INPUT Parameters
   * ============================ */

  TCollection_AsciiString
    filename = ActParamTool::AsAsciiString( argsIn->Value(1) )->GetValue();

  /* =============================
   *  Interpret OUTPUT Parameters
   * ============================= */

  Handle(ActData_TriangulationParameter)
    outParam = ActParamTool::AsTriangulation( argsOut->Value(1) );

  /* ====================
   *  Load triangulation
   * ==================== */

  Handle(Poly_Triangulation) triangulation;
  //
  if ( !asiAlgo_Utils::ReadStl(filename, triangulation, PEntry) )
  {
    PEntry.SendLogMessage(LogErr(Normal) << "Cannot load mesh from file.");
    return 1;
  }

  /* ==================================
   *  Set results to OUTPUT Parameters
   * ================================== */

  // Set output
  outParam->SetTriangulation(triangulation, MT_Impacted);

  // Put logging message
  PEntry.SendLogMessage(LogInfo(Normal) << "STL file was loaded.");

  return 0; // OK
}

//-----------------------------------------------------------------------------

//! Returns accepted INPUT signature for VALIDATION.
//! \return expected INPUT signature.
ActAPI_ParameterTypeStream ottoEngine_LoadMeshFunc::inputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_AsciiString;
}

//-----------------------------------------------------------------------------

//! Returns accepted OUTPUT signature for VALIDATION.
//! \return expected OUTPUT signature.
ActAPI_ParameterTypeStream ottoEngine_LoadMeshFunc::outputSignature() const
{
  return ActAPI_ParameterTypeStream() << Parameter_Triangulation;
}
