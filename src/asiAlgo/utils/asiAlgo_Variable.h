//-----------------------------------------------------------------------------
// Created on: 17 May 2016
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiAlgo_Variable_h
#define asiAlgo_Variable_h

// OCCT includes
#include <TCollection_AsciiString.hxx>

//-----------------------------------------------------------------------------

//! Variable type.
enum asiAlgo_VariableType
{
  var_UNDEFINED = -1,
  //
  var_INTEGER = 0,
  var_REAL,
  var_BOOL,
  var_STRING
};

//-----------------------------------------------------------------------------

//! Variable.
class asiAlgo_Variable
{
public:

  asiAlgo_VariableType    Type;        //!< Type.
  TCollection_AsciiString Name;        //!< Name.
  TCollection_AsciiString Value;       //!< Value as string.
  TCollection_AsciiString Description; //!< User-readable description.

public:

  //! Hasher for maps.
  struct Hasher
  {
    //! HashCode() function to be used in OCCT Data Maps.
    //! \param var   [in] variable to calculate a hash code for.
    //! \param upper [in] upper index.
    //! \return hash code.
    static int HashCode(const asiAlgo_Variable& var,
                        const int               upper)
    {
      return ::HashCode(var.Name, upper);
    }

    //! IsEqual() function for variables to be used in OCCT Data Maps.
    //! \param var1 [in] first variable.
    //! \param var2 [in] second variable.
    //! \return true in case of equality, false -- otherwise.
    static int IsEqual(const asiAlgo_Variable& var1,
                       const asiAlgo_Variable& var2)
    {
      return ::IsEqual(var1.Name, var2.Name);
    }
  };

public:

  //! Creates and fully initializes variable.
  //! \param type  [in] type of the variable.
  //! \param name  [in] name of the variable.
  //! \param value [in] value of the variable.
  //! \param descr [in] description of the variable.
  asiAlgo_Variable(const asiAlgo_VariableType     type,
                   const TCollection_AsciiString& name,
                   const TCollection_AsciiString& value,
                   const TCollection_AsciiString& descr)
  {
    Type        = type;
    Name        = name;
    Value       = value;
    Description = descr;
  }

  //! Creates variable.
  //! \param type  [in] type of the variable.
  //! \param name  [in] name of the variable.
  //! \param descr [in] description of the variable.
  asiAlgo_Variable(const asiAlgo_VariableType     type,
                   const TCollection_AsciiString& name,
                   const TCollection_AsciiString& descr)
  {
    Type        = type;
    Name        = name;
    Description = descr;
  }

  //! Creates variable.
  //! \param name [in] name of the variable.
  asiAlgo_Variable(const TCollection_AsciiString& name)
  {
    Type = var_UNDEFINED;
    Name = name;
  }

  //! Creates variable.
  asiAlgo_Variable()
  {
    Type = var_UNDEFINED;
  }
};

#endif
