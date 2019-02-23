//-----------------------------------------------------------------------------
// Created on: 10 November 2018
//-----------------------------------------------------------------------------
// Copyright (c) 2018-present, Sergey Slyadnev
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

#ifndef asiAlgo_BRepNormalizer_HeaderFile
#define asiAlgo_BRepNormalizer_HeaderFile

// asiAlgo includes
#include <asiAlgo_BRepNormalization.h>

// Active Data includes
#include <ActAPI_IAlgorithm.h>

// OCCT includes
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//-----------------------------------------------------------------------------

//! Performs geometric modifications on a shape.
//!
//! This class is a slightly reworked version of BRepTools_Modifier from
//! OpenCascade adding extra error reporting capabilities.
class asiAlgo_BRepNormalizer : public ActAPI_IAlgorithm
{
public:

  //! Creates an empty Modifier.
  asiAlgo_EXPORT
    asiAlgo_BRepNormalizer(ActAPI_ProgressEntry progress = NULL,
                           ActAPI_PlotterEntry  plotter = NULL);

  //! Creates a modifier on the shape <S>.
  asiAlgo_EXPORT
    asiAlgo_BRepNormalizer(const TopoDS_Shape&  S,
                           ActAPI_ProgressEntry progress = NULL,
                           ActAPI_PlotterEntry  plotter  = NULL);

  //! Creates a modifier on  the shape <S>, and performs
  //! the modifications described by <M>.
  asiAlgo_EXPORT
    asiAlgo_BRepNormalizer(const TopoDS_Shape&                      S,
                           const Handle(asiAlgo_BRepNormalization)& M,
                           ActAPI_ProgressEntry                     progress = NULL,
                           ActAPI_PlotterEntry                      plotter  = NULL);

public:

  //! Initializes the modifier with the shape <S>.
  asiAlgo_EXPORT void
    Init(const TopoDS_Shape& S);

  //! Performs the modifications described by <M>.
  //! \param[in] M normalization modification.
  //! \return true in case of success, false -- otherwise.
  asiAlgo_EXPORT bool
    Perform(const Handle(asiAlgo_BRepNormalization)& M);

public:

  //! Returns the modified shape corresponding to <S>.
  const TopoDS_Shape& ModifiedShape(const TopoDS_Shape& S) const
  {
    if (!myMap.IsBound(S)) { throw Standard_NoSuchObject();}
    return myMap(S);
  }

protected:

  asiAlgo_EXPORT void
    put(const TopoDS_Shape& S);

  asiAlgo_EXPORT bool
    rebuildRecursively(const TopoDS_Shape&                      S,
                       const Handle(asiAlgo_BRepNormalization)& M,
                       bool&                                    newGeom);

  asiAlgo_EXPORT bool
    createNewVertices(const TopTools_IndexedDataMapOfShapeListOfShape& MVE,
                      const Handle(asiAlgo_BRepNormalization)&         M);

  asiAlgo_EXPORT bool
    fillNewCurveInfo(const TopTools_IndexedDataMapOfShapeListOfShape& MEF,
                     const Handle(asiAlgo_BRepNormalization)&         M);

  asiAlgo_EXPORT bool
    fillNewSurfaceInfo(const Handle(asiAlgo_BRepNormalization)& M);

  asiAlgo_EXPORT bool
    createOtherVertices(const TopTools_IndexedDataMapOfShapeListOfShape& MVE,
                        const TopTools_IndexedDataMapOfShapeListOfShape& MEF,
                        const Handle(asiAlgo_BRepNormalization)&         M);

protected:

  struct NewCurveInfo
  {
    Handle(Geom_Curve) myCurve;
    TopLoc_Location    myLoc;
    double             myToler;
  };

  struct NewSurfaceInfo
  {
    Handle(Geom_Surface) mySurface;
    TopLoc_Location      myLoc;
    double               myToler;
    bool                 myRevWires;
    bool                 myRevFace;
  };

  TopTools_DataMapOfShapeShape myMap;
  TopoDS_Shape                 myShape;
  TopTools_MapOfShape          myNonUpdFace;
  TopTools_MapOfShape          myHasNewGeom;

  NCollection_DataMap<TopoDS_Edge,
                      NewCurveInfo,
                      TopTools_ShapeMapHasher> myNCInfo;

  NCollection_DataMap<TopoDS_Face,
                      NewSurfaceInfo,
                      TopTools_ShapeMapHasher> myNSInfo;

};

#endif
