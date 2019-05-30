//-----------------------------------------------------------------------------
// Created on: 21 December (*) 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017-present, Sergey Slyadnev
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
#include <asiUI_HistoryGraphAdaptor.h>

// asiUI includes
#include <asiUI_HistoryGraphItem.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkStringArray.h>
#pragma warning(pop)

//-----------------------------------------------------------------------------

vtkSmartPointer<vtkMutableDirectedGraph>
  asiUI_HistoryGraphAdaptor::Convert(const Handle(asiAlgo_History)& history,
                                     const Handle(asiAlgo_Naming)&  naming,
                                     ActAPI_ProgressEntry           progress)
{
  vtkSmartPointer<vtkMutableDirectedGraph>
    result = vtkSmartPointer<vtkMutableDirectedGraph>::New();

  // Array for states
  vtkNew<vtkIntArray> stateArr;
  stateArr->SetNumberOfComponents(1);
  stateArr->SetName(ARRNAME_HISTORY_SUBSHAPE_STATES);

  // Array for vertex labels
  vtkNew<vtkStringArray> labelArr;
  labelArr->SetNumberOfComponents(1);
  labelArr->SetName(ARRNAME_HISTORY_SUBSHAPE_LABELS);

  // Array for evolution attributes associated with arcs
  vtkNew<vtkIntArray> evolArr;
  evolArr->SetNumberOfComponents(1);
  evolArr->SetName(ARRNAME_HISTORY_EVOLUTION_TYPES);

  // Array for pedigree indices (sub-shape IDs)
  vtkNew<vtkIntArray> idsArr;
  idsArr->SetNumberOfComponents(1);
  idsArr->SetName(ARRNAME_HISTORY_ITEM_IDS);

  typedef
    NCollection_DataMap<asiAlgo_History::t_item*,
                        vtkIdType,
                        asiAlgo_History::t_item::Hasher> t_itemNodeMap;
  //
  t_itemNodeMap ItemNodeMap;

  // Add nodes
  for ( asiAlgo_History::RandomIterator hit(history); hit.More(); hit.Next() )
  {
    // Get current item
    asiAlgo_History::t_item* pItem = hit.GetItem();
    //
    const TopoDS_Shape& shape = hit.GetShape();
    //
    const int itemId = hit.GetIndex();

    // Register id
    idsArr->InsertNextValue(itemId);

    // Create VTK data set for graph data
    const vtkIdType vid = result->AddVertex();
    ItemNodeMap.Bind(pItem, vid);

    // Prepare label
    std::string label;
    //
    label += "Shape: ";
    label += asiAlgo_Utils::ShapeAddrWithPrefix(shape).c_str();
    //
    if ( !naming.IsNull() )
    {
      TCollection_AsciiString shapeName;
      naming->FindName(shape, shapeName);
      //
      label += " / ";
      label += ( shapeName.IsEmpty() ? "<empty>" : shapeName.ToCString() );
    }

    // Add label
    labelArr->InsertNextValue(label);

    // State
    if ( history->IsRoot(pItem) )
      stateArr->InsertNextValue(ARRNAME_HISTORY_SUBSHAPE_STATE_ROOT);
    else
    {
      if ( pItem->IsDeleted )
        stateArr->InsertNextValue(ARRNAME_HISTORY_SUBSHAPE_STATE_DELETED);
      else if ( pItem->IsActive )
        stateArr->InsertNextValue(ARRNAME_HISTORY_SUBSHAPE_STATE_ACTIVE);
      else
        stateArr->InsertNextValue(ARRNAME_HISTORY_SUBSHAPE_STATE_INACTIVE);
    }
  }

  // Add arcs
  for ( t_itemNodeMap::Iterator it(ItemNodeMap); it.More(); it.Next() )
  {
    // Get current item
    asiAlgo_History::t_item* pItem = it.Key();

    // Add arcs for generated items
    for ( size_t k = 0; k < pItem->Generated.size(); ++k )
    {
      // Check vertex id
      if ( !ItemNodeMap.IsBound(pItem->Generated[k]) )
      {
        progress.SendLogMessage(LogWarn(Normal) << "History item is not bound to VTK node");
        continue;
      }

      // Add arc
      result->AddEdge( ItemNodeMap(pItem), ItemNodeMap(pItem->Generated[k]) );

      // Add arc attribute
      evolArr->InsertNextValue(ARRNAME_HISTORY_EVOLUTION_TYPE_GENERATED);
    }

    // Add arcs for modified items
    for ( size_t k = 0; k < pItem->Modified.size(); ++k )
    {
      // Check vertex id
      if ( !ItemNodeMap.IsBound(pItem->Modified[k]) )
      {
        progress.SendLogMessage(LogWarn(Normal) << "History item is not bound to VTK node");
        continue;
      }

      // Add arc
      result->AddEdge( ItemNodeMap(pItem), ItemNodeMap(pItem->Modified[k]) );

      // Add arc attribute
      evolArr->InsertNextValue(ARRNAME_HISTORY_EVOLUTION_TYPE_MODIFIED);
    }
  }

  // Set arrays
  result->GetVertexData() ->AddArray( stateArr.GetPointer() );
  result->GetVertexData() ->AddArray( labelArr.GetPointer() );
  result->GetVertexData() ->AddArray( idsArr.GetPointer() );
  result->GetEdgeData()   ->AddArray( evolArr.GetPointer() );

  return result;
}
