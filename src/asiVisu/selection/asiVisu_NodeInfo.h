//-----------------------------------------------------------------------------
// Created on: 16 November 2015
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

#ifndef asiVisu_NodeInfo_h
#define asiVisu_NodeInfo_h

// A-Situs includes
#include <asiVisu.h>

// Active Data (API) includes
#include <ActAPI_INode.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// VTK forward declarations
class vtkActor;
class vtkDataSet;
class vtkInformationObjectBaseKey;

//! VTK holder for Node ID which can be passed through visualization
//! pipelines in order to resolve Nodes by their corresponding actors.
class asiVisu_NodeInfo : public vtkObject
{
public:

  vtkTypeMacro(asiVisu_NodeInfo, vtkObject);

  asiVisu_EXPORT static asiVisu_NodeInfo*
    New();

public:

  asiVisu_EXPORT void
    SetNodeId(const ActAPI_DataObjectId& nodeId);

  asiVisu_EXPORT ActAPI_DataObjectId
    GetNodeId() const;

public:

  asiVisu_EXPORT static vtkInformationObjectBaseKey*
    GetKey();

  asiVisu_EXPORT static asiVisu_NodeInfo*
    Retrieve(vtkActor* actor);

  asiVisu_EXPORT static void
    Store(const ActAPI_DataObjectId& nodeId,
          vtkActor*                  actor);

protected:

  asiVisu_NodeInfo();
  asiVisu_NodeInfo(const asiVisu_NodeInfo&);
  asiVisu_NodeInfo& operator=(const asiVisu_NodeInfo&);
  ~asiVisu_NodeInfo();

protected:

  //! Information key.
  static vtkSmartPointer<vtkInformationObjectBaseKey> m_key;

  //! Node ID wrapped for transferring through a pipeline.
  ActAPI_DataObjectId m_nodeId;

};

#endif
