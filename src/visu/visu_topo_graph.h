//-----------------------------------------------------------------------------
// Created on: 05 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/
//-----------------------------------------------------------------------------

#ifndef visu_topo_graph_h
#define visu_topo_graph_h

// Visualization includes
#include <visu_common.h>

// Qr includes
#include <QrCore.h>

// OCCT includes
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopoDS_Shape.hxx>

// VTK includes
#include <vtkIntArray.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkStringArray.h>

//! Visualizes topology graph.
class visu_topo_graph
{
public:

  visu_topo_graph();

public:

  void Render(const TopoDS_Shape& shape);

public:

  //! Returns shape address as string with a prefix indicating its type,
  //! e.g. "e05FB0" is an edge and "f780A1" is a face.
  //! \param shape [in] shape to extract the address for.
  //! \return address of TShape as string prefixed by type indicator.
  inline static std::string ShapeAddrWithPrefix(const TopoDS_Shape& shape)
  {
    std::string addr_str = ShapeAddr( shape.TShape() );

    std::string prefix;
    const TopAbs_ShapeEnum type = shape.ShapeType();
    if ( type == TopAbs_COMPOUND )
      prefix = "COMPOUND";
    else if ( type == TopAbs_COMPSOLID )
      prefix = "COMPSOLID";
    else if ( type == TopAbs_SOLID )
      prefix = "SOLID";
    else if ( type == TopAbs_SHELL )
    {
      if ( shape.Closed() )
        prefix = "CLOSED";
      else
        prefix = "OPEN";

      prefix += " SHELL";
    }
    else if ( type == TopAbs_FACE )
      prefix = "FACE";
    else if ( type == TopAbs_WIRE )
      prefix = "WIRE";
    else if ( type == TopAbs_EDGE )
      prefix = "EDGE";
    else if ( type == TopAbs_VERTEX )
      prefix = "VERTEX";
    else
      prefix = "SHAPE";

    // Notice extra spacing for better visualization
    return "    " + prefix + " [" + addr_str + "]";
  }

  //! Returns TShape address as string.
  //! \param tshape [in] TShape pointer to extract the address for.
  //! \return address of TShape as string.
  inline static std::string ShapeAddr(const Handle(TopoDS_TShape)& tshape)
  {
    std::string addr_str;
    std::ostringstream ost;
    ost << tshape.get();
    addr_str = ost.str();

    size_t pos = 0;
    while ( addr_str[pos] == '0' )
      pos++;

    if ( pos )
      addr_str = QrCore::substr( addr_str, (int) pos, (int) (addr_str.size() - pos) );

    return addr_str;
  }

  //! Returns TShape address as string.
  //! \param shape [in] shape to extract the address for.
  //! \return address of TShape as string.
  inline static std::string ShapeAddr(const TopoDS_Shape& shape)
  {
    return ShapeAddr( shape.TShape() );
  }

protected:

  void buildRecursively(const TopoDS_Shape&             rootShape,
                        const vtkIdType                 rootId,
                        vtkMutableDirectedGraph*        pDS,
                        vtkStringArray*                 pLabelArr,
                        vtkIntArray*                    pColorArr,
                        TopTools_DataMapOfShapeInteger& shapeVertices);

};

#endif
