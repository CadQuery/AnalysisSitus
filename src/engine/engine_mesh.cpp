//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <engine_mesh.h>

// A-Situs (common) includes
#include <common_facilities.h>

// OCCT includes
#include <TDataStd_RealArray.hxx>

//! Saves the passed mesh parameters to OCAF.
//! \param params [in] mesh parameters.
void engine_mesh::SaveParameters(const tess_params& params)
{
  TDF_Label root = common_facilities::Instance()->Model_XDE->GetPropertiesLabel(xde_model::Property_Mesh);

  // Store mesh properties
  Handle(TDataStd_RealArray) arrAttr = TDataStd_RealArray::Set(root, 0, 1);
  arrAttr->SetValue(0, params.Deflection.Linear);
  arrAttr->SetValue(1, params.Deflection.Angular);
}

//! Extracts mesh parameters from OCAF.
//! \param params [out] mesh parameters.
void engine_mesh::GetParameters(tess_params& params)
{
  TDF_Label root = common_facilities::Instance()->Model_XDE->GetPropertiesLabel(xde_model::Property_Mesh);

  // Find attribute
  Handle(TDataStd_RealArray) arrAttr;
  if ( !root.FindAttribute(TDataStd_RealArray::GetID(), arrAttr) )
  {
    // Set default values
    params.Deflection.Linear  = 1.0;
    params.Deflection.Angular = 15.0;
  }
  else
  {
    // Return mesh properties
    params.Deflection.Linear  = arrAttr->Value(0);
    params.Deflection.Angular = arrAttr->Value(1);
  }
}
