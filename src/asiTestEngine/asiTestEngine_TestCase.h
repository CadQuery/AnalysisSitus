//-----------------------------------------------------------------------------
// Created on: 11 June 2013
//-----------------------------------------------------------------------------
// Copyright (c) 2013-2018, Sergey Slyadnev
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

#ifndef asiTestEngine_TestCase_HeaderFile
#define asiTestEngine_TestCase_HeaderFile

// asiTestEngine includes
#include <asiTestEngine_Macro.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// STD includes
#include <map>
#include <vector>

//-----------------------------------------------------------------------------
// Auxiliary definitions
//-----------------------------------------------------------------------------

#define DEFINE_TEST_VARIABLES \
  asiTestEngine_TestCase::StrStrMap asiTestEngine_TestCase::m_varExpansion;

//! Pointer to Test Function.
//! Please note that {funcID} should be normally passed by Test Case. The
//! convention is to have {funcID} as 1-based integer number.
typedef bool (*AsiTestFunction)(const int funcID);

//! Collection of pointers to Test Functions
class AsiTestFunctions
{
public:

  //! Default constructor.
  AsiTestFunctions() {}

public:

  //! Adds the passed function pointer to the collection of pointers to
  //! Test Functions.
  //! \param funcPtr [in] function pointer to add.
  //! \return this reference for subsequent streaming.
  AsiTestFunctions& operator<<(const AsiTestFunction& funcPtr)
  {
    m_testFunctions.push_back(funcPtr);
    return*this;
  }

  //! Returns size of the collection of function pointers.
  //! \return requested size.
  size_t Size() const
  {
    return m_testFunctions.size();
  }

  //! Returns function pointer for the given index.
  //! \param idx [in] index of the function pointer to access.
  //! \return requested function pointer.
  const AsiTestFunction& Func(const int idx) const
  {
    return m_testFunctions.at(idx);
  }

private:

  //! Internal collection of pointers to Test Functions.
  std::vector<AsiTestFunction> m_testFunctions;

};

//-----------------------------------------------------------------------------
// Base class for Test Cases
//-----------------------------------------------------------------------------

//! Base class for Test Cases.
class asiTestEngine_TestCase
{
public:

  //! shortcut for variable expansion maps.
  typedef std::map<std::string, std::string> StrStrMap;

  //! shortcut for string-string pairs.
  typedef std::pair<std::string, std::string> StrStrPair;

// Verification:
public:

  //! Compares two matrices on exact equality of their elements in the
  //! given 2-dimensional range. Notice that safety of such comparison is
  //! not guaranteed, so make sure that the compared matrices have equal
  //! dimensions.
  //! \param mx     [in] first matrix to compare.
  //! \param mx_ref [in] second matrix to compare.
  //! \param nRows  [in] number of rows to compare.
  //! \param nCols  [in] number of columns to compare.
  //! \return true in case of equality, false -- otherwise.
  static bool CompareExact(double**  mx,
                           double**  mx_ref,
                           const int nRows,
                           const int nCols)
  {
    for ( int i = 0; i < nRows; ++i )
      for ( int j = 0; j < nCols; ++j )
        if ( mx[i][j] != mx_ref[i][j] )
          return false;

    return true;
  }

// Expansion of variables:
public:

  //! Expands variable with the given name to the given value.
  //! \param varName  [in] variable name.
  //! \param varValue [in] variable value.
  //! \param caseID   [in] ID of the Test Case.
  //! \param funcID   [in] 1-based ID of Test Function to be used as a namespace
  //!                      for its own local variables mapping.
  static void SetVarDescr(const std::string& varName,
                          const std::string& varValue,
                          const int          caseID,
                          const int          funcID)
  {
    std::string vName = generateVarName(varName, caseID, funcID);

    if ( m_varExpansion.find(vName) != m_varExpansion.end() )
      m_varExpansion.erase(vName);

    m_varExpansion.insert( StrStrPair(vName, varValue) );
  }

  //! Overloaded variable setter.
  //! \param varName [in] variable name.
  //! \param val     [in] variable value.
  //! \param caseID  [in] ID of the Test Case.
  //! \param funcID  [in] 1-based ID of Test Function to be used as a namespace
  //!                     for its own local variables mapping.
  template <typename VAR_TYPE>
  static void SetVarDescr(const std::string& varName,
                          const VAR_TYPE     val,
                          const int          caseID,
                          const int          funcID)
  {
    SetVarDescr(varName, asiAlgo_Utils::Str::ToString(val), caseID, funcID);
  }

  //! Overloaded variable setter for Boolean values.
  //! \param varName [in] variable name.
  //! \param isYes   [in] variable value.
  //! \param caseID  [in] ID of the Test Case.
  //! \param funcID  [in] 1-based ID of Test Function to be used as a namespace
  //!                     for its own local variables mapping.
  static void SetVarDescr(const std::string& varName,
                          const bool         isYes,
                          const int          caseID,
                          const int          funcID)
  {
    SetVarDescr(varName, isYes ? std::string("yes") : std::string("no"), caseID, funcID);
  }

  //! Overloaded variable setter for XYZ values.
  //! \param varName [in] variable name.
  //! \param XYZ     [in] variable value.
  //! \param caseID  [in] ID of the Test Case.
  //! \param funcID  [in] 1-based ID of Test Function to be used as a namespace
  //!                     for its own local variables mapping.
  static void SetVarDescr(const std::string& varName,
                          const gp_XYZ&      XYZ,
                          const int          caseID,
                          const int          funcID)
  {
    std::string xyzStr = std::string("[")
                       + asiAlgo_Utils::Str::ToString( XYZ.X() ) + ", "
                       + asiAlgo_Utils::Str::ToString( XYZ.Y() ) + ", "
                       + asiAlgo_Utils::Str::ToString( XYZ.Z() )
                       + std::string("]");

    SetVarDescr(varName, xyzStr, caseID, funcID);
  }

  //! Overloaded variable setter for real array.
  //! \param varName [in] variable name.
  //! \param arr     [in] array as a value.
  //! \param n       [in] number of elements in the array.
  //! \param caseID  [in] ID of the Test Case.
  //! \param funcID  [in] 1-based ID of Test Function to be used as a namespace
  //!                     for its own local variables mapping.
  static void SetVarDescr(const std::string& varName,
                          const double*      arr,
                          const int          n,
                          const int          caseID,
                          const int          funcID)
  {
    std::string arrStr("[");
    if ( n )
    {
      for ( int i = 0; i < n; ++i )
      {
        arrStr += asiAlgo_Utils::Str::ToString(arr[i]);
        if ( i != (n - 1) )
          arrStr += ", ";
      }
    }
    arrStr += "]";

    SetVarDescr(varName, arrStr, caseID, funcID);
  }

  //! Overloaded variable setter for a vector.
  //! \param varName [in] variable name.
  //! \param vec     [in] vector as a value.
  //! \param caseID  [in] ID of the Test Case.
  //! \param funcID  [in] 1-based ID of Test Function to be used as a namespace
  //!                     for its own local variables mapping.
  template <typename ELEM_TYPE>
  static void SetVarDescr(const std::string&            varName,
                          const std::vector<ELEM_TYPE>& vec,
                          const int                     caseID,
                          const int                     funcID)
  {
    std::string arrStr("[");
    if ( vec.size() )
    {
      for ( int i = 0; i < vec.size(); ++i )
      {
        arrStr += asiAlgo_Utils::Str::ToString(vec[i]);
        if ( i != (vec.size() - 1) )
          arrStr += ", ";
      }
    }
    arrStr += "]";

    SetVarDescr(varName, arrStr, caseID, funcID);
  }

  //! Overloaded variable setter for real matrix.
  //! \param varName [in] variable name.
  //! \param mx      [in] matrix as a value.
  //! \param nRows   [in] number of rows in the matrix.
  //! \param nCols   [in] number of columns in the matrix.
  //! \param caseID  [in] ID of the Test Case.
  //! \param funcID  [in] 1-based ID of Test Function to be used as a namespace
  //!                     for its own local variables mapping.
  static void SetVarDescr(const std::string& varName,
                          double**           mx,
                          const int          nRows,
                          const int          nCols,
                          const int          caseID,
                          const int          funcID)
  {
    std::string mxStr;
    if ( nRows )
    {
      for ( int i = 0; i < nRows; ++i )
      {
        mxStr += "[";
        for ( int j = 0; j < nCols; ++j )
        {
          mxStr += asiAlgo_Utils::Str::ToString(mx[i][j]);
          if ( j != (nCols - 1) )
            mxStr += ", ";
        }
        mxStr += "]";
        if ( i != (nRows - 1) )
          mxStr += ", ";
      }
    }

    SetVarDescr(varName, mxStr, caseID, funcID);
  }

  //! Returns variables expansion map.
  //! \return expansion map.
  static const StrStrMap& ExpansionMap()
  {
    return m_varExpansion;
  }

private:

  //! Returns variable name using namespace of the Test Function.
  //! \param varName [in] short name.
  //! \param caseID  [in] ID of the Test Case.
  //! \param funcID  [in] 1-based ID of Test Function to be used as a namespace
  //!                     for its own local variables mapping.
  //! \return long variable name which is unique in scope of Test Function.
  static std::string generateVarName(const std::string& varName,
                                     const int          caseID,
                                     const int          funcID)
  {
    return asiAlgo_Utils::Str::ToString(caseID) + asiTestEngine_Macro_NAMESPACE +
           asiAlgo_Utils::Str::ToString(funcID) + asiTestEngine_Macro_NAMESPACE + varName;
  }

private:

  //! Local map for expansion of variables which can be used in
  //! descriptions of the Test Functions.
  static StrStrMap m_varExpansion;

};

#endif
