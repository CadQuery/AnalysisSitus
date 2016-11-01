//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_REContoursDataProvider.h>

// Common includes
#include <common_facilities.h>

//-----------------------------------------------------------------------------

//! Constructor.
asiVisu_REContoursDataProvider::asiVisu_REContoursDataProvider()
: asiVisu_DataProvider()
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_REContoursDataProvider::GetNodeID() const
{
  Handle(asiData_REContoursNode)
    contours_n = common_facilities::Instance()->Model->GetRENode()->Contours();
  //
  if ( contours_n.IsNull() || !contours_n->IsWellFormed() )
    return ActAPI_DataObjectId();

  return contours_n->GetId();
}

//-----------------------------------------------------------------------------

//! \return number of stored contours.
int asiVisu_REContoursDataProvider::GetNumOfContours() const
{
  Handle(asiData_REContoursNode)
    contours_n = common_facilities::Instance()->Model->GetRENode()->Contours();
  //
  if ( contours_n.IsNull() || !contours_n->IsWellFormed() )
    return 0;

  // Iterate over the child Nodes which are the individual contours
  int nbContours = 0;
  for ( Handle(ActAPI_IChildIterator) cit = contours_n->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_REContourNode) contour_n = Handle(asiData_REContourNode)::DownCast( cit->Value() );
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
  asiVisu_REContoursDataProvider::GetContour(const int oneBased_idx) const
{
  Handle(asiData_REContoursNode)
    contours_n = common_facilities::Instance()->Model->GetRENode()->Contours();
  //
  if ( contours_n.IsNull() || !contours_n->IsWellFormed() )
    return TopoDS_Wire();

  // Access individual contour
  Handle(asiData_REContourNode) contour_n = contours_n->Contour(oneBased_idx);
  if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
    return TopoDS_Wire();

  return contour_n->GetContour();
}

//-----------------------------------------------------------------------------

//! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
//! translation process.
//! \return source Parameters.
Handle(ActAPI_HParameterList) asiVisu_REContoursDataProvider::translationSources() const
{
  // Resulting Parameters
  ActParamStream out;

  Handle(asiData_REContoursNode)
    contours_n = common_facilities::Instance()->Model->GetRENode()->Contours();
  //
  if ( contours_n.IsNull() || !contours_n->IsWellFormed() )
    return out;

  // Iterate over the child Nodes which are the individual contours
  for ( Handle(ActAPI_IChildIterator) cit = contours_n->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_REContourNode) contour_n = Handle(asiData_REContourNode)::DownCast( cit->Value() );
    //
    if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
      continue;

    out << contour_n->Parameter(asiData_REContourNode::PID_Contour);
  }

  return out;
}
