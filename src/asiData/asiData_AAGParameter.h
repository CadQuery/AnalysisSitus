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

#ifndef asiData_AAGParameter_h
#define asiData_AAGParameter_h

// asiData includes
#include <asiData_AAGAttr.h>

// Active Data includes
#include <ActData_UserParameter.h>
#include <ActData_Common.h>
#include <ActData_ParameterDTO.h>

//-----------------------------------------------------------------------------
// Parameter DTO
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_AAGDTO, ActData_ParameterDTO)

//! Data Transfer Object (DTO) corresponding to data wrapped with
//! AAG Parameter without any OCAF connectivity.
class asiData_AAGDTO : public ActData_ParameterDTO
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_AAGDTO, ActData_ParameterDTO)

public:

  //! Constructor accepting GID.
  //! \param GID [in] GID.
  asiData_AAGDTO(const ActAPI_ParameterGID& GID) : ActData_ParameterDTO(GID, Parameter_UNDEFINED) {}

public:

  Handle(asiAlgo_AAG) AAG; //!< Attributed Adjacency Graph.

};

//-----------------------------------------------------------------------------
// Parameter
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(asiData_AAGParameter, ActData_UserParameter)

//! Node Parameter representing Attributed Adjacency Graph.
class asiData_AAGParameter : public ActData_UserParameter
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_AAGParameter, ActData_UserParameter)

public:

  asiData_EXPORT static Handle(asiData_AAGParameter)
    Instance();

public:

  asiData_EXPORT void
    SetAAG(const Handle(asiAlgo_AAG)&    aag,
           const ActAPI_ModificationType MType           = MT_Touched,
           const bool                    doResetValidity = true,
           const bool                    doResetPending  = true);

  asiData_EXPORT Handle(asiAlgo_AAG)
    GetAAG();

protected:

  asiData_EXPORT
    asiData_AAGParameter();

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
    DS_AAG = ActData_UserParameter::DS_DatumLast,
    DS_DatumLast = DS_AAG + RESERVED_DATUM_RANGE
  };

};

#endif
