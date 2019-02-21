//-----------------------------------------------------------------------------
// Created on: 25 February 2016
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

#ifndef asiUI_PartGraphItem_h
#define asiUI_PartGraphItem_h

// asiAlgo includes
#include <asiAlgo_FeatureAngleType.h>

// OCCT includes
#include <TopAbs_ShapeEnum.hxx>

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
#define ARRNAME_CHILD_LOCATION    "ChildLocation"
#define ARRNAME_GROUP             "Group"
#define ARRNAME_GROUP_ORDINARY    "Ordinary"
#define ARRNAME_GROUP_HIGHLIGHTED "Highlighted"
#define ARRNAME_GROUP_ATTRIBUTED  "Attributed"
//
#define ARRNAME_GROUP_COMPOUND  "Compound"
#define ARRNAME_GROUP_COMPSOLID "CompSolid"
#define ARRNAME_GROUP_SOLID     "Solid"
#define ARRNAME_GROUP_SHELL     "Shell"
#define ARRNAME_GROUP_FACE      "Face"
#define ARRNAME_GROUP_WIRE      "Wire"
#define ARRNAME_GROUP_EDGE      "Edge"
#define ARRNAME_GROUP_VERTEX    "Vertex"
//
#define ARRNAME_CHILD_ORIENTATION_F 0
#define ARRNAME_CHILD_ORIENTATION_R 1
#define ARRNAME_CHILD_ORIENTATION_I 2
#define ARRNAME_CHILD_ORIENTATION_E 3

//! Item of topology or adjacency graph.
class asiUI_PartGraphItem : public QObject,
                            public vtkGraphItem
{
  Q_OBJECT

public:

  static asiUI_PartGraphItem* New();
  vtkTypeMacro(asiUI_PartGraphItem, vtkGraphItem);

  virtual ~asiUI_PartGraphItem();

signals:

  void vertexPicked(const int              subShapeId,
                    const TopAbs_ShapeEnum subShapeType,
                    const vtkIdType        vertexId);

public:

  void SetColorizeByLocations(const bool isOn)
  {
    m_bColorizeLoc = isOn;
  }

protected:

  vtkIdType focusedVertex;

  //---------------------------------------------------------------------------
  asiUI_PartGraphItem()
  {
    focusedVertex  = -1;
    m_bColorizeLoc = false;
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
    //
    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_COMPOUND )
      return vtkColor4ub(218, 0, 170, 255);
    //
    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_COMPSOLID )
      return vtkColor4ub(228, 11, 180, 255);
    //
    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_SOLID )
      return vtkColor4ub(238, 21, 190, 255);
    //
    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_SHELL )
      return vtkColor4ub(248, 31, 209, 255);
    //
    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_FACE )
      return vtkColor4ub(61, 132, 218, 255);
    //
    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_WIRE )
      return vtkColor4ub(150, 0, 0, 255);
    //
    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_EDGE )
      return vtkColor4ub(255, 0, 0, 255);
    //
    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_VERTEX )
      return vtkColor4ub(255, 242, 0, 255);

    // For AAG nodes only.
    if ( domain && domain->GetVariantValue(vertex).ToString() == ARRNAME_GROUP_ATTRIBUTED )
      return vtkColor4ub(0, 100, 255, 255);

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
    if ( m_bColorizeLoc )
    {
      vtkAbstractArray*
        locations = this->GetGraph()->GetEdgeData()->GetAbstractArray(ARRNAME_CHILD_LOCATION);
      //
      if ( locations )
      {
        vtkVariant val = locations->GetVariantValue(line);

        if ( val.IsValid() )
        {
          vtkStdString loc = locations->GetVariantValue(line).ToString();
          //
          if ( loc.length() )
            return vtkColor4ub(255, 255, 0, 255);
        }

        return vtkColor4ub(45, 45, 45, 255);
      }
    }
    else
    {
      // AAG only
      vtkAbstractArray* angles = this->GetGraph()->GetEdgeData()->GetAbstractArray(ARRNAME_ANGLES);
      if ( angles )
      {
        const int attr = angles->GetVariantValue(line).ToInt();
        if ( attr == FeatureAngleType_Convex )
          return vtkColor4ub(40, 190, 0, 255);
        else if ( attr == FeatureAngleType_Concave )
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

protected:

  bool m_bColorizeLoc; //!< Indicates whether to colorize vertices by sub-shape locations.

};

#endif
