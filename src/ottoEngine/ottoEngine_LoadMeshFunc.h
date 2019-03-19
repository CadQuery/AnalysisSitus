//-----------------------------------------------------------------------------
// Created on: 15 March 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoEngine_LoadMeshFunc_h
#define ottoEngine_LoadMeshFunc_h

// Active Data includes
#include <ActData_BaseTreeFunction.h>

//-----------------------------------------------------------------------------

//! Tree Function for loading STL meshes.
class ottoEngine_LoadMeshFunc : public ActData_BaseTreeFunction
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(ottoEngine_LoadMeshFunc, ActData_BaseTreeFunction)

public:

  static Handle(ottoEngine_LoadMeshFunc)
    Instance();

  static const char*
    GUID();

  virtual const char*
    GetGUID() const;

  virtual const char*
    GetName() const;

public:

  //! Returns true if this Tree Function is HEAVY, false -- otherwise.
  //! \return always false.
  virtual bool IsHeavy() const
  {
    return false;
  }

private:

  virtual int
    execute(const Handle(ActAPI_HParameterList)& argsIn,
            const Handle(ActAPI_HParameterList)& argsOut,
            const Handle(Standard_Transient)&    userData = NULL,
            ActAPI_ProgressEntry                 PEntry = NULL) const;

  virtual ActAPI_ParameterTypeStream
    inputSignature() const;

  virtual ActAPI_ParameterTypeStream
    outputSignature() const;

private:

  ottoEngine_LoadMeshFunc() : ActData_BaseTreeFunction() {}

};

#endif
