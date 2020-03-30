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

#include "diagramitem.h"
#include "arrow.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include<QApplication>
#include<QGraphicsRectItem>

//! [0]
DiagramItem::DiagramItem(DiagramType diagramType, QMenu *contextMenu,
             QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{


    myDiagramType = diagramType;
    myContextMenu = contextMenu;
    //itemIndex = itemIndex;
    //itemIndex++;
    QPainterPath path;
    switch (myDiagramType) {
        case StartEnd:
            path.moveTo(200, 50);
            path.arcTo(150, 0, 50, 50, 0, 90);
            path.arcTo(50, 0, 50, 50, 90, 90);
            path.arcTo(50, 50, 50, 50, 180, 90);
            path.arcTo(150, 50, 50, 50, 270, 90);
            path.lineTo(200, 25);
            myPolygon = path.toFillPolygon();
            break;
        case Conditional:
            myPolygon << QPointF(-100, 0) << QPointF(0, 75)
                      << QPointF(100, 0) << QPointF(0, -75)
                      << QPointF(-100, 0);
            break;
        case Step:
            myPolygon << QPointF(-100, -75) << QPointF(100, -75)
                      << QPointF(100, 75) << QPointF(-100, 75)
                      << QPointF(-100, -75);
            break;
        case Preparation:
            myPolygon << QPointF(-70,-50) << QPointF(70,-50) << QPointF(100,0)
                      << QPointF(70,50) << QPointF(-70,50) << QPointF(-100,0)
                      << QPointF(-70,-50);
            break;
        case OffPageConnector:
             myPolygon << QPointF(25,-25) << QPointF(-25,-25) << QPointF(-25,25)
                       << QPointF(0,40) << QPointF(25,25) << QPointF(25,-25);

             break;
        case Terminal:
                path.moveTo(-50,-50);
                path.arcTo(-100,-50,100,120,90,180);
                path.arcTo(0,-50,100,120,270,180);
                myPolygon = path.toFillPolygon();
              break;
         case Connector:
                path.addEllipse(-25,-25,50,50);
                myPolygon = path.toFillPolygon();
         break;
        default:
            myPolygon << QPointF(-75, -75) << QPointF(-125, 75)
                      << QPointF(75, 75) << QPointF(125, -75)
                      << QPointF(-75, -75);
            break;
    }
    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}
//! [0]

//! [1]
void DiagramItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);

}
//! [1]

//! [2]
void DiagramItem::removeArrows()
{
    foreach (Arrow *arrow, arrows) {
        arrow->startItem()->removeArrow(arrow);
        arrow->endItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}
//! [2]

//! [3]
void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}
//! [3]

//! [4]
QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    return pixmap;
}


void DiagramItem::PoligonSizeChange(double r)
{

   QList<QPointF> l= myPolygon.toList();
    myPolygon.clear();
     QPainterPath path;
    switch (myDiagramType) {
        case StartEnd:
            path.moveTo(200, 50);
            path.arcTo(150, 0, 50, 50, 0, 90);
            path.arcTo(50, 0, 50, 50, 90, 90);
            path.arcTo(50, 50, 50, 50, 180, 90);
            path.arcTo(150, 50, 50, 50, 270, 90);
            path.lineTo(200, 25);
            myPolygon = path.toFillPolygon();
            break;
        case Conditional:
            myPolygon << QPointF(-(100+r), 0) << QPointF(0, 75+r)
                      << QPointF(100+r, 0) << QPointF(0, -(75+r))
                      << QPointF(-(100+r), 0);
            break;
        case Step:
            myPolygon << QPointF(-100-r, -75-r) << QPointF(100+r, -75-r)
                      << QPointF(100+r, 75+r) << QPointF(-100-r, 75+r)
                      << QPointF(-100-r, -75-r);
            break;
        case Preparation:
            myPolygon << QPointF(-70-r,-50-r) << QPointF(70+r,-50-r) << QPointF(100+r,0)
                      << QPointF(70+r,50+r) << QPointF(-70-r,50+r) << QPointF(-100-r,0)
                      << QPointF(-70-r,-50-r);
            break;
        case OffPageConnector:if(r>0)
             myPolygon << QPointF(25+r,-25-r) << QPointF(-25-r,-25-r) << QPointF(-25-r,25+r)
                       << QPointF(0,40+r) << QPointF(25+r,25+r) << QPointF(25+r,-25-r);
        else
            myPolygon << QPointF(25,-25) << QPointF(-25,-25) << QPointF(-25,25)
                      << QPointF(0,40) << QPointF(25,25) << QPointF(25,-25);

             break;
        case Terminal:
                path.moveTo(-50,-50);
                path.arcTo(-100,-50,100,120+r,90,180);
                path.arcTo(0,-50,100,120+r,270,180);
                myPolygon = path.toFillPolygon();
              break;
         case Connector:
                path.addEllipse(-25-r,-25-r,50+r,50+r);
                myPolygon = path.toFillPolygon();
         break;
        default:
            myPolygon << QPointF(-75-r, -75-r) << QPointF(-125-r, 75+r)
                      << QPointF(75+r, 75+r) << QPointF(125+r, -75-r)
                      << QPointF(-75-r, -75-r);
            break;
    }

/*
foreach(QPointF p, l)
{
    if(((p.x()-r)!=0.0)&&((p.y()-r)!=0.0)&&((p.y()+r)!=0.0)&&((p.y()+r)!=0.0))
    {
        if(p.x()<0&&p.y()<0)
            myPolygon<<(p-QPointF(r,r));
        else if(p.x()<0)
            myPolygon<<(p+QPointF(-r,r));
        else if(p.y()<0)
            myPolygon<<(p+QPointF(r,-r));
        else
            myPolygon<<(p+QPointF(r,r));
    }
    else {

        if(p.x()<0&&p.y()<0)
            myPolygon<<(p-QPointF(r,r));
        else if(p.x()<0)
            myPolygon<<(p+QPointF(-r,r));
        else if(p.y()<0)
            myPolygon<<(p+QPointF(r,-r));
        else
            myPolygon<<(p+QPointF(r,r));
    }
}*/
   setPolygon(myPolygon);
  //  setScale(scale()+r);

}
//! [4]

//! [5]
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}
//! [5]

//! [6]
QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Arrow *arrow, arrows) {
            arrow->updatePosition();
        }
    }

    return value;
}
//! [6]

//! [7]
  void DiagramItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
 {
     QApplication::setOverrideCursor(Qt::PointingHandCursor);
    QGraphicsItem::mousePressEvent(event);
 }
//! [7]

//! [8]
 void DiagramItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
  {
      QApplication::restoreOverrideCursor();
     QGraphicsItem::mouseReleaseEvent(event);
  }
//! [8]

