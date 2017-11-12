//-----------------------------------------------------------------------------
// Created on: 10 April 2017
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

// Own include
#include <asiAlgo_InvertFaces.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopoDS_Iterator.hxx>

//-----------------------------------------------------------------------------

asiAlgo_InvertFaces::asiAlgo_InvertFaces(const Handle(asiAlgo_AAG)& aag,
                                         ActAPI_ProgressEntry       progress,
                                         ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm(progress, plotter),
  m_aag(aag)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_InvertFaces::Perform(const TColStd_PackedMapOfInteger& faceIds)
{
  const TopoDS_Shape& master = m_aag->GetMasterCAD();

  if ( master.ShapeType() == TopAbs_FACE )
  {
    m_result = master.Reversed();
    m_progress.SendLogMessage(LogInfo(Normal) << "Reverse isolated face");
  }
  else if ( master.ShapeType() > TopAbs_FACE )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot invert a shape of a topological "
                                                "type which does not contain faces");
    return false;
  }
  else
  {
    // Prepare root
    m_result = master.EmptyCopied();

    // Rebuild topology graph recursively
    this->buildTopoGraphLevel(master, faceIds, m_result);
  }

  return true; // Success
}

//-----------------------------------------------------------------------------

void asiAlgo_InvertFaces::buildTopoGraphLevel(const TopoDS_Shape&               root,
                                              const TColStd_PackedMapOfInteger& faces2Invert,
                                              TopoDS_Shape&                     result) const
{
  BRep_Builder BB;
  for ( TopoDS_Iterator it(root, false, false); it.More(); it.Next() )
  {
    const TopoDS_Shape& currentShape = it.Value();
    TopoDS_Shape newResult;

    if ( currentShape.ShapeType() < TopAbs_FACE )
    {
      newResult = currentShape.EmptyCopied();
      this->buildTopoGraphLevel( currentShape, faces2Invert, newResult );
    }
    else
    {
      if ( currentShape.ShapeType() == TopAbs_FACE )
      {
        // Get index of the current face
        const int fid = m_aag->GetFaceId(currentShape);

        // Reverse if requested
        newResult = ( faces2Invert.Contains(fid) ) ? currentShape.Reversed() : currentShape;
      }
      else
        newResult = currentShape;
    }
    BB.Add(result, newResult);
  }
}
