//-----------------------------------------------------------------------------
// Created on: 16 March 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoEngine_BuildSectionsFunc_h
#define ottoEngine_BuildSectionsFunc_h

// Active Data includes
#include <ActData_BaseTreeFunction.h>

//-----------------------------------------------------------------------------

//! Tree Function for contructing sections on mesh.
class ottoEngine_BuildSectionsFunc : public ActData_BaseTreeFunction
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(ottoEngine_BuildSectionsFunc, ActData_BaseTreeFunction)

public:

  static Handle(ottoEngine_BuildSectionsFunc)
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

  ottoEngine_BuildSectionsFunc() : ActData_BaseTreeFunction() {}

};

#endif
