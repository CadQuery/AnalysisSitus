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

gp_Pnt asiAlgo_BVHAlgo::projectPointSegment(const gp_Pnt& P,
                                            const gp_Pnt& start,
                                            const gp_Pnt& end)
{
  gp_Vec aDir(start, end);
  gp_Vec aPDir(start, P);

  double aProjParam = aPDir.Dot(aDir) / aDir.SquareMagnitude();
  gp_Pnt aProjection;
  if (aProjParam >= -Precision::PConfusion() && aProjParam <= 1.0 + Precision::PConfusion())
    aProjection.ChangeCoord() = start.XYZ() + aProjParam * aDir.XYZ();
  else if (aProjParam < 0.0)
    aProjection = start;
  else
    aProjection = end;

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

bool asiAlgo_BVHAlgo::isOut(const BVH_Vec3d& boxMin,
                            const BVH_Vec3d& boxMax,
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

//-----------------------------------------------------------------------------

double asiAlgo_BVHAlgo::intersectTriangle(const t_ray&     ray,
                                          const BVH_Vec3d& P0,
                                          const BVH_Vec3d& P1,
                                          const BVH_Vec3d& P2)
{
  const BVH_Vec3d E0 = P1 - P0;
  const BVH_Vec3d E1 = P0 - P2;

  // Norm vector.
  const BVH_Vec3d N( E1.y()*E0.z() - E1.z()*E0.y(),
                     E1.z()*E0.x() - E1.x()*E0.z(),
                     E1.x()*E0.y() - E1.y()*E0.x() );

  const double NdotD = N.Dot(ray.Direct);
  //
  if ( Abs(NdotD) < Precision::Confusion() )
    return REAL_MAX;

  const double InvNdotD = 1.0 / NdotD;
  const BVH_Vec3d E2 ( ( P0.x() - ray.Origin.x() )*InvNdotD,
                       ( P0.y() - ray.Origin.y() )*InvNdotD,
                       ( P0.z() - ray.Origin.z() )*InvNdotD);

  const double time = N.Dot(E2);
  if ( time < 0.0 )
    return REAL_MAX;

  const BVH_Vec3d direct( ray.Direct.y()*E2.z() - ray.Direct.z()*E2.y(),
                          ray.Direct.z()*E2.x() - ray.Direct.x()*E2.z(),
                          ray.Direct.x()*E2.y() - ray.Direct.y()*E2.x() );

  const double U = direct.Dot(E1);
  const double V = direct.Dot(E0);

  return ( U < 0.0 || V < 0.0 || U + V > 1.0 ) ? REAL_MAX : time;
}

//-----------------------------------------------------------------------------

//! Computes number of ray-mesh intersections.
int asiAlgo_BVHAlgo::rayMeshHitCount(asiAlgo_BVHFacets* pMesh, const t_ray& ray)
{
  const BVH_Tree<double, 3>* pBVH = (pMesh != nullptr) ? pMesh->BVH().get() : nullptr;
  if ( pBVH == nullptr )
    return 0;

  // Invert.
  BVH_Vec3d invDirect = ray.Direct.cwiseAbs();
  //
  invDirect.x() = 1.0 / std::max( std::numeric_limits<double>::epsilon(), invDirect.x() );
  invDirect.y() = 1.0 / std::max( std::numeric_limits<double>::epsilon(), invDirect.y() );
  invDirect.z() = 1.0 / std::max( std::numeric_limits<double>::epsilon(), invDirect.z() );
  //
  invDirect.x() = std::copysign( invDirect.x(), ray.Direct.x() );
  invDirect.y() = std::copysign( invDirect.y(), ray.Direct.y() );
  invDirect.z() = std::copysign( invDirect.z(), ray.Direct.z() );

  int head = -1; // Stack head.
  int node =  0; // Root index.
  int stack[64];
  //
  for ( int numBounces = 0 ; ; )
  {
    if ( node >= (int) pBVH->NodeInfoBuffer().size() )
      return 0;

    BVH_Vec4i data = pBVH->NodeInfoBuffer()[node];

    if ( data.x() == 0 ) // Inner node.
    {
      BVH_Vec3d time0 = ( pBVH->MinPoint( data.y() ) - ray.Origin ) * invDirect;
      BVH_Vec3d time1 = ( pBVH->MaxPoint( data.y() ) - ray.Origin ) * invDirect;

      BVH_Vec3d timeMax = time0.cwiseMax(time1);
      BVH_Vec3d timeMin = time0.cwiseMin(time1);

      time0 = ( pBVH->MinPoint(data.z() ) - ray.Origin) * invDirect;
      time1 = ( pBVH->MaxPoint(data.z() ) - ray.Origin) * invDirect;

      double timeFinal = std::min( timeMax.x(), std::min( timeMax.y(), timeMax.z() ) );
      double timeStart = std::max( timeMin.x(), std::max( timeMin.y(), timeMin.z() ) );

      timeMax = time0.cwiseMax(time1);
      timeMin = time0.cwiseMin(time1);

      const double timeMin1 = (timeStart <= timeFinal) && (timeFinal >= 0) ? timeStart : REAL_MAX;

      timeFinal = std::min( timeMax.x(), std::min( timeMax.y(), timeMax.z() ) );
      timeStart = std::max( timeMin.x(), std::max( timeMin.y(), timeMin.z() ) );

      const double timeMin2 = (timeStart <= timeFinal) && (timeFinal >= 0) ? timeStart : REAL_MAX;

      const bool hitLft = timeMin1 != REAL_MAX;
      const bool hitRgh = timeMin2 != REAL_MAX;

      if ( hitLft && hitRgh )
      {
        node = (timeMin1 < timeMin2) ? data.y() : data.z();

        stack[++head] = timeMin1 < timeMin2 ? data.z() : data.y();
      }
      else if ( hitLft || hitRgh )
      {
        node = hitLft ? data.y() : data.z();
      }
      else
      {
        if ( head < 0 )
        {
          return numBounces;
        }

        node = stack[head--];
      }
    }
    else // Leaf node.
    {
      for ( int tidx = data.y(); tidx <= data.z(); ++tidx )
      {
        const asiAlgo_BVHFacets::t_facet& facet = pMesh->GetFacet(tidx);

        // Get next facet to test
        const gp_XYZ p0( facet.P0.x(), facet.P0.y(), facet.P0.z() );
        const gp_XYZ p1( facet.P1.x(), facet.P1.y(), facet.P1.z() );
        const gp_XYZ p2( facet.P2.x(), facet.P2.y(), facet.P2.z() );

        const BVH_Vec3d P0( p0.X(), p0.Y(), p0.Z() );
        const BVH_Vec3d P1( p1.X(), p1.Y(), p1.Z() );
        const BVH_Vec3d P2( p2.X(), p2.Y(), p2.Z() );

        // Precise test.
        const double hits = intersectTriangle(ray, P0, P1, P2);
        //
        if ( hits != REAL_MAX )
        {
          ++numBounces;
        }
      }

      if ( head < 0 )
      {
        return numBounces;
      }

      node = stack[head--];
    }
  }
}

//-----------------------------------------------------------------------------

double asiAlgo_BVHAlgo::squaredDistanceToBox(const BVH_Vec3d& P,
                                             const BVH_Vec3d& boxMin,
                                             const BVH_Vec3d& boxMax)
{
  double nearestX = std::min( std::max( P.x(), boxMin.x() ), boxMax.x() );
  double nearestY = std::min( std::max( P.y(), boxMin.y() ), boxMax.y() );
  double nearestZ = std::min( std::max( P.z(), boxMin.z() ), boxMax.z() );

  if ( nearestX == P.x() && nearestY == P.y() && nearestZ == P.z() )
    return 0.0;

  nearestX -= P.x();
  nearestY -= P.y();
  nearestZ -= P.z();

  return nearestX*nearestX + nearestY*nearestY + nearestZ*nearestZ;
}

//-----------------------------------------------------------------------------

double asiAlgo_BVHAlgo::squaredDistanceToTriangle(const BVH_Vec3d& P,
                                                  const BVH_Vec3d& A,
                                                  const BVH_Vec3d& B,
                                                  const BVH_Vec3d& C)
{
  // Special case 1.
  const BVH_Vec3d AB = B - A;
  const BVH_Vec3d AC = C - A;
  const BVH_Vec3d AP = P - A;
  //
  double ABdotAP = AB.Dot(AP);
  double ACdotAP = AC.Dot(AP);
  //
  if ( ABdotAP <= 0.0 && ACdotAP <= 0.0 )
  {
    return AP.Dot(AP);
  }

  // Special case 2.
  const BVH_Vec3d BC = C - B;
  const BVH_Vec3d BP = P - B;
  //
  double BAdotBP = -AB.Dot(BP);
  double BCdotBP =  BC.Dot(BP);
  //
  if ( BAdotBP <= 0.0 && BCdotBP <= 0.0 )
  {
    return BP.Dot(BP);
  }

  // Special case 3.
  const BVH_Vec3d CP = P - C;
  //
  double CBdotCP = -BC.Dot(CP);
  double CAdotCP = -AC.Dot(CP);
  if ( CAdotCP <= 0.0 && CBdotCP <= 0.0 )
  {
    return CP.Dot(CP);
  }

  // Special case 4.
  double ACdotBP = AC.Dot(BP);
  double VC      = ABdotAP*ACdotBP + BAdotBP*ACdotAP;
  //
  if ( VC <= 0.0 && ABdotAP > 0.0 && BAdotBP > 0.0 )
  {
    return ( AP - AB*(ABdotAP/(ABdotAP + BAdotBP)) ).SquareModulus();
  }

  // Special case 5.
  double ABdotCP = AB.Dot(CP);
  double VA      = BAdotBP*CAdotCP - ABdotCP*ACdotBP;
  if ( VA <= 0.0 && BCdotBP > 0.0 && CBdotCP > 0.0 )
  {
    return ( BP - BC*(BCdotBP/(BCdotBP + CBdotCP)) ).SquareModulus();
  }

  // Special case 6.
  double VB = ABdotCP*ACdotAP + ABdotAP*CAdotCP;
  if ( VB <= 0.0 && ACdotAP > 0.0 && CAdotCP > 0.0 )
  {
    return ( AP - AC*(ACdotAP/(ACdotAP + CAdotCP)) ).SquareModulus();
  }

  // General.
  double norm = VA + VB + VC;
  return (P - (A*VA + B*VB + C*VC)/norm).SquareModulus();
}

//-----------------------------------------------------------------------------

double asiAlgo_BVHAlgo::squaredDistanceToMesh(asiAlgo_BVHFacets* pMesh,
                                              const BVH_Vec3d&   P,
                                              const double       upperDist)
{
  const BVH_Tree<double, 3>* pBVH = pMesh != nullptr ? pMesh->BVH().get() : nullptr;
  if ( pBVH == nullptr )
    return REAL_MAX;

  std::pair<int, double> stack[64];
  int head = -1;
  int node =  0; // Root node.

  for ( double minDist2 = upperDist; ; )
  {
    if ( node >= (int) pBVH->NodeInfoBuffer().size() )
      return REAL_MAX;

    BVH_Vec4i data = pBVH->NodeInfoBuffer()[node];

    if ( data.x() == 0 ) // Inner node.
    {
      const double distToLft = squaredDistanceToBox( P,
                                                     pBVH->MinPoint( data.y() ),
                                                     pBVH->MaxPoint( data.y() ) );

      const double distToRgh = squaredDistanceToBox( P,
                                                     pBVH->MinPoint( data.z() ),
                                                     pBVH->MaxPoint( data.z() ) );

      const bool hitLft = distToLft <= minDist2;
      const bool hitRgh = distToRgh <= minDist2;

      if ( hitLft & hitRgh )
      {
        node = (distToLft < distToRgh) ? data.y() : data.z();

        stack[++head] = std::make_pair( distToLft < distToRgh ? data.z() : data.y(),
                                        std::max(distToLft, distToRgh) );
      }
      else
      {
        if ( hitLft | hitRgh)
        {
          node = hitLft ? data.y() : data.z();
        }
        else
        {
          if ( head < 0 )
            return minDist2;

          std::pair<int, double>& entry = stack[head--];

          while ( entry.second > minDist2 )
          {
            if ( head < 0 )
            {
              return minDist2;
            }

            entry = stack[head--];
          }

          node = entry.first;
        }
      }
    }
    else // Leaf node.
    {
      for ( int tidx = data.y(); tidx <= data.z(); ++tidx )
      {
        const asiAlgo_BVHFacets::t_facet& facet = pMesh->GetFacet(tidx);

        // Get next facet to test
        const gp_XYZ v0( facet.P0.x(), facet.P0.y(), facet.P0.z() );
        const gp_XYZ v1( facet.P1.x(), facet.P1.y(), facet.P1.z() );
        const gp_XYZ v2( facet.P2.x(), facet.P2.y(), facet.P2.z() );

        const BVH_Vec3d V0( v0.X(), v0.Y(), v0.Z() );
        const BVH_Vec3d V1( v1.X(), v1.Y(), v1.Z() );
        const BVH_Vec3d V2( v2.X(), v2.Y(), v2.Z() );

        const double triDist2 = squaredDistanceToTriangle(P,
                                                          V0,
                                                          V1,
                                                          V2);
        if ( triDist2 < minDist2 )
        {
          minDist2 = triDist2;
        }
      }

      if ( head < 0 )
      {
        return minDist2;
      }

      std::pair<int, double>& entry = stack[head--];
      while ( entry.second > minDist2 )
      {
        if ( head < 0 )
        {
          return minDist2;
        }

        entry = stack[head--];
      }

      node = entry.first;
    }
  }
}
