//-----------------------------------------------------------------------------
// Created on: 02 March 2017
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

// Own include
#include <asiData_FaceNormsNode.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_FaceNormsNode::asiData_FaceNormsNode() : asiData_FaceNodeBase()
{
  REGISTER_PARAMETER(Real, PID_SampleRate);
}

//! Returns new DETACHED instance of Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Node.
Handle(ActAPI_INode) asiData_FaceNormsNode::Instance()
{
  return new asiData_FaceNormsNode();
}

//! Performs initial actions required to make Node WELL-FORMED.
void asiData_FaceNormsNode::Init()
{
  asiData_FaceNodeBase::Init();

  this->SetSampleRate(0.05);
  this->InitParameter(PID_SampleRate,  "Sampling rate", "", ParameterFlag_IsVisible, true);
}

//! Sets the sample rate value.
//! \param[in] value to set.
void asiData_FaceNormsNode::SetSampleRate(const double value)
{
  ActParamTool::AsReal( this->Parameter(PID_SampleRate) )->SetValue(value);
}

//! \return sample rate value.
double asiData_FaceNormsNode::GetSampleRate() const
{
  return ActParamTool::AsReal( this->Parameter(PID_SampleRate) )->GetValue();
}
