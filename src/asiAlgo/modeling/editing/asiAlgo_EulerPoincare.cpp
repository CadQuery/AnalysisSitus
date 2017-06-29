//-----------------------------------------------------------------------------
// Created on: 21 July 2016
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

// Own include
#include <asiAlgo_EulerPoincare.h>

// Geometry includes
#include <asiAlgo_Utils.h>

//! Checks the passed shape against Euler-Poincare formula. Notice that
//! genus of the manifold has to be selected by user as we have no idea
//! how to perform this tricky genus analysis.
//! \param shape [in] shape to check.
//! \param genus [in] genus of the manifold.
//! \return true if the check has passed, false -- otherwise.
bool asiAlgo_EulerPoincare::Check(const TopoDS_Shape& shape,
                                  const int           genus)
{
  // Summary
  int nbCompsolids = 0,
      nbCompounds  = 0,
      nbSolids     = 0,
      nbShells     = 0,
      nbFaces      = 0,
      nbWires      = 0,
      nbOuterWires = 0,
      nbInnerWires = 0,
      nbEdges      = 0,
      nbDegenEdges = 0,
      nbVertexes   = 0;

  // Extract summary
  asiAlgo_Utils::ShapeSummary(shape,
                           nbCompsolids,
                           nbCompounds,
                           nbSolids,
                           nbShells,
                           nbFaces,
                           nbWires,
                           nbOuterWires,
                           nbInnerWires,
                           nbEdges,
                           nbDegenEdges,
                           nbVertexes);

  // Prepare output string with the gathered summary
  std::cout << "Shape summary:\n";
  std::cout << "- nb compsolids: "        << nbCompsolids << "\n";
  std::cout << "- nb compounds: "         << nbCompounds  << "\n";
  std::cout << "- nb solids: "            << nbSolids     << "\n";
  std::cout << "- nb shells (s): "        << nbShells     << "\n";
  std::cout << "- nb faces (f): "         << nbFaces      << "\n";
  std::cout << "- nb wires: "             << nbWires      << "\n";
  std::cout << "\t- nb outer wires: "     << nbOuterWires << "\n";
  std::cout << "\t- nb inner wires (r): " << nbInnerWires << "\n";
  std::cout << "- nb edges (e - degen): " << nbEdges      << "\n";
  std::cout << "\t- nb degen edges: "     << nbDegenEdges << "\n";
  std::cout << "- nb vertices (v): "      << nbVertexes   << "\n";

  const int v = nbVertexes;
  const int e = nbEdges - nbDegenEdges;
  const int f = nbFaces;
  const int s = nbShells;
  const int h = genus;
  const int r = nbInnerWires;

  const int E = v - e + f - 2*(s - h) - r;

  std::cout << "Euler number (E = v - e + f - 2*(s - h) - r): " << E << std::endl;
  return (E == 0);
}
