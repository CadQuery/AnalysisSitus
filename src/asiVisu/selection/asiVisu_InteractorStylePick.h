//-----------------------------------------------------------------------------
// Created on: 27 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder(s) nor the
//      names of all contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#ifndef asiVisu_InteractorStylePick_h
#define asiVisu_InteractorStylePick_h

// asiVisu includes
#include <asiVisu_Selection.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>
#pragma warning(pop)

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
