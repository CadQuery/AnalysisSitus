//-----------------------------------------------------------------------------
// Created on: 22 September 2016
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

#ifndef asiUI_PickFacetCallback_h
#define asiUI_PickFacetCallback_h

// asiUI includes
#include <asiUI_ViewerCallback.h>

// asiAlgo includes
#include <asiAlgo_BVHFacets.h>

// asiEngine includes
#include <asiEngine_Model.h>

// VTK includes
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

//! Callback for picking a facet in 3D viewer.
class asiUI_PickFacetCallback : public asiUI_ViewerCallback
{
public:

  asiUI_EXPORT static asiUI_PickFacetCallback*
    New();

  vtkTypeMacro(asiUI_PickFacetCallback, asiUI_ViewerCallback);

public:

  asiUI_EXPORT virtual void
    Execute(vtkObject*    pCaller,
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

private:

  asiUI_EXPORT
    asiUI_PickFacetCallback(const Handle(asiAlgo_BVHFacets)& bvh_facets,
                            asiUI_Viewer*                    viewer);

  asiUI_EXPORT
    ~asiUI_PickFacetCallback();

private:

  Handle(asiAlgo_BVHFacets) m_bvh;      //!< Accelerating structure for picking.
  Handle(asiEngine_Model)   m_model;    //!< Data Model instance.
  ActAPI_ProgressEntry      m_notifier; //!< Progress notifier.
  ActAPI_PlotterEntry       m_plotter;  //!< Imperative plotter.

};

#endif
