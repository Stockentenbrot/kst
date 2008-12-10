/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "viewitemdialog.h"

#include "viewitem.h"
#include "filltab.h"
#include "stroketab.h"
#include "layouttab.h"
#include "childviewoptionstab.h"
#include "dimensionstab.h"
#include "dialogpage.h"
#include "viewgridlayout.h"

#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QtGlobal>

namespace Kst {

ViewItemDialog::ViewItemDialog(ViewItem *item, QWidget *parent)
    : Dialog(parent), _item(item) {

  setWindowTitle(tr("Edit View Item"));

  _fillTab = new FillTab(this);
  _strokeTab = new StrokeTab(this);
  _layoutTab = new LayoutTab(this);
//   _childViewOptionsTab = new ChildViewOptionsTab(this);
  connect(_fillTab, SIGNAL(apply()), this, SLOT(fillChanged()));
  connect(_strokeTab, SIGNAL(apply()), this, SLOT(strokeChanged()));
  connect(_layoutTab, SIGNAL(apply()), this, SLOT(layoutChanged()));
//   connect(_childViewOptionsTab, SIGNAL(apply()), this, SLOT(childViewOptionsChanged()));

  DialogPageTab *page = new DialogPageTab(this);
  page->setPageTitle(tr("Appearance"));
  page->addDialogTab(_fillTab);
  page->addDialogTab(_strokeTab);
  page->addDialogTab(_layoutTab);
//   page->addDialogTab(_childViewOptionsTab);
  addDialogPage(page);

  _dimensionsTab = new DimensionsTab(_item, this);
  DialogPage *dimensionsPage = new DialogPage(this);
  dimensionsPage->setPageTitle(tr("Dimensions"));
  dimensionsPage->addDialogTab(_dimensionsTab);
  addDialogPage(dimensionsPage);
  connect(_dimensionsTab, SIGNAL(apply()), this, SLOT(dimensionsChanged()));

  QList<DialogPage*> dialogPages = _item->dialogPages();
  foreach (DialogPage *dialogPage, dialogPages)
    addDialogPage(dialogPage);

  setupFill();
  setupStroke();
  setupLayout();
//   setupChildViewOptions();
  setupDimensions();

  connect(_dimensionsTab, SIGNAL(tabModified()), this, SLOT(modified()));
}


ViewItemDialog::~ViewItemDialog() {
}


void ViewItemDialog::setupFill() {
  Q_ASSERT(_item);
  QBrush b = _item->brush();

  _fillTab->setColor(b.color());
  _fillTab->setStyle(b.style());

  if (const QGradient *gradient = b.gradient()) {
    _fillTab->setGradient(*gradient);
  }
}


void ViewItemDialog::setupStroke() {
  Q_ASSERT(_item);
  QPen p = _item->pen();
  QBrush b = p.brush();

  _strokeTab->setStyle(p.style());
  _strokeTab->setWidth(p.widthF());

  _strokeTab->setBrushColor(b.color());
  _strokeTab->setBrushStyle(b.style());

  _strokeTab->setJoinStyle(p.joinStyle());
  _strokeTab->setCapStyle(p.capStyle());
}


void ViewItemDialog::setupLayout() {
  Q_ASSERT(_item);
/*  ViewGridLayout *layout = _item->layout();

  _layoutTab->setLockAspectRatio(_item->lockAspectRatio());
  _layoutTab->enableLockAspectRatio(!_item->lockAspectRatioFixed());

  if (!layout) {
    _layoutTab->setLayoutEnabled(false);
    return;
  }

  _layoutTab->setHorizontalMargin(layout->margin().width());
  _layoutTab->setVerticalMargin(layout->margin().height());
  _layoutTab->setHorizontalSpacing(layout->spacing().width());
  _layoutTab->setVerticalSpacing(layout->spacing().height());*/
}


void ViewItemDialog::setupDimensions() {
  _dimensionsTab->setupDimensions();
}


void ViewItemDialog::fillChanged() {
  Q_ASSERT(_item);

  QBrush b = _item->brush();

  b.setColor(_fillTab->color());
  b.setStyle(_fillTab->style());

  QGradient gradient = _fillTab->gradient();
  if (gradient.type() != QGradient::NoGradient) {
    b = QBrush(gradient);
  }

  _item->setBrush(b);
}


void ViewItemDialog::strokeChanged() {
  Q_ASSERT(_item);
  QPen p = _item->pen();
  QBrush b = p.brush();

  p.setStyle(_strokeTab->style());
  p.setWidthF(_strokeTab->width());

  b.setColor(_strokeTab->brushColor());
  b.setStyle(_strokeTab->brushStyle());

  p.setJoinStyle(_strokeTab->joinStyle());
  p.setCapStyle(_strokeTab->capStyle());
  p.setBrush(b);

  _item->setPen(p);
}


void ViewItemDialog::layoutChanged() {
  Q_ASSERT(_item);
/*  ViewGridLayout *layout = _item->layout();

  _item->setLockAspectRatio(_layoutTab->lockAspectRatio());
  if (!layout) {
    _layoutTab->setLayoutEnabled(false);
    return;
  }

  layout->setMargin(QSizeF(_layoutTab->horizontalMargin(),
                           _layoutTab->verticalMargin()));
  layout->setSpacing(QSizeF(_layoutTab->horizontalSpacing(),
                            _layoutTab->verticalSpacing()));
  layout->update();*/
}

void ViewItemDialog::dimensionsChanged() {
  Q_ASSERT(_item);

  double pw; // parent width
  double ph; // parent height
  double ptlx; // parent top left x
  double ptly; // parent top left y
  double r; // rotation
  double w; // width
  double h; // height
  double x;
  double y;

  if (_item->parentViewItem()) {
    pw = _item->parentViewItem()->width();
    ph = _item->parentViewItem()->height();
    ptlx = _item->parentViewItem()->rect().topLeft().x();
    ptly = _item->parentViewItem()->rect().topLeft().y();
  } else if (_item->parentView()) {
    pw = _item->parentView()->width();
    ph = _item->parentView()->height();
    ptlx = _item->parentView()->rect().topLeft().x();
    ptly = _item->parentView()->rect().topLeft().y();
  } else {
    Q_ASSERT_X(false,"parent test", "item has no parentview item");
    pw = ph = ptlx = ptly = 1.0;
  }

  if (rect().width()>0) {
    r = double(_item->rect().height()) / double(_item->rect().width());
  } else {
    r = 10000.0;
  }

  w = _dimensionsTab->w() * pw;
  if (_dimensionsTab->fixedAspect()) {
    h = w*r;
    _item->setLockAspectRatio(true);
  } else {
    h = _dimensionsTab->h() * ph;
    _item->setLockAspectRatio(false);
  }
  x = _dimensionsTab->x() * pw + ptlx - w/2.0;
  y = _dimensionsTab->y() * ph + ptly - h/2.0;

  _item->setViewRect(0,0,w,h);
  _item->setPos(x,y);

  QTransform t;
  QPointF origin = _item->centerOfRotation();
  t.translate(origin.x(), origin.y());
  t.rotate(_dimensionsTab->r());
  t.translate(-origin.x(), -origin.y());

  _item->setTransform(t);
  _item->updateRelativeSize();
}


void ViewItemDialog::setupChildViewOptions() {
  _childViewOptionsTab->setShareAxis(_item->shareAxis());
}


void ViewItemDialog::childViewOptionsChanged() {
  _item->setShareAxis(_childViewOptionsTab->shareAxis());
}


}

// vim: ts=2 sw=2 et
