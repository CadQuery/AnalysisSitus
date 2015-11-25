//-----------------------------------------------------------------------------
// Created on: 25 November 2015
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://quaoar.su/blog/
//-----------------------------------------------------------------------------

#ifndef gui_control_pane_h
#define gui_control_pane_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#pragma warning(pop)

//! Widget for main controls.
class gui_control_pane : public QWidget
{
Q_OBJECT

public:

  gui_control_pane(QWidget* parent = NULL);

  virtual ~gui_control_pane();

public slots:

  void onLoadPly();

private:

  QString selectPlyFile() const;

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Buttons.
  struct
  {
    QPushButton* LoadPly; //!< Button for ply loading.

    void Release()
    {
      delete LoadPly; LoadPly = NULL;
    }
  } m_buttons;

};

#endif
