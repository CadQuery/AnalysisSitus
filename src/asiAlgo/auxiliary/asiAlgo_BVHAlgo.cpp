//-----------------------------------------------------------------------------
// Created on: 29 September 2018
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

// Own include
#include <asiAlgo_BVHAlgo.h>

// OCCT includes
#include <Precision.hxx>

//-----------------------------------------------------------------------------

gp_Pnt asiAlgo_BVHAlgo::projectPointSegment(const gp_Pnt& thePoint, const gp_Pnt& theStart, const gp_Pnt& theEnd)
{
  gp_Vec aDir(theStart, theEnd);
  gp_Vec aPDir(theStart, thePoint);

  double aProjParam = aPDir.Dot(aDir) / aDir.SquareMagnitude();
  gp_Pnt aProjection;
  if (aProjParam >= -Precision::PConfusion() && aProjParam <= 1.0 + Precision::PConfusion())
    aProjection.ChangeCoord() = theStart.XYZ() + aProjParam * aDir.XYZ();
  else if (aProjParam < 0.0)
    aProjection = theStart;
  else
    aProjection = theEnd;

  return aProjection;
}

double asiAlgo_BVHAlgo::squaredDistancePointSegment(const gp_Pnt& thePoint, const gp_Pnt& theStart, const gp_Pnt& theEnd)
{
  gp_Pnt aProj;
  return squaredDistancePointSegment(thePoint, theStart, theEnd, aProj);
}

double asiAlgo_BVHAlgo::squaredDistancePointSegment(const gp_Pnt& thePoint, const gp_Pnt& theStart, const gp_Pnt& theEnd, gp_Pnt& theProj)
{
  theProj = projectPointSegment(thePoint, theStart, theEnd);
  return thePoint.SquareDistance(theProj);
}

double asiAlgo_BVHAlgo::distancePointSegment(const gp_Pnt& thePoint, const gp_Pnt& theStart, const gp_Pnt& theEnd)
{
  return Sqrt(squaredDistancePointSegment(thePoint, theStart, theEnd));
}

double asiAlgo_BVHAlgo::distancePointSegment(const gp_Pnt& thePoint, const gp_Pnt& theStart, const gp_Pnt& theEnd, gp_Pnt& theProj)
{
  return Sqrt(squaredDistancePointSegment(thePoint, theStart, theEnd, theProj));
}

gp_Pnt asiAlgo_BVHAlgo::projectPointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3)
{
  gp_Vec aV1(theTriPoint1, theTriPoint2);
  gp_Vec aV2(theTriPoint1, theTriPoint3);

  gp_Vec aNormal = aV1 ^ aV2;
  gp_Pnt aProjection;
  if (aNormal.SquareMagnitude() < Precision::SquareConfusion())
  {
    // project to the longest edge
    double aSqDist[3] = {
        theTriPoint1.SquareDistance(theTriPoint2),
        theTriPoint2.SquareDistance(theTriPoint3),
        theTriPoint3.SquareDistance(theTriPoint1)
    };
    if (aSqDist[0] >= aSqDist[1] && aSqDist[0] >= aSqDist[2])
      aProjection = projectPointSegment(thePoint, theTriPoint1, theTriPoint2);
    else if (aSqDist[0] <= aSqDist[1] && aSqDist[1] >= aSqDist[2])
      aProjection = projectPointSegment(thePoint, theTriPoint2, theTriPoint3);
    else
      aProjection = projectPointSegment(thePoint, theTriPoint3, theTriPoint1);
  }
  else
  {
    aNormal.Normalize();
    aProjection = projectPointTriangle(thePoint, theTriPoint1, theTriPoint2, theTriPoint3, aNormal);
  }

  return aProjection;
}

gp_Pnt asiAlgo_BVHAlgo::projectPointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3, const gp_Vec& theNormal)
{
  const gp_Vec aV1(theTriPoint1, theTriPoint2);
  const gp_Vec aV2(theTriPoint1, theTriPoint3);
  const gp_Vec aVec(theTriPoint1, thePoint);
  const double aDot = theNormal.Dot(aVec);
  gp_Pnt aProjection = thePoint.XYZ() - aDot * theNormal.XYZ();
  const gp_Vec aProjVec(theTriPoint1, aProjection);

  double aParam1 = -1.0;
  double aParam2 = -1.0;
  double aDet = aV1.X() * aV2.Y() - aV1.Y() * aV2.X();
  if (Abs(aDet) > Precision::Confusion())
  {
    // X, Y
    const double anInvDet = 1.0 / aDet;
    aParam1 = (aProjVec.X() * aV2.Y() - aProjVec.Y() * aV2.X()) * anInvDet;
    aParam2 = (aV1.X() * aProjVec.Y() - aProjVec.X() * aV1.Y()) * anInvDet;
  }
  else
  {
    aDet = aV1.X() * aV2.Z() - aV1.Z() * aV2.X();
    if (Abs(aDet) > Precision::Confusion())
    {
      // X,Z
      const double anInvDet = 1.0 / aDet;
      aParam1 = (aProjVec.X() * aV2.Z() - aProjVec.Z() * aV2.X()) * anInvDet;
      aParam2 = (aV1.X() * aProjVec.Z() - aProjVec.X() * aV1.Z()) * anInvDet;
    }
    else
    {
      // Y,Z
      aDet = aV1.Y() * aV2.Z() - aV1.Z() * aV2.Y();
      const double anInvDet = 1.0 / aDet;
      aParam1 = (aProjVec.Y() * aV2.Z() - aProjVec.Z() * aV2.Y()) * anInvDet;
      aParam2 = (aV1.Y() * aProjVec.Z() - aProjVec.Y() * aV1.Z()) * anInvDet;
    }
  }

  if (aParam1 + aParam2 > 1.0 + Precision::PConfusion())
    aProjection = projectPointSegment(aProjection, theTriPoint2, theTriPoint3);
  else if (aParam1 < -Precision::PConfusion())
    aProjection = projectPointSegment(aProjection, theTriPoint3, theTriPoint1);
  else if (aParam2 < -Precision::PConfusion())
    aProjection = projectPointSegment(aProjection, theTriPoint1, theTriPoint2);

  return aProjection;
}

double asiAlgo_BVHAlgo::distancePointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3, gp_Pnt& theProj)
{
  return Sqrt(squaredDistancePointTriangle(thePoint, theTriPoint1, theTriPoint2, theTriPoint3, theProj));
}

double asiAlgo_BVHAlgo::distancePointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3, const gp_Vec& theNormal, gp_Pnt& theProj)
{
  return Sqrt(squaredDistancePointTriangle(thePoint, theTriPoint1, theTriPoint2, theTriPoint3, theNormal, theProj));
}

double asiAlgo_BVHAlgo::squaredDistancePointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3, gp_Pnt& theProj)
{
  theProj = projectPointTriangle(thePoint, theTriPoint1, theTriPoint2, theTriPoint3);
  return thePoint.SquareDistance(theProj);
}

double asiAlgo_BVHAlgo::squaredDistancePointTriangle(const gp_Pnt& thePoint, const gp_Pnt& theTriPoint1, const gp_Pnt& theTriPoint2, const gp_Pnt& theTriPoint3, const gp_Vec& theNormal, gp_Pnt& theProj)
{
  theProj = projectPointTriangle(thePoint, theTriPoint1, theTriPoint2, theTriPoint3, theNormal);
  return thePoint.SquareDistance(theProj);
}

//-----------------------------------------------------------------------------

bool asiAlgo_BVHAlgo::isOut(const BVH_Vec4d& boxMin,
                            const BVH_Vec4d& boxMax,
                            const gp_Pnt&    P,
                            const double     prec)
{
  const double x = P.X(),
               y = P.Y(),
               z = P.Z();
  const double xmin = boxMin.x() - prec,
               ymin = boxMin.y() - prec,
               zmin = boxMin.z() - prec,
               xmax = boxMax.x() + prec,
               ymax = boxMax.y() + prec,
               zmax = boxMax.z() + prec;

  if ( x < xmin || y < ymin || z < zmin ||
       x > xmax || y > ymax || z > zmax )
    return true;

  return false;
}
