//-----------------------------------------------------------------------------
// Created on: 23 June 2017
// Created by: Quaoar
//-----------------------------------------------------------------------------
// Web: http://dev.opencascade.org/, http://quaoar.su/blog
//-----------------------------------------------------------------------------

// Own include
#include <asiUI_DialogOCAFDump.h>

// Active Data includes
#include <ActData_CAFDumper.h>

// Qt includes
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>

//-----------------------------------------------------------------------------

#define BTN_MIN_WIDTH 120

//-----------------------------------------------------------------------------
// Construction & destruction
//-----------------------------------------------------------------------------

//! Constructor.
//! \param model    [in] Data Model instance.
//! \param notifier [in] progress notifier.
//! \param parent   [in] parent widget.
asiUI_DialogOCAFDump::asiUI_DialogOCAFDump(const Handle(ActAPI_IModel)&            model,
                                           const Handle(ActAPI_IProgressNotifier)& notifier,
                                           QWidget*                                parent)
: QDialog    (parent),
  m_model    (model),
  m_notifier (notifier)
{
  // Main layout
  m_pMainLayout = new QVBoxLayout();

  // Group box for parameters
  QGroupBox* pGroup = new QGroupBox("OCAF dump");

  // Editors
  m_widgets.pEditor = new QTextEdit();
  //
  QVBoxLayout* boxLayout = new QVBoxLayout(pGroup);
  boxLayout->addWidget(m_widgets.pEditor);

  // Style
  m_widgets.pEditor->setStyleSheet( QString::fromUtf8("background-color: rgb(30, 30, 30); color: rgb(230, 230, 230); font-size: 10pt") );

  QTextCharFormat fmt = m_widgets.pEditor->currentCharFormat();
  //
  fmt.setFontFixedPitch(true);
  fmt.setFontStyleHint(QFont::Monospace);
  fmt.setFontFamily("monospace");
  //
  m_widgets.pEditor->setCurrentCharFormat(fmt);

  //---------------------------------------------------------------------------
  // Buttons
  //---------------------------------------------------------------------------

  m_widgets.pClose = new QPushButton("Close");

  // Sizing
  m_widgets.pClose->setMaximumWidth(BTN_MIN_WIDTH);

  // Reaction
  connect( m_widgets.pClose, SIGNAL( clicked() ), this, SLOT( onClose() ) );

  //---------------------------------------------------------------------------
  // Layout
  //---------------------------------------------------------------------------

  // Configure main layout
  m_pMainLayout->addWidget(pGroup);
  m_pMainLayout->addWidget(m_widgets.pClose);
  m_pMainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  m_pMainLayout->setContentsMargins(10, 10, 10, 10);

  this->setLayout(m_pMainLayout);
  this->setWindowModality(Qt::WindowModal);
  this->setWindowTitle("OCAF dump");

  // Set good initial size
  this->setMinimumSize( QSize(900, 600) );

  //---------------------------------------------------------------------------
  // Fill editor with data
  //---------------------------------------------------------------------------

  this->initialize();
}

//! Destructor.
asiUI_DialogOCAFDump::~asiUI_DialogOCAFDump()
{
  delete m_pMainLayout;
  m_widgets.Release();
}

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------

//! Fills editor with OCAF dump.
void asiUI_DialogOCAFDump::initialize()
{
  std::ostringstream buff;

  if ( !ActData_CAFDumper::Dump(buff, m_model) )
    m_widgets.pEditor->setText("### FAILED TO DUMP");
  else
    m_widgets.pEditor->setText( buff.str().c_str() );
}

//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

//! Reaction on close.
void asiUI_DialogOCAFDump::onClose()
{
  this->close();
}
