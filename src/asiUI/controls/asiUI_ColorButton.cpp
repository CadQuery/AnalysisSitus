//-----------------------------------------------------------------------------
// Created on: 23 August 2017
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
//    * Neither the name of Sergey Slyadnev nor the
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
// Implementation is inspired by QtxColorButton class from
// SALOME platform (see http://www.salome-platform.org/).
//-----------------------------------------------------------------------------

#pragma warning(disable : 4718)

// Own include
#include <asiUI_ColorButton.h>

// Qt includes
#pragma warning(push, 0)
#include <QMenu>
#include <QStyle>
#include <QLayout>
#include <QBitmap>
#include <QPainter>
#include <QPaintEvent>
#include <QColorDialog>
#include <QStyleOptionToolButton>
#pragma warning(pop)

//-----------------------------------------------------------------------------

asiUI_ColorButton::asiUI_ColorButton(QWidget* parent) : QToolButton(parent)
{
  this->setCheckable(false);
  this->setPopupMode(MenuButtonPopup);

  QMenu* pm = new QMenu(this);
  QGridLayout* grid = new QGridLayout(pm);
  grid->setMargin(5);
  grid->setSpacing(1);

  QList<QColor> cList = this->colorsList();
  int w = 8;
  int h = cList.count() / w;

  for ( int y = 0; y < h; y++ )
  {
    for ( int x = 0; x < w; x++ )
    {
      QColor c = cList.at(x*h + y).toRgb();
      QToolButton* btn = new QToolButton(pm);

      btn->setAutoRaise(true);
      btn->setCheckable(true);
      m_colors.insert(btn, c);
      grid->addWidget(btn, y + 1, x);

      btn->installEventFilter(this);

      connect( btn, SIGNAL( clicked(bool) ), this, SLOT( onToggled(bool) ) );

      this->updateButton(btn);
    }
  }

  QToolButton* other = new QToolButton(pm);
  other->setText("Other colors...");
  other->setAutoRaise(true);
  other->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  grid->addWidget( other, grid->rowCount(), 0, 1, grid->columnCount() );

  connect( other, SIGNAL( clicked(bool) ), this, SLOT( onDialogClicked(bool) ) );

  other->installEventFilter(this);

  this->setMenu(pm);

  connect( this, SIGNAL( clicked(bool) ), this, SLOT( onClicked(bool) ) );
  connect( pm,   SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ) );
}

//-----------------------------------------------------------------------------

asiUI_ColorButton::~asiUI_ColorButton()
{}

//-----------------------------------------------------------------------------

QColor asiUI_ColorButton::color() const
{
  return m_colors.contains(this) ? m_colors[this] : QColor();
}

//-----------------------------------------------------------------------------

void asiUI_ColorButton::setColor( const QColor& c )
{
  m_colors.insert(this, c);
  this->updateState();
  this->update();
  this->updateButton(this);
}

//-----------------------------------------------------------------------------

bool asiUI_ColorButton::eventFilter(QObject* o, QEvent* e)
{
  if ( e->type() == QEvent::Leave )
    this->updateButton( ::qobject_cast<QToolButton*>(o) );

  return QToolButton::eventFilter(o, e);
}

//-----------------------------------------------------------------------------

void asiUI_ColorButton::onAboutToShow()
{
  this->updateState();
}

//-----------------------------------------------------------------------------

void asiUI_ColorButton::onClicked(bool)
{
  emit clicked( this->color() );
}

//-----------------------------------------------------------------------------

void asiUI_ColorButton::onToggled(bool on)
{
  const QToolButton* tb = ::qobject_cast<QToolButton*>( this->sender() );
  if ( !tb )
    return;

  QColor old = this->color();

  if ( on && m_colors.contains(tb) )
  {
    m_colors.insert( this, m_colors[tb] );
    this->updateButton(this);
  }

  if ( this->menu() )
    this->menu()->hide();

  this->updateState();

  if ( old != this->color() )
    emit changed( this->color() );
}

//-----------------------------------------------------------------------------

void asiUI_ColorButton::onDialogClicked(bool)
{
  QColor c = QColorDialog::getColor(this->color(), this);
  //
  if ( !c.isValid() )
    return;

  QColor old = this->color();

  this->setColor(c);

  if ( old != this->color() )
    emit changed( this->color() );
}

//-----------------------------------------------------------------------------

void asiUI_ColorButton::paintEvent(QPaintEvent* e)
{
  QToolButton::paintEvent(e);

  //QStyleOptionToolButton opt;
  //opt.initFrom(this);
  //opt.text     = this->text();
  //opt.icon     = this->icon();
  //opt.features = QStyleOptionToolButton::Menu;

  //QRect r = this->style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButton);
  ////
  //r.setTopLeft( r.topLeft() + QPoint(2, 2) );
  //r.setBottomRight( r.bottomRight() - QPoint(2, 2) );

  //QPixmap pix( r.size() );
  //pix.fill( this->palette().color( this->backgroundRole() ) );

  //if ( this->color().isValid() )
  //{
  //  this->drawColor( &pix, this->color() );
  //}
  //else
  //{
  //  QPainter pixp(&pix);
  //  pixp.drawRect( 2, 2, pix.width() - 4, pix.height() - 4 );
  //  pixp.fillRect( 3, 3, pix.width() - 6, pix.height() - 6, QBrush(Qt::BDiagPattern) );
  //  pixp.end();
  //}

  //QPainter p(this);
  //p.drawPixmap(r, pix);
  //p.end();
}

//-----------------------------------------------------------------------------

void asiUI_ColorButton::updateState()
{
  QList<QToolButton*> bList = this->menu()->findChildren<QToolButton*>();
  //
  for ( QList<QToolButton*>::iterator cit = bList.begin(); cit != bList.end(); ++cit )
    updateButton( *cit );
}

//-----------------------------------------------------------------------------

void asiUI_ColorButton::updateButton(QToolButton* btn)
{
  QColor c = this->color().toRgb();
  bool block = btn->signalsBlocked();
  btn->blockSignals(true);
  btn->setChecked(false);

  if ( m_colors.contains(btn) )
  {
    btn->setIcon( this->buttonIcon( m_colors[btn] ) );
    btn->setChecked( m_colors[btn].toRgb() == c );
  }

  btn->setDown(false);
  btn->blockSignals(block);
}

//-----------------------------------------------------------------------------

QPixmap asiUI_ColorButton::buttonIcon(const QColor& c) const
{
  static QMap<int, QPixmap> pixMap;

  if ( pixMap.contains( c.rgb() ) )
    return pixMap[c.rgb()];

  QPixmap pix(16, 16);

  QColor bg = Qt::white;
  if ( bg == c )
    bg = Qt::gray;

  pix.fill(bg);

  this->drawColor( &pix, c );

  pix.setMask( pix.createHeuristicMask() );

  pixMap.insert( c.rgb(), pix );

  return pix;
}

//-----------------------------------------------------------------------------

void asiUI_ColorButton::drawColor(QPaintDevice* pd,
                                  const QColor& c,
                                  const int     m) const
{
  if ( !pd )
    return;

  QPainter p(pd);
  p.setPen(Qt::black);
  p.fillRect( m, m, pd->width()-2*m-1, pd->height()-2*m-1, QBrush(c) );
  p.drawRect( m, m, pd->width()-2*m-1, pd->height()-2*m-1 );
  p.end();
}

//-----------------------------------------------------------------------------

QList<QColor> asiUI_ColorButton::colorsList() const
{
  QList<QColor> lst;

  for ( int g = 0; g < 4; g++ )
  {
    for ( int r = 0; r < 4; r++ )
    {
      for ( int b = 0; b < 3; b++ )
        lst.append( QColor( qRgb( r*255/3, g*255/3, b*255/2 ) ) );
    }
  }
  return lst;
}

#pragma warning(default : 4718)
