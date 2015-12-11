//-----------------------------------------------------------------------------
// Created on: 08 December 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef gui_controls_section_h
#define gui_controls_section_h

// A-Situs includes
#include <analysis_situs.h>

// A-Situs (GUI) includes
#include <gui_line_edit.h>
#include <gui_prim_data_view.h>

// Qt includes
#pragma warning(push, 0)
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#pragma warning(pop)

//! Widget for main controls to handle design of sections.
class gui_controls_section : public QWidget
{
  Q_OBJECT

public:

  gui_controls_section(QWidget* parent = NULL);
  virtual ~gui_controls_section();

public slots:

  void onCreateSection();
  void onBrowserSelection();
  void onValueChanged();

private:

  QHBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    //! Editor IDs.
    enum editor
    {
      edit_Tx = 0,
      edit_Ty,
      edit_Tz,
      edit_Rx,
      edit_Ry,
      edit_Rz,
    //-------------------------------------------------------------------------
      edit_CircRad,
    //-------------------------------------------------------------------------
      LastEditor
    };

  //---------------------------------------------------------------------------
    QPushButton*   pCreateSection; //!< Create new section.
  //---------------------------------------------------------------------------
    gui_line_edit* pTx;            //!< Translation along OX.
    gui_line_edit* pTy;            //!< Translation along OY.
    gui_line_edit* pTz;            //!< Translation along OZ.
    gui_line_edit* pRx;            //!< Rotation around OX.
    gui_line_edit* pRy;            //!< Rotation around OY.
    gui_line_edit* pRz;            //!< Rotation around OZ.
  //---------------------------------------------------------------------------
    gui_line_edit* pCircleRad;     //!< Radius for circular sections.
  //---------------------------------------------------------------------------

    t_widgets() : pCreateSection (NULL),
                  pTx            (NULL),
                  pTy            (NULL),
                  pTz            (NULL),
                  pRx            (NULL),
                  pRy            (NULL),
                  pRz            (NULL),
                  pCircleRad     (NULL)
    {}

    void Release()
    {
      delete pCreateSection; pCreateSection = NULL;
      delete pTx;            pTx            = NULL;
      delete pTy;            pTy            = NULL;
      delete pTz;            pTz            = NULL;
      delete pRx;            pRx            = NULL;
      delete pRy;            pRy            = NULL;
      delete pRz;            pRz            = NULL;
      delete pCircleRad;     pCircleRad     = NULL;
    }
  };

  t_widgets                        m_widgets; //!< Involved widgets.
  std::vector<gui_prim_data_view*> m_DV;      //!< Data-View bridges.

};

#endif
