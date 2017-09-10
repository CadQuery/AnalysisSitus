//-----------------------------------------------------------------------------
// Created on: 30 March 2016
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

#ifndef asiVisu_PDomainSource_h
#define asiVisu_PDomainSource_h

// OCCT includes
#include <TopoDS_Face.hxx>

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//! Source of polygonal data representing face domain.
class asiVisu_PDomainSource : public vtkPolyDataAlgorithm
{
// RTTI and construction:
public:

  vtkTypeMacro(asiVisu_PDomainSource, vtkPolyDataAlgorithm);
  static asiVisu_PDomainSource* New();

// Kernel methods:
public:

  void SetFace          (const TopoDS_Face& face);
  void SetPCurveModeOn  ();
  void Set3DCurveModeOn ();
  void SetTipNorm       (const gp_Vec& tipNorm);

protected:

  virtual int RequestData(vtkInformation*        request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector*  outputVector);

protected:

  double computeTipSize() const;

protected:

  asiVisu_PDomainSource();
  ~asiVisu_PDomainSource();

private:

  asiVisu_PDomainSource(const asiVisu_PDomainSource&);
  asiVisu_PDomainSource& operator=(const asiVisu_PDomainSource&);

private:

  TopoDS_Face m_face;        //!< Face to convert.
  bool        m_bPCurveMode; //!< Indicates whether p-curve mode is activated.
  gp_Vec      m_tipNorm;     //!< Optional normal vector for tips.

};

#endif
