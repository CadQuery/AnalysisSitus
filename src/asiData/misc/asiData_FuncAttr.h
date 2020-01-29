//-----------------------------------------------------------------------------
// Created on: 02 May 2017
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

#ifndef asiData_FuncAttr_h
#define asiData_FuncAttr_h

// asiData includes
#include <asiData.h>

// asiAlgo includes
#include <asiAlgo_Function.h>

// OCCT includes
#include <Standard_GUID.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>

//! OCAF Attribute representing Function object.
template <typename T_ARGUMENT>
class asiData_FuncAttr : public TDF_Attribute
{
// Construction & settling-down routines:
public:

  //! Default constructor.
  asiData_FuncAttr() : TDF_Attribute() {}

  //! Settles down new Attribute to the given OCAF Label.
  //! \param[in] Label TDF Label to settle down the new Attribute to.
  //! \return newly created Attribute settled down onto the target Label.
  static Handle(asiData_FuncAttr) Set(const TDF_Label& Label)
  {
    Handle(asiData_FuncAttr<T_ARGUMENT>) A;
    //
    if ( !Label.FindAttribute(GUID(), A) )
    {
      A = new asiData_FuncAttr<T_ARGUMENT>();
      Label.AddAttribute(A);
    }
    return A;
  }

// GUID accessors:
public:

  //! Returns statically defined GUID for the Attribute.
  //! \return statically defined GUID.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID AttrGUID("A8DC530F-16E8-4591-8D7E-1D2C19E8B7EB");
    return AttrGUID;
  }

  //! Accessor for GUID associated with this kind of OCAF Attribute.
  //! \return GUID of the OCAF Attribute.
  virtual const Standard_GUID& ID() const
  {
    return GUID();
  }

// Attribute's kernel methods:
public:

  //! Creates new instance of the Attribute which is not initially populated
  //! with any data structures.
  //! \return new instance of the Attribute.
  virtual Handle(TDF_Attribute) NewEmpty() const
  {
    return new asiData_FuncAttr<T_ARGUMENT>();
  }

  //! Performs data transferring from the given OCAF Attribute to this one.
  //! This method is mainly used by OCAF Undo/Redo kernel as a part of
  //! backup functionality.
  //! \param[in] MainAttr OCAF Attribute to copy data from.
  virtual void Restore(const Handle(TDF_Attribute)& mainAttr)
  {
    // Nothing is here
  }

  //! Supporting method for Copy/Paste functionality. Performs full copying of
  //! the underlying data.
  //! \param[in] Into       where to paste.
  //! \param[in] RelocTable relocation table.
  virtual void Paste(const Handle(TDF_Attribute)&       into,
                     const Handle(TDF_RelocationTable)& relocTable) const
  {
    // Nothing is here
  }

// Accessors for domain-specific data:
public:

  //! Sets function object to store.
  //! \param[in] func function object to store.
  void SetFunction(const Handle(asiAlgo_Function<T_ARGUMENT>)& func)
  {
    m_func = func;
  }

  //! Returns the stored function object.
  //! \return stored function object.
  const Handle(asiAlgo_Function<T_ARGUMENT>)& GetFunction() const
  {
    return m_func;
  }

// Members:
private:

  //! Stored function.
  Handle(asiAlgo_Function<T_ARGUMENT>) m_func;

};

#endif
