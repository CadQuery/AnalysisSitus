//-----------------------------------------------------------------------------
// Created on: 27 November 2017
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
#include <asiAlgo_History.h>

//-----------------------------------------------------------------------------

asiAlgo_History::asiAlgo_History() : Standard_Transient()
{}

//-----------------------------------------------------------------------------

asiAlgo_History::~asiAlgo_History()
{
  for ( size_t k = 0; k < m_roots.size(); ++k )
    delete m_roots[k];
}

//-----------------------------------------------------------------------------

bool asiAlgo_History::AddModified(const TopoDS_Shape& before,
                                  const TopoDS_Shape& after,
                                  const bool          create,
                                  const int           opId)
{
  // Get or create active item.
  t_item* pActiveItem = this->findActiveItem(before, opId, create);
  //
  if ( !pActiveItem || pActiveItem->IsDeleted )
    return false;

  // Construct item for the modification.
  t_item* pChildItem = this->makeItem(after, opId);
  //
  pActiveItem->Modified.push_back(pChildItem);

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_History::SetDeleted(const TopoDS_Shape& shape,
                                 const bool          create,
                                 const int           opId)
{
  // Get or create active item.
  t_item* pActiveItem = this->findActiveItem(shape, opId, create);

  if ( !pActiveItem )
    return false;

  // Set as deleted.
  pActiveItem->IsDeleted = true;

  return true;
}

//-----------------------------------------------------------------------------

asiAlgo_History::t_item*
  asiAlgo_History::findActiveItem(const TopoDS_Shape& shape,
                                  const int           opId,
                                  const bool          create)
{
  // Check if there is any active item to continue growing a history on.
  if ( !m_activeItems.IsBound(shape) )
  {
    if ( create )
    {
      t_item* pRootItem = this->makeItem(shape, opId);
      //
      m_roots.push_back(pRootItem);
    }
    else
      return NULL;
  }

  // Get active item.
  t_item* pActiveItem = m_activeItems(shape);
  //
  return pActiveItem;
}

//-----------------------------------------------------------------------------

asiAlgo_History::t_item* asiAlgo_History::makeItem(const TopoDS_Shape& shape,
                                                   const int           opId) const
{
  t_item* pItem = new t_item;
  pItem->TransientPtr = shape;
  pItem->Op           = opId;
  //
  return pItem;
}
