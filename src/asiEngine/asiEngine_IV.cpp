//-----------------------------------------------------------------------------
// Created on: 11 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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
#include <asiEngine_IV.h>

// asiAlgo includes
#include <asiAlgo_MeshConvert.h>

// asiVisu includes
#include <asiVisu_BCurveHandlesPipeline.h>
#include <asiVisu_BCurveRepersPipeline.h>
#include <asiVisu_CurvePipeline.h>

// Active Data includes
#include <ActData_UniqueNodeName.h>

// OCCT includes
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Precision.hxx>

//-----------------------------------------------------------------------------

//! \return newly created IV Node.
Handle(asiData_IVNode) asiEngine_IV::Create_IV()
{
  // Add IV Node to Partition
  Handle(asiData_IVNode) iv_n = Handle(asiData_IVNode)::DownCast( asiData_IVNode::Instance() );
  m_model->GetIVPartition()->AddNode(iv_n);

  // Initialize Node
  iv_n->Init();
  iv_n->SetName("Imperative Viewer");

  // Create underlying Points 2D
  {
    Handle(ActAPI_INode) iv_points_2d_base = asiData_IVPoints2dNode::Instance();
    m_model->GetIVPoints2dPartition()->AddNode(iv_points_2d_base);

    // Initialize
    Handle(asiData_IVPoints2dNode) iv_points_2d_n = Handle(asiData_IVPoints2dNode)::DownCast(iv_points_2d_base);
    iv_points_2d_n->Init();
    iv_points_2d_n->SetName("Points 2D");

    // Add as child
    iv_n->AddChildNode(iv_points_2d_n);
  }

  // Create underlying Points
  {
    Handle(ActAPI_INode) iv_points_base = asiData_IVPointsNode::Instance();
    m_model->GetIVPointsPartition()->AddNode(iv_points_base);

    // Initialize
    Handle(asiData_IVPointsNode) iv_points_n = Handle(asiData_IVPointsNode)::DownCast(iv_points_base);
    iv_points_n->Init();
    iv_points_n->SetName("Points");

    // Add as child
    iv_n->AddChildNode(iv_points_n);
  }

  // Create underlying Curves 2D
  {
    Handle(ActAPI_INode) iv_curves2d_base = asiData_IVCurves2dNode::Instance();
    m_model->GetIVCurves2dPartition()->AddNode(iv_curves2d_base);

    // Initialize
    Handle(asiData_IVCurves2dNode) iv_curves2d_n = Handle(asiData_IVCurves2dNode)::DownCast(iv_curves2d_base);
    iv_curves2d_n->Init();
    iv_curves2d_n->SetName("Curves 2D");

    // Add as child
    iv_n->AddChildNode(iv_curves2d_n);
  }

  // Create underlying Curves
  {
    Handle(ActAPI_INode) iv_curves_base = asiData_IVCurvesNode::Instance();
    m_model->GetIVCurvesPartition()->AddNode(iv_curves_base);

    // Initialize
    Handle(asiData_IVCurvesNode) iv_curves_n = Handle(asiData_IVCurvesNode)::DownCast(iv_curves_base);
    iv_curves_n->Init();
    iv_curves_n->SetName("Curves");

    // Add as child
    iv_n->AddChildNode(iv_curves_n);
  }

  // Create underlying Surfaces
  {
    Handle(ActAPI_INode) iv_surfaces_base = asiData_IVSurfacesNode::Instance();
    m_model->GetIVSurfacesPartition()->AddNode(iv_surfaces_base);

    // Initialize
    Handle(asiData_IVSurfacesNode) iv_surfaces_n = Handle(asiData_IVSurfacesNode)::DownCast(iv_surfaces_base);
    iv_surfaces_n->Init();
    iv_surfaces_n->SetName("Surfaces");

    // Add as child
    iv_n->AddChildNode(iv_surfaces_n);
  }

  // Create underlying Topology container
  {
    Handle(ActAPI_INode) iv_topo_base = asiData_IVTopoNode::Instance();
    m_model->GetIVTopoPartition()->AddNode(iv_topo_base);

    // Initialize
    Handle(asiData_IVTopoNode) iv_topo_n = Handle(asiData_IVTopoNode)::DownCast(iv_topo_base);
    iv_topo_n->Init();
    iv_topo_n->SetName("Topology");

    // Add as child
    iv_n->AddChildNode(iv_topo_n);
  }

  // Create underlying Tessellation container
  {
    Handle(ActAPI_INode) iv_tess_base = asiData_IVTessNode::Instance();
    m_model->GetIVTessPartition()->AddNode(iv_tess_base);

    // Initialize
    Handle(asiData_IVTessNode) iv_tess_n = Handle(asiData_IVTessNode)::DownCast(iv_tess_base);
    iv_tess_n->Init();
    iv_tess_n->SetName("Tessellation");

    // Add as child
    iv_n->AddChildNode(iv_tess_n);
  }

  // Create underlying Text container
  {
    Handle(ActAPI_INode) iv_text_base = asiData_IVTextNode::Instance();
    m_model->GetIVTextPartition()->AddNode(iv_text_base);

    // Initialize
    Handle(asiData_IVTextNode) iv_text_n = Handle(asiData_IVTextNode)::DownCast(iv_text_base);
    iv_text_n->Init();
    iv_text_n->SetName("Text");

    // Add as child
    iv_n->AddChildNode(iv_text_n);
  }

  // Return the just created Node
  return iv_n;
}

//-----------------------------------------------------------------------------

//! Cleans all data objects related to imperative plotter.
void asiEngine_IV::Clean_All()
{
  this->Clean_Curves();
  this->Clean_Curves2d();
  this->Clean_Points();
  this->Clean_Points2d();
  this->Clean_Surfaces();
  this->Clean_Tess();
  this->Clean_Text();
  this->Clean_Topo();
}

//-----------------------------------------------------------------------------

//! Cleans objects by name. If several objects with the given name are found,
//! all them are deleted. The search is done in a tree branch rooted by
//! Imperative Viewer Node.
//!
//! \param name [in] name of the target object.
void asiEngine_IV::Clean(const TCollection_AsciiString& name)
{
  Handle(ActAPI_HNodeList) nodesToDelete = new ActAPI_HNodeList;

  // Find all Nodes with the given name
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->GetChildIterator(true); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    TCollection_ExtendedString nodeName = node->GetName();
    //
    if ( nodeName.IsEqual(name) )
      nodesToDelete->Append(node);
  }

  // Delete all Nodes queued for removal
  for ( ActAPI_NodeList::Iterator nit( *nodesToDelete.operator->() ); nit.More(); nit.Next() )
    m_model->DeleteNode( nit.Value()->GetId() );
}

//-----------------------------------------------------------------------------

//! Finds Node with the given name. Returns nullptr if nothing is found.
//! \param name [in] target name.
//! \return found Node (the first one if several exist) or nullptr.
Handle(asiData_IVPointSet2dNode)
  asiEngine_IV::Find_PointSet2d(const TCollection_AsciiString& name)
{
  // Find the first Node with the given name
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->Points2d()->GetChildIterator(true); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    TCollection_ExtendedString nodeName = node->GetName();
    //
    if ( nodeName.IsEqual(name) )
      return Handle(asiData_IVPointSet2dNode)::DownCast(node);
  }

  return nullptr;
}

//-----------------------------------------------------------------------------

//! Creates Point Set 2D Node.
//! \param coords        [in] target point cloud.
//! \param name          [in] name to set (auto-generated if empty).
//! \param useAutoNaming [in] indicates whether to auto-name entities.
//! \return Point Set 2D Node.
Handle(asiData_IVPointSet2dNode)
  asiEngine_IV::Create_PointSet2d(const Handle(TColStd_HArray1OfReal)& coords,
                                  const TCollection_AsciiString&       name,
                                  const bool                           useAutoNaming)
{
  // Access Model and parent Node
  Handle(asiData_IVPoints2dNode) IV_Parent = m_model->GetIVNode()->Points2d();

  // Add Point Set Node to Partition
  Handle(asiData_IVPointSet2dNode) item_n = Handle(asiData_IVPointSet2dNode)::DownCast( asiData_IVPointSet2dNode::Instance() );
  m_model->GetIVPointSet2dPartition()->AddNode(item_n);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Point Set 2D" : name );
  if ( useAutoNaming )
    item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);

  // Initialize
  item_n->Init();
  item_n->SetUserFlags(NodeFlag_IsPresentedInPartView);
  item_n->SetName(item_name);
  item_n->SetPoints(coords);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

//! Creates a two-dimensional point set from a vector of gp_Pnt2d objects.
//! \param points        [in] vector to store.
//! \param name          [in] desired object name.
//! \param useAutoNaming [in] indicates whether to auto-name entities.
Handle(asiData_IVPointSet2dNode)
  asiEngine_IV::Create_PointSet2d(const std::vector<gp_Pnt2d>&   points,
                                  const TCollection_AsciiString& name,
                                  const bool                     useAutoNaming)
{
  Handle(TColStd_HArray1OfReal) hcoords = new TColStd_HArray1OfReal(0, (int) points.size()*2 - 1);
  //
  for ( int i = 0; i < (int) points.size(); ++i )
  {
    hcoords->ChangeValue(i*2 + 0) = points[i].X();
    hcoords->ChangeValue(i*2 + 1) = points[i].Y();
  }

  // Create from coordinates
  return Create_PointSet2d(hcoords, name, useAutoNaming);
}

//-----------------------------------------------------------------------------

//! Deletes all Point Set 2D Nodes.
void asiEngine_IV::Clean_Points2d()
{
  Handle(asiData_IVPoints2dNode)
    IV_Parent = m_model->GetIVNode()->Points2d();
  //
  this->_cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Finds Node with the given name. Returns nullptr if nothing is found.
//! \param name [in] target name.
//! \return found Node (the first one if several exist) or nullptr.
Handle(asiData_IVPointSetNode)
  asiEngine_IV::Find_PointSet(const TCollection_AsciiString& name)
{
  // Find the first Node with the given name
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->Points()->GetChildIterator(true); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    TCollection_ExtendedString nodeName = node->GetName();
    //
    if ( nodeName.IsEqual(name) )
      return Handle(asiData_IVPointSetNode)::DownCast(node);
  }

  return nullptr;
}

//-----------------------------------------------------------------------------

//! Creates Point Set Node.
//! \param points        [in] target point cloud.
//! \param name          [in] name to set (auto-generated if empty).
//! \param useAutoNaming [in] indicates whether to auto-name entities.
//! \return Point Set Node.
Handle(asiData_IVPointSetNode)
  asiEngine_IV::Create_PointSet(const Handle(asiAlgo_BaseCloud<double>)& points,
                                const TCollection_AsciiString&           name,
                                const bool                               useAutoNaming)
{
  // Access Model and parent Node
  Handle(asiData_IVPointsNode) IV_Parent = m_model->GetIVNode()->Points();

  // Add Point Set Node to Partition
  Handle(asiData_IVPointSetNode) item_n = Handle(asiData_IVPointSetNode)::DownCast( asiData_IVPointSetNode::Instance() );
  m_model->GetIVPointSetPartition()->AddNode(item_n);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Point Set" : name );
  if ( useAutoNaming )
    item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);

  // Initialize
  item_n->Init();
  item_n->SetUserFlags(NodeFlag_IsPresentedInPartView);
  item_n->SetName(item_name);
  item_n->SetPoints(points);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

//! Deletes all Point Set Nodes.
void asiEngine_IV::Clean_Points()
{
  Handle(asiData_IVPointsNode)
    IV_Parent = m_model->GetIVNode()->Points();
  //
  this->_cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! \return last existing curve.
Handle(asiData_IVCurveNode) asiEngine_IV::Get_LastCurve()
{
  Handle(asiData_IVCurveNode) lastCurve;
  //
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->Curves()->GetChildIterator(true);
        cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    lastCurve = Handle(asiData_IVCurveNode)::DownCast(node);
  }

  return lastCurve;
}

//-----------------------------------------------------------------------------

//! Finds Node with the given name. Returns nullptr if nothing is found.
//! \param name [in] target name.
//! \return found Node (the first one if several exist) or nullptr.
Handle(asiData_IVCurveNode)
  asiEngine_IV::Find_Curve(const TCollection_AsciiString& name)
{
  // Find the first Node with the given name
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->Curves()->GetChildIterator(true);
        cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    TCollection_ExtendedString nodeName = node->GetName();
    //
    if ( nodeName.IsEqual(name) )
      return Handle(asiData_IVCurveNode)::DownCast(node);
  }

  return nullptr;
}

//-----------------------------------------------------------------------------

//! Creates Curve Node.
//! \param curve         [in] parametric curve to store.
//! \param uLimit        [in] absolute value to bound infinite parametric domain.
//! \param drawOriTip    [in] indicates whether to draw orientation tip.
//! \param name          [in] name to set (auto-generated if empty).
//! \param useAutoNaming [in] indicates whether to auto-name entities.
//! \return newly created Node.
Handle(asiData_IVCurveNode)
  asiEngine_IV::Create_Curve(const Handle(Geom_Curve)&      curve,
                             const double                   uLimit,
                             const bool                     drawOriTip,
                             const TCollection_AsciiString& name,
                             const bool                     useAutoNaming)
{
  if ( curve.IsNull() )
    return nullptr;

  Handle(asiData_IVCurveNode)
    item_n = this->Create_Curve(name, useAutoNaming);

  this->Update_Curve(item_n, curve, uLimit, drawOriTip);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

Handle(asiData_IVCurveNode)
  asiEngine_IV::Create_Curve(const TCollection_AsciiString& name,
                             const bool                     useAutoNaming)
{
  // Access Model and parent Node
  Handle(asiData_IVCurvesNode) IV_Parent = m_model->GetIVNode()->Curves();

  // Add Curve Node to Partition
  Handle(asiData_IVCurveNode) item_n = Handle(asiData_IVCurveNode)::DownCast( asiData_IVCurveNode::Instance() );
  m_model->GetIVCurvePartition()->AddNode(item_n);

  // Initialize
  item_n->Init();
  item_n->SetUserFlags(NodeFlag_IsPresentedInPartView);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Curve" : name );
  if ( useAutoNaming )
    item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);
  //
  item_n->SetName(item_name);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

//! Updates Curve Node with the passed curve data.
//! \param node       [in] Curve Node to update.
//! \param curve      [in] parametric curve to store.
//! \param uLimit     [in] absolute value to bound infinite parametric domain.
//! \param drawOriTip [in] indicates whether to draw orientation tip.
void asiEngine_IV::Update_Curve(const Handle(asiData_IVCurveNode)& node,
                                const Handle(Geom_Curve)&          curve,
                                const double                       uLimit,
                                const bool                         drawOriTip)
{
  // Handle infinite domains
  double             f      = curve->FirstParameter();
  double             l      = curve->LastParameter();
  bool               toTrim = false;
  Handle(Geom_Curve) geometry;
  //
  if ( Precision::IsInfinite(f) )
  {
    f      = -uLimit;
    toTrim = true;
  }
  if ( Precision::IsInfinite(l) )
  {
    l = uLimit;
    if ( !toTrim ) toTrim = true;
  }
  //
  if ( toTrim )
    geometry = new Geom_TrimmedCurve(curve, f, l);
  else
    geometry = curve;

  // Initialize
  node->Init();
  node->SetUserFlags(NodeFlag_IsPresentedInPartView);
  node->SetCurve(geometry, f, l);
  node->SetDrawOrientationTip(drawOriTip);
}

//-----------------------------------------------------------------------------

//! Deletes all Curve Nodes.
void asiEngine_IV::Clean_Curves()
{
  Handle(asiData_IVCurvesNode)
    IV_Parent = m_model->GetIVNode()->Curves();
  //
  this->_cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Enables or disables interactive picking for the curve handles.
//! \param[in] on      true to enable, false to disable.
//! \param[in] create  indicates whether to activate creation or modification
//!                    of handles.
//! \param[in] pPrsMgr Presentation Manager to access Presentations of Curve Nodes.
void asiEngine_IV::ActivateCurveHandles(const bool          on,
                                        const bool          create,
                                        asiVisu_PrsManager* pPrsMgr)
{
  // Get all Curve Nodes.
  Handle(asiData_Partition<asiData_IVCurveNode>) P = m_model->GetIVCurvePartition();
  //
  for ( ActData_BasePartition::Iterator pit(P); pit.More(); pit.Next() )
  {
    Handle(asiData_IVCurveNode)
      curveNode = Handle(asiData_IVCurveNode)::DownCast( pit.Value() );

    // Get Presentation of Curve.
    if ( !pPrsMgr->IsPresented(curveNode) )
      continue;
    //
    Handle(asiVisu_Prs) curvePrs = pPrsMgr->GetPresentation(curveNode);

    // Iterate over all pipelines making all actors selectable.
    Handle(asiVisu_HPipelineList) pipelines = curvePrs->GetPipelineList();
    //
    for ( asiVisu_HPipelineList::Iterator it(*pipelines); it.More(); it.Next() )
    {
      const Handle(asiVisu_Pipeline)& pl = it.Value();

      // Show handles and adjust pickability.
      if ( pl->IsKind( STANDARD_TYPE(asiVisu_CurvePipeline) ) )
      {
        pl->Actor()->SetPickable(create ? 1 : 0);
      }
      else if ( pl->IsKind( STANDARD_TYPE(asiVisu_BCurveHandlesPipeline) ) )
      {
        pl->Actor()->SetVisibility(on ? 1 : 0);
        pl->Actor()->SetPickable(create ? 0 : 1);
      }
    }
  }
}

//-----------------------------------------------------------------------------

//! Enables or disables interactive picking for the curve repers.
//! \param[in] on      true to enable, false to disable.
//! \param[in] pPrsMgr Presentation Manager to access Presentations of Curve Nodes.
void asiEngine_IV::ActivateCurveRepers(const bool          on,
                                       asiVisu_PrsManager* pPrsMgr)
{
  // Get all Curve Nodes.
  Handle(asiData_Partition<asiData_IVCurveNode>) P = m_model->GetIVCurvePartition();
  //
  for ( ActData_BasePartition::Iterator pit(P); pit.More(); pit.Next() )
  {
    Handle(asiData_IVCurveNode)
      curveNode = Handle(asiData_IVCurveNode)::DownCast( pit.Value() );

    // Get Presentation of Curve.
    if ( !pPrsMgr->IsPresented(curveNode) )
      continue;
    //
    Handle(asiVisu_Prs) curvePrs = pPrsMgr->GetPresentation(curveNode);

    // Iterate over all pipelines making all actors selectable.
    Handle(asiVisu_HPipelineList) pipelines = curvePrs->GetPipelineList();
    //
    for ( asiVisu_HPipelineList::Iterator it(*pipelines); it.More(); it.Next() )
    {
      const Handle(asiVisu_Pipeline)& pl = it.Value();

      // Adjust pickability.
      if ( pl->IsKind( STANDARD_TYPE(asiVisu_BCurveRepersPipeline) ) )
      {
        pl->Actor()->SetVisibility(on ? 1 : 0);
        pl->Actor()->SetPickable(on ? 1 : 0);
      }
    }
  }
}

//-----------------------------------------------------------------------------

//! Finds Node with the given name. Returns nullptr if nothing is found.
//! \param name [in] target name.
//! \return found Node (the first one if several exist) or nullptr.
Handle(asiData_IVCurve2dNode)
  asiEngine_IV::Find_Curve2d(const TCollection_AsciiString& name)
{
  // Find the first Node with the given name
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->Curves2d()->GetChildIterator(true); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    TCollection_ExtendedString nodeName = node->GetName();
    //
    if ( nodeName.IsEqual(name) )
      return Handle(asiData_IVCurve2dNode)::DownCast(node);
  }

  return nullptr;
}

//-----------------------------------------------------------------------------

//! Creates Curve 2D Node.
//! \param curve         [in] parametric curve to store.
//! \param surface       [in] host surface.
//! \param uLimit        [in] absolute value to bound infinite parametric domain.
//! \param name          [in] name to set (auto-generated if empty).
//! \param useAutoNaming [in] indicates whether to auto-name entities.
//! \return newly created Node.
Handle(asiData_IVCurve2dNode)
  asiEngine_IV::Create_Curve2d(const Handle(Geom2d_Curve)&    curve,
                               const Handle(Geom_Surface)&    surface,
                               const double                   uLimit,
                               const TCollection_AsciiString& name,
                               const bool                     useAutoNaming)
{
  if ( curve.IsNull() )
    return nullptr;

  // Access Model and parent Node
  Handle(asiData_IVCurves2dNode) IV_Parent = m_model->GetIVNode()->Curves2d();

  // Add Curve Node to Partition
  Handle(asiData_IVCurve2dNode) item_n = Handle(asiData_IVCurve2dNode)::DownCast( asiData_IVCurve2dNode::Instance() );
  m_model->GetIVCurve2dPartition()->AddNode(item_n);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "CONS" : name );
  if ( useAutoNaming )
    item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);
  //
  item_n->SetName(item_name);

  // Initialize Node
  Update_Curve2d(item_n, curve, surface, uLimit);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

//! Updates Curve Node with the passed curve data.
//! \param node    [in] Curve Node to update.
//! \param curve   [in] parametric curve to store.
//! \param surface [in] host surface.
//! \param uLimit  [in] absolute value to bound infinite parametric domain.
void asiEngine_IV::Update_Curve2d(const Handle(asiData_IVCurve2dNode)& node,
                                  const Handle(Geom2d_Curve)&          curve,
                                  const Handle(Geom_Surface)&          surface,
                                  const double                         uLimit)
{
  // Handle infinite domains
  double               f      = curve->FirstParameter();
  double               l      = curve->LastParameter();
  bool                 toTrim = false;
  Handle(Geom2d_Curve) geometry;
  //
  if ( Precision::IsInfinite(f) )
  {
    f      = -uLimit;
    toTrim = true;
  }
  if ( Precision::IsInfinite(l) )
  {
    l = uLimit;
    if ( !toTrim ) toTrim = true;
  }
  //
  if ( toTrim )
    geometry = new Geom2d_TrimmedCurve(curve, f, l);
  else
    geometry = curve;

  // Initialize
  node->Init();
  node->SetUserFlags(NodeFlag_IsPresentedInPartView);
  node->SetCONS(geometry, surface, f, l);
}

//-----------------------------------------------------------------------------

//! Deletes all Curve 2D Nodes.
void asiEngine_IV::Clean_Curves2d()
{
  Handle(asiData_IVCurves2dNode)
    IV_Parent = m_model->GetIVNode()->Curves2d();
  //
  this->_cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Finds Node with the given name. Returns nullptr if nothing is found.
//! \param name [in] target name.
//! \return found Node (the first one if several exist) or nullptr.
Handle(asiData_IVSurfaceNode)
  asiEngine_IV::Find_Surface(const TCollection_AsciiString& name)
{
  // Find the first Node with the given name
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->Surfaces()->GetChildIterator(true); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    TCollection_ExtendedString nodeName = node->GetName();
    //
    if ( nodeName.IsEqual(name) )
      return Handle(asiData_IVSurfaceNode)::DownCast(node);
  }

  return nullptr;
}

//-----------------------------------------------------------------------------

//! Creates Surface Node.
//! \param surface       [in] parametric surface to store.
//! \param uMin          [in] value to trim min U bound.
//! \param uMax          [in] value to trim max U bound.
//! \param vMin          [in] value to trim min V bound.
//! \param vMax          [in] value to trim max V bound.
//! \param name          [in] name to set (auto-generated if empty).
//! \param useAutoNaming [in] indicates whether to auto-name entities.
//! \return newly created Node.
Handle(asiData_IVSurfaceNode)
  asiEngine_IV::Create_Surface(const Handle(Geom_Surface)&    surface,
                               const double                   uMin,
                               const double                   uMax,
                               const double                   vMin,
                               const double                   vMax,
                               const TCollection_AsciiString& name,
                               const bool                     useAutoNaming)
{
  if ( surface.IsNull() )
    return nullptr;

  // Access Model and parent Node
  Handle(asiData_IVSurfacesNode) IV_Parent = m_model->GetIVNode()->Surfaces();

  // Add Surface Node to Partition
  Handle(asiData_IVSurfaceNode) item_n = Handle(asiData_IVSurfaceNode)::DownCast( asiData_IVSurfaceNode::Instance() );
  m_model->GetIVSurfacePartition()->AddNode(item_n);

  // Generate unique name if a good name is not passed
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Surface" : name );
  if ( useAutoNaming )
    item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);
  //
  item_n->SetName(item_name);

  // Initialize
  Update_Surface(item_n, surface, uMin, uMax, vMin, vMax);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

//! Creates a Surface Deviation Node under the given parent.
//! \param[in] name   name of the Node to create.
//! \param[in] parent parent Node.
Handle(asiData_SurfDeviationNode)
  asiEngine_IV::Create_SurfaceDeviation(const TCollection_AsciiString&       name,
                                        const Handle(asiData_IVSurfaceNode)& parent)
{
  if ( parent.IsNull() || !parent->IsWellFormed() )
    return nullptr;

  // Add new Node to the corresponding Partition.
  Handle(asiData_SurfDeviationNode)
    sd_n = Handle(asiData_SurfDeviationNode)::DownCast( asiData_SurfDeviationNode::Instance() );
  //
  m_model->GetSurfDeviationPartition()->AddNode(sd_n);

  // Get reference mesh.
  Handle(asiData_TriangulationNode) tris_n = m_model->GetTriangulationNode();

  // Initialize Node and connect the surface in question to the reference
  // mesh which is by convention stored in the sole Triangulation Node.
  // ...

  sd_n->Init();
  sd_n->SetName(name);

  // Connect surface.
  if ( !parent.IsNull() )
    sd_n->ConnectReference( asiData_SurfDeviationNode::PID_SurfaceRef, parent );
  else
    m_progress.SendLogMessage(LogErr(Normal) << "Reference Surface Node is not available.");

  // Connect mesh.
  if ( !tris_n.IsNull() )
    sd_n->ConnectReference( asiData_SurfDeviationNode::PID_MeshRef, tris_n );
  else
    m_progress.SendLogMessage(LogErr(Normal) << "Reference Triangulation Node is not available.");

  // Add to parent.
  parent->AddChildNode(sd_n);

  return sd_n;
}

//-----------------------------------------------------------------------------

//! Updates Surface Node.
//! \param node    [in] Data Node to update.
//! \param surface [in] parametric surface to store.
//! \param uMin    [in] trimming value for U min.
//! \param uMax    [in] trimming value for U max.
//! \param vMin    [in] trimming value for V min.
//! \param vMax    [in] trimming value for V max.
void asiEngine_IV::Update_Surface(const Handle(asiData_IVSurfaceNode)& node,
                                  const Handle(Geom_Surface)&          surface,
                                  const double                         uMin,
                                  const double                         uMax,
                                  const double                         vMin,
                                  const double                         vMax)
{
  // Initialize
  node->Init();
  node->SetUserFlags(NodeFlag_IsPresentedInPartView);
  node->SetSurface(surface);
  node->SetLimits(uMin, uMax, vMin, vMax);
}

//-----------------------------------------------------------------------------

//! Deletes all Surface Nodes.
void asiEngine_IV::Clean_Surfaces()
{
  Handle(asiData_IVSurfacesNode)
    IV_Parent = m_model->GetIVNode()->Surfaces();
  //
  this->_cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Finds Node with the given name. Returns nullptr if nothing is found.
//! \param name [in] target name.
//! \return found Node (the first one if several exist) or nullptr.
Handle(asiData_IVTopoItemNode)
  asiEngine_IV::Find_TopoItem(const TCollection_AsciiString& name)
{
  // Find the first Node with the given name
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->Topology()->GetChildIterator(true); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    TCollection_ExtendedString nodeName = node->GetName();
    //
    if ( nodeName.IsEqual(name) )
      return Handle(asiData_IVTopoItemNode)::DownCast(node);
  }

  return nullptr;
}

//-----------------------------------------------------------------------------

//! Creates Topological Item Node.
//! \param shape         [in] shape to store.
//! \param name          [in] name to set (auto-generated if empty).
//! \param useAutoNaming [in] indicates whether to auto-name entities.
//! \return newly created Node.
Handle(asiData_IVTopoItemNode)
  asiEngine_IV::Create_TopoItem(const TopoDS_Shape&            shape,
                                const TCollection_AsciiString& name,
                                const bool                     useAutoNaming)
{
  // Access Model and parent Node
  Handle(asiData_IVTopoNode) IV_Parent = m_model->GetIVNode()->Topology();

  // Add Topological Item Node to Partition
  Handle(asiData_IVTopoItemNode)
    item_n = Handle(asiData_IVTopoItemNode)::DownCast( asiData_IVTopoItemNode::Instance() );
  //
  m_model->GetIVTopoItemPartition()->AddNode(item_n);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Shape" : name );
  if ( useAutoNaming )
    item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);
  //
  item_n->SetName(item_name);

  // Initialize
  Update_TopoItem(item_n, shape);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

//! Updates Topo Item Node.
//! \param node  [in] Data Node to update.
//! \param shape [in] B-Rep shape to store.
void asiEngine_IV::Update_TopoItem(const Handle(asiData_IVTopoItemNode)& node,
                                   const TopoDS_Shape&                   shape)
{
  // Initialize
  node->Init();
  node->SetUserFlags(NodeFlag_IsPresentedInPartView);
  node->SetShape(shape);
}

//-----------------------------------------------------------------------------

//! Deletes all topological items.
void asiEngine_IV::Clean_Topo()
{
  Handle(asiData_IVTopoNode)
    IV_Parent = m_model->GetIVNode()->Topology();
  //
  this->_cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Finds Node with the given name. Returns nullptr if nothing is found.
//! \param name [in] target name.
//! \return found Node (the first one if several exist) or nullptr.
Handle(asiData_IVTessItemNode)
  asiEngine_IV::Find_TessItem(const TCollection_AsciiString& name)
{
  // Find the first Node with the given name
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->Tessellation()->GetChildIterator(true); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    TCollection_ExtendedString nodeName = node->GetName();
    //
    if ( nodeName.IsEqual(name) )
      return Handle(asiData_IVTessItemNode)::DownCast(node);
  }

  return nullptr;
}

//-----------------------------------------------------------------------------

//! Creates tessellation item.
//! \param tess          [in] surface triangulation to create a tessellation item from.
//! \param name          [in] name of the tessellation item.
//! \param useAutoNaming [in] indicates whether to auto-name entities.
//! \return newly created tessellation item.
Handle(asiData_IVTessItemNode)
  asiEngine_IV::Create_TessItem(const Handle(Poly_Triangulation)& tess,
                                const TCollection_AsciiString&    name,
                                const bool                        useAutoNaming)
{
  // Access Model and parent Node
  Handle(asiData_IVTessNode) IV_Parent = m_model->GetIVNode()->Tessellation();

  // Add Tessellation Item Node to Partition
  Handle(asiData_IVTessItemNode) item_n = Handle(asiData_IVTessItemNode)::DownCast( asiData_IVTessItemNode::Instance() );
  m_model->GetIVTessItemPartition()->AddNode(item_n);

  // Generate unique name
  TCollection_ExtendedString item_name = ( name.IsEmpty() ? "Mesh" : name );
  if ( useAutoNaming )
    item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);
  //
  item_n->SetName(item_name);

  // Initialize
  Update_TessItem(item_n, tess);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

//! Updates Tessellation Item Node.
//! \param node [in] Data Node to update.
//! \param tess [in] triangulation to store.
void asiEngine_IV::Update_TessItem(const Handle(asiData_IVTessItemNode)& node,
                                   const Handle(Poly_Triangulation)&     tess)
{
  // Convert to AD mesh
  Handle(ActData_Mesh) mesh;
  if ( !asiAlgo_MeshConvert::ToPersistent(tess, mesh) )
    return;

  // Initialize
  node->Init();
  node->SetUserFlags(NodeFlag_IsPresentedInPartView);
  node->SetMesh(mesh);
}

//-----------------------------------------------------------------------------

//! Deletes all tessellation items.
void asiEngine_IV::Clean_Tess()
{
  Handle(asiData_IVTessNode)
    IV_Parent = m_model->GetIVNode()->Tessellation();
  //
  this->_cleanChildren(IV_Parent);
}

//-----------------------------------------------------------------------------

//! Finds Node with the given name. Returns nullptr if nothing is found.
//! \param name [in] target name.
//! \return found Node (the first one if several exist) or nullptr.
Handle(asiData_IVTextItemNode)
  asiEngine_IV::Find_TextItem(const TCollection_AsciiString& name)
{
  // Find the first Node with the given name
  for ( Handle(ActAPI_IChildIterator) cit = m_model->GetIVNode()->Text()->GetChildIterator(true); cit->More(); cit->Next() )
  {
    Handle(ActAPI_INode) node = cit->Value();
    //
    if ( node.IsNull() || !node->IsWellFormed() )
      continue;

    TCollection_ExtendedString nodeName = node->GetName();
    //
    if ( nodeName.IsEqual(name) )
      return Handle(asiData_IVTextItemNode)::DownCast(node);
  }

  return nullptr;
}

//-----------------------------------------------------------------------------

//! Creates text item.
//! \param text          [in] text to store in the persistent item.
//! \param name          [in] name to set.
//! \param useAutoNaming [in] indicates whether to auto-name entities.
//! \return newly created text item.
Handle(asiData_IVTextItemNode)
  asiEngine_IV::Create_TextItem(const TCollection_AsciiString& text,
                                const TCollection_AsciiString& name,
                                const bool                     useAutoNaming)
{
  // Access Model and parent Node
  Handle(asiData_IVTextNode) IV_Parent = m_model->GetIVNode()->Text();

  // Add Text Item Node to Partition
  Handle(asiData_IVTextItemNode) item_n = Handle(asiData_IVTextItemNode)::DownCast( asiData_IVTextItemNode::Instance() );
  m_model->GetIVTextItemPartition()->AddNode(item_n);

  // Generate name from text
  TCollection_ExtendedString item_name;
  if ( name.IsEmpty() )
  {
    //
    if ( text.IsEmpty() )
      item_name = "Text";
    else
    {
      item_name = text.SubString( 1, Min(text.Length(), 16) );

      if ( text.Length() > item_name.Length() )
        item_name += "...";
    }
  }
  else
    item_name = name;

  // Generate unique
  if ( useAutoNaming )
    item_name = ActData_UniqueNodeName::Generate(ActData_SiblingNodes::CreateForChild(item_n, IV_Parent), item_name);
  //
  item_n->SetName(item_name);

  // Initialize
  Update_TextItem(item_n, text);

  // Add as child
  IV_Parent->AddChildNode(item_n);

  // Return the just created Node
  return item_n;
}

//-----------------------------------------------------------------------------

//! Updates Text Item Node.
//! \param node [in] Data Node to update.
//! \param text [in] text to store.
void asiEngine_IV::Update_TextItem(const Handle(asiData_IVTextItemNode)& node,
                                   const TCollection_AsciiString&        text)
{
  node->Init();
  node->SetUserFlags(NodeFlag_IsPresentedInPartView);
  node->SetText(text);
}

//-----------------------------------------------------------------------------

//! Deletes all text items.
void asiEngine_IV::Clean_Text()
{
  Handle(asiData_IVTextNode)
    IV_Parent = m_model->GetIVNode()->Text();
  //
  this->_cleanChildren(IV_Parent);
}
