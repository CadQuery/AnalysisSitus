//-----------------------------------------------------------------------------
// Created on: 20 November 2017
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
#include <asiData_BVHAttr.h>

// OCCT includes
#include <Standard_GUID.hxx>

//-----------------------------------------------------------------------------
// Construction & settling-down routines
//-----------------------------------------------------------------------------

//! Default constructor.
asiData_BVHAttr::asiData_BVHAttr() : TDF_Attribute()
{}

//! Settles down new BVH Attribute to the given OCAF Label.
//! \param[in] label TDF Label to settle down the new Attribute to.
//! \return newly created Attribute settled down onto the target Label.
Handle(asiData_BVHAttr) asiData_BVHAttr::Set(const TDF_Label& label)
{
  Handle(asiData_BVHAttr) A;
  //
  if ( !label.FindAttribute(GUID(), A) )
  {
    A = new asiData_BVHAttr();
    label.AddAttribute(A);
  }
  return A;
}

//-----------------------------------------------------------------------------
// Accessors for Attribute's GUID
//-----------------------------------------------------------------------------

//! Returns statically defined GUID for BVH Attribute.
//! \return statically defined GUID.
const Standard_GUID& asiData_BVHAttr::GUID()
{
  static Standard_GUID AttrGUID("FB1DF3EF-EA13-4466-A620-AE19BA65B50D");
  return AttrGUID;
}

//! Accessor for GUID associated with this kind of OCAF Attribute.
//! \return GUID of the OCAF Attribute.
const Standard_GUID& asiData_BVHAttr::ID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------
// Attribute's kernel methods:
//-----------------------------------------------------------------------------

//! Creates new instance of BVH Attribute which is not initially populated
//! with any data structures.
//! \return new instance of BVH Attribute.
Handle(TDF_Attribute) asiData_BVHAttr::NewEmpty() const
{
  return new asiData_BVHAttr();
}

//! Performs data transferring from the given OCAF Attribute to this one.
//! This method is mainly used by OCAF Undo/Redo kernel as a part of
//! backup functionality.
//! \param[in] mainAttr OCAF Attribute to copy data from.
void asiData_BVHAttr::Restore(const Handle(TDF_Attribute)& mainAttr)
{
  Handle(asiData_BVHAttr) fromCasted = Handle(asiData_BVHAttr)::DownCast(mainAttr);
  m_BVH = fromCasted->GetBVH();
}

//! Supporting method for Copy/Paste functionality. Performs full copying of
//! the underlying data.
//! \param[in] into       where to paste.
//! \param[in] relocTable relocation table.
void asiData_BVHAttr::Paste(const Handle(TDF_Attribute)&       into,
                            const Handle(TDF_RelocationTable)& asiData_NotUsed(relocTable)) const
{
  Handle(asiData_BVHAttr) intoCasted = Handle(asiData_BVHAttr)::DownCast(into);
  intoCasted->SetBVH(m_BVH);
}

//-----------------------------------------------------------------------------
// Accessors for domain-specific data
//-----------------------------------------------------------------------------

//! Sets BVH to store.
//! \param[in] BVH BVH to store.
void asiData_BVHAttr::SetBVH(const Handle(asiAlgo_BVHFacets)& BVH)
{
  this->Backup();

  m_BVH = BVH;
}

//! Returns the stored BVH.
//! \return stored BVH.
const Handle(asiAlgo_BVHFacets)& asiData_BVHAttr::GetBVH() const
{
  return m_BVH;
}
