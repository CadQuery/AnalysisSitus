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

#ifndef asiAlgo_ReadSTEPWithMeta_h
#define asiAlgo_ReadSTEPWithMeta_h

// asiAlgo includes
#include <asiAlgo_ReadSTEPWithMetaOutput.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OpenCascade includes
#include <STEPControl_Reader.hxx>
#include <XSControl_WorkSession.hxx>

///
#include <IFSelect_ReturnStatus.hxx>
#include <Standard_CString.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopTools_MapOfShape.hxx>
#include <STEPCAFControl_DataMapOfShapePD.hxx>
#include <STEPCAFControl_DataMapOfPDExternFile.hxx>
#include <XCAFDoc_DataMapOfShapeLabel.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <XCAFDimTolObjects_DatumModifiersSequence.hxx>
#include <XCAFDimTolObjects_DatumModifWithValue.hxx>
class XSControl_WorkSession;
class TDocStd_Document;
class TCollection_AsciiString;
class STEPCAFControl_ExternFile;
class STEPControl_Reader;
class TDF_Label;
class TopoDS_Shape;
class XCAFDoc_ShapeTool;
class StepRepr_RepresentationItem;
class Transfer_TransientProcess;
class StepShape_ConnectedFaceSet;
class StepRepr_NextAssemblyUsageOccurrence;
class STEPConstruct_Tool;
class StepDimTol_Datum;

//! STEP reader enriched with possibility to read not only shapes but
//! also some metadata such as colors.
class asiAlgo_ReadSTEPWithMeta : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_ReadSTEPWithMeta, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_ReadSTEPWithMeta(ActAPI_ProgressEntry progress = nullptr,
                             ActAPI_PlotterEntry  plotter  = nullptr);

  asiAlgo_EXPORT
    asiAlgo_ReadSTEPWithMeta(const Handle(XSControl_WorkSession)& WS,
                             const bool                           scratch = true,
                             ActAPI_ProgressEntry                 progress = nullptr,
                             ActAPI_PlotterEntry                  plotter  = nullptr);

public:

  //! Sets output data adaptor.
  //! \param[in] output output data adaptor.
  void SetOutput(const Handle(asiAlgo_ReadSTEPWithMetaOutput)& output)
  {
    m_output = output;
  }

public:

  //! Initializes the STEP reader.
  asiAlgo_EXPORT void
    Init(const Handle(XSControl_WorkSession)& WS,
         const bool                           scratch = true);

  //! Reads file without translation.
  //! \param[in] filename file to read.
  asiAlgo_EXPORT IFSelect_ReturnStatus
    ReadFile(const char* filename);

  //! \return number of roots for transfer.
  asiAlgo_EXPORT int
    GetNbRootsForTransfer();

  asiAlgo_EXPORT bool
    TransferOneRoot(const int num);

  asiAlgo_EXPORT bool
    Transfer();

  asiAlgo_EXPORT bool
    Perform (const TCollection_AsciiString& filename);

  asiAlgo_EXPORT bool
    Perform (const char* filename);

  //! Returns basic reader
  asiAlgo_EXPORT STEPControl_Reader&
    ChangeReader();

  //! \return basic reader.
  asiAlgo_EXPORT const STEPControl_Reader&
    GetReader() const;

  asiAlgo_EXPORT void
    SetColorMode (const bool colormode);

  asiAlgo_EXPORT bool
    GetColorMode() const;

protected:

  asiAlgo_EXPORT bool
    transfer(STEPControl_Reader& rd, const int num, const bool asOne = true);

  asiAlgo_EXPORT bool
    readColors(const Handle(XSControl_WorkSession)& WS) const;

private:

  STEPControl_Reader m_reader;     //!< Reader (without metadata).
  bool               m_bColorMode; //!< Color mode (on/off).

  //! Output data adaptor.
  Handle(asiAlgo_ReadSTEPWithMetaOutput) m_output;

};

#endif
