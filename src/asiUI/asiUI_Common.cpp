//-----------------------------------------------------------------------------
// Created on: 03 February 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

// A-Situs includes
#include <gui_common.h>

// A-Situs (common) includes
#include <common_facilities.h>

// Qt includes
#include <QFileDialog>

//! Allows to select filename for B-Rep format.
//! \param action [in] open or save.
//! \return selected filename.
QString gui_common::selectBRepFile(const OpenSaveAction action)
{
  QStringList filter;
  filter << "B-Rep (*.brep)";
  //
  return selectFile(filter, "Select B-Rep file", "Save B-Rep file", action);
}

//! Allows to select filename for IGES format.
//! \param action [in] open or save.
//! \return selected filename.
QString gui_common::selectIGESFile(const OpenSaveAction action)
{
  QStringList filter;
  filter << "IGES (*.igs)";
  //
  return selectFile(filter, "Select IGES file", "Save IGES file", action);
}

//! Allows to select filename for STEP format.
//! \param action [in] open or save.
//! \return selected filename.
QString gui_common::selectSTEPFile(const OpenSaveAction action)
{
  QStringList filter;
  filter << "STEP (*.stp)";
  //
  return selectFile(filter, "Select STEP file", "Save STEP file", action);
}

//! Allows to select filename for ply format.
//! \param action [in] open or save.
//! \return selected filename.
QString gui_common::selectPlyFile(const OpenSaveAction action)
{
  QStringList filter;
  filter << "PLY (*.ply)";
  //
  return selectFile(filter, "Select PLY file", "Save PLY file", action);
}

//! Allows to select filename for xbf format.
//! \param action [in] open or save.
//! \return selected filename.
QString gui_common::selectXBFFile(const OpenSaveAction action)
{
  QStringList filter;
  filter << "XBF (*.xbf)";
  //
  return selectFile(filter, "Select XBF file", "Save XBF file", action);
}

//! Allows to select filename for xyz format.
//! \param action [in] open or save.
//! \return selected filename.
QString gui_common::selectXYZFile(const OpenSaveAction action)
{
  QStringList filter;
  filter << "XYZ point cloud (*.xyz)";
  //
  return selectFile(filter, "Select XYZ file", "Save XYZ file", action);
}

//! Allows to select filename for OBJ format.
//! \param action [in] open or save.
//! \return selected filename.
QString gui_common::selectOBJFile(const OpenSaveAction action)
{
  QStringList filter;
  filter << "OBJ mesh (*.obj)";
  //
  return selectFile(filter, "Select OBJ file", "Save OBJ file", action);
}

//! Allows to select filename for STL format.
//! \param action [in] open or save.
//! \return selected filename.
QString gui_common::selectSTLFile(const OpenSaveAction action)
{
  QStringList filter;
  filter << "STL mesh (*.stl)";
  //
  return selectFile(filter, "Select STL file", "Save STL file", action);
}

//! Selects filename for opening or saving.
//! \param filter    [in] filter for extensions.
//! \param openTitle [in] title for open dialog.
//! \param saveTitle [in] title for save dialog.
//! \param action    [in] open/save action.
//! \return filename selected by user.
QString gui_common::selectFile(const QStringList&   filter,
                               const QString&       openTitle,
                               const QString&       saveTitle,
                               const OpenSaveAction action)
{
  QString dir;
  QString filename;

  // Open or save
  if ( action == OpenSaveAction_Open )
    filename = QFileDialog::getOpenFileName(NULL, openTitle, dir, filter.join(";;"), NULL);
  else
    filename = QFileDialog::getSaveFileName(NULL, saveTitle, dir, filter.join(";;"), NULL);

  return filename;
}

//-----------------------------------------------------------------------------

//! Easy accessor for the part shape.
//! \param part [out] part shape.
//! \return true if the part shape is accessible and not null.
bool gui_common::PartShape(TopoDS_Shape& part)
{
  // Access Geometry Node
  Handle(asiData_PartNode) N = common_facilities::Instance()->Model->GetPartNode();
  if ( N.IsNull() || !N->IsWellFormed() )
    return false;

  // Working shape
  part = N->GetShape();
  //
  if ( part.IsNull() )
  {
    std::cout << "Error: part shape is null" << std::endl;
    return false;
  }
  return true;
}
