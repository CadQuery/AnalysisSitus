//-----------------------------------------------------------------------------
// Created on: 27 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_InteractorStylePick_h
#define asiVisu_InteractorStylePick_h

// asiVisu includes
#include <asiVisu_Selection.h>

// VTK includes
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>

// OCCT includes
#include <NCollection_Sequence.hxx>

//! Class representing a specific VTK Interactor Style propagating picking
//! event for a 3D viewer.
class asiVisu_InteractorStylePick : public vtkInteractorStyleTrackballCamera
{
public:

  vtkTypeMacro(asiVisu_InteractorStylePick, vtkInteractorStyleTrackballCamera);

  asiVisu_EXPORT static asiVisu_InteractorStylePick*
    New();

// Customization:
public:

  asiVisu_EXPORT void
    SetRenderer(const vtkSmartPointer<vtkRenderer>& theRenderer);

  asiVisu_EXPORT const vtkSmartPointer<vtkRenderer>&
    GetRenderer() const;

  asiVisu_EXPORT unsigned long
    AddRotationCallback(unsigned long theEventID, vtkCommand* theCallback);

  asiVisu_EXPORT bool
    RemoveRotationCallback(unsigned long theEventID, unsigned long theTag);

// Overriding:
public:

  asiVisu_EXPORT virtual void
    OnMouseMove();

  asiVisu_EXPORT virtual void
    OnLeftButtonDown();

  asiVisu_EXPORT virtual void
    OnLeftButtonUp();

  asiVisu_EXPORT virtual void
    EndRotate();

  asiVisu_EXPORT virtual void
    OnKeyPress();

public:

  void GetPickedPos(double& x, double& y) const
  {
    x = m_PickedPos[0];
    y = m_PickedPos[1];
  }

private:

  //! Copying prohibited.
  asiVisu_EXPORT asiVisu_InteractorStylePick(const asiVisu_InteractorStylePick&);

  //! Assignment prohibited.
  asiVisu_EXPORT void operator=(const asiVisu_InteractorStylePick&);

private:

  asiVisu_EXPORT asiVisu_InteractorStylePick();
  asiVisu_EXPORT ~asiVisu_InteractorStylePick();

private:

  //! Instance of active renderer for internal usage.
  vtkSmartPointer<vtkRenderer> m_renderer;

  //! Pick input data.
  asiVisu_PickInput* m_pPickInput;

  //! Indicates whether left mouse button is currently pressed.
  bool m_bIsLeftButtonDown;

  //! Indicates whether rotation is in progress.
  bool m_bIsRotation;

  //! Custom callbacks called on camera rotation event.
  NCollection_Sequence<unsigned long> m_rotationCallbackIds;

  //! Currently picked display position.
  int m_PickedPos[2];

};

#endif
