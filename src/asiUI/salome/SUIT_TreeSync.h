// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File   : SUIT_TreeSync.h
// Author : Alexander SOLOVYOV
//
#ifndef SUIT_TREESYNC_H
#define SUIT_TREESYNC_H

#pragma warning(push, 0)
#include <QList>
#pragma warning(pop)

/*!
  \brief The structure representing difference between source and destination items.

  The different combinations of source and target items values imply the different actions 
  to be performed in the target data tree:
  - source item is null, target item is not null : the item should be removed from the target tree
  - source item is not null, target item is null : new item should be added to the target tree
  - both source and target items are not null : the target item can be updated if necessary
  - both source and target items are null : error   
*/
template <class SrcItem, class TrgItem>
struct DiffItem
{
  SrcItem  mySrc;      //!< source tree item
  TrgItem  myTrg;      //!< target tree item
};


//
// Function prototypes.
//

template <class SrcItem, class TrgItem, class TreeData>
TrgItem synchronize( const SrcItem&, const TrgItem&, const TreeData& );

template <class SrcItem, class TrgItem, class TreeData>
QList< DiffItem<SrcItem,TrgItem> > diffSiblings( const SrcItem&, 
                                                 const TrgItem&, 
                                                 const TreeData& );

template <class SrcItem, class TrgItem, class TreeData>
TrgItem createSubTree( const SrcItem&, const TrgItem&, const TrgItem&, const TreeData& );

template <class SrcItem, class TrgItem, class TreeData>
const typename QList<TrgItem>::const_iterator findEqual( const SrcItem& it,
                                                         const typename QList<TrgItem>::const_iterator& first,
                                                         const typename QList<TrgItem>::const_iterator& last,
                                                         const TreeData& td );


//
// Function imlpementation.
//

/*!
  \brief Synchronize two data trees by recurive comparing of the corresponding items.

  \param r1 starting item from the source  data tree
  \param r2 starting item from the target data tree
  \param td functor class
  \return the target tree item (updated or just created) corresponding to the starting
     data object

  Actual comparing of the items and the syncronization of the trees is performed by the
  functor class which is passed as the last parameter of the function.
  The functor class should implement the following methods:
  - \b bool \b isEqual( \b const \b SrcItem& \p src, \b const \b TrgItem& \p tgt ) \b const;
    - \a src source tree item 
    - \a tgt target tree item
    - compares items and returns \c true if the items are equal (correspond to each other)
  - \b SrcItem \b nullSrc() \b const;
    - returns null source tree itemm
  - \b TrgItem \b nullTrg() \b const
    - returns null target tree item
  - \b TrgItem \b createItem( \b const \b SrcItem& \p src, \b const \b TrgItem& \p parent, \b const \b TrgItem& \p after ) \b const;
    - \a src source item
    - \a parent parent target item
    - \a after target tree item after which new item shoud be inserted (if null, the item is added to the end)
    - creates new ite in the target tree which correspond to the source item and returns created item
  - \b void \b updateItem( \b const \b SrcItem& \p src, \b const \b TrgItem& \p tgt ) \b const;
    - \a src source tree item 
    - \a tgt the item in the target tree to be updated
    - updates target treeitem
  - \b void \b deleteItemWithChildren( \b const \b TrgItem& \p tgt ) \b const;
    - \a tgt the item in the target tree to be removed
    - deletes target tree item (recursively)
  - \b QList<SrcItem> \b children( \b const \b SrcItem& \p parent ) \b const;
    - \a parent the parent item in the source tree
    - returns the child items list
  - \b QList<TrgItem> \b children( \b const \b TrgItem& \p parent ) \b const;
    - \a parent the parent item in the target tree
    - returns the child items list
  - \b TrgItem \b parent( \b const \b TrgItem& \p tgt ) \b const;
    - \a tgt target tree item
    - returns the item which is parent for the specified source tree item
*/
template <class SrcItem, class TrgItem, class TreeData>
TrgItem synchronize( const SrcItem& r1, const TrgItem& r2, const TreeData& td )
{
  if ( td.isEqual( r1, r2 ) ) {
    // update items themselves
    td.updateItem( r1, r2 );
    
    // iterate through children
    QList< DiffItem< SrcItem, TrgItem > > d =  diffSiblings( r1, r2, td );
    
    typename QList< DiffItem< SrcItem, TrgItem > >::const_iterator anIt = d.begin(), aLast = d.end();
    TrgItem lastItem = td.nullTrg();

    for ( ; anIt != aLast; anIt++ ) {
      const DiffItem<SrcItem,TrgItem>& item = *anIt;
      if ( item.mySrc == td.nullSrc() ) {
        if ( item.myTrg == td.nullTrg() )
          qDebug( "error: both null" );
        else
          // delete item
          td.deleteItemWithChildren( item.myTrg );
      }
      else {
        if ( item.myTrg == td.nullTrg() ) {
          // add item (recursively)
          TrgItem nitem = createSubTree( item.mySrc, r2, lastItem, td );
          if ( nitem != td.nullTrg() )
            lastItem = nitem;
        }
        else {
          // update item
          synchronize( item.mySrc, item.myTrg, td );
          lastItem = item.myTrg;
        }
      }
    }
    return r2;
  }
  else {
    TrgItem new_r2 = td.nullTrg();
    if ( r1 != td.nullSrc() ) {
      // add new item (recursively)
      new_r2 = createSubTree( r1, td.parent( r2 ), r2, td );
    }
    if ( r2 != td.nullTrg() ) {
      // delete old one (if it is not null)
      td.deleteItemWithChildren( r2 );
    }
    return new_r2;
  }
}

/*!
  \brief Find the item in the target tree which correspond to the specified source tree item.
  \param it source item for which correspondence is to be found
  \param first iterator pointing to the item in the list \a l from which search shoud be started
  \param last iterator pointing to the item in the list \a l the search to be finished at
  \param td functor class
  \return iterator pointing to the item in the list \l if the correspondence is found or iterator
     \a last if the correspondence is not found
  \sa synchronize()
*/
template <class SrcItem, class TrgItem, class TreeData>
const typename QList<TrgItem>::const_iterator findEqual( const SrcItem& it,
                                                         const typename QList<TrgItem>::const_iterator& first,
                                                         const typename QList<TrgItem>::const_iterator& last,
                                                         const TreeData& td )
{
  typename QList<TrgItem>::const_iterator cur = first;
  for ( ; cur != last; cur++ ) {
    if ( td.isEqual( it, *cur ) )
      return cur;
  }
  return last;
}

/*!
  \brief Compare children of the source and target trees to find differences.
  \param src parent source item
  \param trg parent target item
  \param td functor class
  \return list of the differences
  \sa synchronize()
*/
template <class SrcItem, class TrgItem, class TreeData>
QList< DiffItem<SrcItem,TrgItem> > diffSiblings( const SrcItem& src, const TrgItem& trg,
                                                 const TreeData& td )
{
  //if( src==td.nullSrc() || trg==td.nullTrg() )
  //  return;
  
  QList< DiffItem<SrcItem,TrgItem> > d;
   
  QList<SrcItem> src_ch = td.children( src );
  QList<TrgItem> trg_ch = td.children( trg );

  typename QList<SrcItem>::const_iterator src_it = src_ch.begin(), src_last = src_ch.end();
  typename QList<TrgItem>::const_iterator cur    = trg_ch.begin(), trg_last = trg_ch.end();

  for ( ; src_it != src_last; src_it++ ) {
    typename QList<TrgItem>::const_iterator f =
      findEqual<SrcItem, TrgItem, TreeData>( *src_it, cur, trg_last, td );
    if ( f != trg_last )  { 
      // target is found
      // mark all items before found one as "to be deleted"
      for ( typename QList<TrgItem>::const_iterator it = cur; it != f; it++ ) {
        DiffItem<SrcItem,TrgItem> ndiff;
        ndiff.mySrc = td.nullSrc();
        ndiff.myTrg = *it;        // delete item
        d.append( ndiff );
      }
      cur = f;
      DiffItem<SrcItem,TrgItem> ndiff;
      ndiff.mySrc = *src_it;
      ndiff.myTrg = *cur;         // update this (found) item
      d.append( ndiff );
      cur++;
    }
    else {
      // target is not found
      DiffItem<SrcItem,TrgItem> ndiff;
      ndiff.mySrc = *src_it;
      ndiff.myTrg = td.nullTrg(); // add item
      d.append( ndiff );
    }
  }
  // delete rest items
  for ( ; cur != trg_last; cur++ ) {
    DiffItem<SrcItem,TrgItem> ndiff;
    ndiff.mySrc = td.nullSrc();
    ndiff.myTrg = *cur;           // delete item
    d.append( ndiff );
  }
  
  return d;
}

/*!
  \brief Create an item with all its children recursively in the target tree.
  \param src source tree item
  \param parent parent item in the target tree
  \param after item in the target tree after which new item shoud be inserted
  \param td functor class
  \return created item
  \sa synchronize()
*/
template <class SrcItem, class TrgItem, class TreeData>
TrgItem createSubTree( const SrcItem& src, const TrgItem& parent,
                       const TrgItem& after, const TreeData& td )
{
  if ( src == td.nullSrc() )
    return td.nullTrg();

  TrgItem nitem = td.createItem( src, parent, after );
  if ( nitem == td.nullTrg() )
    return nitem;

  QList<SrcItem> ch = td.children( src );
  typename QList<SrcItem>::const_iterator anIt = ch.begin(), aLast = ch.end();
  TrgItem last = td.nullTrg();
  for( ; anIt != aLast; anIt++ )
    last = createSubTree( *anIt, nitem, last, td );

  return nitem;
}

#endif // SUIT_TREESYNC_H
