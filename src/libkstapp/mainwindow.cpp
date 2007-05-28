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

#include "mainwindow.h"
#include "kstapplication.h"
#include "boxitem.h"
#include "ellipseitem.h"
#include "labelitem.h"
#include "lineitem.h"
#include "view.h"

#include <QtGui>



// Temporaries
#include <QTableView>
#include "vectormodel.h"

namespace Kst {

MainWindow::MainWindow() {

  _tabWidget = new QTabWidget(this);
  _undoGroup = new QUndoGroup(this);

  connect(_tabWidget, SIGNAL(currentChanged(int)),
          this, SLOT(currentPlotChanged()));

  createPlotView();

  setCentralWidget(_tabWidget);

  createActions();
  createMenus();
//   createToolBars();
  createStatusBar();

  readSettings();
}


MainWindow::~MainWindow() {
}


QUndoGroup *MainWindow::undoGroup() const {
  return _undoGroup;
}


QTabWidget *MainWindow::tabWidget() const {
  return _tabWidget;
}


View *MainWindow::currentPlotView() const {
  return qobject_cast<View*>(_tabWidget->currentWidget());
}


View *MainWindow::createPlotView() {
  View *plotView = new View;
  connect(plotView, SIGNAL(destroyed(QObject*)),
          this, SLOT(plotViewDestroyed(QObject*)));
  _undoGroup->addStack(plotView->undoStack());
  _undoGroup->setActiveStack(plotView->undoStack());

  QString label = plotView->objectName().isEmpty() ?
                  QString("Plot %1").arg(QString::number(_tabWidget->count())) :
                  plotView->objectName();

  _tabWidget->addTab(plotView, label);
  return plotView;
}


void MainWindow::currentPlotChanged() {
  _undoGroup->setActiveStack(currentPlotView()->undoStack());
}


void MainWindow::plotViewDestroyed(QObject *object) {
  View *plotView = qobject_cast<View*>(object);
  _tabWidget->removeTab(_tabWidget->indexOf(plotView));
}


void MainWindow::aboutToQuit() {
  writeSettings();
}


void MainWindow::about() {
  //FIXME Build a proper about box...
  QMessageBox::about(this, tr("About Kst"),
          tr("FIXME."));
}


void MainWindow::createBox() {
  CreateBoxCommand *cmd = new CreateBoxCommand;
  cmd->createItem();
}


void MainWindow::createEllipse() {
  CreateEllipseCommand *cmd = new CreateEllipseCommand;
  cmd->createItem();
}


void MainWindow::createLabel() {
  CreateLabelCommand *cmd = new CreateLabelCommand;
  cmd->createItem();
}


void MainWindow::createLine() {
  CreateLineCommand *cmd = new CreateLineCommand;
  cmd->createItem();
}


void MainWindow::demoModel() {
  QTableView *view = new QTableView;
  KstVectorPtr v = new KstVector;
  v->resize(999999);
  double *d = const_cast<double *>(v->value()); // yay :)
  d[0] = 1;
  for (int i = 1; i < v->length(); ++i) {
    d[i] = d[i-1] + 0.002;
  }
  VectorModel *m = new VectorModel(v);
  view->setModel(m);
  view->resize(300, 500);
  view->show();
}


void MainWindow::createActions() {
  _undoAct = _undoGroup->createUndoAction(this);
  _undoAct->setShortcut(tr("Ctrl+Z"));
  _redoAct = _undoGroup->createRedoAction(this);
  _redoAct->setShortcut(tr("Ctrl+Shift+Z"));

  _createLabelAct = new QAction(tr("&Create label"), this);
  _createLabelAct->setStatusTip(tr("Create a label for the current view"));
  connect(_createLabelAct, SIGNAL(triggered()), this, SLOT(createLabel()));

  _createBoxAct = new QAction(tr("&Create box"), this);
  _createBoxAct->setStatusTip(tr("Create a box for the current view"));
  connect(_createBoxAct, SIGNAL(triggered()), this, SLOT(createBox()));

  _createEllipseAct = new QAction(tr("&Create ellipse"), this);
  _createEllipseAct->setStatusTip(tr("Create an ellipse for the current view"));
  connect(_createEllipseAct, SIGNAL(triggered()), this, SLOT(createEllipse()));

  _createLineAct = new QAction(tr("&Create line"), this);
  _createLineAct->setStatusTip(tr("Create a line for the current view"));
  connect(_createLineAct, SIGNAL(triggered()), this, SLOT(createLine()));

  _exitAct = new QAction(tr("E&xit"), this);
  _exitAct->setShortcut(tr("Ctrl+Q"));
  _exitAct->setStatusTip(tr("Exit the application"));
  connect(_exitAct, SIGNAL(triggered()), this, SLOT(close()));

  _aboutAct = new QAction(tr("&About"), this);
  _aboutAct->setStatusTip(tr("Show Kst's About box"));
  connect(_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  _aboutQtAct = new QAction(tr("About &Qt"), this);
  _aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus() {
  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addSeparator();
  _fileMenu->addAction(_exitAct);

  _editMenu = menuBar()->addMenu(tr("&Edit"));
  _editMenu->addAction(_undoAct);
  _editMenu->addAction(_redoAct);

  _plotMenu = menuBar()->addMenu(tr("&Plot"));
  _plotMenu->addAction(_createLabelAct);
  _plotMenu->addAction(_createBoxAct);
  _plotMenu->addAction(_createEllipseAct);
  _plotMenu->addAction(_createLineAct);

  _settingsMenu = menuBar()->addMenu(tr("&Settings"));

  menuBar()->addSeparator();

  _helpMenu = menuBar()->addMenu(tr("&Help"));
  _helpMenu->addAction(_aboutAct);
  _helpMenu->addAction(_aboutQtAct);

  // FIXME: remove this later.
  QMenu *demoMenu = menuBar()->addMenu("&Demo");
  QAction *demo = new QAction("Vector model", this);
  connect(demo, SIGNAL(triggered()), this, SLOT(demoModel()));
  demoMenu->addAction(demo);
}


void MainWindow::createToolBars() {
  _fileToolBar = addToolBar(tr("File"));
  _editToolBar = addToolBar(tr("Edit"));
}


void MainWindow::createStatusBar() {
  statusBar()->showMessage(tr("Ready"));
}


void MainWindow::readSettings() {
  QSettings settings;
  QPoint pos = settings.value("pos", QPoint(20, 20)).toPoint();
  QSize size = settings.value("size", QSize(800, 600)).toSize();
  resize(size);
  move(pos);
}


void MainWindow::writeSettings() {
  QSettings settings;
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

}

#include "mainwindow.moc"

// vim: ts=2 sw=2 et
