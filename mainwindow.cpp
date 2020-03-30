/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "arrow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QtWidgets>
#include <QTextToSpeech>
#include <QLoggingCategory>
#include <QLabel>
#include <QtPrintSupport/QPrinter>
const int InsertTextButton = 10;

//! [0]
MainWindow::MainWindow():  m_speech(0)
{

    m_speech = new QTextToSpeech(this);
    m_speech->setLocale(QLocale::English);
    m_speech->setVolume(0);


   QLoggingCategory::setFilterRules(QStringLiteral("qt.speech.tts=true \n qt.speech.tts.*=true"));
   this->setWindowIcon(QIcon(":/sh.jpg"));
    undoStack = new QUndoStack(this);
    createActions();
    createToolBox();
    createMenus();



    scene = new DiagramScene(itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
    connect(scene, SIGNAL(itemInserted(DiagramItem*)),
            this, SLOT(itemInserted(DiagramItem*)));
    connect(scene, SIGNAL(textInserted(QGraphicsTextItem*)),
            this, SLOT(textInserted(QGraphicsTextItem*)));
    connect(scene, SIGNAL(itemSelected(QGraphicsItem*)),
            this, SLOT(itemSelected(QGraphicsItem*)));


    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    toolBox->setObjectName("toolb");
    layout->addWidget(toolBox);
    view = new QGraphicsView(scene);
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle(tr("Shackles"));
    setUnifiedTitleAndToolBarOnMac(true);
    this->setStyleSheet(loadStyleSheet());
}
//! [0]

//! [1]
void MainWindow::backgroundButtonGroupClicked(QAbstractButton *button)
{
    QList<QAbstractButton *> buttons = backgroundButtonGroup->buttons();
    foreach (QAbstractButton *myButton, buttons) {
        if (myButton != button)
            button->setChecked(false);
    }
    QString text = button->text();
    if (text == tr("Blue Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background1.png"));
    else if (text == tr("White Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background2.png"));
    else if (text == tr("Gray Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background3.png"));
    else
        scene->setBackgroundBrush(QPixmap(":/images/background4.png"));

    scene->update();
    view->update();
}
//! [1]

//! [2]
void MainWindow::buttonGroupClicked(int id)
{
    QList<QAbstractButton *> buttons = buttonGroup->buttons();
    foreach (QAbstractButton *button, buttons) {
        if (buttonGroup->button(id) != button)
            button->setChecked(false);
    }
    if (id == InsertTextButton) {
        scene->setMode(DiagramScene::InsertText);
        m_speech->say("Text");
    } else {
        scene->setItemType(DiagramItem::DiagramType(id));
if(volume->value()!=0)
        switch (id) {
            case DiagramItem::StartEnd:
            m_speech->say("StartEnd");
                break;
            case DiagramItem::Conditional:
              m_speech->say("Conditional");
                break;
            case DiagramItem::Step:
               m_speech->say("Process");
                break;
            case DiagramItem::Preparation:
                m_speech->say("Preparation");
                break;
            case DiagramItem::OffPageConnector:
                 m_speech->say("OffPageConnector");
                 break;
            case DiagramItem::Terminal:
                   m_speech->say("Terminal");
                  break;
             case DiagramItem::Connector:
                  m_speech->say("Connector");
             break;
            default:
               m_speech->say("Item");
                break;

}
        scene->setMode(DiagramScene::InsertItem);
    }
}
//! [2]

//! [3]
void MainWindow::deleteItem()
{
    foreach (QGraphicsItem *item, scene->selectedItems()) {
        if (item->type() == Arrow::Type) {
            scene->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
            delete item;
        }
    }

    foreach (QGraphicsItem *item, scene->selectedItems()) {
         if (item->type() == DiagramItem::Type)
             qgraphicsitem_cast<DiagramItem *>(item)->removeArrows();
         scene->removeItem(item);
         delete item;
     }
}
//! [3]

//! [4]
void MainWindow::pointerGroupClicked(int)
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}
//! [4]

//! [5]
void MainWindow::bringToFront()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}
//! [5]

//! [6]
void MainWindow::sendToBack()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() <= zValue && item->type() == DiagramItem::Type)
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}
//! [6]

//! [7]
void MainWindow::itemInserted(DiagramItem *item)
{
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
    buttonGroup->button(int(item->diagramType()))->setChecked(false);
}
//! [7]

//! [8]
void MainWindow::textInserted(QGraphicsTextItem *)
{
    buttonGroup->button(InsertTextButton)->setChecked(false);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}
//! [8]

//! [9]
void MainWindow::currentFontChanged(const QFont &)
{
    handleFontChange();
}
//! [9]

//! [10]
void MainWindow::fontSizeChanged(const QString &)
{
    handleFontChange();
}
//! [10]

//! [11]
void MainWindow::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = view->matrix();
    view->resetMatrix();
    view->translate(oldMatrix.dx(), oldMatrix.dy());
    view->scale(newScale, newScale);
}
//! [11]

//! [12]
void MainWindow::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/textpointer.png",
                                     qvariant_cast<QColor>(textAction->data())));
    textButtonTriggered();
}
//! [12]

//! [13]
void MainWindow::itemColorChanged()
{
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/floodfill.png",
                                     qvariant_cast<QColor>(fillAction->data())));
    fillButtonTriggered();
}
//! [13]

//! [14]
void MainWindow::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png",
                                     qvariant_cast<QColor>(lineAction->data())));
    lineButtonTriggered();
}
//! [14]

//! [15]
void MainWindow::textButtonTriggered()
{
    scene->setTextColor(qvariant_cast<QColor>(textAction->data()));
}
//! [15]

//! [16]
void MainWindow::fillButtonTriggered()
{
    scene->setItemColor(qvariant_cast<QColor>(fillAction->data()));
}
//! [16]

//! [17]
void MainWindow::lineButtonTriggered()
{
    scene->setLineColor(qvariant_cast<QColor>(lineAction->data()));
}
//! [17]

//! [18]
void MainWindow::handleFontChange()
{
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    font.setItalic(italicAction->isChecked());
    font.setUnderline(underlineAction->isChecked());

    scene->setFont(font);
}
//! [18]

//! [19]
void MainWindow::itemSelected(QGraphicsItem *item)
{
    DiagramTextItem *textItem =
    qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    boldAction->setChecked(font.weight() == QFont::Bold);
    italicAction->setChecked(font.italic());
    underlineAction->setChecked(font.underline());
}
//! [19]

//! [20]
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Diagram Scene"),
                       tr("The <b>Diagram Scene</b> example shows "
                          "use of the graphics framework."));
}
//! [20]

//! [21]
void MainWindow::createToolBox()
{
    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false);
    connect(buttonGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(buttonGroupClicked(int)));
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(createCellWidget(tr("Conditional"), DiagramItem::Conditional), 0, 0);
    layout->addWidget(createCellWidget(tr("Process"), DiagramItem::Step),0, 1);
    layout->addWidget(createCellWidget(tr("Input/Output"), DiagramItem::Io), 1, 0);
    layout->addWidget(createCellWidget(tr("Preparation"),DiagramItem::Preparation),2,0);
    layout->addWidget(createCellWidget(tr("OffPageConnector"),DiagramItem::OffPageConnector),2,1);
    layout->addWidget(createCellWidget(tr("Terminal"),DiagramItem::Terminal),3,0);
    layout->addWidget(createCellWidget(tr("Connector"),DiagramItem::Connector),3,1);

//! [21]

    QToolButton *textButton = new QToolButton;
    textButton->setCheckable(true);
    buttonGroup->addButton(textButton, InsertTextButton);
    textButton->setIcon(QIcon(QPixmap(":/images/textpointer.png")));
    textButton->setIconSize(QSize(50, 50));
    QGridLayout *textLayout = new QGridLayout;
    textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
    textLayout->addWidget(new QLabel(tr("Text")), 1, 0, Qt::AlignCenter);
    QWidget *textWidget = new QWidget;
    textWidget->setLayout(textLayout);
    textWidget->setObjectName("textb");
    layout->addWidget(textWidget, 1, 1);

    layout->setRowStretch(3, 10);
    layout->setColumnStretch(2, 10);

    QWidget *itemWidget = new QWidget;
    itemWidget->setObjectName("Ite1");
    itemWidget->setLayout(layout);

    backgroundButtonGroup = new QButtonGroup(this);
    connect(backgroundButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(backgroundButtonGroupClicked(QAbstractButton*)));

    QGridLayout *backgroundLayout = new QGridLayout;
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Blue Grid"),
                                                           ":/images/background1.png"), 0, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("White Grid"),
                                                           ":/images/background2.png"), 0, 1);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Gray Grid"),
                                                           ":/images/background3.png"), 1, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("No Grid"),
                                                           ":/images/background4.png"), 1, 1);

    backgroundLayout->setRowStretch(3, 10);
    backgroundLayout->setColumnStretch(2, 10);

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setObjectName("Ite2");
    backgroundWidget->setLayout(backgroundLayout);


//! [22]
    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());
    toolBox->addItem(itemWidget, tr("Basic Flowchart Shapes"));
    toolBox->addItem(backgroundWidget, tr("Backgrounds"));
}
//! [22]

//! [23]
void MainWindow::createActions()
{
    toFrontAction = new QAction(QIcon(":/images/bringtofront.png"),
                                tr("Bring to &Front"), this);
    toFrontAction->setShortcut(tr("Ctrl+F"));
    toFrontAction->setStatusTip(tr("Bring item to front"));
    connect(toFrontAction, SIGNAL(triggered()), this, SLOT(bringToFront()));
//! [23]

    sendBackAction = new QAction(QIcon(":/images/sendtoback.png"), tr("Send to &Back"), this);
    sendBackAction->setShortcut(tr("Ctrl+B"));
    sendBackAction->setStatusTip(tr("Send item to back"));
    connect(sendBackAction, SIGNAL(triggered()), this, SLOT(sendToBack()));

    deleteAction = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));


    exitAction =new QAction(tr("E&xit"),this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit Scenediagram example"));
    connect(exitAction,SIGNAL(triggered()),this,SLOT(close()));

    saveAction =new QAction(tr("S&ave"),this);
    saveAction->setShortcut(tr("Ctrl+S"));
    saveAction->setStatusTip(tr("Save flowchart"));
    connect(saveAction,SIGNAL(triggered()),this,SLOT(saveFile()));

    openAction =new QAction(tr("O&pen"),this);
    openAction->setShortcut(tr("Ctrl+O"));
    openAction->setStatusTip(tr("Open file"));
    connect(openAction,SIGNAL(triggered()),this,SLOT(openFile()));

    saveAsAction =new QAction(tr("S&ave As"),this);
    saveAsAction->setShortcut(tr("Ctrl+T"));
    saveAsAction->setStatusTip(tr("Save as"));
    connect(saveAsAction,SIGNAL(triggered()),this,SLOT(fileSaveAs()));

    fileSaveAsPDF=new QAction(tr("S&ave As PDF"),this);
    fileSaveAsPDF->setShortcut(tr("Ctrl+P"));
    fileSaveAsPDF->setStatusTip(tr("Save as"));
    connect(fileSaveAsPDF,SIGNAL(triggered()),this,SLOT(exportPDF()));

    boldAction = new QAction(tr("Bold"), this);
    boldAction->setCheckable(true);
    QPixmap pixmap(":/images/bold.png");
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    italicAction = new QAction(QIcon(":/images/italic.png"), tr("Italic"), this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    underlineAction = new QAction(QIcon(":/images/underline.png"), tr("Underline"), this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, SIGNAL(triggered()), this, SLOT(handleFontChange()));

    copyAction =new QAction(tr("C&opy"),this);
    copyAction->setShortcut(tr("Ctrl+C"));
    copyAction->setStatusTip(tr("copy an object"));
    connect(copyAction,SIGNAL(triggered()),this,SLOT(copy()));

    cutAction =new QAction(tr("C&ut"),this);
    cutAction->setShortcut(tr("Ctrl+U"));
    cutAction->setStatusTip(tr("Cut an object"));
    connect(cutAction,SIGNAL(triggered()),this,SLOT(cut()));

    pasteAction =new QAction(tr("P&aste"),this);
    pasteAction->setShortcut(tr("Ctrl+V"));
    pasteAction->setStatusTip(tr("paste an cut or copy object"));
    connect(pasteAction,SIGNAL(triggered()),this,SLOT(paste()));

    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);

    aboutAction =new QAction(tr("H&elp"),this);
    aboutAction->setShortcut(tr("Ctrl+B"));
    aboutAction->setStatusTip(tr("Help manual"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    groupAction = new QAction(tr("G&roup"),this);
    groupAction->setShortcut(tr("Ctrl+G"));
    groupAction->setStatusTip(tr("Group items"));
    connect(groupAction,SIGNAL(triggered()),this,SLOT(group()));

    ungroupAction = new QAction(tr("U&ngroup"),this);
    ungroupAction->setShortcut(tr("Ctrl+U"));
    ungroupAction->setStatusTip(tr("Ungroup items"));
    connect(ungroupAction,SIGNAL(triggered()),this,SLOT(ungroup()));


    rotationlAction = new QAction(QIcon("://images/lc_undo.png"),tr("Left_Rotation"),this);
    rotationlAction->setShortcut(tr("Ctrl+Q"));
    rotationlAction->setStatusTip(tr("Left rotation"));
    connect(rotationlAction,SIGNAL(triggered()),this,SLOT(drotation()));

    rotationrAction = new QAction(QIcon("://images/lc_redo.png"),tr("Right_Rotation"),this);
    rotationrAction->setShortcut(tr("Ctrl+E"));
    rotationrAction->setStatusTip(tr("Right rotation"));
    connect(rotationrAction,SIGNAL(triggered()),this,SLOT(rotation()));


    undoAction = new QAction(tr("U&ndo"),this);
    undoAction->setShortcut(tr("Ctrl+D"));
    undoAction->setStatusTip(tr("undo the last operation"));
    connect(undoAction, SIGNAL(triggered()),this,SLOT(about()));

    redoAction = new QAction(tr("R&edo"),this);
    redoAction->setShortcut(tr("Ctrl+E"));
    redoAction->setStatusTip(tr("redo the last operation"));
    connect(redoAction, SIGNAL(triggered()),this,SLOT(about()));

    resizeAction = new QAction(QIcon(":/images/lc_zoomin.png"),tr("Zoom &Fit"), this);
    resizeAction->setShortcut(tr("Ctrl+S"));
    resizeAction->setStatusTip(tr("redo the last operation"));
    connect(resizeAction, SIGNAL(triggered()),this,SLOT(rsize()));

    dresizeAction = new QAction(QIcon(":/images/lc_zoomminus.png"),tr("Zoom &Fit"),this);
    dresizeAction->setShortcut(tr("Ctrl+Shift+S"));
    dresizeAction->setStatusTip(tr("redo the last operation"));
    connect(dresizeAction, SIGNAL(triggered()),this,SLOT(drsize()));

}

//! [24]
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(saveAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addAction(fileSaveAsPDF);
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();
    itemMenu->addAction(toFrontAction);
    itemMenu->addAction(sendBackAction);
    itemMenu->addAction(rotationlAction);
    itemMenu->addAction(rotationrAction);
    itemMenu->addAction(resizeAction);
    itemMenu->addAction(dresizeAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);

    editMenu =menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(copyAction);
    editMenu->addAction(cutAction);
    editMenu->addAction(pasteAction);


    toolMenu = menuBar()->addMenu(tr("T&ool"));
    toolMenu->addAction(groupAction);
    toolMenu->addAction(ungroupAction);

    stepMenu = menuBar()->addMenu(tr("S&tep"));
    stepMenu->addAction(undoAction);
    stepMenu->addAction(redoAction);
}
//! [24]

//! [25]
void MainWindow::createToolbars()
{
//! [25]
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(toFrontAction);
    editToolBar->addAction(sendBackAction);
    editToolBar->addAction(rotationlAction);
    editToolBar->addAction(rotationrAction);
    editToolBar->addAction(resizeAction);
    editToolBar->addAction(dresizeAction);
    fontCombo = new QFontComboBox();
    fontCombo->setCurrentFont(QFont("Times New Roman"));
    connect(fontCombo, SIGNAL(currentFontChanged(QFont)),
            this, SLOT(currentFontChanged(QFont)));

    fontSizeCombo = new QComboBox;
    fontSizeCombo->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));
    QIntValidator *validator = new QIntValidator(2, 64, this);
    fontSizeCombo->setValidator(validator);
    connect(fontSizeCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(fontSizeChanged(QString)));

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()), Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();
    fontColorToolButton->setIcon(createColorToolButtonIcon(":/images/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, SIGNAL(clicked()),
            this, SLOT(textButtonTriggered()));

//! [26]
    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()), Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/floodfill.png", Qt::white));
    connect(fillColorToolButton, SIGNAL(clicked()),
            this, SLOT(fillButtonTriggered()));
//! [26]

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()), Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                                     ":/images/linecolor.png", Qt::black));
    connect(lineColorToolButton, SIGNAL(clicked()),
            this, SLOT(lineButtonTriggered()));

    textToolBar = addToolBar(tr("Font"));
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);

    colorToolBar = addToolBar(tr("Color"));
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton, int(DiagramScene::InsertLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(sceneScaleChanged(QString)));

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);

    speak=addToolBar(tr("Speaker options"));

    speak->addWidget(new QLabel("Volume:"));
    volume=new QSlider(this);
    volume->setValue(0);
    volume->setOrientation(Qt::Horizontal);
    volume->setMaximumSize(100,10);
    volume->setMinimumSize(50,10);
    connect(volume, &QSlider::valueChanged, this, &MainWindow::setVolume);
    speak->addWidget(volume);

//! [27]
}
//! [27]

//! [28]
QWidget *MainWindow::createBackgroundCellWidget(const QString &text, const QString &image)
{
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    widget->setObjectName("back");
    return widget;
}
//! [28]

//! [29]
QWidget *MainWindow::createCellWidget(const QString &text, DiagramItem::DiagramType type)
{

    DiagramItem item(type, itemMenu);
    QIcon icon(item.image());

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    buttonGroup->addButton(button, int(type));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignTop|Qt::AlignHCenter);
//layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);
    QWidget *widget = new QWidget;
    widget->setObjectName("toolarea");
    widget->setLayout(layout);

    return widget;
}
//! [29]

//! [30]
QMenu *MainWindow::createColorMenu(const char *slot, QColor defaultColor)
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu(this);
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()), this, slot);
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor)
            colorMenu->setDefaultAction(action);
    }
    return colorMenu;
}
//! [30]

//! [31]
QIcon MainWindow::createColorToolButtonIcon(const QString &imageFile, QColor color)
{
    QPixmap pixmap(50, 80);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);
    // Draw icon centred horizontally on button.
    QRect target(4, 0, 42, 43);
    QRect source(0, 0, 42, 43);
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);

    return QIcon(pixmap);
}
//! [31]

//! [32]
QIcon MainWindow::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    return QIcon(pixmap);
}
//! [32]

//! [33]
void MainWindow::openFile() {
    sFileName = QFileDialog::getOpenFileName(this,
                                                    tr("Choose the file you want to open"),
                                                    "Data File(*.aDat)");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                     "/home",
                                                     tr("Data File(*.aDat)"));
    QFile file(sFileName);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(this,tr("Could not open file"),
                              tr("Error:Could not open file"),QMessageBox::Retry);
        return;
    }
    QDataStream in(&file);
    itemList.clear();
    scene->clear();
    QList<QGraphicsItem*> loadGroupItems;
    for(;!in.atEnd();){
        DiagramItem::DiagramType _Type;
        DiagramItem::DiagramType _endType;
        int type, endType,category,pointSize,lineCount,count,inGroupCategory;
        QPointF position, endPosition;
        QColor color, startColor,endColor;
        qint32 index;
        QString newLine;
        QFont font;
        QString text;
        QPen pen;
        in >> index >> category;
        if(category == 65551){
            in >> type >> position >> color >> newLine;
            _Type = static_cast<DiagramItem::DiagramType>(type);
            DiagramItem *item = new DiagramItem(_Type,itemMenu);
            item->setBrush(color);
            item->setPos(position);
            scene->addItem(item);
            emit itemInserted(item);
            itemList << item;
            }
        else if(category == 65539){
            in >> position >> font >> pointSize >> color >> text >> newLine;
            DiagramTextItem *loadText = new DiagramTextItem();
            loadText->setFont(font);
            loadText->setTextInteractionFlags(Qt::TextEditorInteraction);
            loadText->setZValue(1000.0);
            connect(loadText, SIGNAL(lostFocus(DiagramTextItem*)),
                    this, SLOT(editorLostFocus(DiagramTextItem*)));
            connect(loadText, SIGNAL(selectedChange(QGraphicsItem*)),
                    this, SIGNAL(itemSelected(QGraphicsItem*)));
            loadText->setPos(position);
            loadText->font().setPointSize(pointSize);
            loadText->setPlainText(text);
            scene->addItem(loadText);
            loadText->setDefaultTextColor(color);
            emit textInserted(loadText);
            //itemList << loadText;

        }
        else if(category == 65540){
            in >> position >> startColor >> type >> endPosition >> endColor >> endType >> color >> newLine;
            _Type = static_cast<DiagramItem::DiagramType>(type);
            DiagramItem *startItem = new DiagramItem(_Type,itemMenu);
            startItem->setPos(position);
            startItem->setBrush(startColor);
            _endType = static_cast<DiagramItem::DiagramType>(endType);
            DiagramItem *endItem = new DiagramItem(_endType,itemMenu);
            endItem->setPos(endPosition);
            endItem->setBrush(endColor);
            foreach(QGraphicsItem * arrowItem, itemList){
                if(qgraphicsitem_cast<DiagramItem*>(arrowItem)->pos() == position)
                    //scene->removeItem(qgraphicsitem_cast<DiagramItem*>(arrowItem));
                    arrowItem->hide();
                else if(qgraphicsitem_cast<DiagramItem*>(arrowItem)->pos() == endPosition)
                    //scene->removeItem(qgraphicsitem_cast<DiagramItem*>(arrowItem));
                    arrowItem->hide();
                else continue;
            }
            Arrow *loadArrow = new Arrow(startItem,endItem);
            loadArrow->setColor(color);
            startItem->addArrow(loadArrow);
            endItem->addArrow(loadArrow);
            loadArrow->setZValue(-1000.0);
            scene->addItem(loadArrow);
            //itemList << loadArrow;
            loadArrow->updatePosition();
            startItem->setPos(position);
            scene->addItem(startItem);
            emit itemInserted(startItem);
            itemList << startItem;
            endItem->setPos(endPosition);
            scene->addItem(endItem);
            emit itemInserted(endItem);
            itemList << endItem;
            }
        //}
          else if(category == 65541){
            //QList<QGraphicsItem*> loadGroupItems;
            in >> lineCount >> newLine;
            count = lineCount;
            while(count != 0){
                QString inGroupNewLine;
                in >> inGroupCategory;
                if(inGroupCategory == 65551){
                    count--;
                    in >> type >> position >> color >> inGroupNewLine;
                    _Type = static_cast<DiagramItem::DiagramType>(type);
                    DiagramItem *groupItem = new DiagramItem(_Type,itemMenu);
                    groupItem->setBrush(color);
                    groupItem->setPos(position);
                    loadGroupItems.append(groupItem);
                    /*foreach(QGraphicsItem * ii, itemList){
                        if(qgraphicsitem_cast<DiagramItem*>(ii)->pos() == position)
                            ii->hide();
                    }*/

                    //count++;
                    //scene->addItem(groupItem);
                    //emit itemInserted(groupItem);
                    //itemList << item;
                }
                else if(inGroupCategory == 65539){
                    count--;
                    in >> position >> font >> pointSize >> text >> inGroupNewLine;
                    DiagramTextItem *groupText = new DiagramTextItem();
                    groupText->setFont(font);
                    groupText->setTextInteractionFlags(Qt::TextEditorInteraction);
                    groupText->setZValue(1000.0);
                    connect(groupText, SIGNAL(lostFocus(DiagramTextItem*)),
                            this, SLOT(editorLostFocus(DiagramTextItem*)));
                    connect(groupText, SIGNAL(selectedChange(QGraphicsItem*)),
                            this, SIGNAL(itemSelected(QGraphicsItem*)));
                    groupText->setPos(position);
                    groupText->font().setPointSize(pointSize);
                    groupText->setPlainText(text);
                    //scene->addItem(loadText);
                    groupText->setDefaultTextColor(Qt::black);
                    loadGroupItems.append(groupText);
                    //emit textInserted(groupText);
                    //itemList << groupText;
                    //count++;
                }
                else if(inGroupCategory == 65540){
                    count--;
                    /*in >> position >> type >> endPosition >> endType >> color >> inGroupNewLine;
                    _Type = static_cast<DiagramItem::DiagramType>(type);
                    DiagramItem *groupStartItem = new DiagramItem(_Type,itemMenu);
                    groupStartItem->setPos(position);
                    _endType = static_cast<DiagramItem::DiagramType>(endType);
                    DiagramItem *groupEndItem = new DiagramItem(_endType,itemMenu);
                    groupEndItem->setPos(endPosition);
                    Arrow *groupArrow = new Arrow(groupStartItem,groupEndItem);
                    groupArrow->setColor(color);
                    groupStartItem->addArrow(groupArrow);
                    groupEndItem->addArrow(groupArrow);
                    groupArrow->setZValue(-1000.0);
                    loadGroupItems.append(groupArrow);*/
                    //scene->addItem(groupArrow);
                    //groupArrow->updatePosition();
                    in >> position >> startColor >> type >> endPosition >> endColor >> endType >> color >> newLine;
                    _Type = static_cast<DiagramItem::DiagramType>(type);
                    DiagramItem *startItem = new DiagramItem(_Type,itemMenu);
                    startItem->setPos(position);
                    startItem->setBrush(startColor);
                    _endType = static_cast<DiagramItem::DiagramType>(endType);
                    DiagramItem *endItem = new DiagramItem(_endType,itemMenu);
                    endItem->setPos(endPosition);
                    endItem->setBrush(endColor);
                    foreach(QGraphicsItem * arrowItem, itemList){
                        if(qgraphicsitem_cast<DiagramItem*>(arrowItem)->pos() == position)
                            scene->removeItem(qgraphicsitem_cast<DiagramItem*>(arrowItem));
                        else if(qgraphicsitem_cast<DiagramItem*>(arrowItem)->pos() == endPosition)
                            scene->removeItem(qgraphicsitem_cast<DiagramItem*>(arrowItem));
                        else continue;
                    }
                    Arrow *groupArrow = new Arrow(startItem,endItem);
                    groupArrow->setColor(color);
                    startItem->addArrow(groupArrow);
                    endItem->addArrow(groupArrow);
                    groupArrow->setZValue(-1000.0);
                    scene->addItem(groupArrow);
                    groupArrow->updatePosition();
                    startItem->setPos(position);
                    scene->addItem(startItem);
                    emit itemInserted(startItem);
                    //itemList << startItem;
                    endItem->setPos(endPosition);
                    scene->addItem(endItem);
                    emit itemInserted(endItem);
                    loadGroupItems.append(groupArrow);
                    loadGroupItems.append(startItem);
                    loadGroupItems.append(endItem);
                }
            }

           /* QGraphicsItemGroup *loadGroup = new QGraphicsItemGroup();
            foreach(QGraphicsItem * toLoadGroup,loadGroupItems){
                toLoadGroup->setGroup(loadGroup);
            }
            loadGroup->setFlag(QGraphicsItem::ItemIsMovable,true);
            loadGroup->setFlag(QGraphicsItem::ItemIsSelectable,true);
            groups.append(loadGroup);
            scene->addItem(loadGroup);*/

        }
        /*QGraphicsItemGroup *loadGroup = new QGraphicsItemGroup();
        foreach(QGraphicsItem * toLoadGroup,loadGroupItems){
            toLoadGroup->setGroup(loadGroup);
        }
        loadGroup->setFlag(QGraphicsItem::ItemIsMovable,true);
        loadGroup->setFlag(QGraphicsItem::ItemIsSelectable,true);
        groups.append(loadGroup);
        scene->addItem(loadGroup);*/
    //}
    }
    if(!loadGroupItems.isEmpty()){
    QGraphicsItemGroup *loadGroup = new QGraphicsItemGroup();
    foreach(QGraphicsItem * toLoadGroup,loadGroupItems){
        toLoadGroup->setGroup(loadGroup);
    }
    loadGroup->setFlag(QGraphicsItem::ItemIsMovable,true);
    loadGroup->setFlag(QGraphicsItem::ItemIsSelectable,true);
    groups.append(loadGroup);
    scene->addItem(loadGroup);
    }
    saveOnce = true;
}

//![33]

//![34]
void MainWindow::saveFile(){
      if(saveOnce == false){
        sFileName = QFileDialog::getSaveFileName(this,tr("save file"),
        tr(".Dat"));
        saveOnce = true;
      }
        QFile file(sFileName);
        if((!file.open(QIODevice::WriteOnly) && saveOnce == false)){
            QMessageBox::critical(this,tr("Could not save file"),
            tr("Error: Could not save file"),QMessageBox::Retry);
            return;
        }
            file.open(QIODevice::WriteOnly);
            QDataStream out(&file);
            DiagramItem::DiagramType type;
            itemList = scene->items();
            int index = 1;
            foreach(QGraphicsItem *item,itemList){
               if(item->group() == 0){
                out << (qint32)index;
                if(item->type() == 65551){
                    DiagramItem *token = qgraphicsitem_cast<DiagramItem*>(item);
                    out << (qint32)65551;
                    type = token->diagramType();
                    out << static_cast<int>(type);
                    out << token->pos();
                    out << token->brush().color();
                    out << QString("\n");
                    index++;
                }
                 else if(item->type() == 65539){
                     DiagramTextItem * textToken = qgraphicsitem_cast<DiagramTextItem*>(item);
                     out << (qint32)65539;
                     out << textToken->pos();
                     out << textToken->font();
                     out << textToken->font().pointSize();
                     out << textToken->defaultTextColor();
                     QString content = textToken->toPlainText();
                     out << content;
                     out << QString("\n");
                     index++;
                }
                else if(item->type() == 65540){
                     Arrow * arrowToken = qgraphicsitem_cast<Arrow*>(item);
                     out << (qint32)65540;
                     out << arrowToken->startItem()->pos();
                     //out << arrowToken->startItem()->index();
                     out << arrowToken->startItem()->brush().color();
                     out << static_cast<int>(arrowToken->startItem()->diagramType());
                     out << arrowToken->endItem()->pos();
                     //out << arrowToken->endItem()->index();
                     out << arrowToken->endItem()->brush().color();
                     out << static_cast<int>(arrowToken->endItem()->diagramType());
                     out << arrowToken->getColor();
                     out << QString("\n");
                     index++;
                }
               }
                else{
                    QGraphicsItemGroup * groupToken = item->group();
                        int lineCount = groupToken->childItems().count();
                        out << (qint32)index;
                        out << (qint32)65541;
                        out << (qint32)lineCount;
                        out << QString("\n");
                        foreach(QGraphicsItem * itemInGroup, groupToken->childItems()){
                            if(itemInGroup->type() == 65551){
                                //out << (qint32)index;
                                DiagramItem * groupDiagramItem = qgraphicsitem_cast<DiagramItem*>(itemInGroup);
                                out << (qint32)65551;
                                //out << groupDiagramItem->index();
                                type = groupDiagramItem->diagramType();
                                out << static_cast<int>(type);
                                out << groupDiagramItem->pos();
                                out << groupDiagramItem->brush().color();
                                out << QString("\n");
                                //index++;
                        }
                            else if(itemInGroup->type() == 65539){
                                DiagramTextItem * groupTextItem = qgraphicsitem_cast<DiagramTextItem*>(itemInGroup);
                               // out << (qint32)index;
                                out << (qint32)65539;
                                out << groupTextItem->pos();
                                out << groupTextItem->font();
                                out << groupTextItem->font().pointSize();
                                QString content = groupTextItem->toPlainText();
                                out << content;
                                out << QString("\n");
                                //index++;
                            }
                            else if(itemInGroup->type() == 65540){
                                Arrow * groupArrowItem = qgraphicsitem_cast<Arrow*>(itemInGroup);
                                //out << (qint32)index;
                                out << (qint32)65540;
                                /*out << groupArrowItem->startItem()->pos();
                                out << static_cast<int>(groupArrowItem->startItem()->diagramType());
                                out << groupArrowItem->endItem()->pos();
                                out << static_cast<int>(groupArrowItem->endItem()->diagramType());
                                out << groupArrowItem->getColor();
                                out << QString("\n");*/
                                out << groupArrowItem->startItem()->pos();
                                //out << arrowToken->startItem()->index();
                                out << groupArrowItem->startItem()->brush().color();
                                out << static_cast<int>(groupArrowItem->startItem()->diagramType());
                                out << groupArrowItem->endItem()->pos();
                                //out << arrowToken->endItem()->index();
                                out << groupArrowItem->endItem()->brush().color();
                                out << static_cast<int>(groupArrowItem->endItem()->diagramType());
                                out << groupArrowItem->getColor();
                                out << QString("\n");
                                //index++;
                            }
                    }
                        //out << QString("EndGroup");
                        //out << QString("\n");
                        index++;
            }
            saveOnce = true;
    }
}
//![34]
//! [35]
void MainWindow::copy(){
    copyItems.clear();
    grouped = 0;
    foreach(QGraphicsItem *toCopy, scene->selectedItems()){
        if(toCopy->group() == 0){
            copyItems.append(toCopy);
        }
        else{
            QGraphicsItemGroup *copyGroup = new QGraphicsItemGroup();
            copyGroup = toCopy->group();
            grouped = 1;
            foreach(QGraphicsItem * groupCopy,copyGroup->childItems()){
                groupItemToCopy.append(groupCopy);
            }
         }
}
}
//![35]
//! [36]
void MainWindow::paste(){
    if(copyItems.isEmpty()){
        QMessageBox::critical(this,tr("Notice"),tr("Error:No Item Is Copied"),QMessageBox::Close);
        return;
    }
    else{
    foreach(QGraphicsItem * toPaste, copyItems){
        //if(grouped == 0){
        int category = toPaste->type();
        if(category == 65551){
            DiagramItem::DiagramType type;
            QPointF position;
            QColor color;
            DiagramItem *pasted;
            type = qgraphicsitem_cast<DiagramItem*>(toPaste)->diagramType();
            position = toPaste->pos();
            QPointF offset(100,0);
            color = qgraphicsitem_cast<DiagramItem*>(toPaste)->brush().color();
            pasted = new DiagramItem(type,itemMenu);
            pasted->setPos(position+=offset);
            pasted->setBrush(color);
            scene->addItem(pasted);
            emit itemInserted(pasted);
            itemList << pasted;
        }
        else if(category == 65539){
            QPointF position = toPaste->pos();
            QFont font = qgraphicsitem_cast<DiagramTextItem*>(toPaste)->font();
            int fontSize = qgraphicsitem_cast<DiagramTextItem*>(toPaste)->font().pointSize();
            QString content = qgraphicsitem_cast<DiagramTextItem*>(toPaste)->toPlainText();
            QColor color = qgraphicsitem_cast<DiagramTextItem*>(toPaste)->defaultTextColor();
            DiagramTextItem *pasteText = new DiagramTextItem();
            pasteText->setFont(font);
            pasteText->setTextInteractionFlags(Qt::TextEditorInteraction);
            pasteText->setZValue(1000.0);
            connect(pasteText, SIGNAL(lostFocus(DiagramTextItem*)),
                    this, SLOT(editorLostFocus(DiagramTextItem*)));
            connect(pasteText, SIGNAL(selectedChange(QGraphicsItem*)),
                    this, SIGNAL(itemSelected(QGraphicsItem*)));
            pasteText->setPos(position);
            pasteText->font().setPointSize(fontSize);
            pasteText->setDefaultTextColor(color);
            pasteText->setPlainText(content);
            scene->addItem(pasteText);
            pasteText->setDefaultTextColor(Qt::black);
            emit textInserted(pasteText);
            //itemList << pasteText;
        }
        else if(category == 65540){
            Arrow * arrowToken = qgraphicsitem_cast<Arrow*>(toPaste);
            QPointF startPos = arrowToken->startItem()->pos();
            DiagramItem::DiagramType startType = arrowToken->startItem()->diagramType();
            QPointF endPos = arrowToken->endItem()->pos();
            DiagramItem::DiagramType endType = arrowToken->endItem()->diagramType();
            QColor color = arrowToken->getColor();
            DiagramItem *startItem = new DiagramItem(startType,itemMenu);
            startItem->setPos(startPos);
            DiagramItem *endItem = new DiagramItem(endType,itemMenu);
            endItem->setPos(endPos);
            Arrow *pasteArrow = new Arrow(startItem,endItem);
            pasteArrow->setColor(color);
            startItem->addArrow(pasteArrow);
            endItem->addArrow(pasteArrow);
            pasteArrow->setZValue(-1000.0);
            scene->addItem(pasteArrow);
            pasteArrow->updatePosition();
            pasteArrow->setFlag(QGraphicsItem::ItemIsMovable,true);
        }
    }
    //}

        if(!groupItemToCopy.isEmpty()){
            QGraphicsItemGroup * pasteGroup = new QGraphicsItemGroup();
            foreach(QGraphicsItem * groupPaste, groupItemToCopy){
                int groupCate = groupPaste->type();
                if(groupCate == 65551){
                    DiagramItem::DiagramType gType;
                    QPointF gPosition;
                    QColor gColor;
                    DiagramItem *gPasted;
                    gType = qgraphicsitem_cast<DiagramItem*>(groupPaste)->diagramType();
                    gPosition = groupPaste->pos();
                    gColor = qgraphicsitem_cast<DiagramItem*>(groupPaste)->brush().color();
                    gPasted = new DiagramItem(gType,itemMenu);
                    gPasted->setPos(gPosition);
                    gPasted->setBrush(gColor);
                    readyPaste.append(gPasted);
                    scene->addItem(gPasted);
                }
            }
            foreach(QGraphicsItem * readyPastes,readyPaste){
               readyPastes->setGroup(pasteGroup);
            }
             pasteGroup->setFlag(QGraphicsItem::ItemIsMovable,true);
             pasteGroup->setFlag(QGraphicsItem::ItemIsSelectable,true);
             scene->addItem(pasteGroup);
             foreach(QGraphicsItem * readyPastes,readyPaste){
                readyPastes->setGroup(pasteGroup);
             }
              pasteGroup->setFlag(QGraphicsItem::ItemIsMovable,true);
              pasteGroup->setFlag(QGraphicsItem::ItemIsSelectable,true);
              scene->addItem(pasteGroup);
            }
    }
}

//![36]
//! [37]
void MainWindow::cut(){
    //cutted = 0;
    //if(cutted == 0){
        copyItems.clear();
        foreach(QGraphicsItem* toCut, scene->selectedItems()){
            if(!copyItems.contains(toCut)){
                copyItems.append(toCut);
            }
                scene->removeItem(toCut);
            continue;

     }
}
//! [37]
//! [38]
void MainWindow::group(){
    groupItems.clear();
    foreach(QGraphicsItem * toGroup, scene->selectedItems()){
        groupItems.append(toGroup);
    }
    QGraphicsItemGroup *itemGroup = new QGraphicsItemGroup();
    foreach(QGraphicsItem * group, groupItems){
        group->setGroup(itemGroup);
    }
    itemGroup->setFlag(QGraphicsItem::ItemIsMovable,true);
    itemGroup->setFlag(QGraphicsItem::ItemIsSelectable,true);
    groups.append(itemGroup);
    scene->addItem(itemGroup);
}

//! [38]
//! [39]
void MainWindow::fileSaveAs(){
    saveOnce = false;
    saveFile();
}

//![39]

//![40]
void MainWindow::ungroup(){
    foreach(QGraphicsItemGroup * toUngroup, groups){
        scene->destroyItemGroup(toUngroup);
    }
    itemList  = scene->selectedItems();
    for(int i = 0; i < itemList.count();i++){
        QGraphicsItem * q = itemList.at(i);
        q->hide();
    }
}


//![40]

//![41]
void MainWindow::rotation()
{
    if (scene->selectedItems().isEmpty())
        return;
//qDebug()<<"INININ";
   DiagramItem *selectedItem = qgraphicsitem_cast<DiagramItem *> (scene->selectedItems().first());
  //  selectedItem->setRotation(90);
    if(selectedItem->type()==65540)
        return;



    QTransform trans=selectedItem->transform();
    selectedItem->setTransform(trans*QTransform().rotate(15),false);
    foreach(Arrow*a, selectedItem->arrows)
    {
      
    }
   // selectedItem->setRotation(selectedItem->rotation()+15);
     //  qDebug()<<selectedItem->rotation();
  //  Arrow *arrow = qgraphicsitem_cast<Arrow *>(selectedItem);
 //   arrow->startItem()->removeArrow(arrow);
   // arrow->endItem()->removeArrow(arrow);
   DiagramItem *item=  qgraphicsitem_cast<DiagramItem *>(selectedItem);


//   item->polygon().resize(5);
  // scene->update();
 //  view->update();
// selectedItem->update();
 //  scene->update();

}
//![41]

//![42]
void MainWindow::drotation()
{
    if (scene->selectedItems().isEmpty())
        return;
//qDebug()<<"INININ";
    QGraphicsItem *selectedItem = scene->selectedItems().first();
  //  selectedItem->setRotation(90);
    if(selectedItem->type()==65540)
        return;


    QTransform trans=selectedItem->transform();
    selectedItem->setTransform(trans*QTransform().rotate(-15),false);
    //  qDebug()<<selectedItem->rotation();
    scene->update();
    view->update();
}
//![42]
//![43]
void MainWindow::rsize()
{
    if (scene->selectedItems().isEmpty())
        return;
//qDebug()<<"INININ";
    DiagramItem *selectedItem = qgraphicsitem_cast<DiagramItem*> (scene->selectedItems().first());
  //  selectedItem->setRotation(90);
    if(selectedItem->type()==65540)
        return;


  //  QTransform trans=selectedItem->transform();
  //  selectedItem->setTransform(trans*QTransform().rotate(15),false);
    //selectedItem->setTransform(trans*QTransform().scale(1.25,1.25),false);
     //  qDebug()<<selectedItem->rotation();
    selectedItem->PoligonSizeChange(25);
    selectedItem->update();
    scene->update();
    view->update();
}
//![43]
//![44]
void MainWindow::drsize()
{
    if (scene->selectedItems().isEmpty())
        return;
//qDebug()<<"INININ";
    DiagramItem *selectedItem = qgraphicsitem_cast<DiagramItem*> (scene->selectedItems().first());
  //  selectedItem->setRotation(90);
    if(selectedItem->type()==65540)
        return;

  //  QTransform trans=selectedItem->transform();
  //  selectedItem->setTransform(trans*QTransform().rotate(15),false);
 //   selectedItem->setTransform(trans*QTransform().scale(0.875,0.875),false);
    //  qDebug()<<selectedItem->rotation();
    selectedItem->PoligonSizeChange(-25);
   QList<Arrow*> list=selectedItem->arrows;
   foreach(Arrow*a, list)
   {

   }
    selectedItem->update();
    scene->update();
    view->update();
}
//![44]
//![45]
QString MainWindow::loadStyleSheet()
{
    QString styleSheet;
       QString cssName = ":/css";

       QFile qFile(cssName);
       if (qFile.open(QFile::ReadOnly))
       {
           styleSheet = QLatin1String(qFile.readAll());
       }
       else
       {
           qDebug() << "Error of css";
       }
       return styleSheet;
}
//![45]
//![46]
void MainWindow::setVolume(int volume)
{
    m_speech->setVolume(volume / 100.0);
}
//![46]
//!
void MainWindow::exportPDF()
{
        QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                                                    QString(), "*.pdf");
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
    printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);

    QPainter p(&printer);
    scene->render(&p);
    p.end();
}
//!

