//-----------------------------------------------------------------------------
// Created on: 30 October 2018
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
#include <asiVisu_PointsFromListDataProvider.h>

//-----------------------------------------------------------------------------

asiVisu_PointsFromListDataProvider::asiVisu_PointsFromListDataProvider()
: asiVisu_PointsDataProvider(nullptr)
{
  m_pts = new asiAlgo_BaseCloud<double>;
}

//-----------------------------------------------------------------------------

bool asiVisu_PointsFromListDataProvider::MustExecute(const Handle(ActAux_TimeStamp)&) const
{
  return true;
}

//-----------------------------------------------------------------------------

Handle(asiAlgo_BaseCloud<double>)
  asiVisu_PointsFromListDataProvider::GetPoints() const
{
  return m_pts;
}

//-----------------------------------------------------------------------------

Handle(TColStd_HPackedMapOfInteger)
  asiVisu_PointsFromListDataProvider::GetIndices() const
{
  return nullptr;
}

//-----------------------------------------------------------------------------

void asiVisu_PointsFromListDataProvider::AddPoint(const gp_XYZ& pt)
{
  m_pts->AddElement( pt.X(), pt.Y(), pt.Z() );
}

//-----------------------------------------------------------------------------

void asiVisu_PointsFromListDataProvider::AddPoint(const double x,
                                                  const double y,
                                                  const double z)
{
  m_pts->AddElement(x, y, z);
}

//-----------------------------------------------------------------------------

void asiVisu_PointsFromListDataProvider::Clear()
{
  m_pts->Clear();
}

//-----------------------------------------------------------------------------

Handle(ActAPI_HParameterList)
  asiVisu_PointsFromListDataProvider::translationSources() const
{
  return nullptr;
}
