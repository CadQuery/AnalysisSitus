//-----------------------------------------------------------------------------
// Created on: 27 May 2019
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
#include <asiUI_ParameterEditorListenerDefault.h>

// asiEngine includes
#include <asiEngine_Part.h>

//-----------------------------------------------------------------------------

asiUI_ParameterEditorListenerDefault::asiUI_ParameterEditorListenerDefault(const Handle(asiUI_CommonFacilities)& cf)
: asiUI_ParameterEditorListener(cf)
{}

//-----------------------------------------------------------------------------

void asiUI_ParameterEditorListenerDefault::beforeParameterChanged(const Handle(ActAPI_INode)& N,
                                                                  const int                   pid,
                                                                  const QVariant&             value,
                                                                  bool&                       proceedDefault)
{
  /* =============================
   *  Customization for Part Node
   * ============================= */

  if ( N->IsKind( STANDARD_TYPE(asiData_PartNode) ) )
  {
    Handle(asiData_PartNode) part_n = Handle(asiData_PartNode)::DownCast(N);

    // Assignment of color is customized.
    if ( pid == asiData_PartNode::PID_Color )
    {
      // Customization of color assignment in the Part Node consists in
      // creating a metadata element in case if any face is selected. If,
      // however, there is no active face, this callback simply returns true
      // to let the base class proceed as usual.
      // ...

      asiEngine_Part partApi( m_cf->Model,
                              m_cf->ViewerPart->PrsMgr(),
                              m_cf->Progress,
                              m_cf->Plotter );

      // Get IDs of the highlighted faces.
      TColStd_PackedMapOfInteger fids;
      partApi.GetHighlightedFaces(fids);
      //
      if ( !fids.Extent() )
      {
        proceedDefault = true; // Let the base class do its usual job.
        return;
      }

      // If we are here, then there is at least one active face to attach the
      // custom color to.
      // ...

      for ( TColStd_MapIteratorOfPackedMapOfInteger fit(fids); fit.More(); fit.Next() )
      {
        const int fid = fit.Key();

        // Get the face in question.
        TopoDS_Face F = partApi.GetFace(fid);

        // Find or create Metadata Element for this face.
        Handle(asiData_ElemMetadataNode) meta_n = partApi.FindElemMetadata(F, true);

        // Set color.
        meta_n->SetColor( value.toInt() );

        // Set reference list to metadata in the Part Node modified to allow for
        // actualization in 3D when the existing face was recolored.
        m_cf->Model->GetPartNode()->Parameter(asiData_PartNode::PID_MetadataElems)->SetModified();
      }

      // Set update type for Object Browser. Here we need to repopulate the
      // tree as new object appears.
      m_obUpdateType = OB_UpdateType_All;

      // Log message.
      m_cf->Progress.SendLogMessage( LogInfo(Normal) << "Parameter (pid '%1') changed (new value is '%2')."
                                                     << pid << QStr2AsciiStr( value.toString() ) );

      proceedDefault = false; // Do not allow any actions on the Part Node by the base class.
      return;
    }
  }

  proceedDefault = true; // Let the base class do its usual job.
}
