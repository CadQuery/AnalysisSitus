//-----------------------------------------------------------------------------
// Created on: 20 April 2016
// Created by: Sergey SLYADNEV
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/
//-----------------------------------------------------------------------------

#ifndef gui_dialog_decomp_generate_h
#define gui_dialog_decomp_generate_h

// A-Situs includes
#include <analysis_situs.h>

// Geometry includes
#include <geom_volume_node.h>

// Qt includes
#pragma warning(push, 0)
#include <QDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#pragma warning(pop)

//! Dialog for generating B-Rep model from a stock shape and a set of the
//! recognized features.
class gui_dialog_decomp_generate : public QDialog
{
  Q_OBJECT

public:

  gui_dialog_decomp_generate(const Handle(geom_volume_node)& Node,
                             QWidget*                        parent = NULL);

  virtual ~gui_dialog_decomp_generate();

public slots:

  void onMove2Exclude();
  void onMove2Keep();
  void onGenerate();
  void onFeatureSelected();

protected:

  void populateFeatureLists();
  void populateFeatureList(const Handle(ActAPI_HDataCursorList)& features,
                           QListWidget*                          widget);

protected:

  //! Widgets.
  struct t_widgets
  {
  //---------------------------------------------------------------------------
    QListWidget* pFeatures2Keep;    //!< Features to keep in the model.
    QListWidget* pFeatures2Exclude; //!< Features to exclude from the model.
    QPushButton* pMove2ExcludeList; //!< Button to move an item from one list to another.
    QPushButton* pMove2KeepList;    //!< Button to move an item from one list to another.
    QPushButton* pGenerate;         //!< Button to generate part.
  //---------------------------------------------------------------------------

    t_widgets() : pFeatures2Keep    (NULL),
                  pFeatures2Exclude (NULL),
                  pMove2ExcludeList (NULL),
                  pMove2KeepList    (NULL),
                  pGenerate         (NULL)
    {}

    void Release()
    {
      delete pFeatures2Keep;    pFeatures2Keep    = NULL;
      delete pFeatures2Exclude; pFeatures2Exclude = NULL;
      delete pGenerate;         pGenerate         = NULL;
      delete pMove2KeepList;    pMove2KeepList    = NULL;
      delete pMove2ExcludeList; pMove2ExcludeList = NULL;
    }
  };

  t_widgets                m_widgets;     //!< UI controls.
  QVBoxLayout*             m_pMainLayout; //!< Layout of the widget.
  Handle(geom_volume_node) m_volume;      //!< Volume Node.

};

#endif
