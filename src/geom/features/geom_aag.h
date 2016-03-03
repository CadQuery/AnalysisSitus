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

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_aag, Standard_Transient)

//! AAG.
class geom_aag : public Standard_Transient
{
public:

  typedef NCollection_DataMap<int, TColStd_PackedMapOfInteger> t_adjacency;

public:

  //! Iterator for AAG working in a random manner. This is the very simple
  //! and very straightforward iterator which traverses the entire graph
  //! in the order which you should not rely on.
  class random_iterator
  {
  public:

    //! Creates and initializes iterator for AAG.
    //! \param graph [in] graph to iterate.
    random_iterator(const Handle(geom_aag)& graph)
    {
      this->init(graph);
    }

    //! Initializes iterator with graph.
    //! \param graph [in] graph to iterate.
    inline void init(const Handle(geom_aag)& graph)
    {
      m_graph = graph;
      m_it.Initialize(m_graph->m_neighbors);
    }

    //! \return true if there are still some faces to iterate.
    inline bool more() const
    {
      return m_it.More() > 0;
    }

    //! Moves iterator to another (not adjacent) face.
    inline void next()
    {
      m_it.Next();
    }

    //! \return indices of the neighbor faces for the current face.1
    inline const TColStd_PackedMapOfInteger& neighbors() const
    {
      return m_it.Value();
    }

    //! \return ID of the current face.
    inline const int face_id() const
    {
      return m_it.Key();
    }

  private:

    Handle(geom_aag)      m_graph; //!< AAG being iterated.
    t_adjacency::Iterator m_it;    //!< Internal iterator for adjacency map.

  };

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
