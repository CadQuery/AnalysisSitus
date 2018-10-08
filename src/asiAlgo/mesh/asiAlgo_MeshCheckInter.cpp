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

// Own include
#include <asiAlgo_MeshCheckInter.h>

// OCCT includes
#include <Bnd_Box.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <Poly_Triangle.hxx>
#include <Precision.hxx>

typedef asiAlgo_MeshCheckInter::ElementsTree<int, Bnd_Box, Bnd_Box> MeshElementsTree;

// Fills UB-tree for the elements of the given mesh.
static void FillElementsTree(const Handle(Poly_Triangulation)& mesh, MeshElementsTree& tree);

// Verifies if the triangles with given indices have intersections in the inner points.
static bool AreTrianglesIntersected(const Handle(Poly_Triangulation)& mesh,
                                    const int                         elemId1,
                                    const int                         elemId2);

//-----------------------------------------------------------------------------

Bnd_Box BoundingBoxForTriangle(const Handle(Poly_Triangulation)& mesh,
                               const int                         elemId)
{
  Bnd_Box bndBox;
  const Poly_Triangle& triangle = mesh->Triangle(elemId);
  for (int n = 1; n <= 3; ++n)
  {
    const gp_Pnt& P = mesh->Node( triangle.Value(n) );
    bndBox.Add(P);
  }
  return bndBox;
}

//-----------------------------------------------------------------------------

asiAlgo_MeshCheckInter::asiAlgo_MeshCheckInter(const Handle(Poly_Triangulation)& mesh,
                                               ActAPI_ProgressEntry              progress,
                                               ActAPI_PlotterEntry               plotter)
: m_mesh(mesh), ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

asiAlgo_MeshCheckInter::t_status asiAlgo_MeshCheckInter::Perform()
{
  // fill UB-tree for fast searching
  MeshElementsTree anElemTree;
  FillElementsTree(m_mesh, anElemTree);

  std::vector<gp_Pnt> siNodes;
  std::vector<Poly_Triangle> siTriangles;

  bool hasSI = false;
  const int nbElems = m_mesh->NbTriangles();
  //
  for ( int aEInd = 1; aEInd <= nbElems; ++aEInd )
  {
    Bnd_Box aBox = BoundingBoxForTriangle(m_mesh, aEInd);
    NCollection_Sequence<int> aFound = anElemTree.FindInterferingElements(aBox);
    for (NCollection_Sequence<int>::Iterator anIt(aFound); anIt.More(); anIt.Next())
    {
      if (anIt.Value() > aEInd && AreTrianglesIntersected(m_mesh, aEInd, anIt.Value()))
      {
        {
          std::cout << "Bad element: " << aEInd << std::endl;

          const Poly_Triangle& illElem = m_mesh->Triangle(aEInd);
          //
          int illElemN1, illElemN2, illElemN3;
          illElem.Get(illElemN1, illElemN2, illElemN3);
          //
          siNodes.push_back( m_mesh->Node(illElemN1) ); int idNode1 = int( siNodes.size() );
          siNodes.push_back( m_mesh->Node(illElemN2) ); int idNode2 = int( siNodes.size() );
          siNodes.push_back( m_mesh->Node(illElemN3) ); int idNode3 = int( siNodes.size() );
          //
          siTriangles.push_back( Poly_Triangle(idNode1, idNode2, idNode3) );
        }

        {
          std::cout << "Companion element: " << anIt.Value() << std::endl;

          const Poly_Triangle& companionElem = m_mesh->Triangle( anIt.Value() );
          //
          int illElemN1, illElemN2, illElemN3;
          companionElem.Get(illElemN1, illElemN2, illElemN3);
          //
          siNodes.push_back( m_mesh->Node(illElemN1) ); int idNode1 = int( siNodes.size() );
          siNodes.push_back( m_mesh->Node(illElemN2) ); int idNode2 = int( siNodes.size() );
          siNodes.push_back( m_mesh->Node(illElemN3) ); int idNode3 = int( siNodes.size() );
          //
          siTriangles.push_back( Poly_Triangle(idNode1, idNode2, idNode3) );
        }

        if ( !hasSI )
        {
          hasSI = true;
          break;
        }
      }
    }
  }

  // Create triangulation for plotter.
  if ( hasSI )
  {
    // Prepare nodes.
    TColgp_Array1OfPnt siPolyNodes( 1, int( siNodes.size() ) );
    //
    for ( size_t k = 0; k < siNodes.size(); ++k )
      siPolyNodes( int(k + 1) ) = siNodes[k];

    // Prepare triangles.
    Poly_Array1OfTriangle siPolyTris( 1, int( siTriangles.size() ) );
    //
    for ( size_t k = 0; k < siTriangles.size(); ++k )
      siPolyTris( int(k + 1) ) = siTriangles[k];

    // Build triangulation for the self-intersecting elements and draw it.
    Handle(Poly_Triangulation)
      siRes = new Poly_Triangulation(siPolyNodes, siPolyTris);
    //
    m_plotter.REDRAW_TRIANGULATION("siRes", siRes, Color_Red, 0.75);
  }

  return hasSI ? Status_HasIntersections : Status_Ok;
}

// ===============     Auxiliary functions     ===========================
void FillElementsTree(const Handle(Poly_Triangulation)& mesh,
                      MeshElementsTree&                 tree)
{
  NCollection_UBTreeFiller<int, Bnd_Box> aFiller(tree);

  const int nbElems = mesh->NbTriangles();
  //
  for ( int anInd = 1; anInd <= nbElems; ++anInd )
  {
    Bnd_Box aBox = BoundingBoxForTriangle(mesh, anInd);
    aBox.Enlarge( Precision::Confusion() );
    aFiller.Add(anInd, aBox);
  }
  aFiller.Fill();
}


// Auxiliary class to find triangle-triangle intersection inside of triangle
// Based on: Tomas Moller "A fast trinagle-triangle intesection test"
// http://web.stanford.edu/class/cs277/resources/papers/Moller1997b.pdf
class TriangleIntersection
{
private:
  struct Triangle
  {
    gp_Pnt myPoints[3];
    gp_XYZ myNormal;
    double myDistanceToOther[3];

    double myParMin, myParMax;

    Triangle() {}

    Triangle(const gp_Pnt& thePoint1, const gp_Pnt& thePoint2, const gp_Pnt& thePoint3)
    {
      myPoints[0] = thePoint1;
      myPoints[1] = thePoint2;
      myPoints[2] = thePoint3;

      gp_XYZ diff1 = myPoints[1].XYZ() - myPoints[0].XYZ();
      gp_XYZ diff2 = myPoints[2].XYZ() - myPoints[0].XYZ();
      myNormal = diff1.Crossed(diff2);
      double aLen = myNormal.Modulus();
      if (aLen > Precision::Confusion())
        myNormal /= aLen;
    }

    double SignedDistance(const gp_Pnt& thePoint)
    {
      return myNormal.Dot(thePoint.XYZ() - myPoints[0].XYZ());
    }

    void ComputeDistance(const Triangle& theOther, double theTolerance)
    {
      for (int i = 0; i < 3; ++i)
      {
        myDistanceToOther[i] = SignedDistance(theOther.myPoints[i]);
        if (Abs(myDistanceToOther[i]) < theTolerance)
          myDistanceToOther[i] = 0.0;
      }
    }

    int NbPositiveDistances(double theTolerance) const
    {
      int n = 0;
      for (int i = 0; i < 3; ++i)
        if (myDistanceToOther[i] > theTolerance)
          ++n;
      return n;
    }

    int NbNegativeDistances(double theTolerance) const
    {
      int n = 0;
      for (int i = 0; i < 3; ++i)
        if (myDistanceToOther[i] < -theTolerance)
          ++n;
      return n;
    }

    bool IsInnerPoint(const gp_Pnt& thePoint, double theTolerance) const
    {
      bool isInside = true;
      for (int i = 0; i < 3 && isInside; ++i)
      {
        gp_XYZ anInnerDir = myNormal.Crossed(myPoints[(i + 1) % 3].XYZ() - myPoints[i].XYZ());
        gp_XYZ aPntDir = thePoint.XYZ() - myPoints[i].XYZ();
        isInside = anInnerDir.Dot(aPntDir) > theTolerance * anInnerDir.Modulus();
      }
      return isInside;
    }

    bool IsIntersectSegment(const gp_Pnt& thePoint1, const gp_Pnt& thePoint2, double theTolerance) const
    {
      for (int i = 0; i < 3; ++i)
      {
        gp_Dir anEdgeDir(myPoints[(i + 1) % 3].XYZ() - myPoints[i].XYZ());
        gp_Dir anInsideDir(myNormal.Crossed(anEdgeDir.XYZ()));

        gp_XYZ aPnt1Dir = thePoint1.XYZ() - myPoints[i].XYZ();
        gp_XYZ aPnt2Dir = thePoint2.XYZ() - myPoints[i].XYZ();
        double aDist1 = aPnt1Dir.Dot(anInsideDir.XYZ());
        double aDist2 = aPnt2Dir.Dot(anInsideDir.XYZ());
        if (Abs(aDist1) <= theTolerance || Abs(aDist2) <= theTolerance || aDist1 * aDist2 >= 0.)
          continue;

        double aParam1 = aPnt1Dir.Dot(anEdgeDir.XYZ());
        double aParam2 = aPnt2Dir.Dot(anEdgeDir.XYZ());
        double anIntersectionParam = aParam1 + (aParam2 - aParam1) * aDist1 / (aDist1 - aDist2);
        if (anIntersectionParam > theTolerance &&
            anIntersectionParam < myPoints[(i + 1) % 3].Distance(myPoints[i]) - theTolerance)
          return true;
      }
      return false;
    }
  };

public:
  TriangleIntersection()
    : myNbTriangles(0),
      TOLERANCE(1e-7),
      TOLERANCE_PLANARITY(1e-4)
  {}

  bool AddTriangle(const gp_Pnt& thePoint1, const gp_Pnt& thePoint2, const gp_Pnt& thePoint3)
  {
    myTriangles[myNbTriangles++] = Triangle(thePoint1, thePoint2, thePoint3);
    return myTriangles[myNbTriangles - 1].myNormal.SquareModulus() > TOLERANCE * TOLERANCE;
  }

  bool IsIntersected()
  {
    for (int i = 0; i < 2; ++i)
    {
      myTriangles[i].ComputeDistance(myTriangles[1 - i], TOLERANCE_PLANARITY);
      if (AreNodesOnSameSide(myTriangles[i]))
        return false;
    }

    if (IsCoplanar())
      return IsIntersectedCoplanar();

    if (HasSharedEdge())
      return false;

    // parameters of the track of each triangle on the intersection line between planes of the triangles
    gp_Dir anIntersectionLineDir(myTriangles[0].myNormal.Crossed(myTriangles[1].myNormal));
    for (int i = 0; i < 2; ++i)
      CalculateIntersectionParameters(myTriangles[i], myTriangles[1 - i], anIntersectionLineDir);

    return myTriangles[0].myParMax >= myTriangles[0].myParMin + TOLERANCE && // the interval is not degenerated
           myTriangles[1].myParMax >= myTriangles[1].myParMin + TOLERANCE && // the interval is not degenerated
           myTriangles[0].myParMax >= myTriangles[1].myParMin + TOLERANCE &&
           myTriangles[1].myParMax >= myTriangles[0].myParMin + TOLERANCE;
  }

private:
  // check all nodes of second triangle are on the same side relatively to the given
  bool AreNodesOnSameSide(const Triangle& theTriangle) const
  {
    int nbPositive = theTriangle.NbPositiveDistances(TOLERANCE);
    int nbNegative = theTriangle.NbNegativeDistances(TOLERANCE);
    return (nbPositive == 0 || nbNegative == 0) && nbPositive + nbNegative == 3;
  }

  bool HasSharedEdge() const
  {
    int nbPositive1 = myTriangles[0].NbPositiveDistances(TOLERANCE);
    int nbNegative1 = myTriangles[0].NbNegativeDistances(TOLERANCE);

    int nbPositive2 = myTriangles[1].NbPositiveDistances(TOLERANCE);
    int nbNegative2 = myTriangles[1].NbNegativeDistances(TOLERANCE);

    return (nbPositive1 == 0 || nbNegative1 == 0) && (nbPositive2 == 0 || nbNegative2 == 0);
  }

  bool IsCoplanar() const
  {
    int nbPositive = myTriangles[0].NbPositiveDistances(TOLERANCE);
    int nbNegative = myTriangles[0].NbNegativeDistances(TOLERANCE);
    return nbPositive == 0 && nbNegative == 0;
  }

  // calculate parameters of the intersection of triangle with the line given by its direction
  void CalculateIntersectionParameters(Triangle& theTriangle, const Triangle& theRefTriangle, const gp_Dir& theLineDir) const
  {
    double p[3];
    for (int i = 0; i < 3; ++i)
      p[i] = theTriangle.myPoints[i].XYZ().Dot(theLineDir.XYZ());

    theTriangle.myParMin = Precision::Infinite();
    theTriangle.myParMax = -Precision::Infinite();
    for (int i = 0; i < 3; ++i)
    {
      double d1 = theRefTriangle.myDistanceToOther[i];
      double d2 = theRefTriangle.myDistanceToOther[(i + 1) % 3];
      if (d1 * d2 > 0.)
        continue; // point on the same side to line, link does not intersect it

      double t = p[i] + (p[(i + 1) % 3] - p[i]) * d1 / (d1 - d2);
      if (theTriangle.myParMin > t)
        theTriangle.myParMin = t;
      if (theTriangle.myParMax < t)
        theTriangle.myParMax = t;
    }
  }

  bool IsIntersectedCoplanar() const
  {
    // check at least one edge of the second triangle intersects the first
    for (int i = 0; i < 3; ++i)
      if (myTriangles[0].IsIntersectSegment(myTriangles[1].myPoints[i], myTriangles[1].myPoints[(i + 1) % 3], TOLERANCE))
        return true;
    // triangles do not intersect, check one is fully inside another
    return myTriangles[0].IsInnerPoint(myTriangles[1].myPoints[0], TOLERANCE) ||
           myTriangles[1].IsInnerPoint(myTriangles[0].myPoints[0], TOLERANCE);
  }

  TriangleIntersection& operator=(const TriangleIntersection&) {}

private:
  int          myNbTriangles;
  Triangle     myTriangles[2];
  const double TOLERANCE;
  const double TOLERANCE_PLANARITY; //!< To check planarity of two triangles.
};


bool AreTrianglesIntersected(const Handle(Poly_Triangulation)& theMesh, int theTriaIndex1, int theTriaIndex2)
{
  int aTrIndices[2] = { theTriaIndex1, theTriaIndex2 };
  TriangleIntersection anInter;

  for (int i = 0; i < 2; ++i)
  {
    const Poly_Triangle& aTria = theMesh->Triangle(aTrIndices[i]);
    gp_Pnt aPoints[3];
    for (int n = 1; n <= 3; ++n)
      aPoints[n - 1] = theMesh->Node(aTria.Value(n));
    if (!anInter.AddTriangle(aPoints[0], aPoints[1], aPoints[2]))
      return false; // degenerated triangle
  }
  return anInter.IsIntersected();
}
