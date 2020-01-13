//-----------------------------------------------------------------------------
// Created on: 21 March 2016
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

#ifndef asiAlgo_FeatureAttr_h
#define asiAlgo_FeatureAttr_h

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IPlotter.h>

// OCCT includes
#include <Standard_GUID.hxx>
#include <TCollection_AsciiString.hxx>

class asiAlgo_AAG;

//-----------------------------------------------------------------------------

//! Base class for all feature attributes.
class asiAlgo_FeatureAttr : public Standard_Transient
{
friend class asiAlgo_AAG;

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_FeatureAttr, Standard_Transient)

public:

  virtual ~asiAlgo_FeatureAttr() {}

public:

  virtual const Standard_GUID&
    GetGUID() const = 0;

  virtual const char*
    GetName() const = 0;

public:

  //! Dumps the attribute into a single line (to be used in titles).
  //! \return brief one-line dump of the attribute.
  virtual TCollection_AsciiString DumpInline() const { return ""; }

  //! Dumps the attribute to the passed output stream.
  //! \param[out] out target output stream.
  virtual void Dump(Standard_OStream& out) const
  {
    asiAlgo_NotUsed(out);
  }

  //! Dumps this attribute as a JSON object.
  //! \param[in,out] out       target output stream.
  //! \param[in]     numSpaces number of spaces to use for formatting.
  virtual void DumpJSON(Standard_OStream& out,
                        const int         numSpaces = 0) const
  {
    std::string ws; // whitespace.
    for ( int k = 0; k < numSpaces; ++k ) ws += " ";

    char sguid[Standard_GUID_SIZE_ALLOC];
    this->GetGUID().ToCString(sguid);

    out << "\n" << ws.c_str() << "{";
    out << "\n" << ws.c_str() << "  \"guid\": " << "\"" << sguid << "\"";
    out << ",\n" << ws.c_str() << "  \"name\": " << "\"" << this->GetName() << "\"";
    //
    this->dumpJSON(out, numSpaces + 2);
    //
    out << "\n" << ws.c_str() << "}";
  }

  //! Dumps the attribute to the plotter.
  //! \param[in] plotter plotter to dump the attribute.
  virtual void DumpGraphically(ActAPI_PlotterEntry plotter) const
  {
    asiAlgo_NotUsed(plotter);
  }

public:

  //! Hasher for sets.
  struct t_hasher
  {
    static int HashCode(const Handle(asiAlgo_FeatureAttr)& attr, const int upper)
    {
      return Standard_GUID::HashCode(attr->GetGUID(), upper);
    }

    static bool IsEqual(const Handle(asiAlgo_FeatureAttr)& attr, const Handle(asiAlgo_FeatureAttr)& other)
    {
      return Standard_GUID::IsEqual( attr->GetGUID(), other->GetGUID() );
    }
  };

protected:

  //! Sets back-pointer to AAG.
  //! \param[in] pAAG owner AAG.
  void setAAG(asiAlgo_AAG* pAAG)
  {
    m_pAAG = pAAG;
  }

  //! \return back-pointer to the owner AAG.
  asiAlgo_AAG* getAAG() const
  {
    return m_pAAG;
  }

protected:

  //! Allows sub-classes to dump additional properties to their JSONs.
  virtual void dumpJSON(Standard_OStream&, const int) const {}

protected:

  asiAlgo_AAG* m_pAAG; //!< Back-pointer to the owner AAG.

};

#endif
