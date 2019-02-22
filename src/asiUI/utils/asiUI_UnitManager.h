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

#ifndef asiUI_UnitManager_HeaderFile
#define asiUI_UnitManager_HeaderFile

// asiUI includes
#include <asiUI.h>

// asiAlgo includes
#include <asiAlgo_DictionaryUnitSystem.h>

// Qt includes
#pragma warning(push, 0)
#include <QObject>
#include <QPointer>
#pragma warning(pop)

// OCCT includes
#include <TColStd_SequenceOfAsciiString.hxx>

#pragma warning(disable: 4251)

//! This class represents a tool for manipulating customizable system
//! of units, which is used for converting values and generating
//! unit strings for editor widgets.
class asiUI_EXPORT asiUI_UnitManager : public QObject
{
  Q_OBJECT

public:

  asiUI_UnitManager();
  
  virtual bool
    SetUnit(const TCollection_AsciiString& theDimension,
            const TCollection_AsciiString& theUnits);

  TCollection_AsciiString GetUnit(const TCollection_AsciiString& theDimension) const;

  asiAlgo_DictionaryUnitSystem GetUnitSystem() const;

  //! This method should be overriden in order to load unit information
  //! from model on framework request. The method is called when framework
  //! opens a model.
  //! \return true if units succesfully updated,
  //!         otherwise - returns false.
  virtual bool UpdateFromModel() = 0;

signals:

  //! The signal is emitted when unit system changes.
  void UnitSystemChanged();

  //! The signal is emitted when unit system changes.
  //! \param thePrevSystem [in] previous unit system.
  //! \param theNewSystem [in] new unit system.
  void UnitSystemChanged(const asiAlgo_DictionaryUnitSystem& thePrevSystem,
                         const asiAlgo_DictionaryUnitSystem& theNewSystem);

public:

  void UpdateDictionary();

protected:

  virtual bool
    SetUnits(const TColStd_SequenceOfAsciiString& theDimensions,
             const TColStd_SequenceOfAsciiString& theUnits);

private:

  //! Local unit system.
  asiAlgo_DictionaryUnitSystem m_UnitSystem;
};

#pragma warning(default: 4251)

#endif
