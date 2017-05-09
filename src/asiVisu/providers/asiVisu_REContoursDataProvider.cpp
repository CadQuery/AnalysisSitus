//-----------------------------------------------------------------------------
// Created on: 06 April 2016
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiVisu_REContoursDataProvider.h>

// asiData includes
#include <asiData_REContourNode.h>

//-----------------------------------------------------------------------------

//! Constructor.
//! \param contours_n [in] Contours Node.
asiVisu_REContoursDataProvider::asiVisu_REContoursDataProvider(const Handle(asiData_REContoursNode)& contours_n)
: asiVisu_DataProvider(),
  m_contours(contours_n)
{}

//-----------------------------------------------------------------------------

//! \return ID of the associated Data Node.
ActAPI_DataObjectId asiVisu_REContoursDataProvider::GetNodeID() const
{
  if ( m_contours.IsNull() || !m_contours->IsWellFormed() )
    return ActAPI_DataObjectId();

  return m_contours->GetId();
}

//-----------------------------------------------------------------------------

//! \return number of stored contours.
int asiVisu_REContoursDataProvider::GetNumOfContours() const
{
  if ( m_contours.IsNull() || !m_contours->IsWellFormed() )
    return 0;

  // Iterate over the child Nodes which are the individual contours
  int nbContours = 0;
  for ( Handle(ActAPI_IChildIterator) cit = m_contours->GetChildIterator(); cit->More(); cit->Next() )
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
  if ( m_contours.IsNull() || !m_contours->IsWellFormed() )
    return TopoDS_Wire();

  // Access individual contour
  Handle(asiData_REContourNode)
    contour_n = Handle(asiData_REContourNode)::DownCast( m_contours->GetChildNode(oneBased_idx) );
  //
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

  if ( m_contours.IsNull() || !m_contours->IsWellFormed() )
    return out;

  // Iterate over the child Nodes which are the individual contours
  for ( Handle(ActAPI_IChildIterator) cit = m_contours->GetChildIterator(); cit->More(); cit->Next() )
  {
    Handle(asiData_REContourNode) contour_n = Handle(asiData_REContourNode)::DownCast( cit->Value() );
    //
    if ( contour_n.IsNull() || !contour_n->IsWellFormed() )
      continue;

    out << contour_n->Parameter(asiData_REContourNode::PID_Contour);
  }

  return out;
}
