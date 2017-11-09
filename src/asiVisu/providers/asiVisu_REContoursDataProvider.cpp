//-----------------------------------------------------------------------------
// Created on: 06 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_REContoursDataProvider.h>

// asiData includes
#include <asiData_REContourNode.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param contours_n [in] Contours Node.
asiVisu_REContoursDataProvider::asiVisu_REContoursDataProvider(const Handle(asiData_REContoursNode)& contours_n)
: asiVisu_DataProvider(),
  m_contours(contours_n)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_REContoursDataProvider::GetNodeID() const
{
  if ( m_contours.IsNull() || !m_contours->IsWellFormed() )
    return ActAPI_DataObjectId();

  return m_contours->GetId();
}

//-----------------------------------------------------------------------------

//! \return number of stored contours.
int asiVisu_REContoursDataProvider::GetNumOfContours() const
{
  if ( m_contours.IsNull() || !m_contours->IsWellFormed() )
    return 0;

  // Iterate over the child Nodes which are the individual contours
  int nbContours = 0;
  for ( Handle(ActAPI_IChildIterator) cit = m_contours->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_REContourNode) contour_n = Handle(asiData_REContourNode)::DownCast( cit->Value() );
    //
    if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
      continue;

    ++nbContours;
  }

  return nbContours;
}

//! Returns a contour by its one-based index.
//! \param oneBased_idx [in] 1-based index of the contour to access.
TopoDS_Wire
  asiVisu_REContoursDataProvider::GetContour(const int oneBased_idx) const
{
  if ( m_contours.IsNull() || !m_contours->IsWellFormed() )
    return TopoDS_Wire();

  // Access individual contour
  Handle(asiData_REContourNode)
    contour_n = Handle(asiData_REContourNode)::DownCast( m_contours->GetChildNode(oneBased_idx) );
  //
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
    return TopoDS_Wire();

  return contour_n->GetContour();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_REContoursDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  if ( m_contours.IsNull() || !m_contours->IsWellFormed() )
    return out;

  // Iterate over the child Nodes which are the individual contours
  for ( Handle(ActAPI_IChildIterator) cit = m_contours->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_REContourNode) contour_n = Handle(asiData_REContourNode)::DownCast( cit->Value() );
    //
    if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
      continue;

    out << contour_n->Parameter(asiData_REContourNode::PID_Contour);
  }

  return out;
}
