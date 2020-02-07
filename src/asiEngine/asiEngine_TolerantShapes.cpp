//-----------------------------------------------------------------------------
// Created on: 19 April 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018, Sergey Slyadnev
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
#include <asiEngine_TolerantShapes.h>

// asiAlgo includes
#include <asiAlgo_CheckToler.h>
#include <asiAlgo_Utils.h>

// asiVisu includes
#include <asiAlgo_CheckValidity.h>
#include <asiVisu_MeshResultUtils.h>
#include <asiVisu_Utils.h>

//-----------------------------------------------------------------------------

void asiEngine_TolerantShapes::Populate(const TopoDS_Shape& shape,
                                        const int           numRanges)
{
  if ( numRanges < 1 )
    return;

  // Use small value to expand a bit the extreme tolerance ranges to
  // compensate round-off errors.
  const double resolution = Precision::Confusion()*0.1;

  // Get max and min possible tolerances.
  asiAlgo_CheckValidity checker;
  //
  const double minToler  = Precision::Confusion() - resolution;
  const double maxToler  = checker.MaxTolerance(shape) + resolution;
  const double tolerStep = (maxToler - minToler) / numRanges;

  // Initialize lookup table.
  vtkSmartPointer<vtkLookupTable>
    lookupTable = asiVisu_MeshResultUtils::InitLookupTable(minToler, maxToler);
  //
  lookupTable->ForceBuild();

  // Generate tolerance ranges.
  std::vector<double> tolerRanges;
  //
  for ( int k = 0; k <= numRanges; ++k )
  {
    const double nextToler = minToler + k*tolerStep;
    //
    tolerRanges.push_back(nextToler);
  }

  std::vector<TopoDS_Shape> tolerShapes;
  TopoDS_Shape outOfRangeMin, outOfRangeMax;

  // Analyze tolerances.
  asiAlgo_CheckToler CheckTolerances(shape, m_progress, m_plotter);
  //
  if ( !CheckTolerances.PerformRanges(tolerRanges,
                                      tolerShapes,
                                      outOfRangeMin,
                                      outOfRangeMax) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Tolerance check failed.");
    return;
  }

  if ( m_prsMgr )
    m_prsMgr->DeRenderPresentation( m_model->GetPartNode() );

  float     colorOutOfRangeRGB[3] = {0.25, 0.25, 0.25};
  const int colorOutOfRange       = asiVisu_Utils::ColorToInt(colorOutOfRangeRGB[0],
                                                              colorOutOfRangeRGB[1],
                                                              colorOutOfRangeRGB[2]);

  // Create Node for out-of-range shape.
  if ( !outOfRangeMin.IsNull() )
  {
    // Create Data Node.
    Handle(asiData_TolerantRangeNode)
      range_n = Create_Range(outOfRangeMin, colorOutOfRange, -Precision::Infinite(), tolerRanges[0]);
    //
    if ( !range_n.IsNull() && range_n->IsWellFormed() && m_prsMgr )
      m_prsMgr->Actualize(range_n);
  }

  // Create Node for out-of-range shape.
  if ( !outOfRangeMax.IsNull() )
  {
    // Create Data Node.
    Handle(asiData_TolerantRangeNode)
      range_n = Create_Range(outOfRangeMax, colorOutOfRange, tolerRanges[tolerRanges.size() - 1], Precision::Infinite());
    //
    if ( !range_n.IsNull() && range_n->IsWellFormed() && m_prsMgr )
      m_prsMgr->Actualize(range_n);
  }

  // Create Data Nodes for the tolerance ranges.
  for ( size_t k = 0; k < tolerShapes.size(); ++k )
  {
    if ( tolerShapes[k].IsNull() )
      continue;

    // Get the corresponding tolerance range.
    const double rangeMin = tolerRanges[k];
    const double rangeMax = tolerRanges[k+1];
    const double rangeMid = (rangeMin + rangeMax)*0.5;

    // Choose color.
    double colorRGB[3] = {0.0, 0.0, 0.0};
    lookupTable->GetColor(rangeMid, colorRGB);
    //
    const int color = asiVisu_Utils::ColorToInt(colorRGB[0], colorRGB[1], colorRGB[2]);

    // Create Data Node.
    Handle(asiData_TolerantRangeNode)
      range_n = Create_Range(tolerShapes[k], color, rangeMin, rangeMax);
    //
    if ( range_n.IsNull() || !range_n->IsWellFormed() )
      continue;

    // Actualize.
    if ( m_prsMgr )
      m_prsMgr->Actualize(range_n);
  }
}

//-----------------------------------------------------------------------------

void asiEngine_TolerantShapes::Clean_All()
{
  Handle(ActAPI_HNodeList) nodesToDelete = new ActAPI_HNodeList;

  // Gather all child Nodes.
  for ( Handle(ActAPI_IChildIterator) cit = m_tolShapes->GetChildIterator(true); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    nodesToDelete->Append(node);

    // Clean up presentation.
    if ( m_prsMgr && m_prsMgr->IsPresented(node) )
      m_prsMgr->DeletePresentation(node);
  }

  // Delete all Nodes collected for removal.
  for ( ActAPI_NodeList::Iterator nit( *nodesToDelete.operator->() ); nit.More(); nit.Next() )
    m_model->DeleteNode( nit.Value()->GetId() );
}

//-----------------------------------------------------------------------------

Handle(asiData_TolerantRangeNode)
  asiEngine_TolerantShapes::Create_Range(const TopoDS_Shape& shape,
                                         const int           colorNum,
                                         const double        tolerMin,
                                         const double        tolerMax)
{
  // Add Range Node to the corresponding Partition.
  Handle(asiData_TolerantRangeNode)
    range_n = Handle(asiData_TolerantRangeNode)::DownCast( asiData_TolerantRangeNode::Instance() );
  //
  m_model->GetTolerantRangePartition()->AddNode(range_n);

  // Generate name.
  TCollection_ExtendedString range_name("Range");
  range_name += " [";
  range_name += tolerMin;
  range_name += ", ";
  range_name += tolerMax;
  range_name += "]";

  // Initialize.
  range_n->Init();
  range_n->SetUserFlags(NodeFlag_IsPresentedInPartView);
  range_n->SetName(range_name);
  range_n->SetShape(shape);
  range_n->SetColor(colorNum);
  range_n->SetMinToler(tolerMin);
  range_n->SetMaxToler(tolerMax);

  // Add as child.
  m_tolShapes->AddChildNode(range_n);

  // Return the just created Node.
  return range_n;
}
