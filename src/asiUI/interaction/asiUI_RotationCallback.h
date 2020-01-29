//-----------------------------------------------------------------------------
// Created on: 28 November 2015
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

#ifndef asiUI_RotationCallback_h
#define asiUI_RotationCallback_h

// asiUI includes
#include <asiUI_ViewerCallback.h>

// asiVisu includes
#include <asiVisu_MeshDataProvider.h>
#include <asiVisu_MeshPipeline.h>

// VTK includes
#pragma warning(push, 0)
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkWorldPointPicker.h>
#pragma warning(pop)

// Mesh (Active Data) includes
#include <ActData_Mesh.h>

//-----------------------------------------------------------------------------
// Rotation center provider
//-----------------------------------------------------------------------------

//! Data Provider for rotation center.
class asiUI_RotationCenterProvider : public asiVisu_MeshDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiUI_RotationCenterProvider, asiVisu_MeshDataProvider)

public:

  asiUI_EXPORT
    asiUI_RotationCenterProvider();

public:

  asiUI_EXPORT void
    Init(const double x,
         const double y,
         const double z,
         const double side);

public:

  //! \return mesh data structure.
  virtual Handle(ActData_Mesh) GetMeshDS() const
  {
    return m_mesh;
  }

  //! Returns ID of the associated Node. Not used in this case.
  //! \return ID of the associated Node.
  virtual ActAPI_DataObjectId GetNodeID() const
  {
    return ActAPI_DataObjectId();
  }

  //! Not used.
  virtual bool MustExecute(const Handle(ActAux_TimeStamp)&) const
  {
    return true;
  }

protected:

  //! Returns sources for domain-VTK translation. Not used in this case.
  //! \return translation sources.
  virtual Handle(ActAPI_HParameterList) translationSources() const
  {
    return nullptr;
  }

protected:

  Handle(ActData_Mesh) m_mesh; //!< Mesh representing the cube to draw.

};

//-----------------------------------------------------------------------------
// Rotation callback
//-----------------------------------------------------------------------------

//! Callback for rotation operation. Cooperates with VTK window via VTK
//! command pattern. Manages two events: rotation start and rotation end.
class asiUI_RotationCallback : public asiUI_ViewerCallback
{
public:

  asiUI_EXPORT static asiUI_RotationCallback*
    New();

  asiUI_EXPORT static asiUI_RotationCallback*
    New(asiUI_Viewer* viewer);

  vtkTypeMacro(asiUI_RotationCallback, asiUI_ViewerCallback);

public:

  asiUI_EXPORT virtual void
    Execute(vtkObject*    pCaller,
            unsigned long eventId,
            void*         pCallData);

protected:

  asiUI_EXPORT void
    setVisualProps();

  asiUI_EXPORT double
    getScaledSize(vtkRenderer* pRenderer);

protected:

  asiUI_EXPORT
    asiUI_RotationCallback(asiUI_Viewer* pViewer);

  asiUI_EXPORT
    ~asiUI_RotationCallback();

protected:

  Handle(asiVisu_MeshPipeline)         m_pl;         //!< Pipeline for rotation center.
  Handle(asiUI_RotationCenterProvider) m_prv;        //!< Data Provider.
  bool                                 m_bIsStarted; //!< Used for optimization only.

};

#endif
