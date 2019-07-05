//-----------------------------------------------------------------------------
// Created on: 03 August 2018
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

// Own include
#include <asiAlgo_InvertShells.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopoDS_Iterator.hxx>

//-----------------------------------------------------------------------------

asiAlgo_InvertShells::asiAlgo_InvertShells(const TopoDS_Shape&  shape,
                                           ActAPI_ProgressEntry progress,
                                           ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter),
  m_shape(shape)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_InvertShells::Perform()
{
  if ( m_shape.ShapeType() == TopAbs_SHELL ||
       m_shape.ShapeType() == TopAbs_FACE )
  {
    m_result = BRepBuilderAPI_Copy(m_shape).Shape().Reversed();

    m_progress.SendLogMessage(LogInfo(Normal) << "Reverse isolated shell (face).");
  }
  else if ( m_shape.ShapeType() > TopAbs_FACE )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot invert a shape of a topological "
                                                "type which does not contain shells or faces.");
    return false;
  }
  else
  {
    // Prepare root.
    m_result = m_shape.EmptyCopied();

    // Rebuild topology graph recursively.
    this->buildTopoGraphLevel(m_shape, m_result);
  }

  return true; // Success.
}

//-----------------------------------------------------------------------------

void asiAlgo_InvertShells::buildTopoGraphLevel(const TopoDS_Shape& root,
                                               TopoDS_Shape&       result) const
{
  BRep_Builder BB;

  // NOTICE: we enable accumulation of locations because TopoDS_Builder::Add()
  //         recomputes the relative transformations. So if we do not accumulate
  //         transformations here, we will have an improperly placed result.
  //         E.g., imagine that your root shape is transformed. Then, all its
  //         children will be reverted by OpenCascade because of the logic in
  //         TopoDS_Builder::Add().
  for ( TopoDS_Iterator it(root, false, true); it.More(); it.Next() )
  {
    const TopoDS_Shape& currentShape = it.Value();
    TopoDS_Shape newResult;

    if ( currentShape.ShapeType() < TopAbs_SHELL )
    {
      newResult = currentShape.EmptyCopied();

      this->buildTopoGraphLevel(currentShape, newResult);
    }
    else
    {
      if ( currentShape.ShapeType() == TopAbs_SHELL )
        newResult = currentShape.Reversed(); // Reverse.
      else
        newResult = currentShape;
    }
    BB.Add(result, newResult);
  }
}
