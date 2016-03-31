//-----------------------------------------------------------------------------
// Created on: 10 March 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef geom_feature_pattern_h
#define geom_feature_pattern_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_aag_iterator.h>

// OCCT includes
#include <TopTools_IndexedMapOfShape.hxx>

//-----------------------------------------------------------------------------

DEFINE_STANDARD_HANDLE(geom_feature_pattern, Standard_Transient)

//! Base class for all feature patterns.
class geom_feature_pattern : public Standard_Transient
{
public:

  DEFINE_STANDARD_RTTI_INLINE(geom_feature_pattern, Standard_Transient)

public:

  //! Creates feature pattern initializing it with the given AAG iterator.
  //! \param aag_it [in] AAG iterator.
  inline geom_feature_pattern(const Handle(geom_aag_iterator)& aag_it) : Standard_Transient()
  {
    m_it = aag_it;
  }

public:

  //! \return faces traversed during recognition.
  inline const TColStd_PackedMapOfInteger& JustTraversed() const
  {
    return m_traversed;
  }

  //! Sets faces as traversed.
  //! \param face_idx [in] index of the just traversed face.
  inline void SetTraversed(const int face_idx)
  {
    m_traversed.Add(face_idx);
  }

  //! Recognition interface.
  //! \param featureFaces [out] detected faces.
  //! \return true in case of success, false -- otherwise.
  inline bool Recognize(TopTools_IndexedMapOfShape& featureFaces)
  {
    m_traversed.Clear();
    //
    return this->recognize(featureFaces);
  }

private:

  virtual bool
    recognize(TopTools_IndexedMapOfShape& featureFaces) = 0;

protected:

  //! Checks face type.
  //! \param face [in] face to check.
  //! \return true/false.
  template<typename TSurf>
  bool isTypeOf(const TopoDS_Face& face) const
  {
    Handle(Geom_Surface) surf = BRep_Tool::Surface(face);
    if ( surf->IsInstance( STANDARD_TYPE(TSurf) ) )
      return true;

    return false;
  }

protected:

  Handle(geom_aag_iterator)  m_it;        //!< AAG iterator.
  TColStd_PackedMapOfInteger m_traversed; //!< Faces traversed during recognition.

};

#endif