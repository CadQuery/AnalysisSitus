//-----------------------------------------------------------------------------
// Created on: 09 February 2016
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
