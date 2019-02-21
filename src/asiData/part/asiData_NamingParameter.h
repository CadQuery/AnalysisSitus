//-----------------------------------------------------------------------------
// Created on: 11 December(*) 2017
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

#ifndef asiData_NamingParameter_h
#define asiData_NamingParameter_h

// asiData includes
#include <asiData_NamingAttr.h>

// Active Data includes
#include <ActData_UserParameter.h>
#include <ActData_Common.h>
#include <ActData_ParameterDTO.h>

//-----------------------------------------------------------------------------
// Parameter DTO
//-----------------------------------------------------------------------------

//! Data Transfer Object (DTO) corresponding to data wrapped with
//! Naming Parameter without any OCAF connectivity.
class asiData_NamingDTO : public ActData_ParameterDTO
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_NamingDTO, ActData_ParameterDTO)

public:

  //! Constructor accepting GID.
  //! \param GID [in] GID.
  asiData_NamingDTO(const ActAPI_ParameterGID& GID) : ActData_ParameterDTO(GID, Parameter_UNDEFINED) {}

public:

  Handle(asiAlgo_Naming) Naming; //!< Naming service.

};

//-----------------------------------------------------------------------------
// Parameter
//-----------------------------------------------------------------------------

//! Node Parameter representing Naming service.
class asiData_NamingParameter : public ActData_UserParameter
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_NamingParameter, ActData_UserParameter)

public:

  asiData_EXPORT static Handle(asiData_NamingParameter)
    Instance();

public:

  asiData_EXPORT void
    SetNaming(const Handle(asiAlgo_Naming)& Naming,
              const ActAPI_ModificationType MType           = MT_Touched,
              const bool                    doResetValidity = true,
              const bool                    doResetPending  = true);

  asiData_EXPORT Handle(asiAlgo_Naming)
    GetNaming();

protected:

  asiData_EXPORT
    asiData_NamingParameter();

private:

  virtual bool isWellFormed() const;
  virtual int parameterType() const;

private:

  virtual void
    setFromDTO(const Handle(ActData_ParameterDTO)& DTO,
               const ActAPI_ModificationType       MType = MT_Touched,
               const bool                          doResetValidity = true,
               const bool                          doResetPending = true);

  virtual Handle(ActData_ParameterDTO)
    createDTO(const ActAPI_ParameterGID& GID);

protected:

  //! Tags for the underlying CAF Labels.
  enum Datum
  {
    DS_Naming = ActData_UserParameter::DS_DatumLast,
    DS_DatumLast = DS_Naming + RESERVED_DATUM_RANGE
  };

};

#endif
