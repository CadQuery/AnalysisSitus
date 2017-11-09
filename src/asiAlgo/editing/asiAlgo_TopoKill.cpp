//-----------------------------------------------------------------------------
// Created on: 05 September 2017
//-----------------------------------------------------------------------------
// Copyright (c) 2017, Sergey Slyadnev
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the Sergey Slyadnev.
// 4. Neither the name of the Sergey Slyadnev nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY Sergey Slyadnev ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL Sergey Slyadnev BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

// Own include
#include <asiAlgo_TopoKill.h>

// OCCT includes
#include <BRep_Builder.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>

//-----------------------------------------------------------------------------

asiAlgo_TopoKill::asiAlgo_TopoKill(const TopoDS_Shape&  masterCAD,
                                   ActAPI_ProgressEntry progress,
                                   ActAPI_PlotterEntry  plotter)
: ActAPI_IAlgorithm(progress, plotter)
{
  m_master = masterCAD;
}

//-----------------------------------------------------------------------------

bool asiAlgo_TopoKill::AskRemove(const TopoDS_Shape& subshape)
{
  if ( m_toReplace.IsBound(subshape) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Replacement is already asked for the shape you want to remove.");
    return false;
  }

  m_toRemove.Add(subshape);
  return true;
}

//-----------------------------------------------------------------------------

bool asiAlgo_TopoKill::AskReplace(const TopoDS_Shape& what,
                                  const TopoDS_Shape& with)
{
  // Check replacement type
  if ( what.ShapeType() != with.ShapeType() )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Cannot kill by replacement with different type.");
    return false;
  }

  // Check if any replacement is already recorded for the target sub-shape
  if ( m_toReplace.IsBound(what) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Such replacement is already asked.");
    return false;
  }

  // Check if removal is already recorded for the target sub-shape
  if ( m_toRemove.Contains(what) )
  {
    m_progress.SendLogMessage(LogErr(Normal) << "Removal is already asked for the shape you want to replace.");
    return false;
  }

  m_toReplace.Bind(what, with);
  return false;
}

//-----------------------------------------------------------------------------

bool asiAlgo_TopoKill::IsAsked(const TopoDS_Shape& subshape,
                               const bool          checkRecursively,
                               bool&               forRemoval,
                               bool&               forReplacement) const
{
  if ( checkRecursively && (forRemoval || forReplacement) )
    return true; // Stop recursion

  // Check the shape itself (without recursion)
  if ( this->IsAsked(subshape, forRemoval, forReplacement) )
    return true;

  // If the shape itself is not asked and the check is recursive, then
  // apply the same check to the direct children of the shape in question
  if ( checkRecursively )
  {
    for ( TopoDS_Iterator it(subshape, false, false); it.More(); it.Next() )
    {
      const TopoDS_Shape& currentShape = it.Value();

      if ( this->IsAsked(currentShape, true, forRemoval, forReplacement) )
        return true;
    }
  }

  return false;
}

//-----------------------------------------------------------------------------

bool asiAlgo_TopoKill::IsAsked(const TopoDS_Shape& subshape,
                               bool&               forRemoval,
                               bool&               forReplacement) const
{
  forRemoval     = m_toRemove.Contains(subshape);
  forReplacement = m_toReplace.IsBound(subshape);

  return forRemoval || forReplacement;
}

//-----------------------------------------------------------------------------

bool asiAlgo_TopoKill::Apply()
{
  bool forRemoval = false, forReplacement = false;
  //
  if ( !this->IsAsked(m_master, true, forRemoval, forReplacement) )
  {
    m_progress.SendLogMessage(LogWarn(Normal) << "No sub-shapes are requested for modification.");
    return false;
  }

  // Prepare root of the same type as a master shape
  m_result = m_master.EmptyCopied();

  // Rebuild topology graph recursively
  this->buildTopoGraphLevel(m_master, m_result);

  return true;
}

//-----------------------------------------------------------------------------

void asiAlgo_TopoKill::buildTopoGraphLevel(const TopoDS_Shape& root,
                                           TopoDS_Shape&       result) const
{
  BRep_Builder BB;

  // The two "false" flags passed to the iterator below mean that we do not
  // want to accumulate orientations and locations of sub-shapes.
  for ( TopoDS_Iterator it(root, false, false); it.More(); it.Next() )
  {
    const TopoDS_Shape& currentShape = it.Value();
    TopoDS_Shape newResult;

    // Check recursively if the current sub-shape is asked for modification.
    // The modification can be asked for the shape itself or for any of its
    // sub-shapes at any nesting level. Therefore, we perform a recursive
    // check here. It allows to avoid creating new shapes (TShape objects)
    // for those entities which were not touched.
    bool forRemoval = false, forReplacement = false, excludeSubshape = false;
    //
    if ( !this->IsAsked(currentShape, true, forRemoval, forReplacement) )
    {
      newResult = currentShape; // Just take the shape itself as a new shape

      // NOTICE: the recursion is stopped here because the assignment above
      //         takes effect for sub-shapes also. This assignment actually
      //         means that at a certain level of topology graph for the
      //         resulting shape, a pointer to a piece of old topology graph
      //         will be used.
    }
    else
    {
      // Now we need to know if the shape itself was asked for modification.
      // If not, and if we are here, it means that one of the sub-shapes was
      // affected, so we just go deeper in the topology graph. If the sub-shape
      // itself was modified, then we need to know what actually happened.
      if ( this->IsAsked(currentShape, forRemoval, forReplacement) )
      {
        if ( forRemoval )
          excludeSubshape = true;
        else // Replacement
          newResult = m_toReplace(currentShape);
      }
      else // Shape itself is not touched, but some children are
      {
        // If any sub-shape or the shape itself was touched, then we have
        // to construct another shape. That's because in OpenCascade it is
        // impossible to modify the existing shape handler.
        newResult = currentShape.EmptyCopied();
      }

      // Proceed recursively
      if ( !forRemoval && !forReplacement )
        this->buildTopoGraphLevel(currentShape, newResult);
    }

    if ( !excludeSubshape )
    {
      // Have a look at the following shitty code. We have to reverse
      // orientation a sub-shape, because TopoDS_Builder will reverse all
      // sub-shapes of a REVERSED parent. To avoid this effect, we corrupt
      // out shape here, in order to let TopoDS_Builder corrupt it again...
      //
      // For more, see issue #29264 in OpenCascade bugtracker.
      if ( result.Orientation() == TopAbs_REVERSED )
        newResult.Reverse();

      BB.Add(result, newResult);
    }
  }
}
