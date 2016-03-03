//-----------------------------------------------------------------------------
// Created on: 03 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef common_cad_properties_h
#define common_cad_properties_h

// Qt includes
#include <QString>

//! CAD file properties.
struct common_cad_properties
{
  //! Default constructor.
  common_cad_properties()
  : NbNodes (0),
    NbElements (0) {}

  //! Clear values.
  void Clear()
  {
    Author.clear();
    OriginatingSystem.clear();
    Organization.clear();
    PreProcessor.clear();
    Comments.clear();
    NbNodes    = 0;
    NbElements = 0;
  }

  QString Author;            //!< Author.
  QString OriginatingSystem; //!< Originating system.
  QString Organization;      //!< Organization.
  QString PreProcessor;      //!< Pre-processor.
  QString Comments;          //!< Comment.
  int     NbNodes;           //!< NbNodes.
  int     NbElements;        //!< NbElements.

};

#endif // common_cad_properties_h
