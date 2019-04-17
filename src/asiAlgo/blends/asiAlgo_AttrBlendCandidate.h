//-----------------------------------------------------------------------------
// Created on: 01 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiAlgo_AttrBlendCandidate_h
#define asiAlgo_AttrBlendCandidate_h

// asiAlgo includes
#include <asiAlgo_BlendType.h>
#include <asiAlgo_FeatureAttrFace.h>

// Active Data includes
#include <ActAPI_IPlotter.h>

//-----------------------------------------------------------------------------

//! Attribute to mark a face as a blend candidate.
class asiAlgo_AttrBlendCandidate : public asiAlgo_FeatureAttrFace
{
public:

  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_AttrBlendCandidate, asiAlgo_FeatureAttrFace)

public:

  //! Creates attribute with feature ID.
  //! \param[in] featureId 1-based feature ID.
  asiAlgo_AttrBlendCandidate(const int featureId)
  : asiAlgo_FeatureAttrFace (featureId),
    Kind                    (BlendType_Uncertain),
    Radius                  (0.),
    Confirmed               (false)
  {}

public:

  //! \return static GUID associated with this type of attribute.
  static const Standard_GUID& GUID()
  {
    static Standard_GUID guid("7B7C2DD0-5BD6-429E-9B71-5F935BF1E815");
    return guid;
  }

  //! \return GUID associated with this type of attribute.
  virtual const Standard_GUID& GetGUID() const override
  {
    return GUID();
  }

  //! \return human-readable name of the attribute.
  virtual const char* GetName() const override
  {
    return "blend candidate";
  }

  //! \return brief dump.
  virtual TCollection_AsciiString DumpInline() const
  {
    return (this->Kind == BlendType_Vertex) ? "vertex blend" : "";
  }

public:

  //! Dumps this attribute to the passed output stream.
  //! \param[in, out] target stream.
  asiAlgo_EXPORT virtual void
    Dump(Standard_OStream& out) const;

  //! Dumps this attribute to the passed plotter.
  //! \param[in,out] imperative plotter.
  asiAlgo_EXPORT virtual void
    DumpGraphically(ActAPI_PlotterEntry plotter) const;

protected:

  //! Dumps the custome attribute properties to JSON.
  //! \param[in,out] out       target output stream.
  //! \param[in]     numSpaces number of spaces to use for formatting.
  virtual void dumpJSON(Standard_OStream& out,
                        const int         numSpaces = 0) const
  {
    std::string ws; // whitespace.
    for ( int k = 0; k < numSpaces; ++k ) ws += " ";

    out << ",\n" << ws.c_str() << "\"kind\": "                << "\"" << kindToString(this->Kind)              << "\"";
    out << ",\n" << ws.c_str() << "\"confirmed\": "           << "\"" << this->Confirmed                       << "\"";
    out << ",\n" << ws.c_str() << "\"numSmoothEdges\": "      << "\"" << this->SmoothEdgeIndices.Extent()      << "\"";
    out << ",\n" << ws.c_str() << "\"numSpringEdges\": "      << "\"" << this->SpringEdgeIndices.Extent()      << "\"";
    out << ",\n" << ws.c_str() << "\"numCrossEdges\": "       << "\"" << this->CrossEdgeIndices.Extent()       << "\"";
    out << ",\n" << ws.c_str() << "\"numTerminatingEdges\": " << "\"" << this->TerminatingEdgeIndices.Extent() << "\"";
  }

  //! Converts blend kind enum to string.
  //! \param[in] kind blend kind in question.
  //! \return string representation of a blend kind.
  static std::string kindToString(const asiAlgo_BlendType kind)
  {
    switch ( kind )
    {
      case BlendType_Uncertain: return "uncertain";
      case BlendType_Ordinary:  return "ordinary";
      case BlendType_Vertex:    return "vertex";
      case BlendType_Cliff:     return "cliff";
      default: break;
    }

    return "undefined";
  }

public:

  asiAlgo_BlendType          Kind;                   //!< Blend type.
  double                     Radius;                 //!< Actual radius.
  bool                       Confirmed;              //!< Confirmed/Unconfirmed blend.
  TColStd_PackedMapOfInteger SmoothEdgeIndices;      //!< Smooth edges.
  TColStd_PackedMapOfInteger SpringEdgeIndices;      //!< Spring edges.
  TColStd_PackedMapOfInteger CrossEdgeIndices;       //!< Cross edges.
  TColStd_PackedMapOfInteger TerminatingEdgeIndices; //!< Sharp terminating edges.

};

#endif
