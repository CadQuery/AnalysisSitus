//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_split_faces_h
#define geom_split_faces_h

// Analysis Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_epsilon_base.h>
#include <geom_split_shape.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

//! Interruption callback.
typedef bool (*geom_split_faces_interrupt)();


//! This tool splits faces with prepared boundaries.
class geom_split_faces : public geom_split_shape
{
public:

  ASitus_EXPORT static TopoDS_Shape
    ExtractImmersedContour(const TopoDS_Shape&                      Body,
                           const Handle(TopTools_HSequenceOfShape)& Slicers,
                           const double                             inaccuracy);

public:

  ASitus_EXPORT
    geom_split_faces(const double inaccuracy);

public:

  ASitus_EXPORT bool
    operator()(const TSlicerMap&      Slicers,
               const TopoDS_Compound& PreparedFaces,
               const TopoDS_Shape&    OriginalBody,
               TopoDS_Shape&          ImmersedBody,
               TopoDS_Shape&          ImmersedContour,
               ActAPI_ProgressEntry   Journal) const;

public:

  ASitus_EXPORT bool
    Split(TopoDS_Face&                             F,
          const Handle(TopTools_HSequenceOfShape)& edges,
          TopoDS_Shell&                            ImmersedBody,
          Handle(TopTools_HSequenceOfShape)&       ImmersedEdges,
          bool&                                    isNested,
          ActAPI_ProgressEntry                     Journal,
          geom_split_faces_interrupt               pInterrupt = NULL) const;

  ASitus_EXPORT bool
    WalkLeft(const TopoDS_Face&         F,
             const TopoDS_Shape&        Mosaic,
             TopTools_ListOfShape&      Contours,
             ActAPI_ProgressEntry       Journal,
             geom_split_faces_interrupt pInterrupt = NULL) const;

  ASitus_EXPORT bool
    WalkLeft(const TopoDS_Face&                               F,
             const TopoDS_Shape&                              Mosaic,
             const TopTools_IndexedDataMapOfShapeListOfShape& Seeds,
             TopTools_ListOfShape&                            Contours,
             ActAPI_ProgressEntry                             Journal,
             geom_split_faces_interrupt                       pInterrupt = NULL) const;

  ASitus_EXPORT bool
    WalkLeft(const TopoDS_Face&                               F,
             const TopTools_IndexedDataMapOfShapeListOfShape& All,
             const TopTools_IndexedDataMapOfShapeListOfShape& Seeds,
             const bool                                       checkDeadEnds,
             TopTools_ListOfShape&                            Results,
             ActAPI_ProgressEntry                             Journal,
             geom_split_faces_interrupt                       pInterrupt = NULL) const;

protected:

  //! Traversal opportunity (vertex plus edge).
  struct t_opportunity
  {
    TopoDS_Vertex V;
    TopoDS_Edge   E;

    //! Default constructor.
    t_opportunity() {}

    //! Complete constructor.
    //! \param _V [in] vertex.
    //! \param _E [in] edge.
    t_opportunity(const TopoDS_Vertex _V, const TopoDS_Edge _E) : V(_V), E(_E) {}

    //! Constructor with vertex.
    //! \param _V [in] vertex.
    t_opportunity(const TopoDS_Vertex _V) : V(_V) {}

    //! Checks whether two opportunities are the same.
    //! \param other [in] opportunity to compare with.
    //! \return true/false.
    bool isSame(const t_opportunity& other) const
    {
      return V.IsSame(other.V) && E.IsSame(other.E);
    }
  };

  //! Collection of available opportunities. This is nothing but a convenient
  //! manager class.
  struct t_opportunities
  {
    NCollection_Sequence<t_opportunity> list;

    //! Default constructor.
    t_opportunities() {}

    //! Constructor accepting the list of opportunities to manage.
    //! \param source [in] opportunities to set.
    t_opportunities(const NCollection_Sequence<t_opportunity>& source)
    {
      this->list = source;
    }

    //! Returns difference between this and others.
    //! \param others [in] opportunities to subtract.
    //! \return remaining opportunities.
    t_opportunities diff(const t_opportunities& others) const
    {
      t_opportunities result(this->list);
      for ( int i = 1; i <= others.size(); ++i )
        result.kill( others.take(i) );

      return result;
    }

    //! Adds one more opportunity to the list. Uniqueness is maintained
    //! automatically.
    //! \param one_more [in] opportunity to add.
    //! \return true if the opportunity was added, false -- otherwise.
    bool add(const t_opportunity& one_more)
    {
      bool isFound = false;
      for ( int i = 1; i <= this->list.Length(); ++i )
      {
        if ( one_more.isSame( this->list(i) ) )
        {
          isFound = true;
          break;
        }
      }
      if ( !isFound )
        list.Append(one_more);

      return !isFound;
    }

    //! Adds all opportunities from the given list to this one.
    //! \param others [in] opportunities to add.
    void add(const t_opportunities& others)
    {
      for ( int i = 1; i <= others.size(); ++i )
        this->add( others.take(i) );
    }

    //! Removes the given opportunity from the list.
    //! \param which [in] opportunity to remove.
    //! \return true if the opportunity has been successfully removed,
    //!         false -- otherwise (e.g. it did not exist).
    bool kill(const t_opportunity& which)
    {
      bool isFound = false;
      NCollection_Sequence<t_opportunity> new_list;
      for ( int i = 1; i <= this->list.Length(); ++i )
      {
        if ( which.isSame( this->list(i) ) )
          isFound = true;
        else
          new_list.Append( this->list(i) );
      }

      if ( isFound )
        this->list = new_list;

      return isFound;
    }

    //! \return true if there are any opportunities in the list.
    bool any() const
    {
      return !this->list.IsEmpty();
    }

    //! \return number of opportunities.
    int size() const
    {
      return this->list.Length();
    }

    //! Returns item by its 1-based index.
    //! \param idx [in] 1-based index.
    //! \return opportunity.
    const t_opportunity& take(const int idx) const
    {
      return this->list(idx);
    }

    //! Clears the list.
    void clear()
    {
      this->list.Clear();
    }

  };

  //! Node is a vertex or a fictive vertex.
  struct t_node
  {
    t_opportunity Opp;
    bool          isAlien;

    t_node() : isAlien(true) {}
    t_node(const TopoDS_Vertex& _V, const bool _isAlien) : Opp(_V), isAlien(_isAlien) {}
    t_node(const TopoDS_Vertex& _V, const TopoDS_Edge& _E, const bool _isAlien) : Opp(_V, _E), isAlien(_isAlien) {}

    bool IsNull() const { return Opp.V.IsNull() > 0; }
  };

protected:

  ASitus_EXPORT bool
    nextEdge(const TopoDS_Face&                               F,
             const t_node&                                    V,
             const t_node&                                    V_prev,
             const gp_Dir&                                    prev_dir,
             const gp_Dir&                                    ref_dir,
             const TopTools_IndexedDataMapOfShapeListOfShape& M,
             const bool                                       pickNearest,
             TopoDS_Edge&                                     NextEdge,
             t_node&                                          NextVertex) const;

  ASitus_EXPORT bool
    nextSeed(const TopoDS_Face&                               F,
             const TopTools_IndexedDataMapOfShapeListOfShape& M,
             const TopTools_IndexedMapOfShape&                TraversedVertices,
             const TopTools_IndexedMapOfShape&                TraversedEdges,
             t_node&                                          Node,
             Handle(geom_epsilon_base)&                       eps,
             ActAPI_ProgressEntry                             Journal) const;

  ASitus_EXPORT bool
    isClosedContour(const TopoDS_Shape& Contour) const;

  ASitus_EXPORT void
    sew(TopoDS_Shape& Body) const;

  ASitus_EXPORT bool
    resolveDeadEnds(TopoDS_Shape&               Mosaic,
                    TopoDS_Face&                Face,
                    const TopTools_ListOfShape& DeadEnds,
                    const double                maxGap,
                    double&                     resGap,
                    ActAPI_ProgressEntry        Journal) const;

  ASitus_EXPORT bool
    isImbrication(const TopTools_IndexedDataMapOfShapeListOfShape& M_all,
                  TopTools_IndexedDataMapOfShapeListOfShape&       M_nested) const;

  ASitus_EXPORT void
    traverseByExpansion(const TopoDS_Vertex&                             V_current,
                        const TopTools_IndexedDataMapOfShapeListOfShape& M_all,
                        TopTools_IndexedDataMapOfShapeListOfShape&       Visited) const;

};

#endif
