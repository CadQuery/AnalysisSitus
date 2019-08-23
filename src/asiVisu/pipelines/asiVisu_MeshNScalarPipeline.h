//-----------------------------------------------------------------------------
// Created on: 13 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2015-present, Sergey Slyadnev
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

#ifndef asiVisu_MeshNScalarPipeline_h
#define asiVisu_MeshNScalarPipeline_h

// asiVisu includes
#include <asiVisu_Pipeline.h>
#include <asiVisu_MeshNScalarDataProvider.h>

// VTK includes
#include <vtkLookupTable.h>

//-----------------------------------------------------------------------------

//! Visualization pipeline for meshes with nodal scalars.
class asiVisu_MeshNScalarPipeline : public asiVisu_Pipeline
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_MeshNScalarPipeline, asiVisu_Pipeline)

public:

  //! Ctor.
  asiVisu_EXPORT
    asiVisu_MeshNScalarPipeline();

public:

  //! Sets input data for the pipeline accepting Active Data entities.
  //! Actually this method performs translation of DOMAIN data to VTK POLYGONAL
  //! DATA.
  //! \param[in] dp Data Provider.
  asiVisu_EXPORT virtual void
    SetInput(const Handle(asiVisu_DataProvider)& dp);

protected:

  //! Initializes lookup table for scalar mapping.
  asiVisu_EXPORT void
    initLookupTable();

private:

  //! Callback for AddToRenderer() base routine. Good place to adjust visualization
  //! properties of the pipeline's actor.
  virtual void callback_add_to_renderer(vtkRenderer* renderer);

  //! Callback for RemoveFromRenderer() base routine.
  virtual void callback_remove_from_renderer(vtkRenderer* renderer);

  //! Callback for Update() routine.
  virtual void callback_update();

private:

  //! Copying prohibited.
  asiVisu_MeshNScalarPipeline(const asiVisu_MeshNScalarPipeline&);

  //! Assignment prohibited.
  asiVisu_MeshNScalarPipeline& operator=(const asiVisu_MeshNScalarPipeline&);

protected:

  //! Internally used filters.
  enum FilterId
  {
    Filter_NScalar = 1, //!< Filter for populating point scalar array.
    Filter_Normals,     //!< Filter for calculation of normals.
    Filter_Last
  };

  //! Auxiliary map of internal filters by their correspondent IDs.
  typedef NCollection_DataMap< FilterId, vtkSmartPointer<vtkAlgorithm> > FilterMap;

protected:

  //! Map of internally used filters.
  FilterMap m_filterMap;

  //! Lookup table.
  vtkSmartPointer<vtkLookupTable> m_lookupTable;

  //! Safety range.
  double m_fToler;

};

#endif
