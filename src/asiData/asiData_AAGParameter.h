//-----------------------------------------------------------------------------
// Created on: 21 December 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
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
  asiData_AAGDTO(const ActAPI_ParameterGID& GID) : ActData_ParameterDTO(GID, Parameter_AAG) {}

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
