//-----------------------------------------------------------------------------
// Created on: 30 March 2017
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

#ifndef asiVisu_PartDataProvider_h
#define asiVisu_PartDataProvider_h

// asiVisu includes
#include <asiVisu_DataProvider.h>
#include <asiVisu_ShapeColorSource.h>

// asiData includes
#include <asiData_PartNode.h>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

//! Data provider from Part Node to Presentation.
class asiVisu_PartDataProvider : public asiVisu_DataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_PartDataProvider, asiVisu_DataProvider)

public:

  asiVisu_EXPORT
    asiVisu_PartDataProvider(const Handle(asiData_PartNode)& partNode);

protected:

  asiVisu_EXPORT
    asiVisu_PartDataProvider();

public:

  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

  asiVisu_EXPORT virtual Handle(asiData_PartNode)
    GetPartNode() const;

  asiVisu_EXPORT virtual TopoDS_Shape
    GetShape() const;

  asiVisu_EXPORT virtual Handle(asiAlgo_AAG)
    GetAAG() const;

  asiVisu_EXPORT double
    GetLinearDeflection() const;

  asiVisu_EXPORT double
    GetAngularDeflection() const;

  asiVisu_EXPORT Handle(asiVisu_ShapeColorSource)
    PrepareColorSource();

  asiVisu_EXPORT void
    GetTransformation(double& tx, double& ty, double& tz,
                      double& rx, double& ry, double& rz) const;

  asiVisu_EXPORT vtkSmartPointer<vtkTransform>
    GetTransformation() const;

  asiVisu_EXPORT void
    GetColor(double& r, double& g, double& b) const;

public:

  asiVisu_EXPORT Handle(asiVisu_PartDataProvider)
    Clone() const;

protected:

  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  //! Source Node.
  Handle(asiData_PartNode) m_node;

  //! Source for colors.
  Handle(asiVisu_ShapeColorSource) m_colorSource;

};

#endif
