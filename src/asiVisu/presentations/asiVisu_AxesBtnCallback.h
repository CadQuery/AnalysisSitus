//-----------------------------------------------------------------------------
// Created on: 19 May 2016
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

#ifndef asiVisu_AxesBtnCallback_h
#define asiVisu_AxesBtnCallback_h

// A-Situs (visualization) includes
#include <asiVisu_Prs.h>
#include <asiVisu_PrsManager.h>
#include <asiVisu_Utils.h>

// VTK includes
#include <vtkButtonWidget.h>
#include <vtkCommand.h>

//! Callback for toggling axes.
class asiVisu_AxesBtnCallback : public vtkCommand
{
public:

  static asiVisu_AxesBtnCallback* New();
  vtkTypeMacro(asiVisu_AxesBtnCallback, vtkCommand);

  asiVisu_AxesBtnCallback() {}

public:

  //! Initializes callback with the axes actor to manage.
  //! \param axes [in] axes actor to set.
  void SetAxesActor(const vtkSmartPointer<vtkAxesActor>& axes)
  {
    m_axes = axes;
  }

  //! Initializes callback with the renderer to manage.
  //! \param renderer [in] renderer to set.
  void SetRenderer(const vtkSmartPointer<vtkRenderer>& renderer)
  {
    m_renderer = renderer;
  }

  //! Initializes callback with the owning presentation manager.
  //! \param manager [in] manager to set.
  void SetManager(const vtkSmartPointer<asiVisu_PrsManager>& manager)
  {
    m_manager = manager;
  }

public:

  virtual void Execute(vtkObject*    caller,
                       unsigned long eventId,
                       void*         callData);

protected:

  void colorizeActors();

private:

  vtkSmartPointer<vtkAxesActor>       m_axes;     //!< Axes actor.
  vtkSmartPointer<vtkRenderer>        m_renderer; //!< Renderer.
  vtkSmartPointer<asiVisu_PrsManager> m_manager;  //!< Presentation manager.

};

#endif
