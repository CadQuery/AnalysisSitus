//-----------------------------------------------------------------------------
// Created on: 03 February 2016
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

// A-Situs includes
#include <asiUI_Common.h>

// Qt includes
#pragma warning(push, 0)
#include <QFileDialog>
#pragma warning(pop)

//! Allows to select filename for graphics format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectGraphicsFile(const OpenSaveAction action)
{
  QString filter = "Graphics (*.svg)";
  //
  return selectFile(filter, "Select graphics file", "Save graphics file", action);
}

//! Allows to select filename for B-Rep format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectBRepFile(const OpenSaveAction action)
{
  QString filter = "BREP (*.brep *.bin *.binbrep)";
  //
  return selectFile(filter, "Select BREP file", "Save BREP file", action);
}

//! Allows to select filename for IGES format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectIGESFile(const OpenSaveAction action)
{
  QString filter = "IGES (*.igs)";
  //
  return selectFile(filter, "Select IGES file", "Save IGES file", action);
}

//! Allows to select filename for STEP format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectSTEPFile(const OpenSaveAction action)
{
  QString filter = "STEP (*.stp *.step)";
  //
  return selectFile(filter, "Select STEP file", "Save STEP file", action);
}

//! Allows to select filename for ply format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectPLYFile(const OpenSaveAction action)
{
  QString filter = "PLY (*.ply)";
  //
  return selectFile(filter, "Select PLY file", "Save PLY file", action);
}

//! Allows to select filename for xbf format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectXBFFile(const OpenSaveAction action)
{
  QString filter = "XBF (*.xbf)";
  //
  return selectFile(filter, "Select XBF file", "Save XBF file", action);
}

//! Allows to select filename for xyz format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectXYZFile(const OpenSaveAction action)
{
  QString filter = "XYZ point cloud (*.xyz)";
  //
  return selectFile(filter, "Select XYZ file", "Save XYZ file", action);
}

//! Allows to select filename for REK (Fraunhofer EZRT) format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectREKFile(const OpenSaveAction action)
{
  QString filter = "REK Fraunhofer EZRT (*.rek)";
  //
  return selectFile(filter, "Select REK file", "Save REK file", action);
}

//! Allows to select filename for OBJ format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectOBJFile(const OpenSaveAction action)
{
  QString filter = "OBJ mesh (*.obj)";
  //
  return selectFile(filter, "Select OBJ file", "Save OBJ file", action);
}

//! Allows to select filename for STL format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectSTLFile(const OpenSaveAction action)
{
  QString filter = "STL mesh (*.stl)";
  //
  return selectFile(filter, "Select STL file", "Save STL file", action);
}

//! Allows to select filename for DOT format.
//! \param action [in] open or save.
//! \return selected filename.
QString asiUI_Common::selectDOTFile(const OpenSaveAction action)
{
  QString filter = "DOT graph (*.dot)";
  //
  return selectFile(filter, "Select DOT file", "Save DOT file", action);
}

//! Selects filename for opening or saving.
//! \param filter    [in] filter for extensions.
//! \param openTitle [in] title for open dialog.
//! \param saveTitle [in] title for save dialog.
//! \param action    [in] open/save action.
//! \return filename selected by user.
QString asiUI_Common::selectFile(const QString&       filter,
                                 const QString&       openTitle,
                                 const QString&       saveTitle,
                                 const OpenSaveAction action)
{
  QString dir;
  QString filename;

  // Open or save
  if ( action == OpenSaveAction_Open )
    filename = QFileDialog::getOpenFileName(nullptr, openTitle, dir, filter, nullptr);
  else
    filename = QFileDialog::getSaveFileName(nullptr, saveTitle, dir, filter, nullptr);

  return filename;
}

//-----------------------------------------------------------------------------

//! Easy accessor for the part shape.
//! \param model  [in]  Data Model instance.
//! \param part_n [out] Part Node.
//! \param part   [out] part shape.
//! \return true if the part shape is accessible and not null.
bool asiUI_Common::PartShape(const Handle(asiEngine_Model)& model,
                             Handle(asiData_PartNode)&      part_n,
                             TopoDS_Shape&                  part)
{
  // Access Geometry Node
  part_n = model->GetPartNode();
  //
  if ( part_n.IsNull() || !part_n->IsWellFormed() )
    return false;

  // Working shape
  part = part_n->GetShape();
  //
  if ( part.IsNull() )
    return false;

  return true;
}
