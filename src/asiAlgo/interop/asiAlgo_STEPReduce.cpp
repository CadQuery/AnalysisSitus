/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 2018-2019 Kicad Services Corporation
 *
 * @author Seth Hillbrand
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-3.0.html
 * or you may search the http://www.gnu.org website for the version 3 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

// asiAlgo includes
#include <asiAlgo_STEPReduce.h>

// Standard includes
#pragma warning(push, 0)
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <regex>
#include <iostream>
#pragma warning(pop)

namespace
{
  // trim from end (in place)
  static inline void rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
          return !std::isspace(ch);
      }).base(), s.end());
  }
  // trim from start (in place)
  static inline void ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
          return !std::isspace(ch);
      }));
  }
  template<typename Out>
  void split(const std::string &s, char delim, Out result) {
      std::stringstream ss(s);
      std::string item;
      if (std::getline(ss, item, delim)) {
      *(result++) = item;
    }
      if (std::getline(ss, item)) {
      *(result++) = item;
    }
  }
}

//-----------------------------------------------------------------------------

asiAlgo_STEPReduce::asiAlgo_STEPReduce(ActAPI_ProgressEntry progress,
                                       ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{}

//-----------------------------------------------------------------------------

bool asiAlgo_STEPReduce::Peform(const std::string& inFilename,
                                const std::string& outFilename) const
{
  std::vector<std::string> in_lines;
  std::vector<std::string> out_lines;

  std::ifstream is(inFilename);

  auto n_lines = std::count(std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>(), '\n') + 1;
  is.seekg( 0 );
  in_lines.reserve( n_lines );
  out_lines.reserve( n_lines );

  std::vector<std::string> footer;
  std::vector<std::string> header;
  bool past_header = false;
  bool past_data = false;
  bool continuing = false;

  while( is )
  {
    std::string line;
    std::getline( is, line );

    if( past_header )
    {
      if( past_data || line.find( "ENDSEC;") != std::string::npos )
      {
        past_data = true;
        footer.emplace_back( line );
      }
      else
      {
        rtrim(line);

        if( continuing )
        {
          if( std::isalpha(line.front()) )
            out_lines.back().append( " " );
          
          out_lines.back() += line;
        }
        else
        {
          out_lines.emplace_back( line );
        }

        continuing = ( line.back() != ';' );
      }      
    }
    else
    {
      if( line.find( "DATA;") != std::string::npos )
        past_header = true;

      header.emplace_back( line );
    }
  }

  is.close();

  std::unordered_map<std::string, unsigned> uniques;
  std::unordered_map<unsigned, unsigned> lookup;
  std::regex e ("#([0-9]+)");

  do
  {
    in_lines = out_lines;
    out_lines.clear();
    uniques.clear();
    lookup.clear();

    for ( const auto& line : in_lines )
    {
      std::vector<std::string> elems;
      split(line, '=', std::back_inserter(elems));

      // remove the #
      elems[0].erase(0,1);
      unsigned oldnum = std::stol(elems[0]);

      ltrim(elems[1]);
      rtrim(elems[1]);

      auto it = uniques.emplace( std::make_pair(elems[1], unsigned( out_lines.size() ) + 1 ) );

      while( ( elems[1].find("PRODUCT_DEFINITION") == 0
          || elems[1].find("SHAPE_REPRESENTATION") == 0 ) && !it.second )
      {
        elems[1].append( " " );
        it = uniques.emplace( std::make_pair(elems[1], unsigned( out_lines.size() ) + 1 ) );
      }

      if( !it.second )
      {
        if( elems[1] != it.first->first )
          std::cout << "Warning! " << elems[1] << " is not the same as " << it.first->first << std::endl;
        // didn't insert!
        lookup.emplace( oldnum, it.first->second );
      }
      else
      {
        lookup.emplace( std::make_pair( oldnum, unsigned( out_lines.size() ) + 1 ) );
        out_lines.emplace_back( "#" + std::to_string( out_lines.size() + 1 ) + "=" + elems[1]);
      }
    }

    for( size_t i = 0; i < out_lines.size(); i++ )
    {
      std::vector<std::string> elems;
      std::smatch m;
      auto& line = out_lines[i];

      split(line, '=', std::back_inserter(elems));

      line = elems[0] + "=";

      while( std::regex_search( elems[1], m, e, std::regex_constants::match_not_null) )
      {
        if( m.empty() ) 
          break;

        unsigned oldval = std::stol( m[1] );
        auto newval = lookup.find( oldval );

        if( newval == lookup.end() )
          line += ( m.prefix().str() + "#" + m[1].str() );  
        else
          line += ( m.prefix().str() + "#" + std::to_string( newval->second ) );

        elems[1] = m.suffix();
      }

      if( !elems[1].empty() )
        line += elems[1];
    }

  } while( in_lines.size() > out_lines.size() );

  std::ofstream os(outFilename);

  for( const auto& line : header )
    os << line << std::endl;

  for( const auto& line : out_lines )
    os << line << std::endl;

  for( const auto& line : footer )
    os << line << std::endl;

  os.close();

  m_progress.SendLogMessage(LogNotice(Normal) << "Input file %1 with %2 lines was shrunk to "
                                                 "%3 lines in the output file %4."
                                              << inFilename
                                              << int( n_lines )
                                              << int( out_lines.size() + header.size() + footer.size() )
                                              << outFilename );

  return true;
}
