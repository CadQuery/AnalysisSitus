//-----------------------------------------------------------------------------
// Created on: 11 July 2017
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

#ifndef asiVisu_TriangulationNodeInfo_h
#define asiVisu_TriangulationNodeInfo_h

// asiVisu includes
#include <asiVisu_NodeInfo.h>

//! This is a sub-class for generic asiVisu_NodeInfo. Use this key to
//! distinguish between triangulation presentations and other presentations
//! bounded to Active Data Nodes.
class asiVisu_TriangulationNodeInfo : public asiVisu_NodeInfo
{
public:

  vtkTypeMacro(asiVisu_TriangulationNodeInfo, vtkObject);

  asiVisu_EXPORT static asiVisu_TriangulationNodeInfo*
    New();

public:

  asiVisu_EXPORT static vtkInformationObjectBaseKey*
    GetKey();

  asiVisu_EXPORT static asiVisu_TriangulationNodeInfo*
    Retrieve(vtkActor* actor);

  asiVisu_EXPORT static void
    Store(const ActAPI_DataObjectId& nodeId,
          vtkActor*                  actor);

private:

  asiVisu_TriangulationNodeInfo();
  asiVisu_TriangulationNodeInfo(const asiVisu_TriangulationNodeInfo&);
  asiVisu_TriangulationNodeInfo& operator=(const asiVisu_TriangulationNodeInfo&);
  ~asiVisu_TriangulationNodeInfo();

};

#endif
