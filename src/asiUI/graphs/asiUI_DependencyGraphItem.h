//-----------------------------------------------------------------------------
// Created on: 11 August 2019
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

#ifndef asiUI_DependencyGraphItem_h
#define asiUI_DependencyGraphItem_h

// VTK includes
#pragma warning(push, 0)
#include <vtkAbstractArray.h>
#include <vtkColor.h>
#include <vtkContext2D.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>
#include <vtkDataSetAttributes.h>
#include <vtkGraph.h>
#include <vtkGraphItem.h>
#include <vtkStdString.h>
#include <vtkTextProperty.h>
#pragma warning(pop)

// Qt includes
#pragma warning(push, 0)
#include <QObject>
#pragma warning(pop)

//-----------------------------------------------------------------------------

#define ARRNAME_LABELS "Labels"
#define ARRNAME_GROUP  "Group"
//
#define ARRNAME_GROUP_TF_HEAVY "Heavy"
#define ARRNAME_GROUP_TF_LIGHT "Light"

//-----------------------------------------------------------------------------

//! Item of dependency graph.
class asiUI_DependencyGraphItem : public QObject,
                                  public vtkGraphItem
{
  Q_OBJECT

public:

  static asiUI_DependencyGraphItem* New();

  vtkTypeMacro(asiUI_DependencyGraphItem, vtkGraphItem);

  virtual ~asiUI_DependencyGraphItem();

signals:

  void vertexPicked(const vtkIdType vertexId);

protected:

  vtkIdType focusedVertex;

  //---------------------------------------------------------------------------
  asiUI_DependencyGraphItem()
  {
    focusedVertex = -1;
  }

  //---------------------------------------------------------------------------
  virtual float VertexSize(vtkIdType)
  {
    return 15.0f;
  }

  //---------------------------------------------------------------------------
  virtual vtkColor4ub VertexColor(vtkIdType vertex)
  {
    if ( vertex == focusedVertex )
      return vtkColor4ub(255, 255, 0, 255);

    vtkAbstractArray* groups = this->GetGraph()->GetVertexData()->GetAbstractArray(ARRNAME_GROUP);
    //
    if ( groups && groups->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_TF_HEAVY )
      return vtkColor4ub(255, 0, 0, 255);

    return vtkColor4ub(0, 255, 0, 255);
  }

  //---------------------------------------------------------------------------
  virtual vtkStdString VertexTooltip(vtkIdType vertex)
  {
    vtkAbstractArray* labels = this->GetGraph()->GetVertexData()->GetAbstractArray(ARRNAME_LABELS);
    //
    if ( labels )
      return labels->GetVariantValue(vertex).ToString();

    return "";
  }

  //---------------------------------------------------------------------------
  virtual vtkColor4ub EdgeColor(vtkIdType /*line*/, vtkIdType /*point*/)
  {
    return vtkColor4ub(128, 128, 128, 128);
  }

  //---------------------------------------------------------------------------
  virtual float EdgeWidth(vtkIdType /*line*/, vtkIdType /*point*/)
  {
    return 1.0f;
  }

  //---------------------------------------------------------------------------
  virtual void PaintBuffers(vtkContext2D* painter)
  {
    // Turn off the tooltip if the superclass turned it on
    this->PlaceTooltip(-1);

    this->Superclass::PaintBuffers(painter);

    if ( focusedVertex >= 0 )
    {
      painter->GetTextProp()->SetColor(1, 1, 1);
      painter->GetTextProp()->SetJustificationToCentered();
      painter->GetTextProp()->BoldOff();

      vtkVector2f    pos = this->VertexPosition(focusedVertex);
      vtkStdString label = this->VertexTooltip(focusedVertex);

      painter->GetTextProp()->SetFontSize(12);
      painter->DrawString(pos.GetX(), pos.GetY(), label);
    }
  }

  //---------------------------------------------------------------------------
  virtual bool MouseButtonPressEvent(const vtkContextMouseEvent& evt)
  {
    this->Superclass::MouseButtonPressEvent(evt);

    focusedVertex = this->HitVertex( evt.GetPos() );

    this->GetGraph()->Modified();
    this->GetScene()->SetDirty(true);

    emit vertexPicked(focusedVertex);
    return true;
  }

  //---------------------------------------------------------------------------
  virtual bool KeyPressEvent(const vtkContextKeyEvent&)
  {
    return true;
  }

};

#endif
