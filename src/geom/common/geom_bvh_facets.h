//-----------------------------------------------------------------------------
// Created on: 21 September 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_bvh_facets_h
#define geom_bvh_facets_h

// Analysis Situs includes
#include <analysis_situs.h>

// OCCT includes
#include <TopoDS_Face.hxx>
#include <BVH_Types.hxx>
#include <BVH_PrimitiveSet.hxx>
#include <NCollection_Vector.hxx>

// STL includes
#include <vector>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_bvh_facets, Standard_Transient)

//! BVH-based accelerating structure representing CAD model's
//! facets in computations.
class geom_bvh_facets : public BVH_PrimitiveSet<double, 4>,
                        public Standard_Transient
{
public:

  //! Structure representing a single facet.
  struct t_facet
  {
    t_facet() : FaceIndex(-1) {}
    t_facet(const int fidx) : FaceIndex(fidx) {}

    BVH_Vec4d P0, P1, P2; //!< Triangle nodes.
    int       FaceIndex;  //!< Index of the host face.
  };

  //! Type of BVH builder to use.
  enum BuilderType
  {
    Builder_Binned,
    Builder_Linear
  };

public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(geom_bvh_facets, Standard_Transient)

public:

  geom_bvh_facets(const TopoDS_Shape&  model,
                  const BuilderType    builderType,
                  ActAPI_ProgressEntry progress,
                  ActAPI_PlotterEntry  plotter);

public:

  virtual int
    Size() const override;

  virtual BVH_Box<double, 4>
    Box(const int index) const override;

  virtual double
    Center(const int index,
           const int axis) const override;

  virtual void
    Swap(const int index1,
         const int index2) override;

public:

  void
    GetVertices(const int  index,
                BVH_Vec4d& vertex1,
                BVH_Vec4d& vertex2,
                BVH_Vec4d& vertex3) const;

  double
    GetBoundingDiag() const;

public:

  void
    Dump(ActAPI_PlotterEntry IV);

public:

  //! Returns a facet by its 0-based index.
  //! \param[in] index index of the facet of interest.
  //! \return requested facet.
  const t_facet& GetFacet(const int index)
  {
    return m_facets[index];
  }

protected:

  bool
    init(const TopoDS_Shape& model,
         const BuilderType   builderType);

  bool
    addFace(const TopoDS_Face& face,
            const int          face_idx);

protected:

  //! Array of facets.
  std::vector<t_facet> m_facets;

  //! Characteristic size of the model.
  double m_fBoundingDiag;

  //! Progress Entry.
  ActAPI_ProgressEntry m_progress;

  //! Imperative Plotter.
  ActAPI_PlotterEntry m_plotter;

};

#endif
