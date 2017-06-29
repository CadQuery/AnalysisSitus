//-----------------------------------------------------------------------------
// Created on: 21 December 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
  if ( !ActData_Utils::CheckLabelAttr( m_label, DS_AAG,
                                       asiData_AAGAttr::GUID() ) )
    return false;

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
