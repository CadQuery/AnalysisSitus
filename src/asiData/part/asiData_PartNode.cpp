//-----------------------------------------------------------------------------
// Created on: 28 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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
#include <asiData_PartNode.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
asiData_PartNode::asiData_PartNode() : ActData_BaseNode()
{
  // Register standard Active Data Parameters.
  REGISTER_PARAMETER(Name,          PID_Name);
  REGISTER_PARAMETER(Bool,          PID_AutoAAG);
  REGISTER_PARAMETER(Shape,         PID_Geometry);
  //
  REGISTER_PARAMETER(Group,         PID_GroupTrsf);
  REGISTER_PARAMETER(Real,          PID_TrsfTx);
  REGISTER_PARAMETER(Real,          PID_TrsfTy);
  REGISTER_PARAMETER(Real,          PID_TrsfTz);
  REGISTER_PARAMETER(Real,          PID_TrsfRx);
  REGISTER_PARAMETER(Real,          PID_TrsfRy);
  REGISTER_PARAMETER(Real,          PID_TrsfRz);
  REGISTER_PARAMETER(RealArray,     PID_TrsfMx);
  //
  REGISTER_PARAMETER(Group,         PID_GroupTess);
  REGISTER_PARAMETER(Real,          PID_TessLinDefl);
  REGISTER_PARAMETER(Real,          PID_TessAngDefl);
  REGISTER_PARAMETER(Bool,          PID_KeepTessParams);
  //
  REGISTER_PARAMETER(Group,         PID_GroupPrs);
  REGISTER_PARAMETER(Int,           PID_DisplayMode);
  REGISTER_PARAMETER(Bool,          PID_UseScalars);
  REGISTER_PARAMETER(Int,           PID_Color);
  REGISTER_PARAMETER(Bool,          PID_HasVertices);
  REGISTER_PARAMETER(Bool,          PID_HasBackface);
  REGISTER_PARAMETER(Bool,          PID_ShowFaultyFaces);
  REGISTER_PARAMETER(Bool,          PID_IsCoincidentTopo);
  //
  REGISTER_PARAMETER(ReferenceList, PID_MetadataElems);

  // Register custom Parameters specific to Analysis Situs.
  this->registerParameter(PID_AAG,    asiData_AAGParameter    ::Instance(), false);
  this->registerParameter(PID_BVH,    asiData_BVHParameter    ::Instance(), false);
  this->registerParameter(PID_Naming, asiData_NamingParameter ::Instance(), false);
}

//! Returns new DETACHED instance of Geometry Node ensuring its correct
//! allocation in a heap.
//! \return new instance of Geometry Node.
Handle(ActAPI_INode) asiData_PartNode::Instance()
{
  return new asiData_PartNode();
}

//! Performs initial actions required to make Geometry Node WELL-FORMED.
void asiData_PartNode::Init(const bool resetNaming)
{
  // Set empty initial B-Rep and AAG.
  this->setShape  ( TopoDS_Shape() );
  this->setAAG    ( nullptr );
  this->setBVH    ( nullptr );
  //
  if ( resetNaming )
    this->setNaming ( nullptr );

  // Set default values to primitive Parameters.
  this->SetAutoAAG               (true);
  this->SetLinearDeflection      (0.0);
  this->SetAngularDeflection     (0.0);
  this->SetKeepTessParams        (false);
  this->SetUseScalars            (true);
  this->SetColor                 (190 << 16 | 190 << 8 | 190); // Initial color.
  this->SetDisplayMode           (0x020);                      // Shading with edges.
  this->SetHasVertices           (false);
  this->SetHasBackface           (false);
  this->SetShowFaultyFaces       (true);
  this->SetResolveCoincidentTopo (true);

  // Set identity transformation.
  ActParamTool::AsRealArray( this->Parameter(PID_TrsfMx) )->SetArray( new HRealArray(0, 11, 0.) );
  //
  this->SetTransformation(0., 0., 0., 0., 0., 0.);

  // Initialize Parameter flags.
  this->InitParameter(PID_Name,             "Name",               "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_AutoAAG,          "Auto-construct AAG", "",               ParameterFlag_IsVisible, true);
  //
  this->InitParameter(PID_GroupTrsf,        "Transformation",     "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TrsfTx,           "Tx",                 "Part_TX",        ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TrsfTy,           "Ty",                 "Part_TY",        ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TrsfTz,           "Tz",                 "Part_TZ",        ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TrsfRx,           "Rx",                 "Part_RX",        ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TrsfRy,           "Ry",                 "Part_RY",        ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TrsfRz,           "Rz",                 "Part_RZ",        ParameterFlag_IsVisible, true);
  //
  this->InitParameter(PID_GroupTess,        "Tessellation",       "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TessLinDefl,      "Linear deflection",  "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_TessAngDefl,      "Angular deflection", "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_KeepTessParams,   "Keep parameters",    "",               ParameterFlag_IsVisible, true);
  //
  this->InitParameter(PID_GroupPrs,         "Presentation",       "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_DisplayMode,      "Display mode",       "PrsDisplayMode", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_UseScalars,       "Use scalars",        "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_Color,            "Color",              "PrsCustomColor", ParameterFlag_IsVisible, true);
  this->InitParameter(PID_HasVertices,      "Show vertices",      "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_HasBackface,      "Show backface",      "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_ShowFaultyFaces,  "Show faulty faces",  "",               ParameterFlag_IsVisible, true);
  this->InitParameter(PID_IsCoincidentTopo, "Resolve coin. topo", "",               ParameterFlag_IsVisible, true);
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString asiData_PartNode::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param name [in] name to set.
void asiData_PartNode::SetName(const TCollection_ExtendedString& name)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(name);
}

//-----------------------------------------------------------------------------
// Handy accessors
//-----------------------------------------------------------------------------

//! Returns the stored CAD part.
//! \param[in] applyTransform indicates whether to apply the persistent
//!                           transformation.
//! \return stored shape.
TopoDS_Shape asiData_PartNode::GetShape(const bool applyTransform) const
{
  TopoDS_Shape shape = ActParamTool::AsShape( this->Parameter(PID_Geometry) )->GetShape();

  if ( applyTransform )
  {
    gp_Trsf T = this->GetTransformationMx();
    //
    if ( T.Form() != gp_Identity )
      shape.Move(T);
  }

  return shape;
}

//! Sets flag for automatic AAG construction.
//! \param[in] flag on/off.
void asiData_PartNode::SetAutoAAG(const bool flag) const
{
  ActParamTool::AsBool( this->Parameter(PID_AutoAAG) )->SetValue(flag);
}

//! \return true/false.
bool asiData_PartNode::IsAutoAAG() const
{
  return ActParamTool::AsBool( this->Parameter(PID_AutoAAG) )->GetValue();
}

//! \return stored AAG.
Handle(asiAlgo_AAG) asiData_PartNode::GetAAG() const
{
  return Handle(asiData_AAGParameter)::DownCast( this->Parameter(PID_AAG) )->GetAAG();
}

//! \return stored BVH.
Handle(asiAlgo_BVHFacets) asiData_PartNode::GetBVH() const
{
  return Handle(asiData_BVHParameter)::DownCast( this->Parameter(PID_BVH) )->GetBVH();
}

//! \return stored naming service.
Handle(asiAlgo_Naming) asiData_PartNode::GetNaming() const
{
  return Handle(asiData_NamingParameter)::DownCast( this->Parameter(PID_Naming) )->GetNaming();
}

//! \return true if naming service is initialized.
bool asiData_PartNode::HasNaming() const
{
  return !this->GetNaming().IsNull();
}

//! Sets the transformation components for the Part Node.
//! \param[in] tx    translation along the OX axis (in model units).
//! \param[in] ty    translation along the OY axis (in model units).
//! \param[in] tz    translation along the OZ axis (in model units).
//! \param[in] rxDeg rotation around OX axis (in degrees).
//! \param[in] ryDeg rotation around OY axis (in degrees).
//! \param[in] rzDeg rotation around OZ axis (in degrees).
void asiData_PartNode::SetTransformation(const double tx,
                                         const double ty,
                                         const double tz,
                                         const double rxDeg,
                                         const double ryDeg,
                                         const double rzDeg)
{
  ActParamTool::AsReal( this->Parameter(PID_TrsfTx) )->SetValue(tx);
  ActParamTool::AsReal( this->Parameter(PID_TrsfTy) )->SetValue(ty);
  ActParamTool::AsReal( this->Parameter(PID_TrsfTz) )->SetValue(tz);
  ActParamTool::AsReal( this->Parameter(PID_TrsfRx) )->SetValue(rxDeg);
  ActParamTool::AsReal( this->Parameter(PID_TrsfRy) )->SetValue(ryDeg);
  ActParamTool::AsReal( this->Parameter(PID_TrsfRz) )->SetValue(rzDeg);

  this->UpdateTransformationMx();
}

//! Gets the transformation components for the Part Node.
//! \param[out] tx    translation along the OX axis (in model units).
//! \param[out] ty    translation along the OY axis (in model units).
//! \param[out] tz    translation along the OZ axis (in model units).
//! \param[out] rxDeg rotation around OX axis (in degrees).
//! \param[out] ryDeg rotation around OY axis (in degrees).
//! \param[out] rzDeg rotation around OZ axis (in degrees).
void asiData_PartNode::GetTransformation(double& tx,
                                         double& ty,
                                         double& tz,
                                         double& rxDeg,
                                         double& ryDeg,
                                         double& rzDeg) const
{
  tx    = ActParamTool::AsReal( this->Parameter(PID_TrsfTx) )->GetValue();
  ty    = ActParamTool::AsReal( this->Parameter(PID_TrsfTy) )->GetValue();
  tz    = ActParamTool::AsReal( this->Parameter(PID_TrsfTz) )->GetValue();
  rxDeg = ActParamTool::AsReal( this->Parameter(PID_TrsfRx) )->GetValue();
  ryDeg = ActParamTool::AsReal( this->Parameter(PID_TrsfRy) )->GetValue();
  rzDeg = ActParamTool::AsReal( this->Parameter(PID_TrsfRz) )->GetValue();
}

//! Sets transformation matrix by its components.
//!
//! \verbatim
//!   V1   V2   V3     T       XYZ       XYZ
//! | a11  a12  a13   a14 |   | x |     | x'|
//! | a21  a22  a23   a24 |   | y |     | y'|
//! | a31  a32  a33   a34 |   | z |  =  | z'|
//! |  0    0    0     1  |   | 1 |     | 1 |
//! \endverbatim
void asiData_PartNode::SetTransformationMx(const double a11,
                                           const double a12,
                                           const double a13,
                                           const double a14,
                                           const double a21,
                                           const double a22,
                                           const double a23,
                                           const double a24,
                                           const double a31,
                                           const double a32,
                                           const double a33,
                                           const double a34)
{
  Handle(ActData_RealArrayParameter)
    param = ActParamTool::AsRealArray( this->Parameter(PID_TrsfMx) );

  Handle(HRealArray) arr = param->GetArray();
  //
  arr->ChangeValue(0)  = a11;
  arr->ChangeValue(1)  = a12;
  arr->ChangeValue(2)  = a13;
  arr->ChangeValue(3)  = a14;
  arr->ChangeValue(4)  = a21;
  arr->ChangeValue(5)  = a22;
  arr->ChangeValue(6)  = a23;
  arr->ChangeValue(7)  = a24;
  arr->ChangeValue(8)  = a31;
  arr->ChangeValue(9)  = a32;
  arr->ChangeValue(10) = a33;
  arr->ChangeValue(11) = a34;

  param->SetArray(arr);
}

//! Returns the stored transformation matrix by its components.
void asiData_PartNode::GetTransformationMx(double& a11,
                                           double& a12,
                                           double& a13,
                                           double& a14,
                                           double& a21,
                                           double& a22,
                                           double& a23,
                                           double& a24,
                                           double& a31,
                                           double& a32,
                                           double& a33,
                                           double& a34) const
{
  Handle(ActData_RealArrayParameter)
    param = ActParamTool::AsRealArray( this->Parameter(PID_TrsfMx) );

  Handle(HRealArray) arr = param->GetArray();
  //
  a11 = arr->Value(0);
  a12 = arr->Value(1);
  a13 = arr->Value(2);
  a14 = arr->Value(3);
  a21 = arr->Value(4);
  a22 = arr->Value(5);
  a23 = arr->Value(6);
  a24 = arr->Value(7);
  a31 = arr->Value(8);
  a32 = arr->Value(9);
  a33 = arr->Value(10);
  a34 = arr->Value(11);
}

//! \return transformation matrix.
gp_Trsf asiData_PartNode::GetTransformationMx() const
{
  // Get components of the transformation matrix.
  double a11, a12, a13, a14, a21, a22, a23, a24, a31, a32, a33, a34;
  this->GetTransformationMx(a11, a12, a13, a14,
                            a21, a22, a23, a24,
                            a31, a32, a33, a34);

  gp_Trsf T;
  T.SetValues(a11, a12, a13, a14,
              a21, a22, a23, a24,
              a31, a32, a33, a34);
  //
  return T;
}

//! Updates transformation matrix from `Tx`, `Ty`, `Tz`, `Rx`, `Ry`, `Rz`
//! parameters.
void asiData_PartNode::UpdateTransformationMx()
{
  double tx, ty, tz, rxDeg, ryDeg, rzDeg;
  this->GetTransformation(tx, ty, tz, rxDeg, ryDeg, rzDeg);

  const double rx = rxDeg*M_PI/180.;
  const double ry = ryDeg*M_PI/180.;
  const double rz = rzDeg*M_PI/180.;

  // Update transformation matrix.
  gp_Trsf T = asiAlgo_Utils::Transformation(tx, ty, tz, rx, ry, rz);
  //
  const double a11 = T.Value(1, 1);
  const double a12 = T.Value(1, 2);
  const double a13 = T.Value(1, 3);
  const double a14 = T.Value(1, 4);
  const double a21 = T.Value(2, 1);
  const double a22 = T.Value(2, 2);
  const double a23 = T.Value(2, 3);
  const double a24 = T.Value(2, 4);
  const double a31 = T.Value(3, 1);
  const double a32 = T.Value(3, 2);
  const double a33 = T.Value(3, 3);
  const double a34 = T.Value(3, 4);
  //
  this->SetTransformationMx(a11, a12, a13, a14,
                            a21, a22, a23, a24,
                            a31, a32, a33, a34);
}

//! Sets linear deflection.
//! \param defl [in] value to set.
void asiData_PartNode::SetLinearDeflection(const double defl)
{
  ActParamTool::AsReal( this->Parameter(PID_TessLinDefl) )->SetValue(defl);
}

//! \return linear deflection.
double asiData_PartNode::GetLinearDeflection() const
{
  return ActParamTool::AsReal( this->Parameter(PID_TessLinDefl) )->GetValue();
}

//! Sets angular deflection.
//! \param defl [in] value to set.
void asiData_PartNode::SetAngularDeflection(const double defl)
{
  ActParamTool::AsReal( this->Parameter(PID_TessAngDefl) )->SetValue(defl);
}

//! \return angular deflection.
double asiData_PartNode::GetAngularDeflection() const
{
  return ActParamTool::AsReal( this->Parameter(PID_TessAngDefl) )->GetValue();
}

//! Sets the flag which determines whether the tessellation parameters
//! (i.e., linear and angular deflections of a faceter) can be reset by
//! Analysis Situs internally (true) or whether these values should be
//! preserved as they are (false).
//! \param[in] flag Boolean value to set (on/off).
void asiData_PartNode::SetKeepTessParams(const bool flag) const
{
  ActParamTool::AsBool( this->Parameter(PID_KeepTessParams) )->SetValue(flag);
}

//! \return true/false.
bool asiData_PartNode::IsKeepTessParams() const
{
  return ActParamTool::AsBool( this->Parameter(PID_KeepTessParams) )->GetValue();
}

//! Sets the Boolean value indicating whether the scalars are to be used for
//! coloring the Part's Presentation.
//! \param on [in] value to set.
void asiData_PartNode::SetUseScalars(const bool on)
{
  ActParamTool::AsBool( this->Parameter(PID_UseScalars) )->SetValue(on);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! scalars are active.
//! \return true/false.
bool asiData_PartNode::GetUseScalars() const
{
  return ActParamTool::AsBool( this->Parameter(PID_UseScalars) )->GetValue();
}

//! Sets color.
//! \param color [in] color to set.
void asiData_PartNode::SetColor(const int color)
{
  ActParamTool::AsInt( this->Parameter(PID_Color) )->SetValue(color);
}

//! Accessor for the stored color value.
//! \return color value.
int asiData_PartNode::GetColor() const
{
  return ActParamTool::AsInt( this->Parameter(PID_Color) )->GetValue();
}

//! Sets display mode.
//! \param mode [in] display mode value to set.
void asiData_PartNode::SetDisplayMode(const int mode)
{
  ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->SetValue(mode);
}

//! Accessor for the stored display mode value.
//! \return display mode value.
int asiData_PartNode::GetDisplayMode() const
{
  return ActParamTool::AsInt( this->Parameter(PID_DisplayMode) )->GetValue();
}

//! Sets the Boolean value indicating whether the vertices are active.
//! \param hasVertices [in] value to set.
void asiData_PartNode::SetHasVertices(const bool hasVertices)
{
  ActParamTool::AsBool( this->Parameter(PID_HasVertices) )->SetValue(hasVertices);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! vertices are active.
//! \return true/false.
bool asiData_PartNode::HasVertices() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasVertices) )->GetValue();
}

//! Sets the Boolean value indicating whether the backface is active.
//! \param on [in] value to set.
void asiData_PartNode::SetHasBackface(const bool on)
{
  ActParamTool::AsBool( this->Parameter(PID_HasBackface) )->SetValue(on);
}

//! Accessor for the value of the Boolean Parameter indicating whether the
//! backface is active.
//! \return true/false.
bool asiData_PartNode::HasBackface() const
{
  return ActParamTool::AsBool( this->Parameter(PID_HasBackface) )->GetValue();
}

//! Enables/disables the visualization of faulty faces.
//! \param[in] on value to set.
void asiData_PartNode::SetShowFaultyFaces(const bool on)
{
  ActParamTool::AsBool( this->Parameter(PID_ShowFaultyFaces) )->SetValue(on);
}

//! \return true/false.
bool asiData_PartNode::IsShowFaultyFaces() const
{
  return ActParamTool::AsBool( this->Parameter(PID_ShowFaultyFaces) )->GetValue();
}

//! Enables/disables resolving the coincident topology in VTK mapper.
//! \param[in] on value to set.
void asiData_PartNode::SetResolveCoincidentTopo(const bool on)
{
  ActParamTool::AsBool( this->Parameter(PID_IsCoincidentTopo) )->SetValue(on);
}

//! \return true/false.
bool asiData_PartNode::IsResolveCoincidentTopo() const
{
  return ActParamTool::AsBool( this->Parameter(PID_IsCoincidentTopo) )->GetValue();
}

//-----------------------------------------------------------------------------

//! \return underlying face representation Node.
Handle(asiData_FaceNode) asiData_PartNode::GetFaceRepresentation() const
{
  Handle(asiData_FaceNode) face_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    face_n = Handle(asiData_FaceNode)::DownCast( cit->Value() );

    if ( !face_n.IsNull() && face_n->IsWellFormed() )
      return face_n;
  }

  return nullptr;
}

//! \return underlying normal vectors representation Node.
Handle(asiData_FaceNormsNode) asiData_PartNode::GetNormsRepresentation() const
{
  Handle(asiData_FaceNormsNode) norms_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    norms_n = Handle(asiData_FaceNormsNode)::DownCast( cit->Value() );

    if ( !norms_n.IsNull() && norms_n->IsWellFormed() )
      return norms_n;
  }

  return nullptr;
}

//! \return underlying surface representation Node.
Handle(asiData_SurfNode) asiData_PartNode::GetSurfaceRepresentation() const
{
  Handle(asiData_SurfNode) surf_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    surf_n = Handle(asiData_SurfNode)::DownCast( cit->Value() );

    if ( !surf_n.IsNull() && surf_n->IsWellFormed() )
      return surf_n;
  }

  return nullptr;
}

//! \return underlying Face Contour Node.
Handle(asiData_FaceContourNode) asiData_PartNode::GetContourRepresentation() const
{
  Handle(asiData_FaceContourNode) contour_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    contour_n = Handle(asiData_FaceContourNode)::DownCast( cit->Value() );

    if ( !contour_n.IsNull() && contour_n->IsWellFormed() )
      return contour_n;
  }

  return nullptr;
}

//! \return underlying boundary edges representation Node.
Handle(asiData_BoundaryEdgesNode) asiData_PartNode::GetBoundaryEdgesRepresentation() const
{
  Handle(asiData_BoundaryEdgesNode) edges_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    edges_n = Handle(asiData_BoundaryEdgesNode)::DownCast( cit->Value() );

    if ( !edges_n.IsNull() && edges_n->IsWellFormed() )
      return edges_n;
  }

  return nullptr;
}

//! \return underlying edge representation Node.
Handle(asiData_EdgeNode) asiData_PartNode::GetEdgeRepresentation() const
{
  Handle(asiData_EdgeNode) edge_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    edge_n = Handle(asiData_EdgeNode)::DownCast( cit->Value() );

    if ( !edge_n.IsNull() && edge_n->IsWellFormed() )
      return edge_n;
  }

  return nullptr;
}

//! \return underlying curve representation Node.
Handle(asiData_CurveNode) asiData_PartNode::GetCurveRepresentation() const
{
  Handle(asiData_CurveNode) curve_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    curve_n = Handle(asiData_CurveNode)::DownCast( cit->Value() );

    if ( !curve_n.IsNull() && curve_n->IsWellFormed() )
      return curve_n;
  }

  return nullptr;
}

//! \return underlying contour Node.
Handle(asiData_ContourNode) asiData_PartNode::GetContour() const
{
  Handle(asiData_ContourNode) contour_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    contour_n = Handle(asiData_ContourNode)::DownCast( cit->Value() );

    if ( !contour_n.IsNull() && contour_n->IsWellFormed() )
      return contour_n;
  }

  return nullptr;
}

//! \return underlying Vertex Node.
Handle(asiData_VertexNode) asiData_PartNode::GetVertexRepresentation() const
{
  Handle(asiData_VertexNode) vertex_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    vertex_n = Handle(asiData_VertexNode)::DownCast( cit->Value() );

    if ( !vertex_n.IsNull() && vertex_n->IsWellFormed() )
      return vertex_n;
  }

  return nullptr;
}

//! \return underlying Node representing the group of tolerant shapes.
Handle(asiData_TolerantShapesNode) asiData_PartNode::GetTolerantShapes() const
{
  Handle(asiData_TolerantShapesNode) tolShapes_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    tolShapes_n = Handle(asiData_TolerantShapesNode)::DownCast( cit->Value() );

    if ( !tolShapes_n.IsNull() && tolShapes_n->IsWellFormed() )
      return tolShapes_n;
  }

  return nullptr;
}

//! \return underlying Node which stores metadata.
Handle(asiData_MetadataNode) asiData_PartNode::GetMetadata() const
{
  Handle(asiData_MetadataNode) meta_n;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    meta_n = Handle(asiData_MetadataNode)::DownCast( cit->Value() );

    if ( !meta_n.IsNull() && meta_n->IsWellFormed() )
      return meta_n;
  }

  return nullptr;
}

//-----------------------------------------------------------------------------

//! Sets shape to store.
//! \param shape [in] shape to store.
void asiData_PartNode::setShape(const TopoDS_Shape& shape)
{
  ActParamTool::AsShape( this->Parameter(PID_Geometry) )->SetShape(shape);
}

//! Sets AAG to store.
//! \param aag [in] AAG to store.
void asiData_PartNode::setAAG(const Handle(asiAlgo_AAG)& aag)
{
  Handle(asiData_AAGParameter)::DownCast( this->Parameter(PID_AAG) )->SetAAG(aag);
}

//! Sets BVH to store.
//! \param bvh [in] BVH to store.
void asiData_PartNode::setBVH(const Handle(asiAlgo_BVHFacets)& bvh)
{
  Handle(asiData_BVHParameter)::DownCast( this->Parameter(PID_BVH) )->SetBVH(bvh);
}

//! Sets naming service to store.
//! \param naming [in] naming service to store.
void asiData_PartNode::setNaming(const Handle(asiAlgo_Naming)& naming)
{
  Handle(asiData_NamingParameter)::DownCast( this->Parameter(PID_Naming) )->SetNaming(naming);
}
