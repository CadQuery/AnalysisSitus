//-----------------------------------------------------------------------------
// Created on: 21 December (*) 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
#include <asiData_AAGParameter.h>

// Active Data includes
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Parameter
//-----------------------------------------------------------------------------

//! Default constructor.
asiData_AAGParameter::asiData_AAGParameter() : ActData_UserParameter()
{}

//! Ensures correct construction of the Parameter object, e.g. prevents
//! allocating it in stack memory.
//! \return Parameter instance.
Handle(asiData_AAGParameter) asiData_AAGParameter::Instance()
{
  return new asiData_AAGParameter();
}

//! Sets AAG.
//! \param aag             [in] AAG to set.
//! \param MType           [in] modification type.
//! \param doResetValidity [in] indicates whether to reset validity flag.
//! \param doResetPending  [in] indicates whether this Parameter must lose its
//!                             PENDING (or out-dated) property.
void asiData_AAGParameter::SetAAG(const Handle(asiAlgo_AAG)&    aag,
                                  const ActAPI_ModificationType MType,
                                  const bool                    doResetValidity,
                                  const bool                    doResetPending)
{
  if ( this->IsDetached() )
    Standard_ProgramError::Raise("Cannot access detached data");

  // Settle down an attribute an populate it with data
  TDF_Label               dataLab = ActData_Utils::ChooseLabelByTag(m_label, DS_AAG, true);
  Handle(asiData_AAGAttr) attr    = asiData_AAGAttr::Set(dataLab);
  //
  attr->SetAAG(aag);

  // Mark root label of the Parameter as modified (Touched, Impacted or Silent)
  SPRING_INTO_FUNCTION(MType)
  // Reset Parameter's validity flag if requested
  RESET_VALIDITY(doResetValidity)
  // Reset Parameter's PENDING property
  RESET_PENDING(doResetPending)
}

//! Accessor for the stored AAG.
//! \return stored AAG.
Handle(asiAlgo_AAG) asiData_AAGParameter::GetAAG()
{
  if ( !this->IsWellFormed() )
    Standard_ProgramError::Raise("Data inconsistent");

  // Choose a data label ensuring not to create it
  TDF_Label dataLab = ActData_Utils::ChooseLabelByTag(m_label, DS_AAG, false);
  //
  if ( dataLab.IsNull() )
    return NULL;

  // Get AAG attribute
  Handle(asiData_AAGAttr) attr;
  dataLab.FindAttribute(asiData_AAGAttr::GUID(), attr);
  //
  if ( attr.IsNull() )
    return NULL;

  return attr->GetAAG();
}

//! Checks if this Parameter object is mapped onto CAF data structure in a
//! correct way.
//! \return true if the object is well-formed, false -- otherwise.
bool asiData_AAGParameter::isWellFormed() const
{
  // AAG may not be present, that's Ok for such sort of transient attributes.

  return true;
}

//! Returns Parameter type.
//! \return Parameter type.
int asiData_AAGParameter::parameterType() const
{
  return Parameter_AAG;
}

//-----------------------------------------------------------------------------
// DTO construction
//-----------------------------------------------------------------------------

//! Populates Parameter from the passed DTO.
//! \param DTO             [in] DTO to source data from.
//! \param MType           [in] modification type.
//! \param doResetValidity [in] indicates whether validity flag must be
//!                             reset or not.
//! \param doResetPending  [in] indicates whether pending flag must be reset
//!                             or not.
void asiData_AAGParameter::setFromDTO(const Handle(ActData_ParameterDTO)& DTO,
                                      const ActAPI_ModificationType       MType,
                                      const bool                          doResetValidity,
                                      const bool                          doResetPending)
{
  Handle(asiData_AAGDTO) MyDTO = Handle(asiData_AAGDTO)::DownCast(DTO);
  this->SetAAG(MyDTO->AAG, MType, doResetValidity, doResetPending);
}

//! Creates and populates DTO.
//! \param GID [in] ready-to-use GID for DTO.
//! \return constructed DTO instance.
Handle(ActData_ParameterDTO)
  asiData_AAGParameter::createDTO(const ActAPI_ParameterGID& GID)
{
  Handle(asiData_AAGDTO) res = new asiData_AAGDTO(GID);
  res->AAG = this->GetAAG();
  return res;
}
