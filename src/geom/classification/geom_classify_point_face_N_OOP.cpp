//-----------------------------------------------------------------------------
// Created on: 21 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_classify_point_face_N_OOP.h>

// Geometry includes
#include <geom_epsilon_solid_interior_hemi.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>

//! Constructor.
//! \param F          [in] face to classify the point against.
//! \param inaccuracy [in] inaccuracy of the input geometry.
//! \param precision  [in] numerical precision for optimization.
geom_classify_point_face_N_OOP::geom_classify_point_face_N_OOP(const TopoDS_Face& F,
                                                               const double       inaccuracy,
                                                               const double       precision)
: geom_classify_point_face(F, inaccuracy, precision)
{}

//! Performs point-face classification.
//! \param PonS    [in]     point to classify.
//! \param N       [out]    neighborhood information.
//! \param Journal [in/out] journal instance.
//! \return classification result.
geom_membership
  geom_classify_point_face_N_OOP::operator()(const gp_Pnt2d&            PonS,
                                             Handle(geom_epsilon_base)& N,
                                             ActAPI_ProgressEntry       Journal)
{
  // Basic classification
  const geom_membership result = geom_classify_point_face::operator()(PonS, Journal);

  if ( result == Membership_In )
  {
    // We use the convention that in the solid models the face normals
    // look outside the material. Therefore the neighborhood of the point
    // lying in the interior of the face can be simply represented with the
    // normal vector of the face

    gp_Pnt P;
    gp_Vec d1U, d1V;
    BRep_Tool::Surface(m_F)->D1(PonS.X(), PonS.Y(), P, d1U, d1V);

    gp_Dir norm = d1U.Crossed(d1V);
    if ( m_F.Orientation() == TopAbs_REVERSED )
      norm.Reverse();

    N = new geom_epsilon_solid_interior_hemi(norm);
  }
  else if ( result == Membership_On )
  {
    // TODO add other neighborhood types
    Standard_ProgramError::Raise("NYI");
  }

  return result;
}
