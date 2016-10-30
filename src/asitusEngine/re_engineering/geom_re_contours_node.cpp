//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <geom_re_contours_node.h>

// Common includes
#include <common_facilities.h>

// Engine includes
#include <engine_re.h>

// Active Data includes
#include <ActData_ParameterFactory.h>

//-----------------------------------------------------------------------------

//! Default constructor. Registers all involved Parameters.
geom_re_contours_node::geom_re_contours_node() : ActData_BaseNode()
{
  REGISTER_PARAMETER(Name, PID_Name);
}

//! Returns new DETACHED instance of the Node ensuring its correct
//! allocation in a heap.
//! \return new instance of the Node.
Handle(ActAPI_INode) geom_re_contours_node::Instance()
{
  return new geom_re_contours_node();
}

//! Performs initial actions required to make Node WELL-FORMED.
void geom_re_contours_node::Init()
{
  // Initialize name Parameter
  this->InitParameter(PID_Name, "Name");
}

//-----------------------------------------------------------------------------
// Generic naming
//-----------------------------------------------------------------------------

//! Accessor for the Node's name.
//! \return name of the Node.
TCollection_ExtendedString geom_re_contours_node::GetName()
{
  return ActParamTool::AsName( this->Parameter(PID_Name) )->GetValue();
}

//! Sets name for the Node.
//! \param theName [in] name to set.
void geom_re_contours_node::SetName(const TCollection_ExtendedString& theName)
{
  ActParamTool::AsName( this->Parameter(PID_Name) )->SetValue(theName);
}

//-----------------------------------------------------------------------------
// Handy API
//-----------------------------------------------------------------------------

//! Accessor for a single persistent contour.
//! \param oneBased_idx [in] 1-based contour index.
//! \return Contour Node.
Handle(geom_re_contour_node) geom_re_contours_node::Contour(const int oneBased_idx)
{
  int idx = 1;
  for ( Handle(ActAPI_IChildIterator) cit = this->GetChildIterator(); cit->More(); cit->Next() )
  {
    if ( oneBased_idx == idx )
    {
      Handle(geom_re_contour_node)
        contour_n = Handle(geom_re_contour_node)::DownCast( cit->Value() );
      //
      return contour_n;
    }

    ++idx;
  }

  return NULL;
}

//! [TRANSACTION] Adds another Contour Node for the given wire.
//! \param contour [in] wire to store as a contour.
//! \return just created Contour Node.
Handle(geom_re_contour_node)
  geom_re_contours_node::TX_AddContour(const TopoDS_Wire& contour)
{
  Handle(geom_re_contour_node) contour_n;
  //
  common_facilities::Instance()->Model->OpenCommand();
  {
    contour_n = engine_re::Create_Contour(contour);
  }
  common_facilities::Instance()->Model->CommitCommand();
  //
  return contour_n;
}

//! [TRANSACTION] Deletes all underlying contours.
void geom_re_contours_node::TX_CleanContours()
{
  common_facilities::Instance()->Model->OpenCommand();
  {
    engine_re::Clean_Contours();
  }
  common_facilities::Instance()->Model->CommitCommand();
}
