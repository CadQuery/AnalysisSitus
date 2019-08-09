//-----------------------------------------------------------------------------
// Created on: 22 June 2017
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoExe_GraphItem_h
#define ottoExe_GraphItem_h

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
#include <QObject>

//-----------------------------------------------------------------------------

#define ARRNAME_LABELS "Labels"
#define ARRNAME_GROUP  "Group"
//
#define ARRNAME_GROUP_TF_HEAVY "Heavy"
#define ARRNAME_GROUP_TF_LIGHT "Light"

//-----------------------------------------------------------------------------

//! Item of dependency graph.
class ottoExe_GraphItem : public QObject,
                          public vtkGraphItem
{
  Q_OBJECT

public:

  static ottoExe_GraphItem* New();

  vtkTypeMacro(ottoExe_GraphItem, vtkGraphItem);

  virtual ~ottoExe_GraphItem();

signals:

  void vertexPicked(const vtkIdType vertexId);

protected:

  vtkIdType focusedVertex;

  //---------------------------------------------------------------------------
  ottoExe_GraphItem()
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
