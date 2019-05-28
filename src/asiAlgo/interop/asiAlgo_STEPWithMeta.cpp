//-----------------------------------------------------------------------------
// Created on: 28 May 2019
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

// Own include
#include <asiAlgo_STEPWithMeta.h>

// OCCT includes
#include <Interface_EntityIterator.hxx>
#include <Interface_Static.hxx>
#include <STEPControl_ActorWrite.hxx>
#include <STEPControl_Controller.hxx>
#include <STEPConstruct.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepVisual_Colour.hxx>
#include <StepVisual_CurveStyle.hxx>
#include <StepVisual_MechanicalDesignGeometricPresentationRepresentation.hxx>
#include <StepVisual_PointStyle.hxx>
#include <StepVisual_PresentationStyleByContext.hxx>
#include <StepVisual_PresentationStyleSelect.hxx>
#include <StepVisual_StyledItem.hxx>
#include <StepVisual_SurfaceStyleUsage.hxx>
#include <TopoDS_Iterator.hxx>
#include <Transfer_TransientListBinder.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//-----------------------------------------------------------------------------

asiAlgo_STEPWithMeta::asiAlgo_STEPWithMeta(ActAPI_ProgressEntry progress,
                                           ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm (progress, plotter),
  m_bColorMode      (true)
{
  STEPControl_Controller::Init();
  Handle(XSControl_WorkSession) WS = new XSControl_WorkSession;
  this->Init(WS);
}

//-----------------------------------------------------------------------------

asiAlgo_STEPWithMeta::asiAlgo_STEPWithMeta(const Handle(XSControl_WorkSession)& WS,
                                           const bool                           scratch,
                                           ActAPI_ProgressEntry                 progress,
                                           ActAPI_PlotterEntry                  plotter)
: ActAPI_IAlgorithm (progress, plotter),
  m_bColorMode      (true)
{
  STEPControl_Controller::Init();
  this->Init(WS, scratch);
}

//-----------------------------------------------------------------------------

void asiAlgo_STEPWithMeta::Init(const Handle(XSControl_WorkSession)& WS,
                                const bool scratch)
{
  WS->SelectNorm("STEP");
  m_writer.SetWS(WS, scratch);
}

//-----------------------------------------------------------------------------

IFSelect_ReturnStatus asiAlgo_STEPWithMeta::Write(const char* filename)
{
  IFSelect_ReturnStatus status = m_writer.Write(filename);

  return status;
}

//-----------------------------------------------------------------------------

bool asiAlgo_STEPWithMeta::Transfer(const STEPControl_StepModelType mode)
{
  return this->transfer(m_writer, mode);
}

//-----------------------------------------------------------------------------

bool asiAlgo_STEPWithMeta::Perform(const TCollection_AsciiString& filename)
{
  if ( !this->Transfer() )
    return false;

  return this->Write( filename.ToCString() ) == IFSelect_RetDone;
}

//-----------------------------------------------------------------------------

bool asiAlgo_STEPWithMeta::transfer(STEPControl_Writer&             writer,
                                    const STEPControl_StepModelType mode)
{
  Handle(STEPControl_ActorWrite)
    Actor = Handle(STEPControl_ActorWrite)::DownCast( writer.WS()->NormAdaptor()->ActorWrite() );

  // Get application protocol.
  const int ap = Interface_Static::IVal("write.step.schema");

  // Get shape to write.
  TopoDS_Shape shape = m_input->GetShape();
  //
  if ( shape.IsNull() )
    return false;

  // Transfer shape.
  writer.Transfer(shape, mode, false);

  // Compute graph explicitly here.
  writer.WS()->ComputeGraph(true);

  // Write colors
  if ( m_bColorMode )
    this->writeColors( writer.WS() );

  // register all MDGPRs in model
  const Handle(Interface_InterfaceModel) &Model = writer.WS()->Model();
  MoniTool_DataMapIteratorOfDataMapOfShapeTransient anItr(m_mapCompMDGPR);
  for (; anItr.More(); anItr.Next())
    Model->AddWithRefs( anItr.Value() );

  Interface_Static::SetIVal ("write.step.schema", ap);

  // Refresh graph
  writer.WS()->ComputeGraph(true);

  return true;
}

//-----------------------------------------------------------------------------

static int FindEntities(const Handle(Transfer_FinderProcess) &FP,
                        const TopoDS_Shape &S,
                        TopLoc_Location &L,
                        TColStd_SequenceOfTransient &seqRI)
{
  Handle(StepRepr_RepresentationItem) item = STEPConstruct::FindEntity ( FP, S, L );

  if ( ! item.IsNull() ) {
    seqRI.Append ( item );
    return 1;
  }
      
  // may be S was splited during shape processing
  Handle(TransferBRep_ShapeMapper) mapper = TransferBRep::ShapeMapper ( FP, S );
  Handle(Transfer_Binder) bnd = FP->Find(mapper);
  if ( bnd.IsNull() ) return 0;
  
  Handle(Transfer_TransientListBinder) TransientListBinder =
    //Handle(Transfer_TransientListBinder)::DownCast( bnd->Next(true) );
    Handle(Transfer_TransientListBinder)::DownCast( bnd );
  int nres=0;
  if ( TransientListBinder.IsNull() && S.ShapeType() == TopAbs_COMPOUND) 
  {
    for ( TopoDS_Iterator it(S); it.More(); it.Next() ) {
      Handle(StepRepr_RepresentationItem) aLocalItem = STEPConstruct::FindEntity ( FP, it.Value(), L );
      if (aLocalItem.IsNull() ) continue;
      nres++;
      seqRI.Append (aLocalItem);
    }
  }
  else if(!TransientListBinder.IsNull())
  {

    const int nb = TransientListBinder->NbTransients();
    for (int i=1; i<=nb; i++) {
      Handle(Standard_Transient) t = TransientListBinder->Transient(i);
      item = Handle(StepRepr_RepresentationItem)::DownCast(t);
      if ( item.IsNull() ) continue;
      nres++;
      seqRI.Append ( item );
    }
  }
  return nres;
}

//-----------------------------------------------------------------------------

bool asiAlgo_STEPWithMeta::writeColors(const Handle(XSControl_WorkSession)& WS)
{
  STEPConstruct_Styles                        Styles(WS);
  STEPConstruct_DataMapOfAsciiStringTransient DPDCs;
  STEPConstruct_DataMapOfPointTransient       ColRGBs;

  // Iterate over all individual sub-shapes.
  const int numSubShapes = m_input->GetNumSubShapes();
  //
  for ( int i = 0; i < numSubShapes; ++i )
  {
    // Get a target shape and try to find corresponding representation context.
    TopoDS_Shape subShape = m_input->GetSubShape(i);
    //
    Handle(StepRepr_RepresentationContext) context = Styles.FindContext(subShape);
    //
    if ( context.IsNull() )
      continue;

    // Create STEP styles.
    Handle(StepVisual_StyledItem) override;
    TopTools_MapOfShape Map;

    this->makeSTEPStyles(Styles, subShape, override, Map, DPDCs, ColRGBs);

    // create MDGPR and record it in model
    Handle(StepVisual_MechanicalDesignGeometricPresentationRepresentation) aMDGPR;

    if ( m_mapCompMDGPR.IsBound(subShape) )
      m_progress.SendLogMessage(LogWarn(Normal) << "Current shape have MDGPR already.");

    Styles.CreateMDGPR(context, aMDGPR);

    if ( !aMDGPR.IsNull() )
      m_mapCompMDGPR.Bind(subShape, aMDGPR);
  }

  return true;
}

//-----------------------------------------------------------------------------

static Handle(StepRepr_ProductDefinitionShape) FindPDS(const Interface_Graph &theGraph,
                                                       const Handle(Standard_Transient) &theEnt,
                                                       Handle(StepRepr_RepresentationContext) &theRC)
{
  if (theEnt.IsNull())
    return NULL;
  Handle(StepRepr_ProductDefinitionShape) aPDS;

  // try to find shape_representation in sharings
  Interface_EntityIterator anIter = theGraph.Sharings(theEnt);
  for (anIter.Start(); anIter.More() && aPDS.IsNull(); anIter.Next()) {
    Handle(StepShape_ShapeRepresentation) aSR = Handle(StepShape_ShapeRepresentation)::DownCast(anIter.Value());
    if (aSR.IsNull())
      continue;
    theRC = aSR->ContextOfItems();
    Interface_EntityIterator aSDRIt = theGraph.Sharings(aSR);
    for (aSDRIt.Start(); aSDRIt.More() && aPDS.IsNull(); aSDRIt.Next()) {
      Handle(StepShape_ShapeDefinitionRepresentation) aSDR =
        Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(aSDRIt.Value());
      if (aSDR.IsNull()) continue;
      Handle(StepRepr_PropertyDefinition) aPropD = aSDR->Definition().PropertyDefinition();
      if (aPropD.IsNull()) continue;
      aPDS = Handle(StepRepr_ProductDefinitionShape)::DownCast(aPropD);
    }
  }
  if (!aPDS.IsNull())
    return aPDS;

  anIter = theGraph.Sharings(theEnt);
  for (anIter.Start(); anIter.More(); anIter.Next()) {
    if (anIter.Value()->IsKind(STANDARD_TYPE(StepShape_TopologicalRepresentationItem)) ||
      anIter.Value()->IsKind(STANDARD_TYPE(StepGeom_GeometricRepresentationItem)))
    {
      aPDS = FindPDS(theGraph, anIter.Value(), theRC);
      if (!aPDS.IsNull())
        return aPDS;
    }
  }

  return aPDS;
}

//-----------------------------------------------------------------------------

void asiAlgo_STEPWithMeta::SetColorMode (const bool colormode)
{
  m_bColorMode = colormode;
}

//-----------------------------------------------------------------------------

bool asiAlgo_STEPWithMeta::GetColorMode () const
{
  return m_bColorMode;
}

//-----------------------------------------------------------------------------

void asiAlgo_STEPWithMeta::makeSTEPStyles(STEPConstruct_Styles&                        Styles,
                                          const TopoDS_Shape&                          S,
                                          Handle(StepVisual_StyledItem)&               override,
                                          TopTools_MapOfShape&                         Map,
                                          STEPConstruct_DataMapOfAsciiStringTransient& DPDCs,
                                          STEPConstruct_DataMapOfPointTransient&       ColRGBs)
{
  // Skip already processed shapes.
  if ( !Map.Add(S) )
    return;

  Quantity_Color shapeColor = m_input->GetColor(S);

  // Translate colors to STEP.
  Handle(StepVisual_Colour) surfColor;
  if ( m_input->HasColor(S) )
    surfColor = Styles.EncodeColor(shapeColor, DPDCs, ColRGBs);
  
  bool hasOwn = !surfColor.IsNull();

  // Find target item and assign style to it.
  Handle(StepVisual_StyledItem) STEPstyle = override;
  //
  if ( hasOwn )
  {
    if ( S.ShapeType() != TopAbs_COMPOUND ) // Skip compounds, let subshapes inherit its colors.
    {
      TopLoc_Location L;
      TColStd_SequenceOfTransient seqRI;
      int nb = FindEntities(Styles.FinderProcess(), S, L, seqRI);

      if ( nb <= 0 )
        m_progress.SendLogMessage( LogWarn(Normal) << "Cannot find RI for %1."
                                                   << S.TShape()->DynamicType()->Name() );

      for ( int i = 1; i <= nb; ++i )
      {
        Handle(StepRepr_RepresentationItem)
          item = Handle(StepRepr_RepresentationItem)::DownCast( seqRI(i) );

        Handle(StepVisual_PresentationStyleAssignment) PSA;
        if ( !surfColor.IsNull() )
          PSA = Styles.MakeColorPSA(item, surfColor, NULL, false);
        else
        {
          // Default color is white.
          surfColor = Styles.EncodeColor(Quantity_Color(1, 1, 1, Quantity_TOC_RGB), DPDCs, ColRGBs);
          //
          PSA = Styles.MakeColorPSA(item, surfColor, NULL, false);
        }

        STEPstyle = Styles.AddStyle(item, PSA, override);
        hasOwn = false;
      }
    }
  }
}
