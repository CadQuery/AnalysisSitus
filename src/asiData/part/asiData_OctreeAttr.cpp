//-----------------------------------------------------------------------------
// Created on: 25 November 2019
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
#include <asiData_OctreeAttr.h>

// OCCT includes
#include <Standard_GUID.hxx>

//-----------------------------------------------------------------------------
// Construction & settling-down routines
//-----------------------------------------------------------------------------

//! Default constructor.
asiData_OctreeAttr::asiData_OctreeAttr()
: TDF_Attribute (),
  m_pOctree     (nullptr)
{}

//! Settles down new Octree Attribute to the given OCAF Label.
//! \param[in] label TDF Label to settle down the new Attribute to.
//! \return newly created Attribute settled down onto the target Label.
Handle(asiData_OctreeAttr) asiData_OctreeAttr::Set(const TDF_Label& label)
{
  Handle(asiData_OctreeAttr) A;
  //
  if ( !label.FindAttribute(GUID(), A) )
  {
    A = new asiData_OctreeAttr();
    label.AddAttribute(A);
  }
  return A;
}

//-----------------------------------------------------------------------------
// Accessors for Attribute's GUID
//-----------------------------------------------------------------------------

//! Returns statically defined GUID for Octree Attribute.
//! \return statically defined GUID.
const Standard_GUID& asiData_OctreeAttr::GUID()
{
  static Standard_GUID AttrGUID("FFD33C63-5EE4-48DD-871D-5422CEDFDC04");
  return AttrGUID;
}

//! Accessor for GUID associated with this kind of OCAF Attribute.
//! \return GUID of the OCAF Attribute.
const Standard_GUID& asiData_OctreeAttr::ID() const
{
  return GUID();
}

//-----------------------------------------------------------------------------
// Attribute's kernel methods:
//-----------------------------------------------------------------------------

//! Creates new instance of Octree Attribute which is not initially populated
//! with any data structures.
//! \return new instance of Octree Attribute.
Handle(TDF_Attribute) asiData_OctreeAttr::NewEmpty() const
{
  return new asiData_OctreeAttr();
}

//! Performs data transferring from the given OCAF Attribute to this one.
//! This method is mainly used by OCAF Undo/Redo kernel as a part of
//! backup functionality.
//! \param[in] mainAttr OCAF Attribute to copy data from.
void asiData_OctreeAttr::Restore(const Handle(TDF_Attribute)& mainAttr)
{
  Handle(asiData_OctreeAttr) fromCasted = Handle(asiData_OctreeAttr)::DownCast(mainAttr);
  m_pOctree = fromCasted->GetOctree();
}

//! Supporting method for Copy/Paste functionality. Performs full copying of
//! the underlying data.
//! \param[in] into       where to paste.
//! \param[in] relocTable relocation table.
void asiData_OctreeAttr::Paste(const Handle(TDF_Attribute)&       into,
                               const Handle(TDF_RelocationTable)& relocTable) const
{
  asiData_NotUsed(relocTable);

  Handle(asiData_OctreeAttr) intoCasted = Handle(asiData_OctreeAttr)::DownCast(into);
  intoCasted->SetOctree(m_pOctree);
}

//-----------------------------------------------------------------------------
// Accessors for domain-specific data
//-----------------------------------------------------------------------------

//! Sets octree to store.
//! \param[in] octree octree node to store.
void asiData_OctreeAttr::SetOctree(void* octree)
{
  this->Backup();

  m_pOctree = octree;
}

//! Returns the stored octree node.
//! \return stored octree node.
void* asiData_OctreeAttr::GetOctree() const
{
  return m_pOctree;
}
