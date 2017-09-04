//-----------------------------------------------------------------------------
// Created on: 25 February 2016
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

#ifndef asiUI_TopoGraphItem_h
#define asiUI_TopoGraphItem_h

// asiAlgo includes
#include <asiAlgo_FeatureAngle.h>

// OCCT includes
#include <TopAbs_ShapeEnum.hxx>

// Mobius includes
#include <mobius/core.h>

// VTK includes
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

// Qt includes
#include <QObject>

#define ARRNAME_LABELS            "Labels"
#define ARRNAME_PIDS              "Pids"
#define ARRNAME_COLORS            "Colors"
#define ARRNAME_ANGLES            "Angles"
#define ARRNAME_CHILD_ORIENTATION "ChildOrientation"
#define ARRNAME_GROUP             "Group"
#define ARRNAME_GROUP_ORDINARY    "Ordinary"
#define ARRNAME_GROUP_HIGHLIGHTED "Highlighted"
//
#define ARRNAME_GROUP_COMPOUND    "Compound"
#define ARRNAME_GROUP_FACE        "Face"
#define ARRNAME_GROUP_WIRE        "Wire"
#define ARRNAME_GROUP_EDGE        "Edge"
#define ARRNAME_GROUP_VERTEX      "Vertex"
//
#define ARRNAME_CHILD_ORIENTATION_F 0
#define ARRNAME_CHILD_ORIENTATION_R 1
#define ARRNAME_CHILD_ORIENTATION_I 2
#define ARRNAME_CHILD_ORIENTATION_E 3

//! Item of topology graph.
class asiUI_TopoGraphItem : public QObject,
                            public vtkGraphItem
{
  Q_OBJECT

public:

  static asiUI_TopoGraphItem* New();
  vtkTypeMacro(asiUI_TopoGraphItem, vtkGraphItem);

  virtual ~asiUI_TopoGraphItem();

signals:

  void vertexPicked(const int              subShapeId,
                    const TopAbs_ShapeEnum subShapeType,
                    const vtkIdType        vertexId);

protected:

  vtkIdType focusedVertex;

  //---------------------------------------------------------------------------
  asiUI_TopoGraphItem()
  {
    focusedVertex = -1;
  }

  //---------------------------------------------------------------------------
  virtual vtkColor4ub VertexColor(vtkIdType vertex)
  {
    if ( vertex == focusedVertex )
      return vtkColor4ub(255, 0, 0, 255);

    vtkAbstractArray* domain = this->GetGraph()->GetVertexData()->GetAbstractArray(ARRNAME_GROUP);
    //
    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_HIGHLIGHTED )
      return vtkColor4ub(255, 255, 0, 255);

    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_COMPOUND )
      return vtkColor4ub(248, 31, 209, 255);

    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_FACE )
      return vtkColor4ub(61, 132, 218, 255);

    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_WIRE )
      return vtkColor4ub(150, 0, 0, 255);

    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_EDGE )
      return vtkColor4ub(255, 0, 0, 255);

    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_VERTEX )
      return vtkColor4ub(255, 242, 0, 255);

    return vtkColor4ub(175, 128, 128, 255);
  }

  //---------------------------------------------------------------------------
  virtual vtkStdString VertexTooltip(vtkIdType vertex)
  {
    vtkAbstractArray* labels = this->GetGraph()->GetVertexData()->GetAbstractArray(ARRNAME_LABELS);
    if ( labels )
      return labels->GetVariantValue(vertex).ToString();

    return "";
  }

  //---------------------------------------------------------------------------
  virtual vtkColor4ub EdgeColor(vtkIdType line, vtkIdType /*point*/)
  {
    // AAG only
    vtkAbstractArray* angles = this->GetGraph()->GetEdgeData()->GetAbstractArray(ARRNAME_ANGLES);
    if ( angles )
    {
      const int attr = angles->GetVariantValue(line).ToInt();
      if ( attr == Angle_Convex )
        return vtkColor4ub(40, 190, 0, 255);
      else if ( attr == Angle_Concave )
        return vtkColor4ub(190, 40, 0, 255);
    }

    // Topology graph only
    vtkAbstractArray* oris = this->GetGraph()->GetEdgeData()->GetAbstractArray(ARRNAME_CHILD_ORIENTATION);
    if ( oris )
    {
      const int attr = oris->GetVariantValue(line).ToInt();
      if ( attr == ARRNAME_CHILD_ORIENTATION_F )
        return vtkColor4ub(250, 70, 40, 255);
      else if ( attr == ARRNAME_CHILD_ORIENTATION_R )
        return vtkColor4ub(40, 170, 250, 255);
      else if ( attr == ARRNAME_CHILD_ORIENTATION_I )
        return vtkColor4ub(190, 190, 0, 255);
      else // EXTERNAL orientation
        return vtkColor4ub(0, 190, 190, 255);
    }

    return vtkColor4ub(128, 128, 128, 128);
  }

  //---------------------------------------------------------------------------
  virtual float EdgeWidth(vtkIdType /*line*/, vtkIdType /*point*/)
  {
    return 1.0f;
  }

  //---------------------------------------------------------------------------
  virtual float VertexSize(vtkIdType)
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

    // Get sub-shape ID
    TopAbs_ShapeEnum shapeType   = TopAbs_SHAPE;
    int              subShapeIdx = -1;
    //
    vtkAbstractArray* pids   = this->GetGraph()->GetVertexData()->GetAbstractArray(ARRNAME_PIDS);
    vtkAbstractArray* groups = this->GetGraph()->GetVertexData()->GetAbstractArray(ARRNAME_GROUP);
    //
    if ( pids )
    {
      subShapeIdx = pids->GetVariantValue(focusedVertex).ToInt();
    }
    //
    if ( groups )
    {
      if ( groups->GetVariantValue(focusedVertex).ToString() == ARRNAME_GROUP_FACE )
        shapeType = TopAbs_FACE;
      else if ( groups->GetVariantValue(focusedVertex).ToString() == ARRNAME_GROUP_EDGE )
        shapeType = TopAbs_EDGE;
      else if ( groups->GetVariantValue(focusedVertex).ToString() == ARRNAME_GROUP_VERTEX )
        shapeType = TopAbs_VERTEX;
    }

    emit vertexPicked(subShapeIdx, shapeType, focusedVertex);
    return true;
  }

  //---------------------------------------------------------------------------
  virtual bool KeyPressEvent(const vtkContextKeyEvent&)
  {
    return true;
  }

};

#endif
