//-----------------------------------------------------------------------------
// Created on: 11 April 2020
//-----------------------------------------------------------------------------
// Copyright (c) 2020-present, Sergey Slyadnev
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
#include <asiData_UniformGridParameter.h>

// Active Data includes
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Parameter
//-----------------------------------------------------------------------------

//! Default constructor.
asiData_UniformGridParameter::asiData_UniformGridParameter() : ActData_UserParameter()
{}

//! Ensures correct construction of the Parameter object, e.g. prevents
//! allocating it in stack memory.
//! \return Parameter instance.
Handle(asiData_UniformGridParameter) asiData_UniformGridParameter::Instance()
{
  return new asiData_UniformGridParameter();
}

//! Sets the uniform grid to store.
//! \param[in] grid            uniform to set.
//! \param[in] MType           modification type.
//! \param[in] doResetValidity indicates whether to reset validity flag.
//! \param[in] doResetPending  indicates whether this Parameter must lose its
//!                            PENDING (or out-dated) property.
void asiData_UniformGridParameter::SetGrid(const Handle(asiAlgo_UniformGrid<float>)& grid,
                                           const ActAPI_ModificationType             MType,
                                           const bool                                doResetValidity,
                                           const bool                                doResetPending)
{
  if ( this->IsDetached() )
    Standard_ProgramError::Raise("Cannot access detached data");

  // Settle down an attribute an populate it with data.
  TDF_Label                       dataLab = ActData_Utils::ChooseLabelByTag(m_label, DS_Grid, true);
  Handle(asiData_UniformGridAttr) attr    = asiData_UniformGridAttr::Set(dataLab);
  //
  attr->SetGrid(grid);

  // Mark root label of the Parameter as modified (Touched, Impacted or Silent).
  SPRING_INTO_FUNCTION(MType)
  // Reset Parameter's validity flag if requested.
  RESET_VALIDITY(doResetValidity)
  // Reset Parameter's PENDING property.
  RESET_PENDING(doResetPending)
}

//! Accessor for the stored uniform grid.
//! \return stored uniform grid.
Handle(asiAlgo_UniformGrid<float>) asiData_UniformGridParameter::GetGrid()
{
  if ( !this->IsWellFormed() )
    Standard_ProgramError::Raise("Data inconsistent");

  // Choose a data label ensuring not to create it.
  TDF_Label dataLab = ActData_Utils::ChooseLabelByTag(m_label, DS_Grid, false);
  //
  if ( dataLab.IsNull() )
    return nullptr;

  // Get the uniform grid attribute.
  Handle(asiData_UniformGridAttr) attr;
  dataLab.FindAttribute(asiData_UniformGridAttr::GUID(), attr);
  //
  if ( attr.IsNull() )
    return nullptr;

  return attr->GetGrid();
}

//! Checks if this Parameter object is mapped onto CAF data structure in a
//! correct way.
//! \return true if the object is well-formed, false -- otherwise.
bool asiData_UniformGridParameter::isWellFormed() const
{
  // The grid may not be present, that's Ok for such sort of transient attributes.

  return true;
}

//! Returns Parameter type.
//! \return Parameter type.
int asiData_UniformGridParameter::parameterType() const
{
  return Parameter_UniformGrid;
}

//-----------------------------------------------------------------------------
// DTO construction
//-----------------------------------------------------------------------------

//! Populates Parameter from the passed DTO.
//! \param[in] DTO             DTO to source data from.
//! \param[in] MType           modification type.
//! \param[in] doResetValidity indicates whether validity flag must be
//!                            reset or not.
//! \param[in] doResetPending  indicates whether pending flag must be reset
//!                            or not.
void asiData_UniformGridParameter::setFromDTO(const Handle(ActData_ParameterDTO)& DTO,
                                              const ActAPI_ModificationType       MType,
                                              const bool                          doResetValidity,
                                              const bool                          doResetPending)
{
  Handle(asiData_UniformGridDTO) MyDTO = Handle(asiData_UniformGridDTO)::DownCast(DTO);
  this->SetGrid(MyDTO->Grid, MType, doResetValidity, doResetPending);
}

//! Creates and populates DTO.
//! \param[in] GID ready-to-use GID for DTO.
//! \return constructed DTO instance.
Handle(ActData_ParameterDTO)
  asiData_UniformGridParameter::createDTO(const ActAPI_ParameterGID& GID)
{
  Handle(asiData_UniformGridDTO) res = new asiData_UniformGridDTO(GID);
  res->Grid = this->GetGrid();
  return res;
}
