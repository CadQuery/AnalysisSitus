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

#ifndef asiAlgo_History_h
#define asiAlgo_History_h

// asiAlgo includes
#include <asiAlgo_ShapePartnerHasher.h>

// Active Data includes
#include <ActAPI_IPlotter.h>
#include <ActAPI_IProgressNotifier.h>

// OCCT includes
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>

// Standard includes
#include <stack>
#include <vector>

//-----------------------------------------------------------------------------

//! \brief History graph for topological elements.
//!
//! This utility class serves tracking history of topological modifications.
class asiAlgo_History : public Standard_Transient
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_History, Standard_Transient)

public:

  //! Possible evolution of a sub-shape.
  enum EvolutionType
  {
    Evolution_Modified,
    Evolution_Generated,
    Evolution_Deleted
  };

  //! Node in the history graph.
  struct t_item
  {
    int                  Op;           //!< Operation index.
    TopoDS_Shape         TransientPtr; //!< Shape as a transient pointer.
    std::vector<t_item*> Generated;    //!< List of items generated from this one.
    std::vector<t_item*> Modified;     //!< List of items substituted this one.
    bool                 IsDeleted;    //!< Identifies the item as deleted one.
    bool                 IsActive;     //!< Indicates whether this sub-shape is alive in the model.

    //! Constructor.
    t_item() : Op(0), IsDeleted(false), IsActive(false) {}

    //! Destructor.
    ~t_item()
    {
      for ( size_t k = 0; k < Generated.size(); ++k )
        delete Generated[k];

      for ( size_t k = 0; k < Modified.size(); ++k )
        delete Modified[k];
    }

    //! Returns child items realizing the given evolution type.
    //! \param[in]  evolution evolution type of interest. Expected values are
    //!                       either "generated" or "modified".
    //! \param[out] children  child items.
    void GetChildren(const EvolutionType   evolution,
                     std::vector<t_item*>& children) const
    {
      switch ( evolution )
      {
        case Evolution_Generated: children = this->Generated; break;
        case Evolution_Modified:  children = this->Modified;  break;
        default: break;
      }
    }

    //! Hasher for using history items in OCCT data maps.
    struct Hasher
    {
      static int HashCode(const t_item* pItem, const int Upper)
      {
        const int I  = (int) ptrdiff_t(pItem);
        const int HS = ::HashCode(I, Upper);
        //
        return HS;
      }

      static bool IsEqual(const t_item* pItem1, const t_item* pItem2)
      {
        return pItem1 == pItem2;
      }
    };
  };

  //! Convenience alias for indexed map of shapes-vs-items.
  typedef NCollection_IndexedDataMap<TopoDS_Shape, t_item*, asiAlgo_ShapePartnerHasher> t_shapeItemMap;

public:

  //! \brief History graph iterator which traverses full graph in
  //!        non-deterministic order.
  class RandomIterator
  {
  public:

    //! ctor accepting the history graph to iterate.
    //! \param[in] historyGraph history graph to iterate.
    RandomIterator(const Handle(asiAlgo_History)& historyGraph)
    {
      m_graph  = historyGraph;
      m_iIndex = 1;
    }

  public:

    //! Checks if there are more graph nodes to iterate.
    //! \return true/false.
    bool More() const
    {
      return m_iIndex <= m_graph->GetNodes().Extent();
    }

    //! \return integer 1-based index of the current graph node.
    int GetIndex() const
    {
      return m_iIndex;
    }

    //! \return current node.
    t_item* GetItem()
    {
      return m_graph->GetNodes().FindFromIndex(m_iIndex);
    }

    //! \return current shape.
    const TopoDS_Shape& GetShape() const
    {
      return m_graph->GetNodes().FindKey(m_iIndex);
    }

    //! Moves iterator to the next position.
    void Next()
    {
      m_iIndex++;
    }

  protected:

    //! History graph.
    Handle(asiAlgo_History) m_graph;

    //! Internal index.
    int m_iIndex;

  };

  //-------------------------------------------------------------------------//

  //! Depth-first iterator which does not guarantee that each graph node
  //! is visited only once. MODIFIED items are visited before GENERATED for
  //! each graph node.
  class DepthFirstIterator
  {
  public:

    DepthFirstIterator(const Handle(asiAlgo_History)& historyGraph,
                       t_item*                        pRoot,
                       const bool                     visitModified,
                       const bool                     visitGenerated)
    : m_graph (historyGraph),
      m_pRoot (pRoot),
      m_bGoM  (visitModified),
      m_bGoG  (visitGenerated)
    {
      m_fringe.push(pRoot);
    }

  public:

    //! Checks if there are more graph nodes to iterate.
    //! \return true/false.
    bool More() const
    {
      return !m_fringe.empty();
    }

    //! \return integer 1-based index of the current graph node.
    int GetIndex() const
    {
      return m_graph->GetNodes().FindIndex( this->GetShape() );
    }

    //! \return current node.
    t_item* GetItem() const
    {
      return m_fringe.top();
    }

    //! \return current shape.
    const TopoDS_Shape& GetShape() const
    {
      return this->GetItem()->TransientPtr;
    }

    //! Moves iterator to the next position.
    void Next()
    {
      // Let's throw an exception if there is nothing else to iterate.
      if ( !this->More() )
        Standard_ProgramError::Raise("No next item");

      // Take current.
      t_item* pCurrent = this->GetItem();
      m_fringe.pop(); // Top item is done.

      // Put all nodes pending for iteration to the fringe.
      if ( m_bGoG )
      {
        for ( int k = (int) pCurrent->Generated.size() - 1; k >= 0; --k )
        {
          // Set node to iterate.
          m_fringe.push( pCurrent->Generated[k] );
        }
      }
      //
      if ( m_bGoM )
      {
        for ( int k = (int) pCurrent->Modified.size() - 1; k >= 0; --k )
        {
          // Set node to iterate.
          m_fringe.push( pCurrent->Modified[k] );
        }
      }
    }

  protected:

    Handle(asiAlgo_History) m_graph;  //!< History graph.
    t_item*                 m_pRoot;  //!< Root item to start iterating from.
    std::stack<t_item*>     m_fringe; //!< Where to return.
    bool                    m_bGoM;   //!< Whether to visit modified items.
    bool                    m_bGoG;   //!< Whether to visit generated items.

  };

public:

  //! \brief Initializes history graph.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_History(ActAPI_ProgressEntry progress = NULL,
                    ActAPI_PlotterEntry  plotter  = NULL);

  //! \brief Releases occupied memory.
  asiAlgo_EXPORT
    ~asiAlgo_History();

public:

  //! Add new root node to the history. Use this method to make origins in
  //! the history graph without any succeeding evolution.
  //! \param[in] shape shape to add a root node for.
  //! \return created root node.
  asiAlgo_EXPORT t_item*
    AddRoot(const TopoDS_Shape& shape);

  //! Get root shapes from the history.
  //! \param[out] roots root shapes.
  asiAlgo_EXPORT void
    GetRoots(std::vector<TopoDS_Shape>& roots) const;

  //! Returns root shapes of a certain type from the history.
  //! \param[in]  type  shape type of interest.
  //! \param[out] roots root shapes of a certain type.
  asiAlgo_EXPORT void
    GetRootsOfType(const TopAbs_ShapeEnum     type,
                   std::vector<TopoDS_Shape>& roots) const;

  //! Returns root nodes for the shapes of a certain type from the history.
  //! \param[in]  type  shape type of interest.
  //! \param[out] roots root nodes for the shapes of a certain type.
  asiAlgo_EXPORT void
    GetRootsOfType(const TopAbs_ShapeEnum type,
                   std::vector<t_item*>&  roots) const;

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

  //! \brief Returns single modification of the given shape.
  //!
  //! This function does not perform deep search in a history. It only takes
  //! the direct children of a history record, i.e., gives a one-step history
  //! delta.
  //!
  //! \param[in] shape (sub-)shape in question.
  //!
  //! \return null shape if no modified (sub-)shapes are available for
  //!         the (sub-)shape in question. Else, the first image is returned.
  asiAlgo_EXPORT TopoDS_Shape
    GetModified(const TopoDS_Shape& shape) const;

  //! \brief Returns single modification of the given shape or the shape itself.
  //!
  //! This function does not perform deep search in a history. It only takes
  //! the direct children of a history record, i.e., gives a one-step history
  //! delta. If no modification is available for the passed shape, the argument
  //! shape is returned.
  //!
  //! \param[in] shape (sub-)shape in question.
  //!
  //! \return argument shape if no modified (sub-)shapes are available for
  //!         the (sub-)shape in question. Else, the first image is returned.
  asiAlgo_EXPORT TopoDS_Shape
    GetModifiedOrArg(const TopoDS_Shape& shape) const;

  //! \brief Returns last modification of the given shape or the shape itself.
  //!
  //! This function perform deep iteration in a history to reach the ultimate
  //! leaf. If no modification is available for the passed shape, the argument
  //! shape is returned.
  //!
  //! \param[in] shape (sub-)shape in question.
  //!
  //! \return argument shape if no modified (sub-)shapes are available for
  //!         the (sub-)shape in question. Else, the first image is returned.
  asiAlgo_EXPORT TopoDS_Shape
    GetLastModifiedOrArg(const TopoDS_Shape& shape) const;

  //! \brief Returns last image of the given shape or the shape itself.
  //!
  //! This method is similar to GetLastImageOrArg(). Howeverm if the shape
  //! in question was deleted, it results an empty shape.
  //!
  //! \param[in] shape (sub-)shape in question.
  //!
  //! \return argument shape if no modified (sub-)shapes are available for
  //!         the (sub-)shape in question. Else, the first image or null shape
  //!         is returned.
  asiAlgo_EXPORT TopoDS_Shape
    GetLastImageOrArg(const TopoDS_Shape& shape) const;

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

  //! \brief Checks whether the given (sub-)shape is marked as active.
  //!
  //! \param[in] shape (sub-)shape in question.
  //!
  //! \return true/false.
  asiAlgo_EXPORT bool
    IsActive(const TopoDS_Shape& shape) const;

  //! \brief Gathers all leaf items from the history graph.
  //!
  //! \param[out] leafItems leaf items of the history graph.
  //! \param[in]  shapeType shape type in question. Use the default value
  //!                       to accept any kind of shape.
  asiAlgo_EXPORT void
    GetLeafs(std::vector<t_item*>&  leafItems,
             const TopAbs_ShapeEnum shapeType = TopAbs_SHAPE) const;

public:

  //! Returns history node by the given 1-based index.
  //! \param[in] nid ID of the graph node to access.
  //! \return history item or null if such item does not exist.
  t_item* GetNode(const int nid) const
  {
    if ( nid <= 0 || nid > m_items.Extent() )
      return NULL;

    return m_items(nid);
  }

  //! \return collection of sub-shapes and their graph nodes.
  const t_shapeItemMap& GetNodes() const
  {
    return m_items;
  }

  //! \return root nodes of the history graph.
  const std::vector<t_item*>& GetRoots() const
  {
    return m_roots;
  }

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

  //! Collects leaf items for the given seed and evolution type.
  //! \param[in]  pSeed     seed item.
  //! \param[in]  evolution evolution type.
  //! \param[out] leafs     collected nodes.
  asiAlgo_EXPORT void
    gatherLeafs(t_item*               pSeed,
                const EvolutionType   evolution,
                std::vector<t_item*>& leafs) const;

protected:

  //! Root items in the history.
  std::vector<t_item*> m_roots;

  //! Shapes and their corresponding history items. The map is indexed to
  //! have a persistent key.
  t_shapeItemMap m_items;

  //! Progress notifier.
  mutable ActAPI_ProgressEntry m_progress;

  //! Imperative plotter.
  mutable ActAPI_PlotterEntry m_plotter;

};

#endif
