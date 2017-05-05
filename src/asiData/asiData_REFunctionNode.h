//-----------------------------------------------------------------------------
// Created on: 04 May 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiData_REFunctionNode_h
#define asiData_REFunctionNode_h

// asiData includes
#include <asiData_FuncParameter.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Function Node for Reverse Engineering
//-----------------------------------------------------------------------------

//! \todo move this Node to ABR.
//! Node representing function.
template <typename T_VARIABLE>
class asiData_REFunctionNode : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiData_REFunctionNode, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(asiData_REFunctionNode, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
    PID_Function,     //!< Function.
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  static Handle(ActAPI_INode) Instance()
  {
    return new asiData_REFunctionNode<T_VARIABLE>();
  }

// Initialization:
public:

  void Init()
  {
    // Initialize name Parameter
    this->InitParameter(PID_Name, "Name");

    // Initialize function
    this->SetFunction(NULL);
  }

// Generic naming support:
public:

  virtual TCollection_ExtendedString GetName()
  {
    return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
  }

  virtual void SetName(const TCollection_ExtendedString& name)
  {
    ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
  }

// Handy accessors to the stored data:
public:

  Handle(asiAlgo_Function<T_VARIABLE>) GetFunction() const
  {
    return Handle(asiData_FuncParameter<T_VARIABLE>)::DownCast( this->Parameter(PID_Function) )->GetFunction();
  }

  void SetFunction(const Handle(asiAlgo_Function<T_VARIABLE>)& func)
  {
    Handle(asiData_FuncParameter<T_VARIABLE>)::DownCast( this->Parameter(PID_Function) )->SetFunction(func);
  }

protected:

  //! Allocation is allowed only via Instance method.
  asiData_REFunctionNode() : ActData_BaseNode()
  {
    REGISTER_PARAMETER(Name, PID_Name);

    // Register custom Analysis Situs Parameters
    this->registerParameter(PID_Function, asiData_FuncParameter<T_VARIABLE>::Instance(), false);
  }

};

#endif
