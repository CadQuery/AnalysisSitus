//-----------------------------------------------------------------------------
// Created on: 04 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_xde_prs.h>

//! Creates a Presentation object.
//! \param M [in] XDE Data Model to create a Presentation for.
visu_xde_prs::visu_xde_prs(const Handle(xde_model)& /*M*/) : Standard_Transient()
{
  // ...
}

//! Factory method for Presentation.
//! \param M [in] XDE Data Model to create a Presentation for.
//! \return new Presentation instance.
Handle(visu_xde_prs) visu_xde_prs::Instance(const Handle(xde_model)& M)
{
  return new visu_xde_prs(M);
}
