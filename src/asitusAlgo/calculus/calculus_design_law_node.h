//-----------------------------------------------------------------------------
// Created on: 09 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef calculus_design_law_node_h
#define calculus_design_law_node_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (calculus) includes
#include <calculus_design_law.h>

// Active Data includes
#include <ActData_BaseNode.h>

//-----------------------------------------------------------------------------
// Part Node
//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(calculus_design_law_node, ActData_BaseNode)

//! Node representing design law in persistent fashion.
class calculus_design_law_node : public ActData_BaseNode
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(calculus_design_law_node, ActData_BaseNode)

  // Automatic registration of Node type in global factory
  DEFINE_NODE_FACTORY(calculus_design_law_node, Instance)

public:

  //! IDs for the underlying Parameters.
  enum ParamId
  {
  //------------------//
  // Common           //
  //------------------//
    PID_Name,         //!< Name of the Node.
  //------------------//
  // Geometry         //
  //------------------//
    PID_Poles,        //!< Unidimensional poles.
    PID_Knots,        //!< Knots.
    PID_Mults,        //!< Multiplicities.
    PID_Degree,       //!< Degree.
  //------------------//
    PID_Last = PID_Name + ActData_BaseNode::RESERVED_PARAM_RANGE
  };

public:

  static Handle(ActAPI_INode)
    Instance();

// Generic naming support:
public:

  virtual TCollection_ExtendedString
    GetName();

  virtual void
    SetName(const TCollection_ExtendedString& name);

// Handy accessors to the stored data:
public:

  void               SetPoles  (const Handle(HRealArray)& poles);
  Handle(HRealArray) GetPoles  ()                                 const;
  void               SetKnots  (const Handle(HRealArray)& knots);
  Handle(HRealArray) GetKnots  ()                                 const;
  void               SetMults  (const Handle(HIntArray)& mults);
  Handle(HIntArray)  GetMults  ()                                 const;
  void               SetDegree (const int& degree);
  int                GetDegree ()                                 const;

//-----------------------------------------------------------------------------
  Handle(calculus_design_law) AsLaw() const;
//-----------------------------------------------------------------------------

// Initialization:
public:

  void Init(const Handle(calculus_design_law)& law);

protected:

  //! Allocation is allowed only via Instance method.
  calculus_design_law_node();

};

#endif
