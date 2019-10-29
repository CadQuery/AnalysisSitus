//-----------------------------------------------------------------------------
// Created on: 20 November 2015
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

#ifndef asiAlgo_Collections_h
#define asiAlgo_Collections_h

// asiAlgo includes
#include <asiAlgo_ShapePartnerHasher.h>

// OpenCascade includes
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>

//! Indexed map of shapes distinguished only by their TShape pointers.
typedef NCollection_DataMap<int, TopoDS_Shape> asiAlgo_DataMapOfShape;

//! Indexed map of shapes distinguished only by their TShape pointers.
typedef NCollection_IndexedMap<TopoDS_Shape,
                               asiAlgo_ShapePartnerHasher> asiAlgo_IndexedMapOfTShape;

//! Indexed data map of shapes and their ancestors distinguished only by their
//! TShape pointers.
typedef NCollection_IndexedDataMap<TopoDS_Shape,
                                   TopTools_ListOfShape,
                                   asiAlgo_ShapePartnerHasher> asiAlgo_IndexedDataMapOfTShapeListOfShape;

//! Data map of shapes and their images distinguished only by their
//! TShape pointers.
typedef NCollection_DataMap<TopoDS_Shape,
                            TopoDS_Shape,
                            asiAlgo_ShapePartnerHasher> asiAlgo_DataMapOfShapeShape;

#endif
