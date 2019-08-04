//-----------------------------------------------------------------------------
// Created on: 11 April 2016
//-----------------------------------------------------------------------------
// Copyright (c) 2016-present, Sergey Slyadnev
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

#ifndef asiVisu_IVCurveDataProvider_h
#define asiVisu_IVCurveDataProvider_h

// asiVisu includes
#include <asiVisu_CurveDataProvider.h>

// asiData includes
#include <asiData_IVCurveNode.h>

//-----------------------------------------------------------------------------

//! Data provider for a single curve in IV.
class asiVisu_IVCurveDataProvider : public asiVisu_CurveDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVCurveDataProvider, asiVisu_CurveDataProvider)

public:

  //! Constructor.
  //! \param[in] N source Node.
  asiVisu_EXPORT
    asiVisu_IVCurveDataProvider(const Handle(ActAPI_INode)& N);

public:

  //! \return curve type.
  asiVisu_EXPORT virtual Handle(Standard_Type)
    GetCurveType() const;

  //! Not used.
  asiVisu_EXPORT virtual Handle(Geom2d_Curve)
    GetCurve2d(double&, double&) const;

  //! Accessor for 3D curve.
  //! \param[out] f first parameter.
  //! \param[out] l last parameter.
  //! \return curve.
  asiVisu_EXPORT virtual Handle(Geom_Curve)
    GetCurve(double& f, double& l) const;

  //! \return array of handles.
  asiVisu_EXPORT virtual Handle(HRealArray)
    GetHandles() const;

  //! \return 0-based index of the currently active handle.
  asiVisu_EXPORT virtual int
    GetActiveHandle() const;

  //! Returns an ordered collection of reper points, i.e., the points which
  //! were used to build this curve.
  //! \param[out] pts vector of reper points which were used to build the curve.
  asiVisu_EXPORT virtual void
    GetReperPoints(std::vector<gp_XYZ>& pts) const;

  //! Returns reper point ID which is currently active.
  //! \return 0-based ID of the active reper point or -1 if no reper is active.
  asiVisu_EXPORT virtual int
    GetActiveReper() const;

  //! \return Boolean flag indicating whether to draw orientation tip.
  asiVisu_EXPORT virtual bool
    GetDrawOrientationTip() const;

  //! Returns ID of the Data Node which is being sourced by the visualization
  //! pipeline. This ID is bound to the pipeline's actor in order to have a
  //! back-reference from Presentation to Data Object.
  //! \return Node ID.
  asiVisu_EXPORT virtual ActAPI_DataObjectId
    GetNodeID() const;

public:

  //! Creates a copy of the Data Provider.
  //! \return copy.
  asiVisu_EXPORT Handle(asiVisu_IVCurveDataProvider)
    Clone() const;

protected:

  //! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
  //! translation process.
  //! \return source Parameters.
  virtual Handle(ActAPI_HParameterList)
    translationSources() const;

protected:

  Handle(asiData_IVCurveNode) m_node; //!< Source Node.

};

//-----------------------------------------------------------------------------

//! Data provider for a single curve in IV sensitive to interaction.
class asiVisu_IVCurveDataSensProvider : public asiVisu_IVCurveDataProvider
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiVisu_IVCurveDataSensProvider, asiVisu_IVCurveDataProvider)


public:

  //! Constructor.
  //! \param[in] N source Node.
  asiVisu_IVCurveDataSensProvider(const Handle(ActAPI_INode)& N) : asiVisu_IVCurveDataProvider(N) {}

protected:

  //! Enumerates Data Parameters playing as sources for DOMAIN -> VTK
  //! translation process.
  //! \return source Parameters.
  Handle(ActAPI_HParameterList) translationSources() const
  {
    ActParamStream out;
    out.List = asiVisu_IVCurveDataProvider::translationSources();

    // Add more than in the base class which does not react to PID_ActiveReper.
    out << m_node->Parameter(asiData_IVCurveNode::PID_ActiveReper);

    return out;
  }

};

#endif
