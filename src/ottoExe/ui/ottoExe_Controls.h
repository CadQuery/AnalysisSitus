//-----------------------------------------------------------------------------
// Created on: 16 February 2019
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef ottoExe_Controls_h
#define ottoExe_Controls_h

// Qt includes
#pragma warning(push, 0)
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//-----------------------------------------------------------------------------

//! Widget for Ottobock Workbench controls.
class ottoExe_Controls : public QWidget
{
  Q_OBJECT

public:

  ottoExe_Controls(QWidget* parent = NULL);
  virtual ~ottoExe_Controls();

public slots:

  void onShowGraph();

private:

  QVBoxLayout* m_pMainLayout; //!< Layout of the widget.

  //! Widgets.
  struct t_widgets
  {
    QPushButton* pShowGraph; //!< Shows dependency graph.

    t_widgets() : pShowGraph (NULL)
    {}

    void Release()
    {
      delete pShowGraph; pShowGraph = NULL;
    }
  };

  //! Involved widgets.
  t_widgets m_widgets;


};

#endif
