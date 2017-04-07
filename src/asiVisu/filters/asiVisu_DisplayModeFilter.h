//-----------------------------------------------------------------------------
// Created on: 06 April 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

#ifndef asiVisu_DisplayModeFilter_h
#define asiVisu_DisplayModeFilter_h

// asiVisu includes
#include <asiVisu_DisplayMode.h>

// VTK includes
#include <vtkPolyDataAlgorithm.h>

//! Filters shape polygonal data extracting only those cells which correspond
//! to the desired type of shape visualization primitive. The cells remaining
//! after such extraction compose a specific display mode. There is a predefined
//! set of display modes, e.g. shaded, wireframe, vertices, etc.
class asiVisu_DisplayModeFilter : public vtkPolyDataAlgorithm
{
public:

  vtkTypeMacro(asiVisu_DisplayModeFilter, vtkPolyDataAlgorithm)

  asiVisu_EXPORT static asiVisu_DisplayModeFilter*
    New();

public:

  //! Sets display mode.
  //! \param mode [in] display mode to set.
  asiVisu_EXPORT void
    SetDisplayMode(const asiVisu_DisplayMode mode);

  //! \return display mode.
  asiVisu_EXPORT asiVisu_DisplayMode
    GetDisplayMode() const;

protected:

  asiVisu_EXPORT virtual int
    RequestData(vtkInformation*        pInfo,
                vtkInformationVector** pInputVector,
                vtkInformationVector*  pOutputVector);

protected:

  asiVisu_EXPORT
    asiVisu_DisplayModeFilter();

protected:

  asiVisu_DisplayMode m_displayMode; //!< Display mode.

};

#endif

