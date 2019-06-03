//-----------------------------------------------------------------------------
// Created on: 02 June 2019
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
#include <asiAlgo_ReadSTEPWithMeta.h>

// OpenCascade includes
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <STEPControl_Controller.hxx>
#include <STEPConstruct.hxx>
#include <STEPConstruct_Styles.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_RepresentedDefinition.hxx>
#include <StepRepr_SpecifiedHigherUsageOccurrence.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>
#include <StepShape_ShapeRepresentation.hxx>
#include <StepVisual_PresentationStyleByContext.hxx>
#include <StepVisual_StyledItem.hxx>
#include <TopoDS_Iterator.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

//-----------------------------------------------------------------------------

asiAlgo_ReadSTEPWithMeta::asiAlgo_ReadSTEPWithMeta(ActAPI_ProgressEntry progress,
                                                   ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm (progress, plotter),
  m_bColorMode      (true)
{
  STEPControl_Controller::Init();
}

//-----------------------------------------------------------------------------

asiAlgo_ReadSTEPWithMeta::asiAlgo_ReadSTEPWithMeta(const Handle(XSControl_WorkSession)& WS,
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

void asiAlgo_ReadSTEPWithMeta::Init(const Handle(XSControl_WorkSession)& WS,
                                    const bool                           scratch)
{
  m_reader.SetWS(WS, scratch);
}

//-----------------------------------------------------------------------------

IFSelect_ReturnStatus asiAlgo_ReadSTEPWithMeta::ReadFile(const char* filename)
{
  return m_reader.ReadFile(filename);
}


//-----------------------------------------------------------------------------

int asiAlgo_ReadSTEPWithMeta::GetNbRootsForTransfer()
{
  return m_reader.NbRootsForTransfer();
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReadSTEPWithMeta::TransferOneRoot(const int num)
{
  TDF_LabelSequence Lseq;
  return this->transfer(m_reader, num);
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReadSTEPWithMeta::Transfer()
{
  TDF_LabelSequence Lseq;
  return this->transfer(m_reader, 0);
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReadSTEPWithMeta::Perform(const char* filename)
{
  if ( this->ReadFile(filename) != IFSelect_RetDone )
    return false;

  return this->Transfer();
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReadSTEPWithMeta::Perform(const TCollection_AsciiString& filename)
{
  if ( this->ReadFile( filename.ToCString() ) != IFSelect_RetDone )
    return false;

  return this->Transfer();
}

//-----------------------------------------------------------------------------

STEPControl_Reader& asiAlgo_ReadSTEPWithMeta::ChangeReader()
{
  return m_reader;
}

//-----------------------------------------------------------------------------

const STEPControl_Reader& asiAlgo_ReadSTEPWithMeta::GetReader() const
{
  return m_reader;
}

//-----------------------------------------------------------------------------

void asiAlgo_ReadSTEPWithMeta::SetColorMode(const bool colormode)
{
  m_bColorMode = colormode;
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReadSTEPWithMeta::GetColorMode() const
{
  return m_bColorMode;
}

//-----------------------------------------------------------------------------

static void FillShapesMap(const TopoDS_Shape& S, TopTools_MapOfShape& map)
{
  TopoDS_Shape S0 = S;
  TopLoc_Location loc;
  S0.Location(loc);
  map.Add(S0);
  if ( S.ShapeType() != TopAbs_COMPOUND ) return;
  for ( TopoDS_Iterator it(S); it.More(); it.Next() )
    FillShapesMap(it.Value(), map);
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReadSTEPWithMeta::transfer(STEPControl_Reader& reader,
                                        const int           nroot,
                                        const bool          asOne)
{
  reader.ClearShapes();
  int i;

  // Read all shapes
  int num = reader.NbRootsForTransfer();
  if (num <= 0) return false;
  if (nroot) {
    if (nroot > num) return false;
    reader.TransferOneRoot(nroot);
  }
  else {
    for (i = 1; i <= num; i++) reader.TransferOneRoot(i);
  }
  num = reader.NbShapes();
  if (num <= 0) return false;

  // Fill a map of (top-level) shapes resulting from that transfer
  // Only these shapes will be considered further
  TopTools_MapOfShape ShapesMap, NewShapesMap;
  for (i = 1; i <= num; i++) FillShapesMap(reader.Shape(i), ShapesMap);

  // Collect information on shapes originating from SDRs
  // this will be used to distinguish compounds representing assemblies
  // from the ones representing hybrid models and shape sets
  STEPCAFControl_DataMapOfShapePD ShapePDMap;
  STEPCAFControl_DataMapOfPDExternFile PDFileMap;
  Handle(Interface_InterfaceModel) Model = reader.Model();
  const Handle(Transfer_TransientProcess) &TP = reader.WS()->TransferReader()->TransientProcess();
  int nb = Model->NbEntities();

  Handle(TColStd_HSequenceOfTransient) SeqPDS = new TColStd_HSequenceOfTransient;

  for (i = 1; i <= nb; i++) {
    Handle(Standard_Transient) enti = Model->Value(i);
    if (enti->IsKind(STANDARD_TYPE(StepRepr_ProductDefinitionShape))) {
      // sequence for acceleration ReadMaterials
      SeqPDS->Append(enti);
    }
    if (enti->IsKind(STANDARD_TYPE(StepBasic_ProductDefinition))) {
      Handle(StepBasic_ProductDefinition) PD =
        Handle(StepBasic_ProductDefinition)::DownCast(enti);
      int index = TP->MapIndex(PD);
      if (index > 0) {
        Handle(Transfer_Binder) binder = TP->MapItem(index);
        TopoDS_Shape S = TransferBRep::ShapeResult(binder);
        if (!S.IsNull() && ShapesMap.Contains(S)) {
          NewShapesMap.Add(S);
          ShapePDMap.Bind(S, PD);
          //Handle(STEPCAFControl_ExternFile) EF;
          //PDFileMap.Bind(PD, EF);
        }
      }
    }
    if (enti->IsKind(STANDARD_TYPE(StepShape_ShapeRepresentation))) {
      int index = TP->MapIndex(enti);
      if (index > 0) {
        Handle(Transfer_Binder) binder = TP->MapItem(index);
        TopoDS_Shape S = TransferBRep::ShapeResult(binder);
        if (!S.IsNull() && ShapesMap.Contains(S))
          NewShapesMap.Add(S);
      }
    }
  }

  // Add shape to the document.
  if ( asOne )
    m_output->SetShape( reader.OneShape() ); // Set shape in the output.
  else {
    m_progress.SendLogMessage(LogErr(Normal) << "Transferring several shapes into assemblies is not currently supported.");
    return false;
  }

  // Read colors.
  if ( m_bColorMode )
    this->readColors( reader.WS() );

  return true;
}

//-----------------------------------------------------------------------------

static void findStyledSR(const Handle(StepVisual_StyledItem)&   style,
                         Handle(StepShape_ShapeRepresentation)& aSR)
{
  // search Shape Represenatation for component styled item
  for (int j = 1; j <= style->NbStyles(); j++) {
    Handle(StepVisual_PresentationStyleByContext) PSA =
      Handle(StepVisual_PresentationStyleByContext)::DownCast(style->StylesValue(j));
    if (PSA.IsNull())
      continue;
    StepVisual_StyleContextSelect aStyleCntxSlct = PSA->StyleContext();
    Handle(StepShape_ShapeRepresentation) aCurrentSR =
      Handle(StepShape_ShapeRepresentation)::DownCast(aStyleCntxSlct.Representation());
    if (aCurrentSR.IsNull())
      continue;
    aSR = aCurrentSR;
    break;
  }
}

//-----------------------------------------------------------------------------

bool asiAlgo_ReadSTEPWithMeta::readColors(const Handle(XSControl_WorkSession)& WS) const
{
  STEPConstruct_Styles Styles(WS);

  if ( !Styles.LoadStyles() )
  {
    m_progress.SendLogMessage(LogNotice(Normal) << "No styles are found in the model.");
    return false;
  }

  // searching for invisible items in the model
  Handle(TColStd_HSequenceOfTransient) aHSeqOfInvisStyle = new TColStd_HSequenceOfTransient;
  Styles.LoadInvisStyles(aHSeqOfInvisStyle);

  // parse and search for color attributes
  int nb = Styles.NbStyles();
  for (int i = 1; i <= nb; i++) {
    Handle(StepVisual_StyledItem) style = Styles.Style(i);
    if (style.IsNull()) continue;

    bool IsVisible = true;
    // check the visibility of styled item.
    for (int si = 1; si <= aHSeqOfInvisStyle->Length(); si++) {
      if (style != aHSeqOfInvisStyle->Value(si))
        continue;
      // found that current style is invisible.
      IsVisible = false;
      break;
    }

    Handle(StepVisual_Colour) SurfCol, BoundCol, CurveCol;
    // check if it is component style
    bool IsComponent = false;
    if (!Styles.GetColors(style, SurfCol, BoundCol, CurveCol, IsComponent) && IsVisible)
      continue;

    // find shape
    NCollection_Vector<Handle(Standard_Transient)> anItems;
    if (!style->Item().IsNull()) {
      anItems.Append(style->Item());
    }
    else if (!style->ItemAP242().Representation().IsNull()) {
      //special case for AP242: item can be Reprsentation
      Handle(StepRepr_Representation) aRepr = style->ItemAP242().Representation();
      for (int j = 1; j <= aRepr->Items()->Length(); j++)
        anItems.Append(aRepr->Items()->Value(j));
    }
    for (int itemIt = 0; itemIt < anItems.Length(); itemIt++) {
      TopoDS_Shape S = STEPConstruct::FindShape(Styles.TransientProcess(),
        Handle(StepRepr_RepresentationItem)::DownCast(anItems.Value(itemIt)));
      bool isSkipSHUOstyle = false;
      // take shape with real location.
      while (IsComponent) {
        // take SR of NAUO
        Handle(StepShape_ShapeRepresentation) aSR;
        findStyledSR(style, aSR);
        // search for SR along model
        if (aSR.IsNull())
          break;
        Interface_EntityIterator subs = WS->HGraph()->Graph().Sharings(aSR);
        Handle(StepShape_ShapeDefinitionRepresentation) aSDR;
        for (subs.Start(); subs.More(); subs.Next()) {
          aSDR = Handle(StepShape_ShapeDefinitionRepresentation)::DownCast(subs.Value());
          if (aSDR.IsNull())
            continue;
          StepRepr_RepresentedDefinition aPDSselect = aSDR->Definition();
          Handle(StepRepr_ProductDefinitionShape) PDS =
            Handle(StepRepr_ProductDefinitionShape)::DownCast(aPDSselect.PropertyDefinition());
          if (PDS.IsNull())
            continue;
          StepRepr_CharacterizedDefinition aCharDef = PDS->Definition();

          Handle(StepRepr_AssemblyComponentUsage) ACU =
            Handle(StepRepr_AssemblyComponentUsage)::DownCast(aCharDef.ProductDefinitionRelationship());
          if (ACU.IsNull())
            continue;
          // PTV 10.02.2003 skip styled item that refer to SHUO
          if (ACU->IsKind(STANDARD_TYPE(StepRepr_SpecifiedHigherUsageOccurrence))) {
            isSkipSHUOstyle = true;
            break;
          }
          Handle(StepRepr_NextAssemblyUsageOccurrence) NAUO =
            Handle(StepRepr_NextAssemblyUsageOccurrence)::DownCast(ACU);
          if (NAUO.IsNull())
            continue;
        }
        break;
      }
      if (isSkipSHUOstyle)
        continue; // skip styled item which refer to SHUO

      if (S.IsNull())
        continue;


      if (!SurfCol.IsNull() || !BoundCol.IsNull() || !CurveCol.IsNull())
      {
        Quantity_Color aSCol, aBCol, aCCol;
        if ( !SurfCol.IsNull() )
        {
          Styles.DecodeColor(SurfCol, aSCol);
          //
          m_output->SetColor(S, aSCol);
        }
        if ( !BoundCol.IsNull() )
        {
          Styles.DecodeColor(BoundCol, aBCol);
          //
          m_output->SetColor(S, aBCol);
        }
        if ( !CurveCol.IsNull() )
        {
          Styles.DecodeColor(CurveCol, aCCol);
          //
          m_output->SetColor(S, aBCol);
        }
      }
    }
  }
  return true;
}
