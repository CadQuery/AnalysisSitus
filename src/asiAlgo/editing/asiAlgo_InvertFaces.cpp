//-----------------------------------------------------------------------------
// Created on: 10 April 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
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
