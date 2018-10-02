//-----------------------------------------------------------------------------
// Created on: 01 October 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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
//    * Neither the name of Sergey Slyadnev nor the
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
#include <asiAlgo_Recognizer.h>

//-----------------------------------------------------------------------------

asiAlgo_Recognizer::asiAlgo_Recognizer(const TopoDS_Shape&        masterCAD,
                                       const Handle(asiAlgo_AAG)& aag,
                                       ActAPI_ProgressEntry       progress,
                                       ActAPI_PlotterEntry        plotter)
//
: ActAPI_IAlgorithm ( progress, plotter ),
  m_formulation     ( FeatureFormulation_Full ), // Default formulation
  m_master          ( masterCAD ),
  m_aag             ( aag )
{}

//-----------------------------------------------------------------------------

asiAlgo_Recognizer::asiAlgo_Recognizer(const Handle(asiAlgo_AAG)& aag,
                                       ActAPI_ProgressEntry       progress,
                                       ActAPI_PlotterEntry        plotter)
//
: ActAPI_IAlgorithm ( progress, plotter ),
  m_formulation     ( FeatureFormulation_Full ), // Default formulation
  m_master          ( aag->GetMasterCAD() ),
  m_aag             ( aag )
{}

//-----------------------------------------------------------------------------

asiAlgo_Recognizer::asiAlgo_Recognizer(ActAPI_ProgressEntry progress,
                                       ActAPI_PlotterEntry  plotter)
//
: ActAPI_IAlgorithm ( progress, plotter ),
  m_formulation     ( FeatureFormulation_Full ) // Default formulation
{}

//-----------------------------------------------------------------------------

asiAlgo_Recognizer::~asiAlgo_Recognizer()
{}

//-----------------------------------------------------------------------------

void asiAlgo_Recognizer::SetFormulationFull()
{
  m_formulation = FeatureFormulation_Full;
  m_seedFace.Nullify();

  this->onFormulation(m_formulation);
}

//-----------------------------------------------------------------------------

void asiAlgo_Recognizer::SetFormulationSupportFace(const TopoDS_Face& face)
{
  m_formulation = FeatureFormulation_SupportFace;
  m_seedFace    = face;

  this->onFormulation(m_formulation);
}

//-----------------------------------------------------------------------------

void asiAlgo_Recognizer::SetFormulationGuessFace(const TopoDS_Face& face)
{
  m_formulation = FeatureFormulation_GuessFace;
  m_seedFace    = face;

  this->onFormulation(m_formulation);
}

//-----------------------------------------------------------------------------

void asiAlgo_Recognizer::Init(const TopoDS_Shape& masterCAD)
{
  // Clean up and reinitialize inputs.
  m_seedFace.Nullify();
  m_master = masterCAD;
  m_aag.Nullify(); // AAG is not relevant anymore.

  // Clean up result.
  m_result.Clear();
}
