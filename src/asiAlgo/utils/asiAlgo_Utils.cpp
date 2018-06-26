//-----------------------------------------------------------------------------
// Created on: 20 November 2015
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
#include <asiAlgo_Utils.h>

// OS dependent
#include <windows.h>

// asiAlgo includes
#include <asiAlgo_ClassifyPointFace.h>
#include <asiAlgo_Timer.h>

// OCCT includes
#include <BinTools.hxx>
#include <Bnd_Box.hxx>
#include <BOPAlgo_BOP.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgo_Common.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Defeaturing.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Status.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepLProp_SLProps.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepTools.hxx>
#include <GC_MakeCircle.hxx>
#include <GCPnts_QuasiUniformAbscissa.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert.hxx>
#include <GeomLProp_CLProps.hxx>
#include <gp_Circ.hxx>
#include <gp_Quaternion.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <NCollection_IncAllocator.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

// Eigen includes
#pragma warning(disable : 4701 4702)
#include <Eigen/Dense>
#pragma warning(default : 4701 4702)

//-----------------------------------------------------------------------------

#undef DUMP_COUT
#undef DUMP_FILES

#define dump_filename_N  "D:\\N_interp_log_OCCT.log"
#define dump_filename_Bx "D:\\Bx_interp_log_OCCT.log"
#define dump_filename_By "D:\\By_interp_log_OCCT.log"
#define dump_filename_Bz "D:\\Bz_interp_log_OCCT.log"

#define BUFSIZE           1000

#undef COUT_DEBUG
#if defined COUT_DEBUG
  #pragma message("===== warning: COUT_DEBUG is enabled")
#endif

//-----------------------------------------------------------------------------

bool IsASCII(const TCollection_AsciiString& filename)
{
  FILE* FILE;
  fopen_s(&FILE, filename.ToCString(), "rb");
  //
  if ( FILE == NULL )
    return false;

  bool isAscii = true;
  char buffer[128] = {};
  //
  if ( fread(buffer, 1, 84, FILE) != 84 )
  {
    fclose(FILE);
    return false;
  }

  size_t readLen = fread(buffer, 1, 128, FILE);
  for ( size_t byteIter = 0; byteIter < readLen; ++byteIter )
  {
    if ( (unsigned char) buffer[byteIter] > (unsigned char )'~' )
    {
      isAscii = false;
      break;
    }
  }

  fclose(FILE);
  return isAscii;
}

//-----------------------------------------------------------------------------

void asiAlgo_Utils::Str::Split(const std::string&        source_str,
                               const std::string&        delim_str,
                               std::vector<std::string>& result)
{
  // Initialize collection of strings to split
  std::vector<std::string> chunks;
  chunks.push_back(source_str);

  // Split by each delimiter consequently
  for ( size_t delim_idx = 0; delim_idx < delim_str.length(); ++delim_idx )
  {
    std::vector<std::string> new_chunks;
    const char delim = delim_str[delim_idx];

    // Split each chunk
    for ( size_t chunk_idx = 0; chunk_idx < chunks.size(); ++chunk_idx )
    {
      const std::string& source = chunks[chunk_idx];
      std::string::size_type currPos = 0, prevPos = 0;
      while ( (currPos = source.find(delim, prevPos) ) != std::string::npos )
      {
        std::string item = source.substr(prevPos, currPos - prevPos);
        if ( item.size() > 0 )
        {
          new_chunks.push_back(item);
        }
        prevPos = currPos + 1;
      }
      new_chunks.push_back( source.substr(prevPos) );
    }

    // Set new collection of chunks for splitting by the next delimiter
    chunks = new_chunks;
  }

  // Set result
  result = chunks;
}

//-----------------------------------------------------------------------------

void asiAlgo_Utils::Str::ReplaceAll(std::string&       str,
                                    const std::string& what,
                                    const std::string& with)
{
  for ( size_t pos = 0; ; pos += with.length() )
  {
    pos = str.find(what, pos); // Locate the substring to replace
    if ( pos == std::string::npos )
      break; // Not found

    // Replace by erasing and inserting
    str.erase( pos, what.length() );
    str.insert(pos, with);
  }
}

//-----------------------------------------------------------------------------

std::string asiAlgo_Utils::Str::SubStr(const std::string& source,
                                       const int          idx_F,
                                       const int          length)
{
  return source.substr(idx_F, length);
}

//-----------------------------------------------------------------------------

std::string asiAlgo_Utils::Str::Slashed(const std::string& strIN)
{
  if ( !strIN.length() )
    return strIN;

  char c = strIN.at(strIN.length() - 1);
  if ( c == *asiAlgo_SlashStr )
    return strIN;

  std::string strOUT(strIN);
  strOUT.append(asiAlgo_SlashStr);
  return strOUT;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::Str::IsNumber(const std::string& str)
{
  char* cnv;
  strtod(str.c_str(), &cnv);
  return cnv != str.data();
}

//-----------------------------------------------------------------------------

//! Returns value of ASI_TEST_DATA environment variable. This variable is used to
//! refer to the directory containing all data files playing as inputs for
//! unit tests.
//! \return value of ASI_TEST_DATA variable.
std::string asiAlgo_Utils::Env::AsiTestData()
{
  return GetVariable(ASI_TEST_DATA);
}

//-----------------------------------------------------------------------------

//! Returns value of ASI_TEST_DUMPING environment variable. This variable is
//! used to refer to the directory containing all temporary files utilized by
//! unit tests.
//! \return value of ASI_TEST_DUMPING variable.
std::string asiAlgo_Utils::Env::AsiTestDumping()
{
  return GetVariable(ASI_TEST_DUMPING);
}

//-----------------------------------------------------------------------------

//! Returns value of ASI_TEST_DESCR environment variable. This variable is used
//! to refer to the directory containing all temporary files utilized by unit
//! tests.
//! \return value of ASI_TEST_DESCR variable.
std::string asiAlgo_Utils::Env::AsiTestDescr()
{
  return GetVariable(ASI_TEST_DESCR);
}

//-----------------------------------------------------------------------------

//! Returns value of the environment variable with the passed name.
//! \param varName [in] variable name.
//! \return value of the variable.
std::string asiAlgo_Utils::Env::GetVariable(const char* varName)
{
  TCHAR chNewEnv[BUFSIZE];
  GetEnvironmentVariable(varName, chNewEnv, BUFSIZE);
  return chNewEnv;
}

//-----------------------------------------------------------------------------

std::string asiAlgo_Utils::FaceGeometryName(const TopoDS_Face& face)
{
  Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
  return SurfaceName(surf);
}

//-----------------------------------------------------------------------------

TCollection_AsciiString
  asiAlgo_Utils::NamedShapeToString(const TopoDS_Shape&           subShape,
                                    const int                     globalId,
                                    const Handle(asiAlgo_Naming)& naming)
{
  TCollection_AsciiString msg("Sub-shape: ");
  msg += ShapeTypeStr(subShape).c_str();
  msg += "\n\t Global ID: "; msg += globalId;
  msg += "\n\t Address: ";   msg += ShapeAddr(subShape).c_str();
  //
  if ( !naming.IsNull() )
  {
    msg += "\n\t Name: ";

    TCollection_AsciiString label;
    naming->FindName(subShape, label);
    //
    if ( !label.IsEmpty() )
      msg += label;
    else
      msg += "<empty>";
  }
  //
  if ( subShape.ShapeType() == TopAbs_EDGE )
  {
    const TopoDS_Edge& edge = TopoDS::Edge(subShape);

    // Get host curve.
    double f, l;
    Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, f, l);
    //
    if ( curve.IsNull() )
      msg += "\n\t Curve: null";
    else
    {
      msg += "\n\t Curve: ";
      msg += curve->DynamicType()->Name();
      msg += "\n\t Min parameter: ";
      msg += curve->FirstParameter();
      msg += "\n\t Max parameter: ";
      msg += curve->LastParameter();

      if ( curve->IsInstance( STANDARD_TYPE(Geom_BSplineCurve) ) )
      {
        Handle(Geom_BSplineCurve)
          bcurve = Handle(Geom_BSplineCurve)::DownCast(curve);
        //
        msg += "\n\t Degree: ";
        msg += bcurve->Degree();
        msg += "\n\t Num poles: ";
        msg += bcurve->NbPoles();
      }
    }
  }
  //
  else if ( subShape.ShapeType() == TopAbs_FACE )
  {
    const TopoDS_Face& face = TopoDS::Face(subShape);

    // Get host surface.
    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);

    // Get parametric bounds.
    double uMin, uMax, vMin, vMax;
    surf->Bounds(uMin, uMax, vMin, vMax);

    // Dump.
    msg += "\n\t Surface: ";
    msg += surf->DynamicType()->Name();
    msg += "\n\t Min U parameter: ";
    msg += uMin;
    msg += "\n\t Max U parameter: ";
    msg += uMax;
    msg += "\n\t Min V parameter: ";
    msg += vMin;
    msg += "\n\t Max V parameter: ";
    msg += vMax;

    if ( surf->IsInstance( STANDARD_TYPE(Geom_BSplineSurface) ) )
    {
      Handle(Geom_BSplineSurface)
        bsurf = Handle(Geom_BSplineSurface)::DownCast(surf);
      //
      msg += "\n\t Degree U: ";
      msg += bsurf->UDegree();
      msg += "\n\t Degree V: ";
      msg += bsurf->VDegree();
      msg += "\n\t Num poles U: ";
      msg += bsurf->NbUPoles();
      msg += "\n\t Num poles V: ";
      msg += bsurf->NbVPoles();
    }
  }

  return msg;
}

//-----------------------------------------------------------------------------

TCollection_AsciiString
  asiAlgo_Utils::OrientationToString(const TopoDS_Shape& shape)
{
  return OrientationToString( shape.Orientation() );
}

//-----------------------------------------------------------------------------

TCollection_AsciiString
  asiAlgo_Utils::OrientationToString(const TopAbs_Orientation ori)
{
  TCollection_AsciiString oriStr;

  // Check orientation.
  if ( ori == TopAbs_FORWARD )
    oriStr = "forward";
  else if ( ori == TopAbs_REVERSED )
    oriStr = "reversed";
  else if ( ori == TopAbs_INTERNAL )
    oriStr = "internal";
  else if ( ori == TopAbs_EXTERNAL )
    oriStr = "external";

  return oriStr;
}

//-----------------------------------------------------------------------------

TCollection_AsciiString
  asiAlgo_Utils::ContinuityToString(const GeomAbs_Shape cont)
{
  TCollection_AsciiString contStr;

  // Check continuity.
  switch ( cont )
  {
    case GeomAbs_C0 : contStr = "C0"; break;
    case GeomAbs_C1 : contStr = "C1"; break;
    case GeomAbs_C2 : contStr = "C2"; break;
    case GeomAbs_C3 : contStr = "C3"; break;
    case GeomAbs_CN : contStr = "CN"; break;
    case GeomAbs_G1 : contStr = "G1"; break;
    case GeomAbs_G2 : contStr = "G2"; break;
    default: break;
  }

  return contStr;
}

//-----------------------------------------------------------------------------

TCollection_AsciiString
  asiAlgo_Utils::LocationToString(const TopLoc_Location& loc)
{
  const gp_Trsf& T      = loc.Transformation();
  gp_Mat         T_roto = T.VectorialPart();
  const gp_XYZ&  T_move = T.TranslationPart();
  gp_TrsfForm    T_form = T.Form();

  TCollection_AsciiString result;
  result += "\n---\nTransformation: ";
  if ( T_form == gp_Identity )
    result += "identity";
  else if ( T_form == gp_Rotation )
    result += "rotation";
  else if ( T_form == gp_Translation )
    result += "translation";
  else if ( T_form == gp_PntMirror )
    result += "point mirror (central symmetry)";
  else if ( T_form == gp_Ax1Mirror )
    result += "axis mirror (rotational symmetry)";
  else if ( T_form == gp_Ax2Mirror )
    result += "plane mirror (bilateral symmetry)";
  else if ( T_form == gp_Scale )
    result += "scaling";
  else if ( T_form == gp_CompoundTrsf )
    result += "combination of orthogonal transformations";
  else
    result += "non-orthogonal transformation";
  //
  result += "\n---\n";
  result += T_roto(1, 1); result += " "; result += T_roto(1, 2); result += " "; result += T_roto(1, 3); result += "\n";
  result += T_roto(2, 1); result += " "; result += T_roto(2, 2); result += " "; result += T_roto(2, 3); result += "\n";
  result += T_roto(3, 1); result += " "; result += T_roto(3, 2); result += " "; result += T_roto(3, 3);
  result += "\n---\n";
  result += T_move.X(); result += " "; result += T_move.Y(); result += " "; result += T_move.Z();

  return result;
}

//-----------------------------------------------------------------------------

std::string asiAlgo_Utils::CurveName(const Handle(Geom_Curve)& curve)
{
  if ( curve->IsInstance( STANDARD_TYPE(Geom_Line) ) )
    return "line";
  //
  if ( curve->IsInstance( STANDARD_TYPE(Geom_Circle) ) )
    return "circle";
  //
  if ( curve->IsInstance( STANDARD_TYPE(Geom_BezierCurve) ) )
    return "bezier";
  //
  if ( curve->IsInstance( STANDARD_TYPE(Geom_BSplineCurve) ) )
    return "b-curve";
  //
  if ( curve->IsInstance( STANDARD_TYPE(Geom_Ellipse) ) )
    return "ellipse";
  //
  if ( curve->IsInstance( STANDARD_TYPE(Geom_Parabola) ) )
    return "parabola";
  //
  if ( curve->IsInstance( STANDARD_TYPE(Geom_Hyperbola) ) )
    return "hyperbola";
  //
  if ( curve->IsInstance( STANDARD_TYPE(Geom_OffsetCurve) ) )
  {
    Handle(Geom_OffsetCurve) oc = Handle(Geom_OffsetCurve)::DownCast(curve);
    return std::string("offset on ") + CurveName( oc->BasisCurve() );
  }
  //
  if ( curve->IsInstance( STANDARD_TYPE(Geom_TrimmedCurve) ) )
  {
    Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(curve);
    return std::string("trimmed on ") + CurveName( tc->BasisCurve() );
  }

  return "unknown";
}

//-----------------------------------------------------------------------------

std::string asiAlgo_Utils::SurfaceName(const Handle(Geom_Surface)& surf)
{
  if ( surf->IsInstance( STANDARD_TYPE(Geom_Plane) ) )
    return "plane";
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_CylindricalSurface) ) )
    return "cylinder";
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_BezierSurface) ) )
    return "bezier";
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_BSplineSurface) ) )
    return "b-surface";
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_ConicalSurface) ) )
    return "cone";
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_OffsetSurface) ) )
  {
    Handle(Geom_OffsetSurface) os = Handle(Geom_OffsetSurface)::DownCast(surf);
    return std::string("offset on ") + SurfaceName( os->BasisSurface() );
  }
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_RectangularTrimmedSurface) ) )
  {
    Handle(Geom_RectangularTrimmedSurface) ts = Handle(Geom_RectangularTrimmedSurface)::DownCast(surf);
    return std::string("rect trimmed on ") + SurfaceName( ts->BasisSurface() );
  }
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_SphericalSurface) ) )
    return "sphere";
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion) ) )
    return "ruled";
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_SurfaceOfRevolution) ) )
    return "revolution";
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_SweptSurface) ) )
    return "sweep";
  //
  if ( surf->IsInstance( STANDARD_TYPE(Geom_ToroidalSurface) ) )
    return "torus";

  return "unknown";
}

//-----------------------------------------------------------------------------

std::string asiAlgo_Utils::ShapeTypeStr(const TopAbs_ShapeEnum& shapeType)
{
  std::string name;
  if ( shapeType == TopAbs_COMPOUND )
    name = "COMPOUND";
  else if ( shapeType == TopAbs_COMPSOLID )
    name = "COMPSOLID";
  else if ( shapeType == TopAbs_SOLID )
    name = "SOLID";
  else if ( shapeType == TopAbs_SHELL )
    name = "SHELL";
  else if ( shapeType == TopAbs_FACE )
    name = "FACE";
  else if ( shapeType == TopAbs_WIRE )
    name = "WIRE";
  else if ( shapeType == TopAbs_EDGE )
    name = "EDGE";
  else if ( shapeType == TopAbs_VERTEX )
    name = "VERTEX";
  else
    name = "SHAPE";

  return name;
}

//-----------------------------------------------------------------------------

std::string asiAlgo_Utils::ShapeTypeStr(const TopoDS_Shape& shape)
{
  return ShapeTypeStr( shape.ShapeType() );
}

//-----------------------------------------------------------------------------

std::string asiAlgo_Utils::ShapeAddrWithPrefix(const TopoDS_Shape& shape)
{
  std::string addr_str = ShapeAddr    ( shape.TShape() );
  std::string prefix   = ShapeTypeStr ( shape );

  // Notice extra spacing for better visualization
  return prefix + " [" + addr_str + "]";
}

//-----------------------------------------------------------------------------

std::string asiAlgo_Utils::ShapeAddr(const Handle(TopoDS_TShape)& tshape)
{
  std::string addr_str;
  std::ostringstream ost;
  ost << tshape.get();
  addr_str = ost.str();

  size_t pos = 0;
  while ( addr_str[pos] == '0' )
    pos++;

  if ( pos )
    addr_str = Str::SubStr( addr_str, (int) pos, (int) (addr_str.size() - pos) );

  return addr_str;
}

//-----------------------------------------------------------------------------

std::string asiAlgo_Utils::ShapeAddr(const TopoDS_Shape& shape)
{
  return ShapeAddr( shape.TShape() );
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::IsEmptyShape(const TopoDS_Shape& shape)
{
  if ( shape.IsNull() )
    return true;

  if ( shape.ShapeType() >= TopAbs_FACE )
    return false;

  int numSubShapes = 0;
  for ( TopoDS_Iterator it(shape); it.More(); it.Next() )
    numSubShapes++;

  return numSubShapes == 0;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::IsBasisCircular(const Handle(Geom_Curve)& curve)
{
  if ( curve->IsKind( STANDARD_TYPE(Geom_Circle) ) )
    return true;

  if ( curve->IsInstance( STANDARD_TYPE(Geom_TrimmedCurve) ) )
  {
    Handle(Geom_TrimmedCurve) tcurve = Handle(Geom_TrimmedCurve)::DownCast(curve);
    //
    if ( tcurve->BasisCurve()->IsKind( STANDARD_TYPE(Geom_Circle) ) )
      return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
// Auxiliary functions
//-----------------------------------------------------------------------------

//! Auxiliary toolkit for shape validity checker.
namespace CheckShapeAux
{

  //! Checks whether the given list of shapes contains the passed shape.
  //! \param L [in] list to check.
  //! \param S [in] entry to find.
  //! \return true if shape was found, false -- otherwise.
  bool Contains(const TopTools_ListOfShape& L,
                const TopoDS_Shape&         S)
  {
    TopTools_ListIteratorOfListOfShape it;
    for ( it.Initialize(L); it.More(); it.Next() )
    {
      if ( it.Value().IsSame(S) )
        return true;
    }
    return false;
  }

  //! Extracted from OCCT Draw Test Harness for "checkshape" command. This
  //! function is used for preparation of output for invalid shapes.
  //! \param stat       [in]  OCCT topological check status.
  //! \param NbProblems [out] output array.
  void FillProblems(const BRepCheck_Status            stat,
                    Handle(TColStd_HArray1OfInteger)& NbProblems)
  {
    // Increment the number of anomalies of the given type
    switch ( stat )
    {
      case BRepCheck_InvalidPointOnCurve:
        NbProblems->ChangeValue(BRepCheck_InvalidPointOnCurve)++; break;
        //
      case BRepCheck_InvalidPointOnCurveOnSurface:
        NbProblems->ChangeValue(BRepCheck_InvalidPointOnCurveOnSurface)++; break;
        //
      case BRepCheck_InvalidPointOnSurface:
        NbProblems->ChangeValue(BRepCheck_InvalidPointOnSurface)++; break;
        //
      case BRepCheck_No3DCurve:
        NbProblems->ChangeValue(BRepCheck_No3DCurve)++; break;
        //
      case BRepCheck_Multiple3DCurve:
        NbProblems->ChangeValue(BRepCheck_Multiple3DCurve)++; break;
        //
      case BRepCheck_Invalid3DCurve:
        NbProblems->ChangeValue(BRepCheck_Invalid3DCurve)++; break;
        //
      case BRepCheck_NoCurveOnSurface:
        NbProblems->ChangeValue(BRepCheck_NoCurveOnSurface)++; break;
        //
      case BRepCheck_InvalidCurveOnSurface:
        NbProblems->ChangeValue(BRepCheck_InvalidCurveOnSurface)++; break;
        //
      case BRepCheck_InvalidCurveOnClosedSurface:
        NbProblems->ChangeValue(BRepCheck_InvalidCurveOnClosedSurface)++; break;
        //
      case BRepCheck_InvalidSameRangeFlag:
        NbProblems->ChangeValue(BRepCheck_InvalidSameRangeFlag)++; break;
        //
      case BRepCheck_InvalidSameParameterFlag:
        NbProblems->ChangeValue(BRepCheck_InvalidSameParameterFlag)++; break;
        //
      case BRepCheck_InvalidDegeneratedFlag:
        NbProblems->ChangeValue(BRepCheck_InvalidDegeneratedFlag)++; break;
        //
      case BRepCheck_FreeEdge:
        NbProblems->ChangeValue(BRepCheck_FreeEdge)++; break;
        //
      case BRepCheck_InvalidMultiConnexity:
        NbProblems->ChangeValue(BRepCheck_InvalidMultiConnexity)++; break;
        //
      case BRepCheck_InvalidRange:
        NbProblems->ChangeValue(BRepCheck_InvalidRange)++; break;
        //
      case BRepCheck_EmptyWire:
        NbProblems->ChangeValue(BRepCheck_EmptyWire)++; break;
        //
      case BRepCheck_RedundantEdge:
        NbProblems->ChangeValue(BRepCheck_RedundantEdge)++; break;
        //
      case BRepCheck_SelfIntersectingWire:
        NbProblems->ChangeValue(BRepCheck_SelfIntersectingWire)++; break;
        //
      case BRepCheck_NoSurface:
        NbProblems->ChangeValue(BRepCheck_NoSurface)++; break;
        //
      case BRepCheck_InvalidWire:
        NbProblems->ChangeValue(BRepCheck_InvalidWire)++; break;
        //
      case BRepCheck_RedundantWire:
        NbProblems->ChangeValue(BRepCheck_RedundantWire)++; break;
        //
      case BRepCheck_IntersectingWires:
        NbProblems->ChangeValue(BRepCheck_IntersectingWires)++; break;
        //
      case BRepCheck_InvalidImbricationOfWires:
        NbProblems->ChangeValue(BRepCheck_InvalidImbricationOfWires)++; break;
        //
      case BRepCheck_EmptyShell:
        NbProblems->ChangeValue(BRepCheck_EmptyShell)++; break;
        //
      case BRepCheck_RedundantFace:
        NbProblems->ChangeValue(BRepCheck_RedundantFace)++; break;
        //
      case BRepCheck_InvalidImbricationOfShells:
        NbProblems->ChangeValue(BRepCheck_InvalidImbricationOfShells)++; break;
        //
      case BRepCheck_UnorientableShape:
        NbProblems->ChangeValue(BRepCheck_UnorientableShape)++; break;
        //
      case BRepCheck_NotClosed:
        NbProblems->ChangeValue(BRepCheck_NotClosed)++; break;
        //
      case BRepCheck_NotConnected:
        NbProblems->ChangeValue(BRepCheck_NotConnected)++; break;
        //
      case BRepCheck_SubshapeNotInShape:
        NbProblems->ChangeValue(BRepCheck_SubshapeNotInShape)++; break;
        //
      case BRepCheck_BadOrientation:
        NbProblems->ChangeValue(BRepCheck_BadOrientation)++; break;
        //
      case BRepCheck_BadOrientationOfSubshape:
        NbProblems->ChangeValue(BRepCheck_BadOrientationOfSubshape)++; break;
        //
      case BRepCheck_InvalidPolygonOnTriangulation:
        NbProblems->ChangeValue(BRepCheck_InvalidPolygonOnTriangulation)++; break;
        //
      case BRepCheck_InvalidToleranceValue:
        NbProblems->ChangeValue(BRepCheck_InvalidToleranceValue)++; break;
        //
      case BRepCheck_EnclosedRegion:
        NbProblems->ChangeValue(BRepCheck_EnclosedRegion)++; break;
        //
      case BRepCheck_CheckFail:
        NbProblems->ChangeValue(BRepCheck_CheckFail)++; break;
        //
      default: break;
    }
  }

  //! \todo provide description here.
  void GetProblemSub(const BRepCheck_Analyzer&           Ana,
                     const TopoDS_Shape&                 Shape,
                     Handle(TopTools_HSequenceOfShape)&  sl,
                     Handle(TColStd_HArray1OfInteger)&   NbProblems,
                     const TopAbs_ShapeEnum              Subtype,
                     TopTools_DataMapOfShapeListOfShape& Map)
  {
    BRepCheck_ListIteratorOfListOfStatus itl;
    TopExp_Explorer exp;
    for ( exp.Init(Shape, Subtype); exp.More(); exp.Next() )
    {
      // Get current shape
      const TopoDS_Shape& sub = exp.Current();

      // Get result for the current shape
      const Handle(BRepCheck_Result)& res = Ana.Result(sub);

      // Iterate over the result
      for ( res->InitContextIterator(); res->MoreShapeInContext(); res->NextShapeInContext() )
      {
        if ( res->ContextualShape().IsSame(Shape) && !Contains(Map(sub), Shape) )
        {
          Map(sub).Append(Shape);
          itl.Initialize( res->StatusOnShape() );

          if ( itl.Value() != BRepCheck_NoError )
          {
            int ii = 0;
            for ( ii = 1; ii <= sl->Length(); ii++ )
              if ( sl->Value(ii).IsSame(sub) )
                break;

            if ( ii > sl->Length() )
            {
              sl->Append(sub);
              FillProblems(itl.Value(), NbProblems);
            }

            for ( ii = 1; ii <= sl->Length(); ii++ )
              if ( sl->Value(ii).IsSame(Shape) )
                break;

            if ( ii > sl->Length() )
            {
              sl->Append(Shape);
              FillProblems(itl.Value(), NbProblems);
            }
          }
          break;
        }
      }
    }
  }

  //! \todo provide description here.
  static void GetProblemShapes(const BRepCheck_Analyzer&           Ana,
                               const TopoDS_Shape&                 Shape,
                               Handle(TopTools_HSequenceOfShape)&  sl,
                               Handle(TColStd_HArray1OfInteger)&   NbProblems,
                               TopTools_DataMapOfShapeListOfShape& theMap)
  {
    for ( TopoDS_Iterator iter(Shape); iter.More(); iter.Next() )
    {
      GetProblemShapes(Ana, iter.Value(), sl,  NbProblems, theMap);
    }

    TopAbs_ShapeEnum styp = Shape.ShapeType();
    BRepCheck_ListIteratorOfListOfStatus itl;

    if ( !Ana.Result(Shape).IsNull() && !theMap.IsBound(Shape) )
    {
      itl.Initialize( Ana.Result(Shape)->Status() );

      if ( itl.Value() != BRepCheck_NoError )
      {
        sl->Append(Shape);
        FillProblems(itl.Value(), NbProblems);
      }
    }

    if ( !theMap.IsBound(Shape) )
    {
      TopTools_ListOfShape thelist;
      theMap.Bind(Shape, thelist);
    }

    switch ( styp )
    {
      case TopAbs_EDGE:
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_VERTEX, theMap);
        break;
      case TopAbs_FACE:
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_WIRE,   theMap);
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_EDGE,   theMap);
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_VERTEX, theMap);
        break;
      case TopAbs_SHELL:
        break;
      case TopAbs_SOLID:
        GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_SHELL, theMap);
        break;
      default:
        break;
    }
  }

  //! Dumps all detected invalidities to Logger.
  //! \param Journal  [in] Progress Entry.
  //! \param theAna   [in] checker tool.
  //! \param theShape [in] shape to check.
  void StructuralDump(ActAPI_ProgressEntry      Journal,
                      const BRepCheck_Analyzer& theAna,
                      const TopoDS_Shape&       theShape)
  {
    if ( Journal.Access().IsNull() )
      return; // Cannot dump to NULL Logger

    Journal.SendLogMessage(LogErr(Normal) << "Shape is invalid.");

    // Initialize array storing the number of problems
    Handle(TColStd_HArray1OfInteger) NbProblems = new TColStd_HArray1OfInteger(1, BRepCheck_CheckFail);
    for ( int i = 1; i <= BRepCheck_CheckFail; i++ )
      NbProblems->SetValue(i, 0);

    Handle(TopTools_HSequenceOfShape) sl = new TopTools_HSequenceOfShape();

    TopTools_DataMapOfShapeListOfShape MAP;
    GetProblemShapes(theAna, theShape, sl, NbProblems, MAP);

    if ( NbProblems->Value(BRepCheck_InvalidPointOnCurve) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid point on curve (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidPointOnCurve) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidPointOnCurveOnSurface) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid point on CONS (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidPointOnCurveOnSurface) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidPointOnSurface) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid point on surface (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidPointOnSurface) );
    //
    if ( NbProblems->Value(BRepCheck_No3DCurve) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Missing 3D curve (count: %1)."
                                              << NbProblems->Value(BRepCheck_No3DCurve) );
    //
    if ( NbProblems->Value(BRepCheck_Multiple3DCurve) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Multiple 3D curve (count: %1)."
                                              << NbProblems->Value(BRepCheck_Multiple3DCurve) );
    //
    if ( NbProblems->Value(BRepCheck_Invalid3DCurve) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid 3D curve (count: %1)."
                                              << NbProblems->Value(BRepCheck_Invalid3DCurve) );
    //
    if ( NbProblems->Value(BRepCheck_NoCurveOnSurface) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Missing p-curve (count: %1)."
                                              << NbProblems->Value(BRepCheck_NoCurveOnSurface) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidCurveOnSurface) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid p-curve (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidCurveOnSurface) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidCurveOnClosedSurface) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid p-curve on closed surface (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidCurveOnClosedSurface) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidSameRangeFlag) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Same-range flag is turned off (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidSameRangeFlag) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidSameParameterFlag) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Same-parameter flag is turned off (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidSameParameterFlag) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidDegeneratedFlag) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid degenerated flag (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidDegeneratedFlag) );
    //
    if ( NbProblems->Value(BRepCheck_FreeEdge) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Unexpected free edge (count: %1)."
                                              << NbProblems->Value(BRepCheck_FreeEdge) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidMultiConnexity) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Multi-connexity problem (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidMultiConnexity) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidRange) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid range (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidRange) );
    //
    if ( NbProblems->Value(BRepCheck_EmptyWire) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Empty wire (count: %1)."
                                              << NbProblems->Value(BRepCheck_EmptyWire) );
    //
    if ( NbProblems->Value(BRepCheck_RedundantEdge) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Redundant edge (count: %1)."
                                              << NbProblems->Value(BRepCheck_RedundantEdge) );
    //
    if ( NbProblems->Value(BRepCheck_SelfIntersectingWire) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Self-intersecting wire (count: %1)."
                                              << NbProblems->Value(BRepCheck_SelfIntersectingWire) );
    //
    if ( NbProblems->Value(BRepCheck_NoSurface) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Missing surface (count: %1)."
                                              << NbProblems->Value(BRepCheck_NoSurface) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidWire) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid wire (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidWire) );
    //
    if ( NbProblems->Value(BRepCheck_RedundantWire) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Redundant wire (count: %1)."
                                              << NbProblems->Value(BRepCheck_RedundantWire) );
    //
    if ( NbProblems->Value(BRepCheck_IntersectingWires) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Intersecting wires (count: %1)."
                                              << NbProblems->Value(BRepCheck_IntersectingWires) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidImbricationOfWires) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid imbrication of wires (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidImbricationOfWires) );
    //
    if ( NbProblems->Value(BRepCheck_EmptyShell) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Empty shell (count: %1)."
                                              << NbProblems->Value(BRepCheck_EmptyShell) );
    //
    if ( NbProblems->Value(BRepCheck_RedundantFace) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Redundant face (count: %1)."
                                              << NbProblems->Value(BRepCheck_RedundantFace) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidImbricationOfShells) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid imbrication of shells (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidImbricationOfShells) );
    //
    if ( NbProblems->Value(BRepCheck_UnorientableShape) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Unorientable shape (count: %1)."
                                              << NbProblems->Value(BRepCheck_UnorientableShape) );
    //
    if ( NbProblems->Value(BRepCheck_NotClosed) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Not closed (count: %1)."
                                              << NbProblems->Value(BRepCheck_NotClosed) );
    //
    if ( NbProblems->Value(BRepCheck_NotConnected) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Not connected (count: %1)."
                                              << NbProblems->Value(BRepCheck_NotConnected) );
    //
    if ( NbProblems->Value(BRepCheck_SubshapeNotInShape) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Sub-shape is not in shape (count: %1)."
                                              << NbProblems->Value(BRepCheck_SubshapeNotInShape) );
    //
    if ( NbProblems->Value(BRepCheck_BadOrientation) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Bad orientation (count: %1)."
                                              << NbProblems->Value(BRepCheck_BadOrientation) );
    //
    if ( NbProblems->Value(BRepCheck_BadOrientationOfSubshape) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Bad orientation of sub-shape (count: %1)."
                                              << NbProblems->Value(BRepCheck_BadOrientationOfSubshape) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidPolygonOnTriangulation) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid polygon on triangulation (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidPolygonOnTriangulation) );
    //
    if ( NbProblems->Value(BRepCheck_InvalidToleranceValue) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Invalid tolerance value (count: %1)."
                                              << NbProblems->Value(BRepCheck_InvalidToleranceValue) );
    //
    if ( NbProblems->Value(BRepCheck_EnclosedRegion) > 0 )
      Journal.SendLogMessage( LogWarn(Normal) << "Enclosed region (count: %1)."
                                              << NbProblems->Value(BRepCheck_EnclosedRegion) );
    //
    if ( NbProblems->Value(BRepCheck_CheckFail) > 0 )
      Journal.SendLogMessage( LogErr(Normal) << "Shape checker failed (count: %1)."
                                             << NbProblems->Value(BRepCheck_CheckFail) );
  }

};

//-----------------------------------------------------------------------------

TopoDS_Shape asiAlgo_Utils::ApplyTransformation(const TopoDS_Shape& theShape,
                                                const double        theXPos,
                                                const double        theYPos,
                                                const double        theZPos,
                                                const double        theAngleA,
                                                const double        theAngleB,
                                                const double        theAngleC,
                                                const bool          doCopy)
{
  gp_Trsf T = Transformation(theXPos, theYPos, theZPos,
                             theAngleA, theAngleB, theAngleC);

  return ApplyTransformation(theShape, T, doCopy);
}

//-----------------------------------------------------------------------------

gp_Trsf asiAlgo_Utils::Transformation(const double theXPos,
                                      const double theYPos,
                                      const double theZPos,
                                      const double theAngleA,
                                      const double theAngleB,
                                      const double theAngleC)
  {
  gp_Vec aTranslation(theXPos, theYPos, theZPos);

  gp_Quaternion aRotationX(gp::DX(), theAngleA);
  gp_Quaternion aRotationY(gp::DY(), theAngleB);
  gp_Quaternion aRotationZ(gp::DZ(), theAngleC);

  gp_Trsf aTrsf;
  aTrsf.SetRotation(aRotationZ * aRotationY * aRotationX);
  aTrsf.SetTranslationPart(aTranslation);

  return aTrsf;
}

//-----------------------------------------------------------------------------

TopoDS_Shape
  asiAlgo_Utils::ApplyTransformation(const TopoDS_Shape& theShape,
                                     const gp_Trsf&      theTransform,
                                     const bool          doCopy)
  {
  if ( doCopy )
  {
    TopoDS_Shape copy = BRepBuilderAPI_Copy(theShape, 1).Shape();
    return copy.Moved(theTransform);
  }

  return theShape.Moved(theTransform);
}

//-----------------------------------------------------------------------------

TopoDS_Shape
  asiAlgo_Utils::AssembleShapes(const TopTools_ListOfShape& shapes)
{
  TopoDS_Shape result, singleShape;
  int numShapes = 0;

  for ( TopTools_ListIteratorOfListOfShape it(shapes); it.More(); it.Next() )
  {
    if ( !it.Value().IsNull() )
    {
      ++numShapes;
      singleShape = it.Value();
    }
  }

  if ( numShapes > 1)
  {
    TopoDS_Compound compound;
    BRep_Builder B;
    B.MakeCompound(compound);

    for ( TopTools_ListIteratorOfListOfShape it(shapes); it.More(); it.Next() )
    {
      const TopoDS_Shape& shape = it.Value();
      //
      if ( shape.IsNull() )
        continue;

      ++numShapes;
      B.Add(compound, shape);
    }

    result = compound;
  }
  else
    result = singleShape;

  return result;
}

//-----------------------------------------------------------------------------

TopoDS_Shape
  asiAlgo_Utils::AssembleShapes(const Handle(TopTools_HSequenceOfShape)& shapes)
{
  TopTools_ListOfShape shapeList;

  // Repack as list for unification.
  if ( !shapes.IsNull() )
    for ( TopTools_SequenceOfShape::Iterator it(*shapes); it.More(); it.Next() )
      shapeList.Append( it.Value() );

  return AssembleShapes(shapeList);
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::Bounds(const TopoDS_Shape& shape,
                           double& XMin, double& YMin, double& ZMin,
                           double& XMax, double& YMax, double& ZMax,
                           const double tolerance)
{
  Bnd_Box bndBox;

  // Yeah, BRepBndLib::Add() may throw exceptions, did you know that?!
  try
  {
    BRepBndLib::Add(shape, bndBox);
  }
  catch ( ... )
  {
    return false;
  }
  //
  if ( bndBox.IsVoid() )
    return false;

  bndBox.Get(XMin, YMin, ZMin, XMax, YMax, ZMax);
  bndBox.Enlarge(tolerance);
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::Bounds(const Handle(Poly_Triangulation)& tris,
                           double& XMin, double& YMin, double& ZMin,
                           double& XMax, double& YMax, double& ZMax,
                           const double tolerance)
{
  TopoDS_Face fictiveFace;
  BRep_Builder().MakeFace(fictiveFace);
  BRep_Builder().UpdateFace(fictiveFace, tris);

  return Bounds(fictiveFace, XMin, YMin, ZMin, XMax, YMax, ZMax, tolerance);
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::CheckShape(const TopoDS_Shape&  shape,
                               ActAPI_ProgressEntry Journal)
{
  BRepCheck_Analyzer Checker(shape);

  // Check validity
  if ( Checker.IsValid() )
  {
    Journal.SendLogMessage(LogInfo(Normal) << "Part is topologically and geometrically sound.");
    return true;
  }

  // ...
  // Shape is known to be invalid at this stage
  // ...

  // Perform structural dump
  if ( Journal.Access() )
    CheckShapeAux::StructuralDump(Journal, Checker, shape);

  return false;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::HasAllClosedWires(const TopoDS_Face& face,
                                      const double       coincConfusion3d)
{
  // Loop over the wires
  for ( TopoDS_Iterator it(face); it.More(); it.Next() )
  {
    const TopoDS_Shape& subshape = it.Value();
    //
    if ( subshape.ShapeType() != TopAbs_WIRE )
      continue;

    const TopoDS_Wire& wire = TopoDS::Wire(subshape);

    // Check each wire individually.
    if ( !IsClosedGeometrically(wire, face, coincConfusion3d) )
      return false;
  }
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::IsClosedGeometrically(const TopoDS_Wire& wire,
                                          const TopoDS_Face& face,
                                          const double       coincConfusion3d)
{
  // Convert spatial tolerance to UV-parametric tolerance using resolutions.
  // This is actually a weak check as sometimes correction according to
  // resolution gives unreliable 2D tolerances. However, we think that using
  // such resolutions is still better than application of three-dimensional
  // tolerances in two-dimensional space.
  BRepAdaptor_Surface bas(face);
  //
  const double uToler      = bas.UResolution(coincConfusion3d);
  const double vToler      = bas.VResolution(coincConfusion3d);
  const double uvConfusion = Max(uToler, vToler);

  // This method takes into account topological orientation of the wire
  // in question. However, it does not check any orientation flags. It simply
  // tests all extremities of one edge with all extremities of another edge.

  // It should be noted that in OpenCascade there is no restriction for a
  // wire to contain all edges in the order of traverse. That is, a wire
  // traversed as E1->E2->E3->E4 may physically contain those edges in
  // the order like E1, E2, E4, E3. Therefore, simple sequential iteration
  // from previous to next edge is not enough to check that all extremities
  // of all edges are 1-manifold. To check this, we use an auxiliary map of
  // edges together with valences of their extremities. For example:
  //
  //  Edge | <Extremity 1; Extremity 2> <-> <Valence 1; Valence 2>
  // ------+------------------------------------------------------
  //   E1  | <(0,0); (0,1)>             <-> <2, 2>
  //   E2  | <(1,1); (0,1)>             <-> <1, 2>
  //  ...
  //
  // This map allows making verdict on closeness of the wire in question.
  // Apparently, in a well-defined contour, all edges should have all
  // extremities with valence 2.
  //
  // NOTICE: to avoid degeneracies in 3D for special cases like cylinders,
  //         cones, spheres, tori, etc., we use (u,v) parametric space
  //         for extremity coordinates. We make use of the fact, that a valid
  //         parametric domain is a topological rectangle. Our reasoning of
  //         valences is valid only in this assumption.

  // Check number of edges
  TopTools_IndexedMapOfShape edges;
  TopExp::MapShapes(wire, TopAbs_EDGE, edges);
  const int nEdges = edges.Extent();
  //
  if ( !nEdges )
    return false;
  //
  if ( nEdges == 1 )
  {
    double f, l;
    Handle(Geom_Curve) C = BRep_Tool::Curve( TopoDS::Edge( edges(1) ), f, l );

    if ( C.IsNull() )
      return false;

    // In case of sole edge, the boundary will be closed if two conditions
    // are met:
    //
    // - 3D curve is closed
    // - 2D curve is closed

    gp_Pnt Pf = C->Value( C->FirstParameter() );
    gp_Pnt Pl = C->Value( C->LastParameter() );

    // The following condition is Ok for closed curves
    const bool isOk3d = ( Pf.Distance(Pl) < coincConfusion3d );

    // Check 2d
    gp_Pnt2d Pf2d, Pl2d;
    BRep_Tool::UVPoints(TopoDS::Edge( edges(1) ), face, Pf2d, Pl2d);

    // The following condition is Ok for closed curves
    const bool isOk2d = ( Pf2d.Distance(Pl2d) < uvConfusion );

    return isOk3d && isOk2d;
  }

  struct t_extremities
  {
    gp_Pnt2d P1, P2;
  };
  struct t_valences
  {
    int V1, V2;
  };
  typedef NCollection_DataMap< TopoDS_Edge, std::pair<t_extremities, t_valences> > t_valence_map;

  // Prepare valence map
  t_valence_map valenceMap;

  // Check connectivity. We use explorer to loop over the wire, because we need
  // all coedges to participate in that loop. E.g. a single seam edge should be
  // iterated twice as it has two p-curves.
  for ( TopExp_Explorer exp(wire, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    TopoDS_Edge E = TopoDS::Edge( exp.Current() );

    // Get 2D points. Notice that we get 2D points as extermities of CONS
    // (Curve ON Surface) and not with UVPoints() method of BRep_Tool. This
    // is because the latter method returns pre-evaluated UV coordinates
    // which suffer from instability when it comes to periodic surfaces.
    // E.g., U coordinate may be either 0 or 2*PI depending on how to evaluate
    // the corresponding UV pair. However, if we use pcurve explicitly, such
    // instability does not happen as we essentially use geometry which we see
    // in parametric domain of a face (always a finite boundedregion without
    // any weird periodicity anomalies).
    gp_Pnt2d Pf, Pl;
    double f, l;
    Handle(Geom2d_Curve) c2d = BRep_Tool::CurveOnSurface(E, face, f, l);
    //
    if ( !c2d.IsNull() )
    {
      Pf = c2d->Value(f);
      Pl = c2d->Value(l);
    }
    else
    {
      // Use cached points if p-curve is not available...
      BRep_Tool::UVPoints(E, face, Pf, Pl);
    }

    // Add initial valence record for the new edge
    valenceMap.Bind( E, std::pair<t_extremities, t_valences>() );
    valenceMap(E).first.P1  = Pf;
    valenceMap(E).first.P2  = Pl;
    valenceMap(E).second.V1 = 1;
    valenceMap(E).second.V2 = 1;

    // Update valence map wrt new record
    for ( t_valence_map::Iterator it2(valenceMap); it2.More(); it2.Next() )
    {
      const TopoDS_Edge& otherEdge = it2.Key();
      //
      if ( otherEdge.IsPartner(E) )
        continue;

      std::pair<t_extremities, t_valences>& otherRecord = it2.ChangeValue();

      const gp_Pnt2d& otherPf = otherRecord.first.P1;
      const gp_Pnt2d& otherPl = otherRecord.first.P2;

      if ( Pf.Distance(otherPf) < uvConfusion )
      {
        valenceMap(E).second.V1++;
        otherRecord.second.V1++;
      }
      if ( Pf.Distance(otherPl) < uvConfusion )
      {
        valenceMap(E).second.V1++;
        otherRecord.second.V2++;
      }
      if ( Pl.Distance(otherPf) < uvConfusion )
      {
        valenceMap(E).second.V2++;
        otherRecord.second.V1++;
      }
      if ( Pl.Distance(otherPl) < uvConfusion )
      {
        valenceMap(E).second.V2++;
        otherRecord.second.V2++;
      }
    } // end update valence map
  } // end accumulating of valence map

#if defined COUT_DEBUG
  for ( t_valence_map::Iterator it(valenceMap); it.More(); it.Next() )
  {
    const TopoDS_Edge&                          E      = it.Key();
    const std::pair<t_extremities, t_valences>& record = it.Value();

    std::cout << ShapeAddrWithPrefix(E).c_str() << std::endl;
    std::cout << "\t(" << record.first.P1.X() << ", "
                       << record.first.P1.Y() << ") ~ "
                       << record.second.V1 << std::endl;
    std::cout << "\t(" << record.first.P2.X() << ", "
                       << record.first.P2.Y() << ") ~ "
                       << record.second.V2 << std::endl;
  }
  std::cout << "***" << std::endl;
#endif

  // Now check valence map. If any record with valence < 2 exists, return false.
  for ( t_valence_map::Iterator it(valenceMap); it.More(); it.Next() )
  {
    const std::pair<t_extremities, t_valences>& record = it.Value();
    //
    if ( record.second.V1 < 2 || record.second.V2 < 2 )
      return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::HasEdgesWithoutVertices(const TopoDS_Face& face)
{
  for ( TopExp_Explorer exp(face, TopAbs_EDGE); exp.More(); exp.Next() )
  {
    TopTools_IndexedMapOfShape vertices;
    TopExp::MapShapes(exp.Current(), TopAbs_VERTEX, vertices);

    if ( vertices.IsEmpty() )
      return true;
  }
  return false;
}

//-----------------------------------------------------------------------------

double asiAlgo_Utils::MaxTolerance(const TopoDS_Shape& shape)
{
  ShapeAnalysis_ShapeTolerance TolerChecker;
  const double maxToler = TolerChecker.Tolerance(shape, 1); // 1 means max

  return maxToler;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::ReadBRep(const TCollection_AsciiString& filename,
                             TopoDS_Shape&                  shape)
{
  if ( IsASCII(filename) )
  {
    BRep_Builder BB;
    return BRepTools::Read(shape, filename.ToCString(), BB);
  }

  // Try to read as binary
  return BinTools::Read( shape, filename.ToCString() );
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::WriteBRep(const TopoDS_Shape&            theShape,
                              const TCollection_AsciiString& theFilename)
{
  BRepTools::Clean(theShape);
  return BRepTools::Write( theShape, theFilename.ToCString() );
}

//-----------------------------------------------------------------------------

void asiAlgo_Utils::ShapeSummary(const TopoDS_Shape& shape,
                                 int&                nbCompsolids,
                                 int&                nbCompounds,
                                 int&                nbSolids,
                                 int&                nbShells,
                                 int&                nbFaces,
                                 int&                nbWires,
                                 int&                nbOuterWires,
                                 int&                nbInnerWires,
                                 int&                nbEdges,
                                 int&                nbDegenEdges,
                                 int&                nbVertexes)
{
  if ( shape.IsNull() )
    return;

  // Get all outer wires in the model
  TopTools_IndexedMapOfShape outerWires;
  for ( TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next() )
  {
    const TopoDS_Face& F = TopoDS::Face( exp.Current() );
    outerWires.Add( BRepTools::OuterWire(F) );
  }

  // The following map is used to skip already traversed TShapes
  TopTools_IndexedMapOfShape M;

  // Traverse all sub-shapes
  TopTools_ListOfShape shapeList;
  shapeList.Append(shape);
  //
  for ( TopTools_ListIteratorOfListOfShape itL(shapeList); itL.More(); itL.Next() )
  {
    const TopoDS_Shape& currentShape = itL.Value();
    //
    if ( M.Contains( currentShape.Located( TopLoc_Location() ) ) )
      continue;
    else
      M.Add( currentShape.Located( TopLoc_Location() ) );

    if ( currentShape.ShapeType() == TopAbs_COMPSOLID )
      nbCompsolids++;
    else if ( currentShape.ShapeType() == TopAbs_COMPOUND )
      nbCompounds++;
    else if ( currentShape.ShapeType() == TopAbs_SOLID )
      nbSolids++;
    else if ( currentShape.ShapeType() == TopAbs_SHELL )
      nbShells++;
    else if ( currentShape.ShapeType() == TopAbs_FACE )
      nbFaces++;
    else if ( currentShape.ShapeType() == TopAbs_WIRE )
    {
      nbWires++;
      if ( outerWires.Contains(currentShape) )
        nbOuterWires++;
      else
        nbInnerWires++;
    }
    else if ( currentShape.ShapeType() == TopAbs_EDGE )
    {
      nbEdges++;
      if ( BRep_Tool::Degenerated( TopoDS::Edge(currentShape) ) )
        nbDegenEdges++;
    }
    else if ( currentShape.ShapeType() == TopAbs_VERTEX )
      nbVertexes++;

    // Iterate over the direct children of a shape
    for ( TopoDS_Iterator sub_it(currentShape); sub_it.More(); sub_it.Next() )
    {
      const TopoDS_Shape& subShape = sub_it.Value();

      // Add sub-shape to iterate over
      shapeList.Append(subShape);
    }
  }
}

//-----------------------------------------------------------------------------

void asiAlgo_Utils::ShapeSummary(const TopoDS_Shape&      shape,
                                 TCollection_AsciiString& info)
{
  // Summary
  int nbCompsolids = 0,
      nbCompounds  = 0,
      nbSolids     = 0,
      nbShells     = 0,
      nbFaces      = 0,
      nbWires      = 0,
      nbOuterWires = 0,
      nbInnerWires = 0,
      nbEdges      = 0,
      nbDegenEdges = 0,
      nbVertexes   = 0;

  // Extract summary
  ShapeSummary(shape,
               nbCompsolids,
               nbCompounds,
               nbSolids,
               nbShells,
               nbFaces,
               nbWires,
               nbOuterWires,
               nbInnerWires,
               nbEdges,
               nbDegenEdges,
               nbVertexes);

  // Prepare output string with the gathered summary
  info += "PART SUMMARY (EQ TSHAPE, ANY TRSF, ANY ORI):\n";
  info += "- nb compsolids: "; info += nbCompsolids; info += "\n";
  info += "- nb compounds: ";  info += nbCompounds;  info += "\n";
  info += "- nb solids: ";     info += nbSolids;     info += "\n";
  info += "- nb shells: ";     info += nbShells;     info += "\n";
  info += "- nb faces: ";      info += nbFaces;      info += "\n";
  info += "- nb wires: ";      info += nbWires;      info += "\n";
  info += "- nb edges: ";      info += nbEdges;      info += "\n";
  info += "- nb vertices: ";   info += nbVertexes;   info += "\n";
}

//-----------------------------------------------------------------------------

TopoDS_Wire asiAlgo_Utils::CreateCircularWire(const double radius)
{
  Handle(Geom_Circle) C = GC_MakeCircle(gp_Ax1( gp::Origin(), gp::DZ() ), radius);

  // Let's convert our circle to b-curve
  Handle(Geom_BSplineCurve) BC = GeomConvert::CurveToBSplineCurve(C, Convert_QuasiAngular);

  // Build a wire
  return BRepBuilderAPI_MakeWire( BRepBuilderAPI_MakeEdge(BC) );
}

//-----------------------------------------------------------------------------

TopoDS_Shape asiAlgo_Utils::MakeSkin(const TopTools_SequenceOfShape& wires)
{
  if ( wires.Length() < 2 )
  {
    std::cout << "Error: not enough sections" << std::endl;
    return TopoDS_Shape();
  }

  // Initialize and build
  BRepOffsetAPI_ThruSections ThruSections(false, false, 1.0e-1);
  ThruSections.SetSmoothing(false);
  ThruSections.SetMaxDegree(8);
  //
  for ( int k = 1; k <= wires.Length(); ++k )
  {
    if ( wires(k).ShapeType() != TopAbs_WIRE )
    {
      std::cout << "Warning: section " << k << " is not a wire" << std::endl;
      continue;
    }
    //
    ThruSections.AddWire( TopoDS::Wire( wires(k) ) );
  }
  try
  {
    ThruSections.Build();
  }
  catch ( ... )
  {
    std::cout << "Error: crash in BRepOffsetAPI_ThruSections" << std::endl;
    return TopoDS_Shape();
  }
  //
  if ( !ThruSections.IsDone() )
  {
    std::cout << "Error: IsDone() false in BRepOffsetAPI_ThruSections" << std::endl;
    return TopoDS_Shape();
  }

  // Get the result
  TopExp_Explorer exp(ThruSections.Shape(), TopAbs_SHELL);
  if ( !exp.More() )
    return TopoDS_Shape();
  //
  TopoDS_Shell ResultShell = TopoDS::Shell( exp.Current() );
  //
  std::cout << "Skinning was done successfully" << std::endl;
  return ResultShell;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::Sew(const TopoDS_Shape& shape,
                        const double        tolerance,
                        TopoDS_Shape&       result)
{
  BRepBuilderAPI_Sewing Sewer(tolerance);
  Sewer.Load(shape);

  // Perform sewing
  Sewer.Perform();
  result = Sewer.SewedShape();
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::MaximizeFaces(TopoDS_Shape& shape)
{
  ShapeUpgrade_UnifySameDomain Unify(shape);
  try
  {
    Unify.Build();
  }
  catch ( ... )
  {
    return false;
  }
  shape = Unify.Shape();
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::InterpolatePoints(const std::vector<gp_Pnt>& points,
                                      const int                  p,
                                      Handle(Geom_BSplineCurve)& result)
{
  // Number of unknown control points
  const int numPoles = (int) points.size();
  const int n        = numPoles - 1;
  const int m        = n + p + 1;

  // Calculate total chord length
  double chordTotal = 0.0;
  for ( size_t k = 1; k < numPoles; ++k )
    chordTotal += Sqrt( ( points[k].XYZ() - points[k-1].XYZ() ).Modulus() );

  //-------------------------------------------------------------------------
  // Choose parameters
  double* pParams = new double[numPoles];
  pParams[0] = 0.0;
  for ( int k = 1; k < numPoles - 1; ++k )
  {
    pParams[k] = pParams[k - 1] + Sqrt( ( points[k].XYZ() - points[k-1].XYZ() ).Modulus() ) / chordTotal;
  }
  pParams[numPoles - 1] = 1.0;

  //-------------------------------------------------------------------------
  // Choose knots
  double* pKnots = new double[m + 1];
  //
  for ( int k = 0; k < p + 1; ++k )
    pKnots[k] = 0.0;
  for ( int k = m; k > m - p - 1; --k )
    pKnots[k] = 1.0;
  for ( int j = 1; j <= n - p; ++j )
  {
    pKnots[j + p] = 0.0;
    for ( int i = j; i <= j + p - 1; ++i )
    {
      pKnots[j + p] += pParams[i];
    }
    pKnots[j + p] /= p;
  }

  // Convert to flat knots
  TColStd_Array1OfReal flatKnots(1, m + 1);
  for ( int k = 0; k <= m; ++k )
    flatKnots(k + 1) = pKnots[k];

  // Initialize matrix from the passed row pointer
  const int dim = n + 1;
  Eigen::MatrixXd eigen_N_mx(dim, dim);

  // Calculate non-zero B-splines for each parameter value
  for ( int k = 0; k <= n; ++k )
  {
    int firstNonZeroIdx;
    math_Matrix N_mx(1, 1, 1, p + 1);
    BSplCLib::EvalBsplineBasis(0, p + 1, flatKnots, pParams[k], firstNonZeroIdx, N_mx);

#if defined DUMP_COUT
    // Dump
    std::cout << "\tFirst Non-Zero Index for " << pParams[k] << " is " << firstNonZeroIdx << std::endl;
    for ( int kk = 1; kk <= p + 1; ++kk )
    {
      std::cout << "\t" << N_mx(1, kk);
    }
    std::cout << std::endl;
#endif

    // Fill Eigen's matrix
    for ( int c = 0; c < firstNonZeroIdx - 1; ++c )
    {
      eigen_N_mx(k, c) = 0.0;
    }
    //
    for ( int c = firstNonZeroIdx - 1, kk = 1; c < firstNonZeroIdx + p; ++c, ++kk )
    {
      eigen_N_mx(k, c) = N_mx(1, kk);
    }
    //
    for ( int c = firstNonZeroIdx + p; c < dim; ++c )
    {
      eigen_N_mx(k, c) = 0.0;
    }
  }

#if defined DUMP_FILES
  // Dump Eigen matrix
  {
    std::ofstream FILE;
    FILE.open(dump_filename_N, std::ios::out | std::ios::trunc);
    //
    if ( !FILE.is_open() )
    {
      std::cout << "Error: cannot open file for dumping" << std::endl;
      return false;
    }
    //
    for ( int i = 0; i < dim; ++i )
    {
      for ( int j = 0; j < dim; ++j )
      {
        FILE << eigen_N_mx(i, j);
        if ( (i + j + 2) < (dim + dim) )
          FILE << "\t";
      }
      FILE << "\n";
    }
  }
#endif

  // Initialize vector of right hand side
  Eigen::MatrixXd eigen_B_mx(dim, 3);
  for ( int r = 0; r < dim; ++r )
  {
    eigen_B_mx(r, 0) = points[r].X();
    eigen_B_mx(r, 1) = points[r].Y();
    eigen_B_mx(r, 2) = points[r].Z();
  }

#if defined DUMP_FILES
  // Dump Eigen B [X]
  {
    std::ofstream FILE;
    FILE.open(dump_filename_Bx, std::ios::out | std::ios::trunc);
    //
    if ( !FILE.is_open() )
    {
      std::cout << "Error: cannot open file for dumping" << std::endl;
      return false;
    }
    //
    for ( int i = 0; i < dim; ++i )
    {
      FILE << eigen_B_mx(i, 0) << "\n";
    }
  }
#endif

  //---------------------------------------------------------------------------
  // BEGIN: solve linear system
  //---------------------------------------------------------------------------

  TIMER_NEW
  TIMER_GO

  // Solve
  Eigen::ColPivHouseholderQR<Eigen::MatrixXd> QR(eigen_N_mx);
  Eigen::MatrixXd eigen_X_mx = QR.solve(eigen_B_mx);

  TIMER_FINISH
  TIMER_COUT_RESULT_MSG("B-curve interpolation")

  //---------------------------------------------------------------------------
  // END: solve linear system
  //---------------------------------------------------------------------------

  // Fill poles
  TColgp_Array1OfPnt poles(1, dim);
  for ( int i = 0; i < dim; ++i )
  {
    gp_Pnt P( eigen_X_mx(i, 0), eigen_X_mx(i, 1), eigen_X_mx(i, 2) );
    poles(i + 1) = P;
  }

  // Fill knots
  TColStd_Array1OfReal knots(1, m + 1 - 2*p);
  TColStd_Array1OfInteger mults(1, m + 1 - 2*p);
  //
  knots(1) = 0; mults(1) = p + 1;
  //
  for ( int j = 2, jj = p + 1; j <= m - 2*p; ++j, ++jj )
  {
    knots(j) = pKnots[jj]; mults(j) = 1;
  }
  //
  knots( knots.Upper() ) = 1; mults( mults.Upper() ) = p + 1;

#if defined DUMP_COUT
  // Dump knots
  for ( int k = knots.Lower(); k <= knots.Upper(); ++k )
  {
    std::cout << "knots(" << k << ") = " << knots(k) << "\t" << mults(k) << std::endl;
  }
#endif

  // Create B-spline curve
  Handle(Geom_BSplineCurve) bcurve = new Geom_BSplineCurve(poles, knots, mults, p);

  // Save result
  result = bcurve;

  // Delete parameters
  delete[] pParams;
  delete[] pKnots;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::InterpolatePoints(const Handle(asiAlgo_BaseCloud<double>)& points,
                                      const int                                p,
                                      Handle(Geom_BSplineCurve)&               result)
{
  std::vector<gp_Pnt> occPts;

  // Repack data points.
  for ( int i = 0; i < points->GetNumberOfElements(); ++i )
  {
    double x, y, z;
    points->GetElement(i, x, y, z);
    //
    occPts.push_back( gp_Pnt(x, y, z) );
  }

  return InterpolatePoints(occPts, p, result);
}

//-----------------------------------------------------------------------------

TopoDS_Shape asiAlgo_Utils::BooleanCut(const TopoDS_Shape& Object,
                                       const TopoDS_Shape& Tool,
                                       const double        fuzzy)
{
  // Prepare the arguments
  TopTools_ListOfShape BOP_args;
  BOP_args.Append(Object);
  BOP_args.Append(Tool);

  // Prepare data structure (calculate interferences)
  Handle(NCollection_BaseAllocator) Alloc = new NCollection_IncAllocator;
  //
  BOPAlgo_PaveFiller DSFiller(Alloc);
  DSFiller.SetArguments(BOP_args);
  DSFiller.SetRunParallel(0);
  DSFiller.SetFuzzyValue(fuzzy);
  DSFiller.Perform();

  // Check data structure
  bool hasErr = DSFiller.HasErrors();
  if ( hasErr )
  {
    std::cout << "Error: cannot cut" << std::endl;
    return TopoDS_Shape();
  }

  // Run BOP
  BOPAlgo_BOP BOP(Alloc);
  BOP.AddArgument(Object);
  BOP.AddTool(Tool);
  BOP.SetRunParallel(0);
  BOP.SetOperation(BOPAlgo_CUT);
  BOP.PerformWithFiller(DSFiller);
  hasErr = BOP.HasErrors();
  if ( hasErr )
  {
    std::cout << "Error: cannot cut the part model from the stock model" << std::endl;
    return TopoDS_Shape();
  }

  return BOP.Shape();
}

//-----------------------------------------------------------------------------

TopoDS_Shape asiAlgo_Utils::BooleanCut(const TopoDS_Shape&         Object,
                                       const TopTools_ListOfShape& Tools,
                                       const bool                  isParallel,
                                       const double                fuzz)
{
  BRepAlgoAPI_Cut API;
  //
  return BooleanCut(Object, Tools, isParallel, fuzz, API);
}

//-----------------------------------------------------------------------------

TopoDS_Shape asiAlgo_Utils::BooleanCut(const TopoDS_Shape&         Object,
                                       const TopTools_ListOfShape& Tools,
                                       const bool                  isParallel,
                                       const double                fuzz,
                                       BRepAlgoAPI_Cut&            API)
{
  // Prepare the arguments
  TopTools_ListOfShape Objects;
  Objects.Append(Object);

  // Set the arguments
  API.SetArguments(Objects);
  API.SetTools(Tools);
  API.SetRunParallel(isParallel);
  API.SetFuzzyValue(fuzz);

  // Run the algorithm 
  API.Build(); 

  // Check the result
  const bool hasErr = API.HasErrors();
  //
  if ( hasErr )
  {
    std::cout << "Error: cannot cut the part model from the stock model" << std::endl;
    return TopoDS_Shape();
  }

  return API.Shape();
}

//-----------------------------------------------------------------------------

//! Performs Boolean fuse operation on the passed objects.
//! \param[in] objects shapes to fuse.
//! \return result of fuse.
TopoDS_Shape asiAlgo_Utils::BooleanFuse(const TopTools_ListOfShape& objects)
{
  TopTools_ListIteratorOfListOfShape it(objects);
  TopoDS_Shape result = it.Value();
  it.Next();

  for ( ; it.More(); it.Next() )
  {
    TopoDS_Shape arg = it.Value();
    result = BRepAlgoAPI_Fuse(result, arg);
  }

  return result;
}

//-----------------------------------------------------------------------------

//! Performs Boolean intersection operation on the passed objects.
//! \param[in] objects shapes to intersect.
//! \return result of intersection.
TopoDS_Shape asiAlgo_Utils::BooleanIntersect(const TopTools_ListOfShape& objects)
{
  TopTools_ListIteratorOfListOfShape it(objects);
  TopoDS_Shape result = it.Value();
  it.Next();

  for ( ; it.More(); it.Next() )
  {
    TopoDS_Shape arg = it.Value();
    result = BRepAlgoAPI_Common(result, arg);
  }

  return result;
}

//-----------------------------------------------------------------------------

//! Performs Boolean General Fuse for the passed objects.
//! \param[in]  objects objects to fuse in non-manifold manner.
//! \param[in]  fuzz    fuzzy value to control the tolerance.
//! \param[out] API     Boolean algorithm to access history.
TopoDS_Shape asiAlgo_Utils::BooleanGeneralFuse(const TopTools_ListOfShape& objects,
                                               const double                fuzz,
                                               BOPAlgo_Builder&            API)
{
  const bool bRunParallel = false;

  BOPAlgo_PaveFiller DSFiller;
  DSFiller.SetArguments(objects);
  DSFiller.SetRunParallel(bRunParallel);
  DSFiller.SetFuzzyValue(fuzz);
  DSFiller.Perform();
  bool hasErr = DSFiller.HasErrors();
  //
  if ( hasErr )
  {
    return TopoDS_Shape();
  }

  API.SetArguments(objects);
  API.SetRunParallel(bRunParallel);
  API.PerformWithFiller(DSFiller);
  hasErr = API.HasErrors();
  //
  if ( hasErr )
  {
    return TopoDS_Shape();
  }

  return API.Shape();
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::BooleanRemoveFaces(const TopoDS_Shape&  shape,
                                       const TopoDS_Shape&  face2Remove,
                                       const bool           runParallel,
                                       const bool           trackHistory,
                                       TopoDS_Shape&        result,
                                       ActAPI_ProgressEntry progress)
{
  TopTools_ListOfShape faces2Remove; faces2Remove.Append(face2Remove);

  return BooleanRemoveFaces(shape,
                            faces2Remove,
                            runParallel,
                            trackHistory,
                            result,
                            progress);
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::BooleanRemoveFaces(const TopoDS_Shape&         shape,
                                       const TopTools_ListOfShape& faces2Remove,
                                       const bool                  runParallel,
                                       const bool                  trackHistory,
                                       TopoDS_Shape&               result,
                                       ActAPI_ProgressEntry        progress)
{
  // Prepare tool.
  BRepAlgoAPI_Defeaturing API;
  //
  API.SetShape         (shape);
  API.AddFacesToRemove (faces2Remove);
  API.SetRunParallel   (runParallel);
  API.TrackHistory     (trackHistory);

  // Perform.
  API.Build();
  //
  if ( !API.IsDone() )
  {
    progress.SendLogMessage(LogErr(Normal) << "Smart face removal is not done.");
    return false;
  }
  //
  if ( API.HasWarnings() )
  {
    std::ostringstream out;
    API.DumpWarnings(out);

    progress.SendLogMessage( LogWarn(Normal) << "Smart face removal finished with warnings:\n\t %1"
                                             << out.str().c_str() );
  }

  // Set result.
  result = API.Shape();
  return true;
}

//-----------------------------------------------------------------------------

//! Performs Boolean General Fuse for the passed objects.
//! \param[in]  objects objects to fuse in non-manifold manner.
//! \param[in]  fuzz    fuzzy value to control the tolerance.
//! \param[out] API     Boolean algorithm to access history.
TopoDS_Shape asiAlgo_Utils::BooleanGeneralFuse(const TopTools_ListOfShape& objects,
                                               const double                fuzz)
{
  BOPAlgo_Builder API;
  return BooleanGeneralFuse(objects, fuzz, API);
}

//-----------------------------------------------------------------------------

//! Explodes the passed shape by solids.
//! \param model  [in]  input CAD part.
//! \param solids [out] extracted solids.
void asiAlgo_Utils::ExplodeBySolids(const TopoDS_Shape&   model,
                                    TopTools_ListOfShape& solids)
{
  for ( TopExp_Explorer exp(model, TopAbs_SOLID); exp.More(); exp.Next() )
  {
    solids.Append( exp.Current() );
  }
}

//-----------------------------------------------------------------------------

//! Inverts the passed face so that all internal loops become individual
//! faces while the outer loop is simply erased.
//! \param face     [in]  face to invert.
//! \param inverted [out] inversion result (collection of faces).
//! \return true in case of success, false -- otherwise.
bool asiAlgo_Utils::InvertFace(const TopoDS_Face&    face,
                               TopTools_ListOfShape& inverted)
{
  TopTools_IndexedMapOfShape wires;
  TopExp::MapShapes(face, TopAbs_WIRE, wires);
  //
  if ( wires.Extent() < 2 )
    return false; // Cannot invert a face without holes

  // Get outer wire
  TopoDS_Wire outerWire = BRepTools::OuterWire(face);

  // Get host geometry
  Handle(Geom_Surface) surf = BRep_Tool::Surface(face);

  // Search for internal loops: those which are not the outer wire
  for ( int w = 1; w <= wires.Extent(); ++w )
  {
    TopoDS_Wire wire = TopoDS::Wire( wires(w) );
    //
    if ( wire.IsSame(outerWire) )
      continue;

    // Construct another face on the same host with a different wire
    wire.Reverse();
    TopoDS_Face innerFace = BRepBuilderAPI_MakeFace(surf, wire, false);
    //
    ShapeFix_Face fix(innerFace);
    if ( fix.Perform() )
      innerFace = fix.Face();
    //
    inverted.Append(innerFace);
  }
  return true;
}

//-----------------------------------------------------------------------------

double asiAlgo_Utils::AutoSelectLinearDeflection(const TopoDS_Shape& model)
{
  double xMin, yMix, zMin, xMax, yMax, zMax;
  if ( !Bounds(model, xMin, yMix, zMin, xMax, yMax, zMax, 0.0001) )
  {
    xMin = yMix = zMin = 0.0;
    xMax = yMax = zMax = 1.0;
  }

  // Use a fraction of a bounding diagonal
  const double diag = ( gp_XYZ(xMin, yMix, zMin) - gp_XYZ(xMax, yMax, zMax) ).Modulus();
  return 0.001*diag;
}

//-----------------------------------------------------------------------------

double asiAlgo_Utils::AutoSelectAngularDeflection(const TopoDS_Shape& model)
{
  asiAlgo_NotUsed(model);

  return 0.5; // In degrees
}

//-----------------------------------------------------------------------------

Handle(Geom_BSplineCurve) asiAlgo_Utils::PolylineAsSpline(const std::vector<gp_XYZ>& trace)
{
  // Initialize properties for spline trajectories
  TColgp_Array1OfPnt poles( 1, (int) trace.size() );
  //
  for ( int k = 0; k < trace.size(); ++k )
  {
    poles(k + 1) = gp_Pnt( trace[k] );
  }

  const int n = poles.Upper() - 1;
  const int p = 1;
  const int m = n + p + 1;
  const int k = m + 1 - (p + 1)*2;

  // Knots
  TColStd_Array1OfReal knots(1, k + 2);
  knots(1) = 0;
  //
  for ( int j = 2; j <= k + 1; ++j )
    knots(j) = knots(j-1) + 1.0 / (k + 1);
  //
  knots(k + 2) = 1;

  // Multiplicities
  TColStd_Array1OfInteger mults(1, k + 2);
  mults(1) = 2;
  //
  for ( int j = 2; j <= k + 1; ++j )
    mults(j) = 1;
  //
  mults(k + 2) = 2;

  return new Geom_BSplineCurve(poles, knots, mults, 1);
}

//-----------------------------------------------------------------------------

Handle(Geom2d_BSplineCurve) asiAlgo_Utils::PolylineAsSpline(const std::vector<gp_XY>& trace)
{
  // Initialize properties for spline trajectories
  TColgp_Array1OfPnt2d poles( 1, (int) trace.size() );
  //
  for ( int k = 0; k < trace.size(); ++k )
  {
    poles(k + 1) = gp_Pnt2d( trace[k] );
  }

  const int n = poles.Upper() - 1;
  const int p = 1;
  const int m = n + p + 1;
  const int k = m + 1 - (p + 1)*2;

  // Knots
  TColStd_Array1OfReal knots(1, k + 2);
  knots(1) = 0;
  //
  for ( int j = 2; j <= k + 1; ++j )
    knots(j) = knots(j-1) + 1.0 / (k + 1);
  //
  knots(k + 2) = 1;

  // Multiplicities
  TColStd_Array1OfInteger mults(1, k + 2);
  mults(1) = 2;
  //
  for ( int j = 2; j <= k + 1; ++j )
    mults(j) = 1;
  //
  mults(k + 2) = 2;

  return new Geom2d_BSplineCurve(poles, knots, mults, 1);
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::Contains(const TopoDS_Shape& shape,
                             const TopoDS_Shape& subShape)
{
  TopTools_IndexedMapOfShape allSubShapes;
  TopExp::MapShapes(shape, allSubShapes);

  return allSubShapes.Contains(subShape);
}

//-----------------------------------------------------------------------------

TopoDS_Shape asiAlgo_Utils::GetImage(const TopoDS_Shape&       source,
                                     BRepBuilderAPI_MakeShape& API)
{
  const TopTools_ListOfShape& modified = API.Modified(source);
  //
  if ( modified.Extent() == 1 )
    return modified.First();
  //
  if ( modified.Extent() )
  {
    TopoDS_Compound C;
    BRep_Builder().MakeCompound(C);

    for ( TopTools_ListIteratorOfListOfShape lit(modified); lit.More(); lit.Next() )
    {
      const TopoDS_Shape& image = lit.Value();
      BRep_Builder().Add(C, image);
    }

    return C;
  }

  const TopTools_ListOfShape& generated = API.Generated(source);
  //
  if ( generated.Extent() == 1 )
    return generated.First();
  //
  if ( generated.Extent() )
  {
    TopoDS_Compound C;
    BRep_Builder().MakeCompound(C);

    for ( TopTools_ListIteratorOfListOfShape lit(generated); lit.More(); lit.Next() )
    {
      const TopoDS_Shape& image = lit.Value();
      BRep_Builder().Add(C, image);
    }

    return C;
  }

  // This check is placed after all other checks intentionally. The idea is
  // that for edges we may have GENERATED faces from them, while the edges
  // themselves are DELETED. Since our focus is on features (which are
  // essentially just sets of faces), we prefer returning an image face
  // rather than null shape for a deleted edge (like it happens in filleting).
  if ( API.IsDeleted(source) )
    return TopoDS_Shape();

  return source;
}

//-----------------------------------------------------------------------------

Handle(Poly_Triangulation)
  asiAlgo_Utils::CreateTriangle(const gp_Pnt& P0,
                                const gp_Pnt& P1,
                                const gp_Pnt& P2)
{
  // Create array of nodes
  TColgp_Array1OfPnt nodes(1, 3);
  nodes(1) = P0;
  nodes(2) = P1;
  nodes(3) = P2;

  // Prepare single triangle
  Poly_Array1OfTriangle tris(1, 1);
  tris(1) = Poly_Triangle(1, 2, 3);

  // Create triangulation
  return new Poly_Triangulation(nodes, tris);
}

//-----------------------------------------------------------------------------

//! Originally taken from http://www.redblobgames.com/grids/hexagons/.
void asiAlgo_Utils::HexagonPoles(const gp_XY& center,
                                 const double dist2Pole,
                                 gp_XY&       P1,
                                 gp_XY&       P2,
                                 gp_XY&       P3,
                                 gp_XY&       P4,
                                 gp_XY&       P5,
                                 gp_XY&       P6)
{
  std::vector<gp_XY*> res = {&P1, &P2, &P3, &P4, &P5, &P6};

  std::vector<gp_XY> poles;
  PolygonPoles(center, dist2Pole, 6, poles);
  //
  for ( int k = 0; k < 6; ++k )
    *res[k] = poles[k];
}

//-----------------------------------------------------------------------------

void asiAlgo_Utils::PolygonPoles(const gp_XY&        center,
                                 const double        dist2Pole,
                                 const int           numPoles,
                                 std::vector<gp_XY>& poles)
{
  const double angStep  = 360.0/numPoles;
  const double angDelta = angStep/2;

  for ( int i = 0; i < numPoles; ++i )
  {
    const double angle_deg = angStep*i + angDelta;
    const double angle_rad = angle_deg / 180.0 * M_PI;

    const double x = center.X() + dist2Pole * Cos(angle_rad);
    const double y = center.X() + dist2Pole * Sin(angle_rad);

    poles.push_back( gp_XY(x, y) );
  }
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::CalculateFaceNormals(const TopoDS_Face&                face,
                                         Handle(asiAlgo_BaseCloud<float>)& points,
                                         Handle(asiAlgo_BaseCloud<float>)& vectors)
{
  // Take surface
  Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
  //
  if ( surf.IsNull() )
    return false;

  // Prepare a point cloud as a result
  points  = new asiAlgo_BaseCloud<float>;
  vectors = new asiAlgo_BaseCloud<float>;

  // Take face domain
  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);
  //
  const double uStep = (uMax - uMin)*0.05;
  const double vStep = (vMax - vMin)*0.05;

  // Prepare classifier
  asiAlgo_ClassifyPointFace classifier(face, BRep_Tool::Tolerance(face), 0.01);

  // Sample points
  double u = uMin;
  bool uStop = false;
  while ( !uStop )
  {
    if ( u > uMax )
    {
      u     = uMax;
      uStop = true;
    }

    double v = vMin;
    bool vStop = false;
    while ( !vStop )
    {
      if ( v > vMax )
      {
        v     = vMax;
        vStop = true;
      }

      // Perform point membership classification
      asiAlgo_Membership pmc = classifier( gp_Pnt2d(u, v) );
      //
      if ( pmc & Membership_InOn )
      {
        gp_Pnt P;
        gp_Vec D1U, D1V;
        surf->D1(u, v, P, D1U, D1V);
        gp_Vec N = (D1U^D1V).Normalized();
        //
        points->AddElement( (float) P.X(), (float) P.Y(), (float) P.Z() );
        vectors->AddElement( (float) N.X(), (float) N.Y(), (float) N.Z() );
      }

      v += vStep;
    }

    u += uStep;
  }
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::GetFaceAnyInteriorPoint(const TopoDS_Face& face,
                                            gp_Pnt2d&          uv,
                                            gp_Pnt&            xyz)
{
  // Take surface
  Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
  //
  if ( surf.IsNull() )
    return false;

  // Take face domain
  double uMin, uMax, vMin, vMax;
  BRepTools::UVBounds(face, uMin, uMax, vMin, vMax);
  //
  const double uStep = (uMax - uMin)*0.05;
  const double vStep = (vMax - vMin)*0.05;

  // Prepare classifier
  asiAlgo_ClassifyPointFace classifier(face, BRep_Tool::Tolerance(face), 0.01);

  // Sample points
  double u = uMin;
  bool uStop = false;
  while ( !uStop )
  {
    if ( u > uMax )
    {
      u     = uMax;
      uStop = true;
    }

    double v = vMin;
    bool vStop = false;
    while ( !vStop )
    {
      if ( v > vMax )
      {
        v     = vMax;
        vStop = true;
      }

      // Perform point membership classification
      asiAlgo_Membership pmc = classifier( gp_Pnt2d(u, v) );
      //
      if ( pmc & Membership_In )
      {
        gp_Pnt P;
        gp_Vec D1U, D1V;
        surf->D0(u, v, P);

        uv.SetCoord( u, v );
        xyz.SetCoord( P.X(), P.Y(), P.Z() );

        return true;
      }

      v += vStep;
    }

    u += uStep;
  }
  return false;
}

//-----------------------------------------------------------------------------

void asiAlgo_Utils::PrintSurfaceDetails(const Handle(Geom_Surface)& surf,
                                        Standard_OStream&           out)
{
  // Print common header.
  out << "Surface type: " << surf->DynamicType()->Name() << "\n";

  // Rectangular Trimmed Surface.
  if ( surf->IsInstance( STANDARD_TYPE(Geom_RectangularTrimmedSurface) ) )
  {
    Handle(Geom_RectangularTrimmedSurface)
      RS = Handle(Geom_RectangularTrimmedSurface)::DownCast(surf);

    // Let's check basis surface.
    Handle(Geom_Surface) BS = RS->BasisSurface();
    //
    out << "Basis surface: ";
    out << ( BS.IsNull() ? "NONE" : BS->DynamicType()->Name() ) << "\n";
  }

  // B-surface
  else if ( surf->IsKind( STANDARD_TYPE(Geom_BSplineSurface) ) )
  {
    // Downcast.
    Handle(Geom_BSplineSurface)
      bsurf = Handle(Geom_BSplineSurface)::DownCast(surf);

    out << "U degree: "   << bsurf->UDegree()                          << "\n";
    out << "V degree: "   << bsurf->VDegree()                          << "\n";
    out << "Continuity: " << ContinuityToString( bsurf->Continuity() ) << "\n";

    if ( bsurf->IsUPeriodic() )
      out << "U-periodic\n";
    else
      out << "Not U-periodic\n";

    if ( bsurf->IsVPeriodic() )
      out << "V-periodic\n";
    else
      out << "Not V-periodic\n";

    // Get number of control points.
    const int numPoles = bsurf->Poles().ColLength() * bsurf->Poles().RowLength();
    //
    out << "Num. of poles: " << numPoles << "\n";
  }

  // Cylindrical surface.
  else if ( surf->IsKind( STANDARD_TYPE(Geom_CylindricalSurface) ) )
  {
    Handle(Geom_CylindricalSurface)
      CS = Handle(Geom_CylindricalSurface)::DownCast(surf);

    const double r = CS->Radius();
    //
    out << "Radius: " << r << "\n";
  }

  // Spherical surface.
  else if ( surf->IsKind( STANDARD_TYPE(Geom_SphericalSurface) ) )
  {
    Handle(Geom_SphericalSurface)
      SS = Handle(Geom_SphericalSurface)::DownCast(surf);

    const double r = SS->Radius();
    //
    out << "Radius: " << r << "\n";
  }

  // Toroidal surface.
  else if ( surf->IsKind( STANDARD_TYPE(Geom_ToroidalSurface) ) )
  {
    Handle(Geom_ToroidalSurface)
      TS = Handle(Geom_ToroidalSurface)::DownCast(surf);

    const double minr = TS->MinorRadius();
    const double majr = TS->MajorRadius();
    //
    out << "Minor radius: " << minr << "\n";
    out << "Major radius: " << majr << "\n";
  }
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::CalculateCurvatureComb(const Handle(Geom_Curve)& curve,
                                           const double              u,
                                           const double              curvAmpl,
                                           gp_Pnt&                   p,
                                           double&                   k,
                                           gp_Vec&                   c)
{
  p = curve->Value(u);

  GeomLProp_CLProps lProps( curve, u, 2, gp::Resolution() );
  //
  if ( !lProps.IsTangentDefined() )
    return false;

  // Calculate the first derivative.
  const gp_Vec& x_1 = lProps.D1();
  //
  if ( x_1.Magnitude() < 1.0e-7 )
    return false;

  // Calculate the second derivative.
  const gp_Vec& x_2 = lProps.D2();
  //
  if ( x_2.Magnitude() < 1.0e-7 )
    return false;

  // Calculate binormal.
  gp_Dir b = x_1 ^ x_2;

  // Calculate normal.
  gp_Dir n = b ^ x_1;

  // Calculate curvature.
  k = lProps.Curvature();

  // Calculate comb.
  c = p.XYZ() - curvAmpl*k*n.XYZ();

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::CalculateCurvatureCombs(const Handle(Geom_Curve)& curve,
                                            const double              f,
                                            const double              l,
                                            const int                 numPts,
                                            const double              curvAmpl,
                                            std::vector<gp_Pnt>&      points,
                                            std::vector<double>&      params,
                                            std::vector<double>&      curvatures,
                                            std::vector<gp_Vec>&      combs,
                                            std::vector<bool>&        combsOk)
{
  // Discretize with a uniform curvilinear step.
  GeomAdaptor_Curve gac(curve, f, l);
  GCPnts_QuasiUniformAbscissa Defl(gac, numPts);
  //
  if ( !Defl.IsDone() )
    return false;

  // Calculate combs at discretization points.
  for ( int i = 1; i <= numPts; ++i )
  {
    const double param = Defl.Parameter(i);

    gp_Pnt p;
    double curvature;
    gp_Vec comb;
    //
    const bool
      isOk = asiAlgo_Utils::CalculateCurvatureComb(curve,
                                                   param,
                                                   curvAmpl,
                                                   p,
                                                   curvature,
                                                   comb);

    // Fill result arrays.
    combsOk    .push_back( isOk );
    points     .push_back( p );
    params     .push_back( param );
    curvatures .push_back( curvature );
    combs      .push_back( comb.XYZ() );
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::ReparametrizeBSpl(const Handle(Geom2d_Curve)&  curve,
                                      const double                 newFirst,
                                      const double                 newLast,
                                      const bool                   toCopy,
                                      Handle(Geom2d_BSplineCurve)& result)
{
  Handle(Geom2d_BSplineCurve)
    curveBSpl = Handle(Geom2d_BSplineCurve)::DownCast(curve);
  //
  if ( curveBSpl.IsNull() )
    return false;

  // Prepare a copy if necessary.
  if ( toCopy )
    result = Handle(Geom2d_BSplineCurve)::DownCast( curveBSpl->Copy() );
  else
    result = curveBSpl;

  // Reparameterize.
  const int numKnots = result->NbKnots();
  //
  TColStd_Array1OfReal knots(1, numKnots);
  result->Knots(knots);
  BSplCLib::Reparametrize(newFirst, newLast, knots);
  result->SetKnots(knots);
  //
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::EvaluateAlongCurvature(const TopoDS_Face& face,
                                           const TopoDS_Edge& edge,
                                           const double       t,
                                           double&            k)
{
  // Get host geometries
  double f, l;
  Handle(Geom2d_Curve) c2d = BRep_Tool::CurveOnSurface(edge, face, f, l);
  BRepAdaptor_Surface surf(face);

  // Evaluate curve
  gp_Pnt2d UV;
  gp_Vec2d T;
  c2d->D1(t, UV, T);

  // Calculate differential properties
  BRepLProp_SLProps Props(surf, UV.X(), UV.Y(), 2, 1e-7);
  //
  if ( !Props.IsCurvatureDefined() )
  {
#if defined COUT_DEBUG
    std::cout << "Error: curvature is not defined" << std::endl;
#endif
    return false;
  }

  // Get differential properties
  const gp_Vec Xu  = Props.D1U();
  const gp_Vec Xv  = Props.D1V();
  const gp_Vec Xuu = Props.D2U();
  const gp_Vec Xuv = Props.DUV();
  const gp_Vec Xvv = Props.D2V();
  const gp_Vec n   = Props.Normal();

  // Coefficients of the FFF
  const double E = Xu.Dot(Xu);
  const double F = Xu.Dot(Xv);
  const double G = Xv.Dot(Xv);

  // Coefficients of the SFF
  const double L = n.Dot(Xuu);
  const double M = n.Dot(Xuv);
  const double N = n.Dot(Xvv);

  // Calculate curvature using the coefficients of both fundamental forms
  if ( Abs( T.X() ) < 1.0e-5 )
    k = N / G;
  else
  {
    const double lambda = T.Y() / T.X();
    k = (L + 2*M*lambda + N*lambda*lambda) / (E + 2*F*lambda + G*lambda*lambda);
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::EvaluateAlongCurvature(const TopoDS_Face& face,
                                           const TopoDS_Edge& edge,
                                           double&            k)
{
  double f, l;
  BRep_Tool::Range(edge, f, l);
  const double t = (f + l)*0.5;

  return EvaluateAlongCurvature(face, edge, t, k);
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::CalculateMidCurvature(const Handle(Geom_Curve)& curve,
                                          gp_Pnt&                   P,
                                          gp_Dir&                   T,
                                          double&                   k,
                                          double&                   r,
                                          gp_Pnt&                   center)
{
  const double uMid = ( curve->FirstParameter() + curve->LastParameter() )*0.5;

  GeomLProp_CLProps lProps( curve, uMid, 2, gp::Resolution() );
  //
  if ( !lProps.IsTangentDefined() )
    return false;

  // Calculate point and tangent.
  P = lProps.Value();
  //
  lProps.Tangent(T);

  // Calculate curvature.
  k = lProps.Curvature();
  //
  if ( Abs(k) < 1.e-5 )
    r = Precision::Infinite();
  else
  {
    r = Abs(1.0 / k);
    //
    lProps.CentreOfCurvature(center);
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::CalculateMidCurvature(const Handle(Geom_Curve)& curve,
                                          double&                   k,
                                          double&                   r,
                                          gp_Pnt&                   center)
{
  gp_Pnt P;
  gp_Dir T;
  return CalculateMidCurvature(curve, P, T, k, r, center);
}

//-----------------------------------------------------------------------------

bool asiAlgo_Utils::CalculateMidCurvature(const Handle(Geom_Curve)& curve,
                                          double&                   k,
                                          double&                   r)
{
  gp_Pnt center;
  return CalculateMidCurvature(curve, k, r, center);
}
