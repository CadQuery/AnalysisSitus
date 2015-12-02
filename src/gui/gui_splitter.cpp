//-----------------------------------------------------------------------------
// Created on: 27 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

// Own include
#include <gui_splitter.h>

// Qt includes
#pragma warning(push, 0)
#include <QPlastiqueStyle>
#pragma warning(pop)

//! Constructor.
//! \param theOrientation [in] the orientation.
//! \param theParent      [in] the parent.
gui_splitter::gui_splitter(Qt::Orientation theOrientation,
                           QWidget*        theParent)
: QSplitter(theOrientation, theParent)
{
  setStyle( new QPlastiqueStyle() );
}

//! Constructor.
//! \param theParent [in] the parent.
gui_splitter::gui_splitter(QWidget* theParent) : QSplitter(theParent)
{
  setStyle( new QPlastiqueStyle() );
}
