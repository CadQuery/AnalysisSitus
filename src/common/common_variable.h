//-----------------------------------------------------------------------------
// Created on: 17 May 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef common_variable_h
#define common_variable_h

//-----------------------------------------------------------------------------

// OCCT includes
#include <TCollection_AsciiString.hxx>

//-----------------------------------------------------------------------------

//! Variable type.
enum common_var_type
{
  var_UNDEFINED = -1,
  //
  var_INTEGER = 0,
  var_REAL,
  var_STRING
};

//! Variable.
class common_var
{
public:

  common_var_type         Type;        //!< Type.
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
    static inline int HashCode(const common_var& var,
                               const int         upper)
    {
      return ::HashCode(var.Name, upper);
    }

    //! IsEqual() function for variables to be used in OCCT Data Maps.
    //! \param var1 [in] first variable.
    //! \param var2 [in] second variable.
    //! \return true in case of equality, false -- otherwise.
    static inline int IsEqual(const common_var& var1,
                              const common_var& var2)
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
  common_var(const common_var_type          type,
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
  common_var(const common_var_type          type,
             const TCollection_AsciiString& name,
             const TCollection_AsciiString& descr)
  {
    Type        = type;
    Name        = name;
    Description = descr;
  }

  //! Creates variable.
  //! \param name [in] name of the variable.
  common_var(const TCollection_AsciiString& name)
  {
    Type = var_UNDEFINED;
    Name = name;
  }

  //! Creates variable.
  common_var()
  {
    Type = var_UNDEFINED;
  }
};

#endif
