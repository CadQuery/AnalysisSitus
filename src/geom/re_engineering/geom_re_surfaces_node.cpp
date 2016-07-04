//-----------------------------------------------------------------------------
// Created on: 05 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_re_surfaces_node.h>

// Common includes
#include <common_facilities.h>

// Engine includes
#include <engine_re.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_re_surfaces_node::geom_re_surfaces_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) geom_re_surfaces_node::Instance()
{
  return new geom_re_surfaces_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void geom_re_surfaces_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_re_surfaces_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_re_surfaces_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! Accessor for an individual persistent surface.
//! \param oneBased_idx [in] 1-based surface index.
//! \return Surface Node.
Handle(geom_re_surface_node) geom_re_surfaces_node::Surface(const int oneBased_idx)
{
  int idx = 1;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    if ( oneBased_idx == idx )
    {
      Handle(geom_re_surface_node)
        surf_n = Handle(geom_re_surface_node)::DownCast( cit->Value() );
      //
      return surf_n;
    }

    ++idx;
  }

  return NULL;
}

//! [TRANSACTION] Adds another Surface Node for the given parametric surface.
//! \param surface [in] parametric surface to store.
//! \param uLimit  [in] bounding value for U curvilinear axis.
//! \param vLimit  [in] bounding value for V curvilinear axis.
//! \return just created Surface Node.
Handle(geom_re_surface_node)
  geom_re_surfaces_node::TX_AddSurface(const Handle(Geom_Surface)& surface,
                                       const double                uLimit,
                                       const double                vLimit)
{
  Handle(geom_re_surface_node) surf_n;
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    surf_n = engine_re::Create_Surface(surface, uLimit, vLimit);
  }
  common_facilities::Instance()->Model->CommitCommand();
  //
  return surf_n;
}

//! [TRANSACTION] Deletes all underlying surfaces.
void geom_re_surfaces_node::TX_CleanSurfaces()
{
  common_facilities::Instance()->Model->OpenCommand();
  {
    engine_re::Clean_Surfaces();
  }
  common_facilities::Instance()->Model->CommitCommand();
}
