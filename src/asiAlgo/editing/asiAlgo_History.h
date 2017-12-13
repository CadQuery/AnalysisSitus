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

#ifndef asiAlgo_History_h
#define asiAlgo_History_h

// asiAlgo includes
#include <asiAlgo.h>

// OCCT includes
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>

// Standard includes
#include <vector>

//! \brief History graph for topological elements.
//!
//! This utility class serves tracking history of topological modifications.
class asiAlgo_History : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_History, Standard_Transient)

public:

  //! Node in the history graph.
  struct t_item
  {
    int                  Op;           //!< Operation index.
    TopoDS_Shape         TransientPtr; //!< Shape as a transient pointer.
    std::vector<t_item*> Generated;    //!< List of items generated from this one.
    std::vector<t_item*> Modified;     //!< List of items substituted this one.
    bool                 IsDeleted;    //!< Identifies the item as deleted one.

    //! Constructor.
    t_item() : Op(0), IsDeleted(false) {}

    //! Destructor.
    ~t_item()
    {
      for ( size_t k = 0; k < Generated.size(); ++k )
        delete Generated[k];

      for ( size_t k = 0; k < Modified.size(); ++k )
        delete Modified[k];
    }
  };

public:

  //! \brief Initializes history graph.
  asiAlgo_EXPORT
    asiAlgo_History();

  //! \brief Releases occupied memory.
  asiAlgo_EXPORT
    ~asiAlgo_History();

public:

  //! \brief Adds modification record to the history.
  //!
  //! \param[in] before shape (transient pointer) before modification.
  //! \param[in] after  shape (transient pointer) after modification.
  //! \param[in] create Boolean flag indicating whether this function is
  //!                   allowed to create another root node for the 'before'
  //!                   shape in case it does not exist.
  //! \param[in] opId   auxiliary operation ID which can be useful for
  //!                   composing feature trees.
  //!
  //! \return true in case of success, false -- otherwise. E.g., this function
  //!         may return false in case when modification record is requested
  //!         for a node which has 'IsDeleted' flag turned on.
  asiAlgo_EXPORT bool
    AddModified(const TopoDS_Shape& before,
                const TopoDS_Shape& after,
                const bool          create = true,
                const int           opId = 0);

  //! \brief Gathers modifications of the given shape.
  //!
  //! This function does not perform deep search in a history. It only takes
  //! the direct children of a history record, i.e., gives a one-step history
  //! delta.
  //!
  //! \param[in]  shape    (sub-)shape in question.
  //! \param[out] modified image (sub-)shapes.
  //!
  //! \return false if no modified (sub-)shapes are available for
  //!         the (sub-)shape in question.
  asiAlgo_EXPORT bool
    GetModified(const TopoDS_Shape&        shape,
                std::vector<TopoDS_Shape>& modified) const;

  //! \brief Checks whether the given (sub-)shape is modified.
  //!
  //! This method simply checks if there are any modification records for
  //! a history item corresponding to the given (sub-)shape.
  //!
  //! \param[in] shape (sub-)shape in question.
  //!
  //! \return true/false.
  asiAlgo_EXPORT bool
    IsModified(const TopoDS_Shape& shape) const;

  //! \brief Adds generation record to the history.
  //!
  //! \param[in] source   shape (transient pointer) which is a source.
  //! \param[in] creation shape (transient pointer) which was generated from source.
  //! \param[in] create   Boolean flag indicating whether this function is
  //!                     allowed to create another root node for the 'before'
  //!                     shape in case it does not exist.
  //! \param[in] opId     auxiliary operation ID which can be useful for
  //!                     composing feature trees.
  //!
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    AddGenerated(const TopoDS_Shape& source,
                 const TopoDS_Shape& creation,
                 const bool          create = true,
                 const int           opId = 0);

  //! \brief Gathers generated (sub-)shapes for the given shape.
  //!
  //! This function does not perform deep search in a history. It only takes
  //! the direct children of a history record, i.e., gives a one-step history
  //! delta.
  //!
  //! \param[in]  shape     (sub-)shape in question.
  //! \param[out] generated image (sub-)shapes.
  //!
  //! \return false if no generated (sub-)shapes are available for
  //!         the (sub-)shape in question.
  asiAlgo_EXPORT bool
    GetGenerated(const TopoDS_Shape&        shape,
                 std::vector<TopoDS_Shape>& generated) const;

  //! \brief Checks whether the given (sub-)shape has generated any (sub-)shapes.
  //!
  //! This method simply checks if there are any generation records for
  //! a history item corresponding to the given (sub-)shape.
  //!
  //! \param[in] shape (sub-)shape in question.
  //!
  //! \return true/false.
  asiAlgo_EXPORT bool
    HasGenerated(const TopoDS_Shape& shape) const;

  //! \brief Adds deletion record to the history.
  //!
  //! If a history node corresponding to the given shape already exists,
  //! a "deletion record" is nothing but indicating that this node is the
  //! ultimate one (by turning on the corresponding Boolean flag).
  //!
  //! \param[in] shape  shape (transient pointer) to mark as deleted.
  //! \param[in] create Boolean flag indicating whether this function is
  //!                   allowed to create another root node for the 'shape'
  //!                   shape in case it does not exist.
  //! \param[in] opId   auxiliary operation ID which can be useful for
  //!                   composing feature trees.
  //!
  //! \return true in case of success, false -- otherwise. E.g., this function
  //!         may return false in case when deletion is requested for a node
  //!         which does not exist while 'create' flag is turned off.
  asiAlgo_EXPORT bool
    SetDeleted(const TopoDS_Shape& shape,
               const bool          create = true,
               const int           opId = 0);

  //! \brief Checks whether the given (sub-)shape is marked as deleted.
  //!
  //! \param[in] shape (sub-)shape in question.
  //!
  //! \return true/false.
  asiAlgo_EXPORT bool
    IsDeleted(const TopoDS_Shape& shape) const;

protected:

  //! Finds history item for the given shape.
  //! \param[in] shape shape to find a history item for.
  //! \return history item for a shape or null if no such item exists.
  asiAlgo_EXPORT t_item*
    findItem(const TopoDS_Shape& shape) const;

  //! Finds or creates history item for the given shape.
  //! \param[in] shape  shape to create a history item for.
  //! \param[in] opId   auxiliary operation ID.
  //! \param[in] create whether to create a new item if it does not exist.
  //! \return history item.
  asiAlgo_EXPORT t_item*
    findItem(const TopoDS_Shape& shape,
             const int           opId,
             const bool          create = true);

  //! Creates history item for the given shape.
  //! \param[in] shape shape to create a history item for.
  //! \param[in] opId  auxiliary operation ID.
  //! \return history item.
  asiAlgo_EXPORT t_item*
    makeItem(const TopoDS_Shape& shape,
             const int           opId);

protected:

  //! Hasher which does not take into account neither locations nor
  //! orientations of shapes. Our killer is extremely cruel in this regard...
  class t_partner_hasher
  {
  public:

    static int HashCode(const TopoDS_Shape& S, const int Upper)
    {
      const int I  = (int) ptrdiff_t( S.TShape().operator->() );
      const int HS = ::HashCode(I, Upper);
      //
      return HS;
    }

    static bool IsEqual(const TopoDS_Shape& S1, const TopoDS_Shape& S2)
    {
      return S1.IsPartner(S2);
    }
  };

protected:

  //! Root items in the history.
  std::vector<t_item*> m_roots;

  //! Shapes and their corresponding history items.
  NCollection_DataMap<TopoDS_Shape, t_item*, t_partner_hasher> m_items;

};

#endif
