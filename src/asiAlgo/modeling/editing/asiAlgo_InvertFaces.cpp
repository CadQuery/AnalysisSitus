//-----------------------------------------------------------------------------
// Created on: 10 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_InvertFaces.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepTools_ReShape.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopoDS_Iterator.hxx>

//-----------------------------------------------------------------------------

static TopoDS_Shape makeShape(const TopAbs_ShapeEnum type)
{
  TopoDS_Shape result;
  BRep_Builder BB;
  switch ( type )
  {
    case TopAbs_COMPOUND:
    {
      TopoDS_Compound compound;
      BB.MakeCompound(compound);
      result = compound;
      break;
    }
    case TopAbs_COMPSOLID:
    {
      TopoDS_CompSolid compSolid;
      BB.MakeCompSolid(compSolid);
      result = compSolid;
      break;
    }
    case TopAbs_SOLID:
    {
      TopoDS_Solid solid;
      BB.MakeSolid(solid);
      result = solid;
      break;
    }
    case TopAbs_SHELL:
    {
      TopoDS_Shell shell;
      BB.MakeShell(shell);
      result = shell;
      break;
    }
    case TopAbs_FACE:
    {
      TopoDS_Face face;
      BB.MakeFace(face);
      result = face;
      break;
    }
    case TopAbs_WIRE:
    {
      TopoDS_Wire wire;
      BB.MakeWire(wire);
      result = wire;
      break;
    }
    case TopAbs_EDGE:
    {
      TopoDS_Edge edge;
      BB.MakeEdge(edge);
      result = edge;
      break;
    }
    case TopAbs_VERTEX:
    {
      TopoDS_Vertex vertex;
      BB.MakeVertex(vertex);
      result = vertex;
      break;
    }
    default: return TopoDS_Shape();
  }

  return result;
}

//-----------------------------------------------------------------------------

//! Constructor.
//! \param aag      [in] AAG instance of the part to modify.
//! \param progress [in] progress notifier.
//! \param plotter  [in] imperative plotter.
asiAlgo_InvertFaces::asiAlgo_InvertFaces(const Handle(asiAlgo_AAG)& aag,
                                         ActAPI_ProgressEntry       progress,
                                         ActAPI_PlotterEntry        plotter)
: ActAPI_IAlgorithm(progress, plotter),
  m_aag(aag)
{}

//-----------------------------------------------------------------------------

//! Performs face inversion.
//! \param faceIds [in] IDs of the faces to invert.
//! \return true in case of success, false -- otherwise.
bool asiAlgo_InvertFaces::Perform(const TColStd_PackedMapOfInteger& faceIds)
{
  Handle(BRepTools_ReShape) ReShape = new BRepTools_ReShape;
  TopoDS_Shape master = m_aag->GetMasterCAD();

  ///
  const int fid = faceIds.GetMinimalMapped();
  const TopoDS_Shape& current_face = m_aag->GetFace(fid);

  ///
  m_result = makeShape( master.ShapeType() );

  this->seek(master, current_face, m_result);

  //// Invert faces one by one
  //for ( TColStd_MapIteratorOfPackedMapOfInteger mit(faceIds); mit.More(); mit.Next() )
  //{
  //  const int fid = mit.Key();
  //  //
  //  if ( !m_aag->HasFace(fid) )
  //  {
  //    m_progress.SendLogMessage(LogWarn(Normal) << "No face with ID %1" << fid);
  //    continue;
  //  }

  //  BRep_Builder BB;
  //  BB.Remove()

  //  // Get face to invert
  //  const TopoDS_Shape& current_face = m_aag->GetFace(fid);

  //  // Copy face
  //  //BRepBuilderAPI_Copy copier(current_face, false);
  //  //const TopoDS_Shape& copy = copier.Shape();

  //  ReShape->Replace( current_face, current_face.Reversed() );
  //}

  //m_result = ReShape->Apply(master);

  return true; // Success
}

//-----------------------------------------------------------------------------

void asiAlgo_InvertFaces::seek(const TopoDS_Shape& root,
                               const TopoDS_Shape& face2Invert,
                               TopoDS_Shape&       result) const
{
  BRep_Builder BB;
  for ( TopoDS_Iterator it(root, false, false); it.More(); it.Next() )
  {
    const TopoDS_Shape& currentShape = it.Value();
    TopoDS_Shape newResult;

    if ( currentShape.ShapeType() < TopAbs_FACE )
      newResult = makeShape( currentShape.ShapeType() );
    else
    {
      if ( currentShape.ShapeType() == TopAbs_FACE && currentShape == face2Invert )
        newResult = currentShape.Reversed();
      else
        newResult = currentShape;
    }
    BB.Add(result, newResult);

    this->seek( currentShape, face2Invert, newResult );
  }
}
