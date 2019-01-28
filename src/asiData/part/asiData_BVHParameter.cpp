//-----------------------------------------------------------------------------
// Created on: 20 November 2017
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
#include <asiData_BVHParameter.h>

// Active Data includes
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Parameter
//-----------------------------------------------------------------------------

//! Default constructor.
asiData_BVHParameter::asiData_BVHParameter() : ActData_UserParameter()
{}

//! Ensures correct construction of the Parameter object, e.g. prevents
//! allocating it in stack memory.
//! \return Parameter instance.
Handle(asiData_BVHParameter) asiData_BVHParameter::Instance()
{
  return new asiData_BVHParameter();
}

//! Sets BVH.
//! \param BVH             [in] BVH to set.
//! \param MType           [in] modification type.
//! \param doResetValidity [in] indicates whether to reset validity flag.
//! \param doResetPending  [in] indicates whether this Parameter must lose its
//!                             PENDING (or out-dated) property.
void asiData_BVHParameter::SetBVH(const Handle(asiAlgo_BVHFacets)& BVH,
                                  const ActAPI_ModificationType    MType,
                                  const bool                       doResetValidity,
                                  const bool                       doResetPending)
{
  if ( this->IsDetached() )
    Standard_ProgramError::Raise("Cannot access detached data");

  // Settle down an attribute an populate it with data
  TDF_Label               dataLab = ActData_Utils::ChooseLabelByTag(m_label, DS_BVH, true);
  Handle(asiData_BVHAttr) attr    = asiData_BVHAttr::Set(dataLab);
  //
  attr->SetBVH(BVH);

  // Mark root label of the Parameter as modified (Touched, Impacted or Silent)
  SPRING_INTO_FUNCTION(MType)
  // Reset Parameter's validity flag if requested
  RESET_VALIDITY(doResetValidity)
  // Reset Parameter's PENDING property
  RESET_PENDING(doResetPending)
}

//! Accessor for the stored BVH.
//! \return stored BVH.
Handle(asiAlgo_BVHFacets) asiData_BVHParameter::GetBVH()
{
  if ( !this->IsWellFormed() )
    Standard_ProgramError::Raise("Data inconsistent");

  // Choose a data label ensuring not to create it
  TDF_Label dataLab = ActData_Utils::ChooseLabelByTag(m_label, DS_BVH, false);
  //
  if ( dataLab.IsNull() )
    return NULL;

  // Get BVH attribute
  Handle(asiData_BVHAttr) attr;
  dataLab.FindAttribute(asiData_BVHAttr::GUID(), attr);
  //
  if ( attr.IsNull() )
    return NULL;

  return attr->GetBVH();
}

//! Checks if this Parameter object is mapped onto CAF data structure in a
//! correct way.
//! \return true if the object is well-formed, false -- otherwise.
bool asiData_BVHParameter::isWellFormed() const
{
  // BVH may not be present, that's Ok for such sort of transient attributes.

  return true;
}

//! Returns Parameter type.
//! \return Parameter type.
int asiData_BVHParameter::parameterType() const
{
  return Parameter_BVH;
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
void asiData_BVHParameter::setFromDTO(const Handle(ActData_ParameterDTO)& DTO,
                                      const ActAPI_ModificationType       MType,
                                      const bool                          doResetValidity,
                                      const bool                          doResetPending)
{
  Handle(asiData_BVHDTO) MyDTO = Handle(asiData_BVHDTO)::DownCast(DTO);
  this->SetBVH(MyDTO->BVH, MType, doResetValidity, doResetPending);
}

//! Creates and populates DTO.
//! \param GID [in] ready-to-use GID for DTO.
//! \return constructed DTO instance.
Handle(ActData_ParameterDTO)
  asiData_BVHParameter::createDTO(const ActAPI_ParameterGID& GID)
{
  Handle(asiData_BVHDTO) res = new asiData_BVHDTO(GID);
  res->BVH = this->GetBVH();
  return res;
}
