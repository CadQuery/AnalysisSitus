//-----------------------------------------------------------------------------
// Created on: 09 February 2016
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
#include <asiVisu_LawDataProvider.h>

//! Creates data provider for the given design parameter's law.
//! \param N [in] law Node.
asiVisu_LawDataProvider::asiVisu_LawDataProvider(const Handle(asiData_DesignLawNode)& N)
: asiVisu_FuncUnivariateDataProvider(),
  m_node(N)
{}

//! Extracts X or Y evolution law from a persistent object.
//! \return X or Y evolution law.
Handle(asiAlgo_FuncUnivariate) asiVisu_LawDataProvider::GetFunc() const
{
  // Let's take all persistent properties and create a transient law
  return m_node->AsLaw();
}

//! \return persistent Node ID.
ActAPI_DataObjectId asiVisu_LawDataProvider::GetNodeID() const
{
  return m_node->GetId();
}

//! \return sensitive Parameters,
Handle(ActAPI_HParameterList) asiVisu_LawDataProvider::translationSources() const
{
  return ActParamStream() << m_node->Parameter(asiData_DesignLawNode::PID_Degree)
                          << m_node->Parameter(asiData_DesignLawNode::PID_Knots)
                          << m_node->Parameter(asiData_DesignLawNode::PID_Mults)
                          << m_node->Parameter(asiData_DesignLawNode::PID_Poles);
}
