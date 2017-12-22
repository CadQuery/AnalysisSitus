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
  // Get or create item.
  t_item* pActiveItem = this->findItem(before, opId, create);
  //
  if ( !pActiveItem || pActiveItem->IsDeleted )
    return false;

  // Construct item for the modification.
  t_item* pChildItem = this->makeItem(after, opId);
  //
  pActiveItem->Modified.push_back(pChildItem);

  // Make successor "alive" instead of this one.
  pActiveItem->IsActive = false;
  pChildItem->IsActive  = true;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_History::GetModified(const TopoDS_Shape&        shape,
                                  std::vector<TopoDS_Shape>& modified) const
{
  // Get item for the shape in question.
  t_item* pActiveItem = this->findItem(shape);
  //
  if ( !pActiveItem || !pActiveItem->Modified.size() )
    return false;

  // Gather target items.
  for ( int k = 0; k < pActiveItem->Modified.size(); ++k )
    modified.push_back(pActiveItem->Modified[k]->TransientPtr);

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_History::IsModified(const TopoDS_Shape& shape) const
{
  // Get item for the shape in question.
  t_item* pActiveItem = this->findItem(shape);
  //
  if ( !pActiveItem )
    return false;

  return pActiveItem->Modified.size() > 0;
}

//-----------------------------------------------------------------------------

bool asiAlgo_History::AddGenerated(const TopoDS_Shape& source,
                                   const TopoDS_Shape& creation,
                                   const bool          create,
                                   const int           opId)
{
  // Get or create item.
  t_item* pActiveItem = this->findItem(source, opId, create);
  //
  if ( !pActiveItem )
    return false;

  // Construct item for the creation.
  t_item* pChildItem = this->makeItem(creation, opId);
  //
  pActiveItem->Generated.push_back(pChildItem);

  // Make successor "alive" together with the current item.
  pChildItem->IsActive = true;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_History::GetGenerated(const TopoDS_Shape&        shape,
                                   std::vector<TopoDS_Shape>& generated) const
{
  // Get item for the shape in question.
  t_item* pActiveItem = this->findItem(shape);
  //
  if ( !pActiveItem || !pActiveItem->Generated.size() )
    return false;

  // Gather target items.
  for ( int k = 0; k < pActiveItem->Generated.size(); ++k )
    generated.push_back(pActiveItem->Generated[k]->TransientPtr);

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_History::HasGenerated(const TopoDS_Shape& shape) const
{
  // Get item for the shape in question.
  t_item* pActiveItem = this->findItem(shape);
  //
  if ( !pActiveItem )
    return false;

  return pActiveItem->Generated.size() > 0;
}

//-----------------------------------------------------------------------------

bool asiAlgo_History::SetDeleted(const TopoDS_Shape& shape,
                                 const bool          create,
                                 const int           opId)
{
  // Get or create item.
  t_item* pActiveItem = this->findItem(shape, opId, create);

  if ( !pActiveItem )
    return false;

  // Set as deleted.
  pActiveItem->IsDeleted = true;
  pActiveItem->IsActive  = false;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_History::IsDeleted(const TopoDS_Shape& shape) const
{
  // Get item for the shape in question.
  t_item* pActiveItem = this->findItem(shape);
  //
  if ( !pActiveItem )
    return false;

  return pActiveItem->IsDeleted;
}

//-----------------------------------------------------------------------------

bool asiAlgo_History::IsActive(const TopoDS_Shape& shape) const
{
  // Get item for the shape in question.
  t_item* pActiveItem = this->findItem(shape);
  //
  if ( !pActiveItem )
    return false;

  return pActiveItem->IsActive;
}

//-----------------------------------------------------------------------------

asiAlgo_History::t_item*
  asiAlgo_History::findItem(const TopoDS_Shape& shape) const
{
  if ( !m_items.Contains(shape) )
    return NULL;

  return m_items.FindFromKey(shape);
}

//-----------------------------------------------------------------------------

asiAlgo_History::t_item*
  asiAlgo_History::findItem(const TopoDS_Shape& shape,
                            const int           opId,
                            const bool          create)
{
  // Check if there is any active item to continue growing a history on.
  if ( !m_items.Contains(shape) )
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

  return m_items.FindFromKey(shape);
}

//-----------------------------------------------------------------------------

asiAlgo_History::t_item* asiAlgo_History::makeItem(const TopoDS_Shape& shape,
                                                   const int           opId)
{
  t_item* pItem = new t_item;
  pItem->TransientPtr = shape;
  pItem->Op           = opId;
  //
  m_items.Add(shape, pItem);

  return pItem;
}
