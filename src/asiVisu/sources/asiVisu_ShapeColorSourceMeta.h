//-----------------------------------------------------------------------------
// Created on: 28 May 2019
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

#ifndef asiVisu_ShapeColorSourceMeta_h
#define asiVisu_ShapeColorSourceMeta_h

// asiVisu includes
#include <asiVisu_ShapeColorSource.h>
#include <asiVisu_Utils.h>

// asiAlgo includes
#include <asiAlgo_ShapePartnerHasher.h>

// asiData includes
#include <asiData_MetadataNode.h>
#include <asiData_PartNode.h>

// OpenCascade includes
#include <NCollection_DataMap.hxx>

//-----------------------------------------------------------------------------

//! Data provider interface for colors associated with sub-shapes. This
//! implementation of the basic interface takes colors from the Metadata
//! Element Nodes.
class asiVisu_ShapeColorSourceMeta : public asiVisu_ShapeColorSource
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_ShapeColorSourceMeta, asiVisu_ShapeColorSource)

public:

  //! Ctor accepting Metadata Node whose sub-Nodes are the sources of
  //! color attributes.
  //! \param[in] node Metadata Node.
  asiVisu_ShapeColorSourceMeta(const Handle(asiData_MetadataNode)& node)
  : asiVisu_ShapeColorSource (),
    m_metaNode               (node)
  {
    // Prepare cache.
    for ( Handle(ActAPI_IChildIterator) cit = m_metaNode->GetChildIterator();
          cit->More(); cit->Next() )
    {
      Handle(asiData_ElemMetadataNode)
        emn = Handle(asiData_ElemMetadataNode)::DownCast( cit->Value() );

      TopoDS_Shape shape  = emn->GetShape();
      const int    icolor = emn->GetColor();

      m_shapeColorMap.Bind(shape, icolor);
    }

    // Get Part Node for faster access.
    m_partNode = Handle(asiData_PartNode)::DownCast( m_metaNode->GetParentNode() );
  }

public:

  //! Returns color (as an integer value) for the given face ID. This data
  //! source uses internal cache to access colors by faces. Such caching
  //! technique is more efficient than searching in the persistent storage.
  //! \param[in] faceId one-based ID of a face.
  //! \return color.
  virtual int GetFaceColor(const int faceId) const
  {
    Handle(asiAlgo_AAG) aag = m_partNode->GetAAG();
    //
    if ( aag.IsNull() )
      return -1; // nullptr check.

    // Check if the ID in question is valid.
    if ( !aag->HasFace(faceId) )
      return -1;

    // Get face by its ID.
    const TopoDS_Face& face = aag->GetFace(faceId);

    // Find metadata element for the shape in question.
    const int* pColor = m_shapeColorMap.Seek(face);
    //
    if ( !pColor )
      return -1;

    // Return color packed as integer value.
    return *pColor;
  }

protected:

  //! Cache of shapes versus colors.
  NCollection_DataMap<TopoDS_Shape, int, asiAlgo_ShapePartnerHasher> m_shapeColorMap;

  //! Part Node.
  Handle(asiData_PartNode) m_partNode;

  //! Metadata Node.
  Handle(asiData_MetadataNode) m_metaNode;

};

#endif
