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
  const TopoDS_Shape& master = m_aag->GetMasterCAD();

  // Prepare root
  m_result = makeShape( master.ShapeType() );

  // Rebuild topology graph recursively
  this->buildTopoGraphLevel(master, faceIds, m_result);

  return true; // Success
}

//-----------------------------------------------------------------------------

void asiAlgo_InvertFaces::buildTopoGraphLevel(const TopoDS_Shape&               root,
                                              const TColStd_PackedMapOfInteger& faces2Invert,
                                              TopoDS_Shape&                     result) const
{
  BRep_Builder BB;
  for ( TopoDS_Iterator it(root, false, false); it.More(); it.Next() )
  {
    const TopoDS_Shape& currentShape = it.Value();
    TopoDS_Shape newResult;

    if ( currentShape.ShapeType() < TopAbs_FACE )
    {
      newResult = makeShape( currentShape.ShapeType() );
      this->buildTopoGraphLevel( currentShape, faces2Invert, newResult );
    }
    else
    {
      if ( currentShape.ShapeType() == TopAbs_FACE )
      {
        // Get index of the current face
        const int fid = m_aag->GetFaceId(currentShape);

        // Reverse if requested
        newResult = ( faces2Invert.Contains(fid) ) ? currentShape.Reversed() : currentShape;
      }
      else
        newResult = currentShape;
    }
    BB.Add(result, newResult);
  }
}
