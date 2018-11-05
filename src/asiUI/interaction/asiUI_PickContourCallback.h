//-----------------------------------------------------------------------------
// Created on: 21 November 2017
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
//    * Neither the name of Sergey Slyadnev nor the
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

#ifndef asiUI_PickContourCallback_h
#define asiUI_PickContourCallback_h

// asiUI includes
#include <asiUI_ObjectBrowser.h>
#include <asiUI_ViewerCallback.h>

// asiEngine includes
#include <asiEngine_Model.h>

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

// Qt includes
#include <QObject>

//! Callback for interactive contour creation.
class asiUI_PickContourCallback : public QObject,
                                  public asiUI_ViewerCallback
{
  Q_OBJECT

public:

  asiUI_EXPORT static asiUI_PickContourCallback* New();
  //
  vtkTypeMacro(asiUI_PickContourCallback, asiUI_ViewerCallback);

public:

  virtual void Execute(vtkObject*    pCaller,
                       unsigned long eventId,
                       void*         pCallData);

public:

  //! Sets accelerating structure to use for picking.
  //! \param bvh_facets [in] accelerating structure to set.
  void SetBVH(const Handle(asiAlgo_BVHFacets)& bvh_facets)
  {
    m_bvh = bvh_facets;
  }

  //! Sets Data Model instance to access the geometry to pick.
  //! \param model [in] Data Model instance.
  void SetModel(const Handle(asiEngine_Model)& model)
  {
    m_model = model;
  }

  //! Sets Object Browser instance to update when creating new objects.
  //! \param[in] pBrowser pointer to the Object Browser to set.
  void SetObjectBrowser(asiUI_ObjectBrowser* pBrowser)
  {
    m_pBrowser = pBrowser;
  }

  //! \return pointer to the Object Browser.
  asiUI_ObjectBrowser* GetObjectBrowser() const
  {
    return m_pBrowser;
  }

private:

  asiUI_PickContourCallback  (asiUI_Viewer* pViewer);
  ~asiUI_PickContourCallback ();

private:

  Handle(asiAlgo_BVHFacets)   m_bvh;      //!< Accelerating structure for picking.
  Handle(asiEngine_Model)     m_model;    //!< Data Model instance.
  Handle(asiData_ContourNode) m_contour;  //!< Contour Node being currently composed.
  Handle(asiData_RePatchNode) m_patch;    //!< Current patch.
  asiUI_ObjectBrowser*        m_pBrowser; //!< Object Browser.

};

#endif
