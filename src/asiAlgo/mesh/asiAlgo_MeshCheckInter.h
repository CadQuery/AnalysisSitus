//-----------------------------------------------------------------------------
// Created on: 21 June 2018
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

#ifndef asiAlgo_MeshCheckInter_HeaderFile
#define asiAlgo_MeshCheckInter_HeaderFile

// asiAlgo includes
#include <asiAlgo.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <NCollection_UBTree.hxx>
#include <Poly_Triangulation.hxx>

//-----------------------------------------------------------------------------

//! Utility to check self-intersections on mesh.
class asiAlgo_MeshCheckInter : public ActAPI_IAlgorithm
{
public:

  // OCCT RTTI
  DEFINE_STANDARD_RTTI_INLINE(asiAlgo_MeshCheckInter, ActAPI_IAlgorithm)

public:

  //! Execution status.
  enum t_status
  {
    Status_Ok               = 0, //!< Mesh is OK.
    Status_HasIntersections = 1, //!< Self-intersections found.
    Status_Failed           = 2  //!< Checker failed for some reason.
  };

public:

  //! Constructs self-intersection checker.
  //! \param[in] mesh     triangulation to check.
  //! \param[in] progress progress notifier.
  //! \param[in] plotter  imperative plotter.
  asiAlgo_EXPORT
    asiAlgo_MeshCheckInter(const Handle(Poly_Triangulation)& mesh,
                           ActAPI_ProgressEntry              progress,
                           ActAPI_PlotterEntry               plotter);

public:

  //! Performs self-intersection check.
  //! \return execution status.
  asiAlgo_EXPORT t_status
    Perform();

public:

  //! Unbalanced tree of bounding boxes of mesh elements
  template<class TheObjectType, class TheBndType, class ThePointType>
  class ElementsTree : public NCollection_UBTree<TheObjectType, TheBndType>
  {
  public:
    /// Selector for elements lying in the area of influence of a given point
    class Selector : public NCollection_UBTree<TheObjectType, TheBndType>::Selector
    {
    public:
      Selector(const ThePointType& thePoint)
        : NCollection_UBTree<TheObjectType, TheBndType>::Selector(),
        myPoint(thePoint)
      {}

      virtual bool Reject(const TheBndType& theBndBox) const
      {
        return theBndBox.IsOut(myPoint);
      }

      virtual bool Accept(const TheObjectType& theObject)
      {
        mySelected.Append(theObject);
        return 1;
      }

      const NCollection_Sequence<TheObjectType>& Selected() const
      {
        return mySelected;
      }

    private:
      ThePointType                        myPoint;    ///< Point to select elements
      NCollection_Sequence<TheObjectType> mySelected; ///< List of applicable elements
    };

    /// Search indices elements which bounding boxes intersect/contain given box/point
    NCollection_Sequence<TheObjectType> FindInterferingElements(const ThePointType& thePoint)
    {
      Selector aSelector(thePoint);
      Select(aSelector);
      return aSelector.Selected();
    }
  };

protected:

  Handle(Poly_Triangulation) m_mesh; //!< Mesh to check.

};

#endif
