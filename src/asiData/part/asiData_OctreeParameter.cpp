//-----------------------------------------------------------------------------
// Created on: 25 November 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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
#include <asiData_OctreeParameter.h>

// Active Data includes
#include <ActData_Utils.h>

//-----------------------------------------------------------------------------
// Parameter
//-----------------------------------------------------------------------------

//! Default constructor.
asiData_OctreeParameter::asiData_OctreeParameter() : ActData_UserParameter()
{}

//! Ensures correct construction of the Parameter object, e.g. prevents
//! allocating it in stack memory.
//! \return Parameter instance.
Handle(asiData_OctreeParameter) asiData_OctreeParameter::Instance()
{
  return new asiData_OctreeParameter();
}

//! Sets octree to store.
//! \param[in] octree          octree to set.
//! \param[in] MType           modification type.
//! \param[in] doResetValidity indicates whether to reset validity flag.
//! \param[in] doResetPending  indicates whether this Parameter must lose its
//!                            PENDING (or out-dated) property.
void asiData_OctreeParameter::SetOctree(void*                         octree,
                                        const ActAPI_ModificationType MType,
                                        const bool                    doResetValidity,
                                        const bool                    doResetPending)
{
  if ( this->IsDetached() )
    Standard_ProgramError::Raise("Cannot access detached data");

  // Settle down an attribute an populate it with data
  TDF_Label                  dataLab = ActData_Utils::ChooseLabelByTag(m_label, DS_Octree, true);
  Handle(asiData_OctreeAttr) attr    = asiData_OctreeAttr::Set(dataLab);
  //
  attr->SetOctree(octree);

  // Mark root label of the Parameter as modified (Touched, Impacted or Silent)
  SPRING_INTO_FUNCTION(MType)
  // Reset Parameter's validity flag if requested
  RESET_VALIDITY(doResetValidity)
  // Reset Parameter's PENDING property
  RESET_PENDING(doResetPending)
}

//! Accessor for the stored octree.
//! \return stored octree node.
void* asiData_OctreeParameter::GetOctree()
{
  if ( !this->IsWellFormed() )
    Standard_ProgramError::Raise("Data inconsistent");

  // Choose a data label ensuring not to create it
  TDF_Label dataLab = ActData_Utils::ChooseLabelByTag(m_label, DS_Octree, false);
  //
  if ( dataLab.IsNull() )
    return NULL;

  // Get octree attribute
  Handle(asiData_OctreeAttr) attr;
  dataLab.FindAttribute(asiData_OctreeAttr::GUID(), attr);
  //
  if ( attr.IsNull() )
    return NULL;

  return attr->GetOctree();
}

//! Checks if this Parameter object is mapped onto CAF data structure in a
//! correct way.
//! \return true if the object is well-formed, false -- otherwise.
bool asiData_OctreeParameter::isWellFormed() const
{
  // Octree may not be present, that's Ok for such sort of transient attributes.

  return true;
}

//! Returns Parameter type.
//! \return Parameter type.
int asiData_OctreeParameter::parameterType() const
{
  return Parameter_Octree;
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
void asiData_OctreeParameter::setFromDTO(const Handle(ActData_ParameterDTO)& DTO,
                                         const ActAPI_ModificationType       MType,
                                         const bool                          doResetValidity,
                                         const bool                          doResetPending)
{
  Handle(asiData_OctreeDTO) MyDTO = Handle(asiData_OctreeDTO)::DownCast(DTO);
  this->SetOctree(MyDTO->pOctree, MType, doResetValidity, doResetPending);
}

//! Creates and populates DTO.
//! \param GID [in] ready-to-use GID for DTO.
//! \return constructed DTO instance.
Handle(ActData_ParameterDTO)
  asiData_OctreeParameter::createDTO(const ActAPI_ParameterGID& GID)
{
  Handle(asiData_OctreeDTO) res = new asiData_OctreeDTO(GID);
  res->pOctree = this->GetOctree();
  return res;
}
