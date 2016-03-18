//-----------------------------------------------------------------------------
// Created on: 26 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_aag_h
#define geom_aag_h

// A-Situs includes
#include <analysis_situs.h>

// STL includes
#include <vector>

// VTK includes
#include <vtkMutableUndirectedGraph.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <TopoDS_Face.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

class geom_aag_random_iterator;

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_aag, Standard_Transient)

//! AAG.
class geom_aag : public Standard_Transient
{
friend class geom_aag_random_iterator;

public:

  typedef NCollection_DataMap<int, TColStd_PackedMapOfInteger> t_adjacency;

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_aag, Standard_Transient)

public:

  ASitus_EXPORT
    geom_aag(const TopoDS_Shape&         masterCAD,
             const TopTools_ListOfShape& selectedFaces);

  ASitus_EXPORT
    geom_aag(const TopoDS_Shape& masterCAD);

  ASitus_EXPORT
    ~geom_aag();

public:

  ASitus_EXPORT
    vtkSmartPointer<vtkMutableUndirectedGraph> ToVTK() const;

  ASitus_EXPORT const TopoDS_Face&
    GetFace(const int face_idx) const;

  ASitus_EXPORT const TColStd_PackedMapOfInteger&
    GetNeighbors(const int face_idx) const;

protected:

  void init(const TopoDS_Shape&         masterCAD,
            const TopTools_ListOfShape& selectedFaces);

  void addMates(const TopTools_ListOfShape& mateFaces);

protected:

  TColStd_PackedMapOfInteger m_selected;
  TopTools_IndexedMapOfShape m_faces;
  t_adjacency                m_neighbors;

};

#endif
