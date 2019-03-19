//-----------------------------------------------------------------------------
// Created on: 19 March 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoEngine_SkinSurfaceFunc_h
#define ottoEngine_SkinSurfaceFunc_h

// Active Data includes
#include <ActData_BaseTreeFunction.h>

//-----------------------------------------------------------------------------

//! Tree Function for surface skinning.
class ottoEngine_SkinSurfaceFunc : public ActData_BaseTreeFunction
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(ottoEngine_SkinSurfaceFunc, ActData_BaseTreeFunction)

public:

  static Handle(ottoEngine_SkinSurfaceFunc)
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

  ottoEngine_SkinSurfaceFunc() : ActData_BaseTreeFunction() {}

};

#endif
