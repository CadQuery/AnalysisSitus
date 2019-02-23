//-----------------------------------------------------------------------------
// Created on: 22 February 2019
//-----------------------------------------------------------------------------
// Copyright (c) 2019-present, Anton Poletaev, Sergey Slyadnev
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

#ifndef asiUI_ItemRoles_HeaderFile
#define asiUI_ItemRoles_HeaderFile

// Qt includes
#pragma warning(push, 0)
#include <QAbstractItemModel>
#pragma warning(pop)

//! Enumerates item roles related to exclusive properties of datum views.
enum DatumViewRole
{
  DatumViewRole_FormattedValue = Qt::DisplayRole,
  DatumViewRole_LocalValue     = Qt::EditRole,
  DatumViewRole_ModelValue     = Qt::UserRole + 100, // model value role
  DatumViewRole_StatusUnits    = Qt::UserRole + 101, // status units role
  DatumViewRole_CopyRole       = Qt::UserRole + 102, // copy value role
  DatumViewRole_LastId         = Qt::UserRole + 103  // id for roles extension
};

//! Enumerates item roles related to exclusive properties of header views.
enum HeaderViewRoles
{
  HeaderViewRole_TextAngle = Qt::UserRole + 50,
  HeaderViewRole_LastId    = Qt::UserRole + 51
  
};

#endif
