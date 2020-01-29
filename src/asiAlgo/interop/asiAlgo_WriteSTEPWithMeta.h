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

#ifndef asiAlgo_WriteSTEPWithMeta_h
#define asiAlgo_WriteSTEPWithMeta_h

// asiAlgo includes
#include <asiAlgo_WriteSTEPWithMetaInput.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <MoniTool_DataMapOfShapeTransient.hxx>
#include <STEPConstruct_Styles.hxx>
#include <STEPControl_Writer.hxx>
#include <TopTools_MapOfShape.hxx>

//! STEP writer enriched with possibility to write not only shapes but
//! also some metadata such as colors.
class asiAlgo_WriteSTEPWithMeta : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_WriteSTEPWithMeta, ActAPI_IAlgorithm)

public:

  asiAlgo_EXPORT
    asiAlgo_WriteSTEPWithMeta(ActAPI_ProgressEntry progress = nullptr,
                              ActAPI_PlotterEntry  plotter  = nullptr);

  asiAlgo_EXPORT
    asiAlgo_WriteSTEPWithMeta(const Handle(XSControl_WorkSession)& WS,
                              const bool                           scratch = true,
                              ActAPI_ProgressEntry                 progress = nullptr,
                              ActAPI_PlotterEntry                  plotter  = nullptr);

public:

  //! Sets input data adaptor.
  //! \param[in] input input data provider.
  void SetInput(const Handle(asiAlgo_WriteSTEPWithMetaInput)& input)
  {
    m_input = input;
  }

public:

  asiAlgo_EXPORT void
    Init(const Handle(XSControl_WorkSession)& WS,
         const bool                           scratch = true);

  asiAlgo_EXPORT IFSelect_ReturnStatus
    Write(const char* filename);

  asiAlgo_EXPORT bool
    Transfer(const STEPControl_StepModelType mode = STEPControl_AsIs);

  asiAlgo_EXPORT bool
    Perform(const TCollection_AsciiString& filename);

  asiAlgo_EXPORT void
    SetColorMode(const bool colormode);

  asiAlgo_EXPORT bool
    GetColorMode() const;

protected:

  asiAlgo_EXPORT bool
    transfer(STEPControl_Writer&             wr,
             const STEPControl_StepModelType mode = STEPControl_AsIs);
  
  asiAlgo_EXPORT bool
    writeColors(const Handle(XSControl_WorkSession)& WS);

private:

  void makeSTEPStyles(STEPConstruct_Styles&                        Styles,
                      const TopoDS_Shape&                          S,
                      Handle(StepVisual_StyledItem)&               override,
                      TopTools_MapOfShape&                         Map,
                      STEPConstruct_DataMapOfAsciiStringTransient& DPDCs,
                      STEPConstruct_DataMapOfPointTransient&       ColRGBs);

private:

  STEPControl_Writer               m_writer;       //!< Writer (without metadata).
  bool                             m_bColorMode;   //!< Color mode (on/off).
  MoniTool_DataMapOfShapeTransient m_mapCompMDGPR; //!< Auxiliary map.

  //! Input data adaptor.
  Handle(asiAlgo_WriteSTEPWithMetaInput) m_input;

};

#endif
