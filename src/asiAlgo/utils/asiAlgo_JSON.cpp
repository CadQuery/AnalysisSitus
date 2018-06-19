//-----------------------------------------------------------------------------
// Created on: 19 June 2018
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
#include <asiAlgo_JSON.h>

// asiAlgo includes
#include <asiAlgo_Utils.h>

// Utility to convert to OCCT knots form with multiplicities.
class asiAlgo_MultResolver
{
public:

  //! Knot value with multiplicity.
  struct t_knotWithMult
  {
    double u; //!< Parameter value.
    int    m; //!< Multiplicity.

    //! Default constructor.
    t_knotWithMult() : u(0.0), m(0) {}

    //! Complete constructor.
    //! \param _knot [in] knot value.
    //! \param _mult [in] multiplicity.
    t_knotWithMult(const double _knot,
                   const int    _mult) : u(_knot), m(_mult) {}
  };

public:

  NCollection_Sequence<t_knotWithMult> Knots; //!< Knots being processed.

public:

  //! Resolves for the passed parameter.
  //! \param[in] u knot value to resolve.
  void Resolve(const double u)
  {
    bool isFound = false;
    int foundIdx = -1;
    t_knotWithMult foundStruct;
    //
    for ( int i = 1; i <= Knots.Length(); ++i )
    {
      const t_knotWithMult& knotWithMult = Knots.Value(i);
      if ( Abs(knotWithMult.u - u) < RealEpsilon() )
      {
        isFound = true;
        foundIdx = i;
        foundStruct = knotWithMult;
        break;
      }
    }
    if ( isFound )
    {
      foundStruct.m += 1;
      Knots.ChangeValue(foundIdx) = foundStruct;
    }
    else
    {
      foundStruct.u = u;
      foundStruct.m = 1;
      Knots.Append(foundStruct);
    }
  }

  //! \return knot array in OCCT form.
  Handle(TColStd_HArray1OfReal) GetOpenCascadeKnots() const
  {
    Handle(TColStd_HArray1OfReal) res = new TColStd_HArray1OfReal( 0, Knots.Length() - 1 );
    for ( int i = 0; i < Knots.Length(); ++i )
    {
      res->SetValue( i, Knots(i + 1).u );
    }
    return res;
  }

  //! \return multiplicity array in OCCT form.
  Handle(TColStd_HArray1OfInteger) GetOpenCascadeMults() const
  {
    Handle(TColStd_HArray1OfInteger) res = new TColStd_HArray1OfInteger( 0, Knots.Length() - 1 );
    for ( int i = 0; i < Knots.Length(); ++i )
    {
      res->SetValue( i, Knots(i + 1).m );
    }
    return res;
  }
};

//-----------------------------------------------------------------------------

//! Constructor accepting JSON string to process.
//! \param[in] json string representing JSON to process.
asiAlgo_JSON::asiAlgo_JSON(const std::string& json)
: m_json(json)
{}

//-----------------------------------------------------------------------------

//! Destructor.
asiAlgo_JSON::~asiAlgo_JSON()
{
}

//-----------------------------------------------------------------------------

void asiAlgo_JSON::DumpCurve(const Handle(Geom_Curve)& curve,
                             Standard_OStream&         out)
{
  // Get parametric bounds.
  const double uMin = curve->FirstParameter();
  const double uMax = curve->LastParameter();

  out << std::setprecision( std::numeric_limits<double>::max_digits10 );
  out << "{";
  out <<  "\n    entity: curve";
  out << ",\n    type: " << asiAlgo_Utils::CurveName(curve);
  out << ",\n    continuity: " << asiAlgo_Utils::ContinuityToString( curve->Continuity() );

  out << ",\n    domain: {"; // Begin 'domain'.
  out <<  "\n        U_min: " << uMin;
  out << ",\n        U_max: " << uMax;
  out <<  "\n    }"; // End 'domain'.

  if ( curve->IsInstance( STANDARD_TYPE(Geom_BSplineCurve) ) )
  {
    Handle(Geom_BSplineCurve)
      bcurve = Handle(Geom_BSplineCurve)::DownCast(curve);

    out << ",\n    flags: {"; // Begin 'flags'.
    out <<  "\n        is_rational: " << bcurve->IsRational();
    out << ",\n        is_periodic: " << bcurve->IsPeriodic();
    out << ",\n        is_closed: "   << bcurve->IsClosed();
    out <<  "\n    }"; // End 'flags'.

    out << ",\n    properties: {"; // Begin 'properties'.
    out <<  "\n        degree: " << bcurve->Degree();

    // Dump U knots.
    const TColStd_Array1OfReal& knots = bcurve->KnotSequence();
    out << ",\n        knots: [";
    //
    for ( int k = knots.Lower(); k <= knots.Upper(); ++k )
    {
      out << knots(k);

      if ( k < knots.Upper() )
        out << ", ";
    }
    out << "]";

    // Dump poles.
    const TColgp_Array1OfPnt& poles = bcurve->Poles();
    //
    out << ",\n        num_poles: " << bcurve->NbPoles();
    out << ",\n        poles: [";
    //
    for ( int uIdx = poles.Lower(); uIdx <= poles.Upper(); ++uIdx )
    {
      const gp_Pnt& P = poles(uIdx);

      out << "[" << P.X() << ", " << P.Y() << ", " << P.Z() << "]";

      if ( uIdx < poles.Upper() )
        out << ", ";
    }
    out << "]";
    out << "\n    }"; // End 'properties'.
  }

  out << "\n}";
}

//-----------------------------------------------------------------------------

void asiAlgo_JSON::DumpSurface(const Handle(Geom_Surface)& surface,
                               Standard_OStream&           out)
{
  // Get parametric bounds.
  double uMin, uMax, vMin, vMax;
  surface->Bounds(uMin, uMax, vMin, vMax);

  out << std::setprecision( std::numeric_limits<double>::max_digits10 );
  out << "{";
  out <<  "\n    entity: surface";
  out << ",\n    type: " << asiAlgo_Utils::SurfaceName(surface);
  out << ",\n    continuity: " << asiAlgo_Utils::ContinuityToString( surface->Continuity() );

  out << ",\n    domain: {"; // Begin 'domain'.
  out <<  "\n        U_min: " << uMin;
  out << ",\n        U_max: " << uMax;
  out << ",\n        V_min: " << vMin;
  out << ",\n        V_max: " << vMax;
  out <<  "\n    }"; // End 'domain'.

  if ( surface->IsInstance( STANDARD_TYPE(Geom_BSplineSurface) ) )
  {
    Handle(Geom_BSplineSurface)
      bsurf = Handle(Geom_BSplineSurface)::DownCast(surface);

    out << ",\n    flags: {"; // Begin 'flags'.
    out <<  "\n        is_U_rational: " << bsurf->IsURational();
    out << ",\n        is_V_rational: " << bsurf->IsVRational();
    out << ",\n        is_U_periodic: " << bsurf->IsUPeriodic();
    out << ",\n        is_V_periodic: " << bsurf->IsVPeriodic();
    out << ",\n        is_U_closed: "   << bsurf->IsUClosed();
    out << ",\n        is_V_closed: "   << bsurf->IsVClosed();
    out <<  "\n    }"; // End 'flags'.

    out << ",\n    properties: {"; // Begin 'properties'.
    out <<  "\n        U_degree: " << bsurf->UDegree();
    out << ",\n        V_degree: " << bsurf->VDegree();

    // Dump U knots.
    const TColStd_Array1OfReal& U_knots = bsurf->UKnotSequence();
    out << ",\n        U_knots: [";
    //
    for ( int k = U_knots.Lower(); k <= U_knots.Upper(); ++k )
    {
      out << U_knots(k);

      if ( k < U_knots.Upper() )
        out << ", ";
    }
    out << "]";

    // Dump V knots.
    const TColStd_Array1OfReal& V_knots = bsurf->VKnotSequence();
    out << ",\n        V_knots: [";
    //
    for ( int k = V_knots.Lower(); k <= V_knots.Upper(); ++k )
    {
      out << V_knots(k);

      if ( k < V_knots.Upper() )
        out << ", ";
    }
    out << "]";

    // Dump poles.
    const TColgp_Array2OfPnt& poles = bsurf->Poles();
    //
    out << ",\n        num_poles_in_U_axis: " << bsurf->NbUPoles();
    out << ",\n        num_poles_in_V_axis: " << bsurf->NbVPoles();
    out << ",\n        poles: {"; // Begin 'poles'.
    //
    for ( int uIdx = poles.LowerRow(); uIdx <= poles.UpperRow(); ++uIdx )
    {
      out << "\n            u" << (uIdx - 1) << ": [";
      for ( int vIdx = poles.LowerCol(); vIdx <= poles.UpperCol(); ++vIdx )
      {
        const gp_Pnt& P = poles(uIdx, vIdx);

        out << "[" << P.X() << ", " << P.Y() << ", " << P.Z() << "]";

        if ( vIdx < poles.UpperCol() )
          out << ", ";
      }
      out << "]";
      if ( uIdx < poles.UpperRow() )
        out << ",";
    }
    out << "\n        }"; // End 'poles'.
    out << "\n    }"; // End 'properties'.
  }

  out << "\n}";
}

//-----------------------------------------------------------------------------

bool asiAlgo_JSON::ExtractBCurve(Handle(Geom_BSplineCurve)& curve) const
{
  /* =====================================
   *  Read data from JSON in generic form
   * ===================================== */

  // Extract degree.
  int p = 0;
  if ( !this->ExtractNumericBlockForKey<int>("degree", p, 0) )
    return false;

  // Extract knot vector.
  std::vector<double> U;
  if ( !this->ExtractVector1d("knots", U) )
    return false;

  // Extract poles.
  std::vector<gp_XYZ> poles;
  if ( !this->ExtractVector3d("poles", poles) )
    return false;

  /* =========================================
   *  Prepare OpenCascade's native structures
   * ========================================= */

  // Poles are transferred as-is.
  TColgp_Array1OfPnt occtPoles( 1, (int) poles.size() );
  for ( int i = occtPoles.Lower(); i <= occtPoles.Upper(); ++i )
  {
    gp_Pnt P( poles[i - 1].X(), poles[i - 1].Y(), poles[i - 1].Z() );
    occtPoles(i) = P;
  }

  // OCCT stores each knot value just once, however, requires additional
  // array with multiplicities. E.g. U = (0, 0, 1, 2, 2) is represented by
  // two arrays in OCCT: (0, 1, 2) and (2, 1, 2).
  asiAlgo_MultResolver MResolver;
  for ( int i = 0; i < (int) U.size(); ++i )
  {
    MResolver.Resolve(U[i]);
  }

  // Access handles.
  Handle(TColStd_HArray1OfReal)    hKnots = MResolver.GetOpenCascadeKnots();
  Handle(TColStd_HArray1OfInteger) hMults = MResolver.GetOpenCascadeMults();

  // Access actual knots and multiplicities.
  const TColStd_Array1OfReal&    occtKnots = hKnots->Array1();
  const TColStd_Array1OfInteger& occtMults = hMults->Array1();

  // Construct B-curve.
  try
  {
    curve = new Geom_BSplineCurve(occtPoles, occtKnots, occtMults, p);
  }
  catch ( ... )
  {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_JSON::ExtractBSurface(Handle(Geom_BSplineSurface)& surface) const
{
  /* =====================================
   *  Read data from JSON in generic form
   * ===================================== */

  // Extract degrees.
  int p = 0, q = 0;
  if ( !this->ExtractNumericBlockForKey<int>("U_degree", p, 0) )
    return false;
  if ( !this->ExtractNumericBlockForKey<int>("V_degree", q, 0) )
    return false;

  // Extract knot vectors.
  std::vector<double> U, V;
  if ( !this->ExtractVector1d("U_knots", U) )
    return false;
  if ( !this->ExtractVector1d("V_knots", V) )
    return false;

  // Extract poles.
  std::vector< std::vector<gp_XYZ> > poles;
  if ( !this->ExtractGrid3d("poles", poles) )
    return false;

  /* =========================================
   *  Prepare OpenCascade's native structures
   * ========================================= */

  // Poles are transferred as-is.
  TColgp_Array2OfPnt occt_Poles( 1, (int) poles.size(),
                                 1, (int) poles[0].size() );
  //
  for ( int i = occt_Poles.LowerRow(); i <= occt_Poles.UpperRow(); ++i )
  {
    for ( int j = occt_Poles.LowerCol(); j <= occt_Poles.UpperCol(); ++j )
    {
      const gp_XYZ& P = poles[i-1][j-1];
      occt_Poles(i, j) = gp_Pnt( P.X(), P.Y(), P.Z() );
    }
  }

  // Resolve U knots.
  asiAlgo_MultResolver uResolver;
  for ( int i = 0; i < (int) U.size(); ++i )
    uResolver.Resolve(U[i]);

  // Resolve V knots.
  asiAlgo_MultResolver vResolver;
  for ( int i = 0; i < (int) V.size(); ++i )
    vResolver.Resolve(V[i]);

  // Access OCCT collections.
  Handle(TColStd_HArray1OfReal)    occt_UKnots = uResolver.GetOpenCascadeKnots();
  Handle(TColStd_HArray1OfInteger) occt_UMults = uResolver.GetOpenCascadeMults();
  Handle(TColStd_HArray1OfReal)    occt_VKnots = vResolver.GetOpenCascadeKnots();
  Handle(TColStd_HArray1OfInteger) occt_VMults = vResolver.GetOpenCascadeMults();

  // Construct B-surface.
  try
  {
    surface = new Geom_BSplineSurface(occt_Poles,
                                      occt_UKnots->Array1(),
                                      occt_VKnots->Array1(),
                                      occt_UMults->Array1(),
                                      occt_VMults->Array1(),
                                      p,
                                      q,
                                      /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
                                      /* Periodic surfaces are not currently supported (june, 2018) */
                                      /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
                                      false, 
                                      false);
  }
  catch ( ... )
  {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_JSON::ExtractBlockForKey(const std::string& key,
                                      std::string&       block) const
{
  // Find position of the key word.
  size_t pos  = 0;
  bool   stop = false;
  //
  while ( !stop )
  {
    pos = m_json.find(key, pos); // Locate the key.

    if ( pos == std::string::npos )
      return false; // Key not found.

    // We need a whole word match, so we check special characters here.
    if ( pos > 0 && pos < m_json.length() )
      if ( m_json[pos-1] != ' '  &&
           m_json[pos-1] != '\t' &&
           m_json[pos+1] != ' '  &&
           m_json[pos-1] != '\t' &&
           m_json[pos-1] != ':' )
      {
        pos++;
        continue;
      }

    stop = true;
    pos += key.length();
  }

  // Take the string after the found key.
  std::string tmpBlock = m_json.substr(pos);

  // Check if there is a sub-structure.
  size_t subStructureBegin = 0,
         subStructureEnd   = 0,
         firstSeparatorPos = 0,
         firstCommaPos     = 0,
         subStructureCount = 0;
  //
  for ( size_t k = 0; k < tmpBlock.size(); ++k )
  {
    // Skip all characters which can occur between the key and curled bracket.
    // ...

    if ( tmpBlock[k] == ' '  ||
         tmpBlock[k] == '\n' ||
         tmpBlock[k] == '\r' ||
         tmpBlock[k] == '\t' )
      continue;

    if ( tmpBlock[k] == ':' )
    {
      if ( !firstSeparatorPos )
        firstSeparatorPos = k;

      continue;
    }

    if ( tmpBlock[k] == ',' )
    {
      if ( !firstCommaPos )
        firstCommaPos = k;

      if ( !subStructureBegin )
        break; // Do not proceed if comma is found while sub-structure never began.

      continue;
    }

    if ( tmpBlock[k] == '{' || tmpBlock[k] == '[' )
    {
      if ( !subStructureBegin )
        subStructureBegin = k;

      subStructureCount++;
      continue;
    }

    if ( tmpBlock[k] == '}' || tmpBlock[k] == ']' )
    {
      subStructureEnd = k;
      subStructureCount--;

      if ( !subStructureCount ) // To process nested brackets.
        break;
    }
  }

  if ( subStructureEnd > subStructureBegin ) // Extract sub-structure.
  {
    block = tmpBlock.substr(subStructureBegin + 1, subStructureEnd - subStructureBegin - 1);
  }
  else if ( firstCommaPos > firstSeparatorPos ) // Extract inline data.
  {
    block = tmpBlock.substr(firstSeparatorPos + 1, firstCommaPos - firstSeparatorPos - 1);
  }
  else
    return false;

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_JSON::ExtractVector1d(const std::string&   keyword,
                                   std::vector<double>& vector) const
{
  std::string block;
  this->ExtractBlockForKey(keyword, block);

  std::vector<std::string> chunks;
  asiAlgo_Utils::Str::Split(block, ",", chunks);

  for ( size_t k = 0; k < chunks.size(); ++k )
  {
    if ( !asiAlgo_Utils::Str::IsNumber(chunks[k]) )
      return false;

    vector.push_back( asiAlgo_Utils::Str::ToNumber<double>(chunks[k]) );
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_JSON::ExtractVector3d(const std::string&   keyword,
                                   std::vector<gp_XYZ>& vector) const
{
  std::string block;
  this->ExtractBlockForKey(keyword, block);

  std::vector<std::string> chunks;
  asiAlgo_Utils::Str::Split(block, "][", chunks);

  // Extract 3-coordinate tuples.
  for ( size_t k = 0; k < chunks.size(); ++k )
  {
    if ( !asiAlgo_Utils::Str::IsNumber(chunks[k]) )
      continue;

    // Extract coordinates.
    const std::string& coordStr = chunks[k];
    std::vector<std::string> coordChunks;
    asiAlgo_Utils::Str::Split(coordStr, ",", coordChunks);

    if ( coordChunks.size() != 3 )
      return false;

    gp_XYZ P( asiAlgo_Utils::Str::ToNumber<double>(coordChunks[0], 0),
              asiAlgo_Utils::Str::ToNumber<double>(coordChunks[1], 0),
              asiAlgo_Utils::Str::ToNumber<double>(coordChunks[2], 0) );
    //
    vector.push_back(P);
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_JSON::ExtractGrid3d(const std::string&                  keyword,
                                 std::vector< std::vector<gp_XYZ> >& vector) const
{
  std::string block;
  this->ExtractBlockForKey(keyword, block);

  std::vector<std::string> rowChunks;
  asiAlgo_Utils::Str::Split(block, ":", rowChunks);

  for ( size_t k = 0; k < rowChunks.size(); ++k )
  {
    std::vector<std::string> tupleChunks;
    asiAlgo_Utils::Str::Split(rowChunks[k], "][", tupleChunks);

    if ( tupleChunks.size() < 3 )
      continue;

    std::vector<gp_XYZ> row;
    for ( size_t j = 0; j < tupleChunks.size(); ++j )
    {
      std::vector<std::string> coordChunks;
      asiAlgo_Utils::Str::Split(tupleChunks[j], ",", coordChunks);

      if ( coordChunks.size() != 3 )
        continue;

      gp_XYZ P( asiAlgo_Utils::Str::ToNumber<double>(coordChunks[0], 0),
                asiAlgo_Utils::Str::ToNumber<double>(coordChunks[1], 0),
                asiAlgo_Utils::Str::ToNumber<double>(coordChunks[2], 0) );
      //
      row.push_back(P);
    }
    //
    vector.push_back(row);
  }

  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_JSON::IsCurve() const
{
  std::string block;
  this->ExtractBlockForKey("entity", block);

  const size_t pos = m_json.find("curve", 0);
  return pos != std::string::npos;
}

//-----------------------------------------------------------------------------

bool asiAlgo_JSON::IsSurface() const
{
  std::string block;
  this->ExtractBlockForKey("entity", block);

  const size_t pos = m_json.find("surface", 0);
  return pos != std::string::npos;
}
