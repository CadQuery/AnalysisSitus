//-----------------------------------------------------------------------------
// Created on: 04 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Sergey Slyadnev
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

// Own include
#include <asiAlgo_ExtractFeatures.h>

// asiAlgo includes
#include <asiAlgo_FeatureAttrFace.h>
#include <asiAlgo_AAGIterator.h>

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

asiAlgo_ExtractFeatures::asiAlgo_ExtractFeatures(ActAPI_ProgressEntry progress,
                                                 ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_ExtractFeatures::RegisterFeatureType(const int            type,
                                                  const Standard_GUID& guid)
{
  bool isOk = !m_types.IsBound(type);
  if ( isOk )
    m_types.Bind(type, guid);

  return isOk;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ExtractFeatures::Perform(const Handle(asiAlgo_AAG)&                   aag,
                                      Handle(asiAlgo_ExtractFeaturesResult)&       result,
                                      const Handle(asiAlgo_ExtractFeaturesFilter)& filter) const
{
  // Prepare result.
  if ( result.IsNull() )
    result = new asiAlgo_ExtractFeaturesResult;

  // Iterate AAG randomly.
  Handle(asiAlgo_AAGRandomIterator)
    it = new asiAlgo_AAGRandomIterator(aag);
  //
  for ( ; it->More(); it->Next() )
  {
    // Get current face and its attributes.
    const int fid = it->GetFaceId();
    //
    if ( !aag->HasNodeAttributes(fid) )
    {
#if defined COUT_DEBUG
      std::cout << "No attributes exist for face " << fid << std::endl;
#endif
      continue;
    }
    //
    const asiAlgo_AAG::t_attr_set& attrs = aag->GetNodeAttributes(fid);

    // Loop over the supported feature types and classify the current
    // face as belonging to one or another group.
    for ( NCollection_DataMap<int, Standard_GUID>::Iterator typeIt(m_types); typeIt.More(); typeIt.Next() )
    {
      int                  type = typeIt.Key();
      const Standard_GUID& guid = typeIt.Value();

      if ( attrs.Contains(guid) )
      {
        Handle(asiAlgo_FeatureAttrFace)
          attr = Handle(asiAlgo_FeatureAttrFace)::DownCast( attrs(guid) );
        //
        if ( attr.IsNull() )
          return false; // Should never happen.

        // Filter out the extracted attribute.
        if ( !filter.IsNull() && !(*filter)(attr) )
          continue;

        // Dump graphically.
        attr->DumpGraphically(m_plotter);

        // Get group of face indices representing a single feature.
        TColStd_PackedMapOfInteger&
          feature = result->FindFeature( type, attr->GetFeatureId() );

        // Add face.
        feature.Add(fid);
      }
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ExtractFeatures::PerformForType(const Handle(asiAlgo_AAG)&                   aag,
                                             const int                                    type,
                                             Handle(asiAlgo_ExtractFeaturesResult)&       result,
                                             const Handle(asiAlgo_ExtractFeaturesFilter)& filter) const
{
  // Prepare result.
  if ( result.IsNull() )
    result = new asiAlgo_ExtractFeaturesResult;

  // Get GUID of interest.
  Standard_GUID guid = this->guidByType(type);

  // Iterate AAG randomly.
  Handle(asiAlgo_AAGRandomIterator)
    it = new asiAlgo_AAGRandomIterator(aag);
  //
  for ( ; it->More(); it->Next() )
  {
    // Get current face and its attributes.
    const int                         fid = it->GetFaceId();
    const asiAlgo_AAG::t_attr_set& attrs = aag->GetNodeAttributes(fid);
    //
    if ( attrs.Contains(guid) )
    {
      Handle(asiAlgo_FeatureAttrFace)
        attr = Handle(asiAlgo_FeatureAttrFace)::DownCast( attrs(guid) );
      //
      if ( attr.IsNull() )
        return false; // Should never happen.

      // Filter out the extracted attribute.
      if ( !filter.IsNull() && !(*filter)(attr) )
        continue;

      // Get group of face indices representing a single feature.
      TColStd_PackedMapOfInteger&
        feature = result->FindFeature( type, attr->GetFeatureId() );

      // Add face.
      feature.Add(fid);
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

//! Returns GUID of the AAG attribute corresponding to the passed feature type.
//! \param[in] type feature type.
//! \return GUID of AAG attribute.
Standard_GUID asiAlgo_ExtractFeatures::guidByType(const int type) const
{
  const Standard_GUID* guidPtr = m_types.Seek(type);
  if ( guidPtr != NULL )
    return (*guidPtr);
  else
    m_progress.SendLogMessage(LogWarn(Normal) << "Non-supported feature type.");

  return Standard_GUID();
}
