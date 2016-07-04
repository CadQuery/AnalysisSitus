//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <visu_re_contours_data_provider.h>

// Common includes
#include <common_facilities.h>

//-----------------------------------------------------------------------------

//! Constructor.
visu_re_contours_data_provider::visu_re_contours_data_provider()
: visu_data_provider()
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId visu_re_contours_data_provider::GetNodeID() const
{
  Handle(geom_re_contours_node)
    contours_n = common_facilities::Instance()->Model->RENode()->Contours();
  //
  if ( contours_n.IsNull() || !contours_n->IsWellFormed() )
    return ActAPI_DataObjectId();

  return contours_n->GetId();
}

//-----------------------------------------------------------------------------

//! \return number of stored contours.
int visu_re_contours_data_provider::GetNumOfContours() const
{
  Handle(geom_re_contours_node)
    contours_n = common_facilities::Instance()->Model->RENode()->Contours();
  //
  if ( contours_n.IsNull() || !contours_n->IsWellFormed() )
    return 0;

  // Iterate over the child Nodes which are the individual contours
  int nbContours = 0;
  for ( Handle(ActAPI_IChildIterator) cit = contours_n->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(geom_re_contour_node) contour_n = Handle(geom_re_contour_node)::DownCast( cit->Value() );
    //
    if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
      continue;

    ++nbContours;
  }

  return nbContours;
}

//! Returns a contour by its one-based index.
//! \param oneBased_idx [in] 1-based index of the contour to access.
TopoDS_Wire
  visu_re_contours_data_provider::GetContour(const int oneBased_idx) const
{
  Handle(geom_re_contours_node)
    contours_n = common_facilities::Instance()->Model->RENode()->Contours();
  //
  if ( contours_n.IsNull() || !contours_n->IsWellFormed() )
    return TopoDS_Wire();

  // Access individual contour
  Handle(geom_re_contour_node) contour_n = contours_n->Contour(oneBased_idx);
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
    return TopoDS_Wire();

  return contour_n->GetContour();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) visu_re_contours_data_provider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(geom_re_contours_node)
    contours_n = common_facilities::Instance()->Model->RENode()->Contours();
  //
  if ( contours_n.IsNull() || !contours_n->IsWellFormed() )
    return out;

  // Iterate over the child Nodes which are the individual contours
  for ( Handle(ActAPI_IChildIterator) cit = contours_n->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(geom_re_contour_node) contour_n = Handle(geom_re_contour_node)::DownCast( cit->Value() );
    //
    if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
      continue;

    out << contour_n->Parameter(geom_re_contour_node::PID_Contour);
  }

  return out;
}
