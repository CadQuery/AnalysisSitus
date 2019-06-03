//-----------------------------------------------------------------------------
// Created on: 21 December (*) 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017-present, Sergey Slyadnev
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

#ifndef asiUI_HistoryGraphItem_h
#define asiUI_HistoryGraphItem_h

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

// Indices of items in the algorithmic history graph
#define ARRNAME_HISTORY_ITEM_IDS "HistoryItemIds"

// Sub-shape state is encoded by nodes
#define ARRNAME_HISTORY_SUBSHAPE_STATES "HistorySubshapeState"
//
#define ARRNAME_HISTORY_SUBSHAPE_STATE_INACTIVE 0
#define ARRNAME_HISTORY_SUBSHAPE_STATE_ACTIVE   1
#define ARRNAME_HISTORY_SUBSHAPE_STATE_DELETED  2
#define ARRNAME_HISTORY_SUBSHAPE_STATE_ROOT     3

// Evolution type is encoded by arcs
#define ARRNAME_HISTORY_EVOLUTION_TYPES "HistoryEvolutionType"
//
#define ARRNAME_HISTORY_EVOLUTION_TYPE_MODIFIED 0
#define ARRNAME_HISTORY_EVOLUTION_TYPE_GENERATED 1

// Labels
#define ARRNAME_HISTORY_SUBSHAPE_LABELS "HistorySubshapeLabels"

//! Item of history graph.
class asiUI_HistoryGraphItem : public QObject,
                               public vtkGraphItem
{
  Q_OBJECT

public:

  static asiUI_HistoryGraphItem* New();
  vtkTypeMacro(asiUI_HistoryGraphItem, vtkGraphItem);

  virtual ~asiUI_HistoryGraphItem();

signals:

  void vertexPicked(const int graphNodeId, const vtkIdType vertexId);

protected:

  vtkIdType focusedVertex;

  //---------------------------------------------------------------------------
  asiUI_HistoryGraphItem()
  {
    focusedVertex = -1;
  }

  //---------------------------------------------------------------------------
  virtual vtkColor4ub VertexColor(vtkIdType vertex)
  {
    if ( vertex == focusedVertex )
      return vtkColor4ub(255, 0, 0, 255);

    vtkAbstractArray*
      state = this->GetGraph()->GetVertexData()->GetAbstractArray(ARRNAME_HISTORY_SUBSHAPE_STATES);
    //
    if ( state )
    {
      if ( state->GetVariantValue(vertex).ToInt() == ARRNAME_HISTORY_SUBSHAPE_STATE_ACTIVE )
        return vtkColor4ub(55, 255, 0, 255);
      if ( state->GetVariantValue(vertex).ToInt() == ARRNAME_HISTORY_SUBSHAPE_STATE_DELETED )
        return vtkColor4ub(255, 55, 0, 255);
      if ( state->GetVariantValue(vertex).ToInt() == ARRNAME_HISTORY_SUBSHAPE_STATE_ROOT )
        return vtkColor4ub(255, 0, 255, 255);
    }

    return vtkColor4ub(128, 128, 128, 255);
  }

  //---------------------------------------------------------------------------
  virtual vtkStdString VertexTooltip(vtkIdType vertex)
  {
    vtkAbstractArray* labels = this->GetGraph()->GetVertexData()->GetAbstractArray(ARRNAME_HISTORY_SUBSHAPE_LABELS);
    if ( labels )
      return labels->GetVariantValue(vertex).ToString();

    return "";
  }

  //---------------------------------------------------------------------------
  virtual vtkColor4ub EdgeColor(vtkIdType line, vtkIdType /*point*/)
  {
    vtkAbstractArray*
      evol = this->GetGraph()->GetEdgeData()->GetAbstractArray(ARRNAME_HISTORY_EVOLUTION_TYPES);
    //
    if ( evol )
    {
      const int attr = evol->GetVariantValue(line).ToInt();
      if ( attr == ARRNAME_HISTORY_EVOLUTION_TYPE_MODIFIED )
        return vtkColor4ub(255, 0, 0, 255);
      else if ( attr == ARRNAME_HISTORY_EVOLUTION_TYPE_GENERATED )
        return vtkColor4ub(0, 55, 255, 255);
    }

    return vtkColor4ub(128, 128, 128, 128);
  }

  //---------------------------------------------------------------------------
  virtual float EdgeWidth(vtkIdType /*line*/, vtkIdType /*point*/)
  {
    return 1.0f;
  }

  //---------------------------------------------------------------------------
  virtual float VertexSize(vtkIdType /*vertex*/)
  {
    return 12.0f;
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

      painter->GetTextProp()->SetFontSize(14);
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

    // Get array of IDs
    vtkAbstractArray*
      ids = this->GetGraph()->GetVertexData()->GetAbstractArray(ARRNAME_HISTORY_ITEM_IDS);
    //
    int graphNodeId = 0;
    //
    if ( ids )
    {
      graphNodeId = ids->GetVariantValue(focusedVertex).ToInt();
    }

    emit vertexPicked(graphNodeId, focusedVertex);
    return true;
  }

  //---------------------------------------------------------------------------
  virtual bool KeyPressEvent(const vtkContextKeyEvent&)
  {
    return true;
  }

};

#endif
