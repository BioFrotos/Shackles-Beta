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

#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QGraphicsPixmapItem>
#include <QList>
#include <QGraphicsItem>

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsItem;
class QGraphicsScene;
class QTextEdit;
class QGraphicsSceneMouseEvent;
class QMenu;
class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QPolygonF;
QT_END_NAMESPACE

class Arrow;

//! [0]
class DiagramItem : public QGraphicsPolygonItem
{
public:

    enum { Type = UserType + 15 };
    enum DiagramType { Step, Conditional, StartEnd, Io,Preparation,Terminal,OffPageConnector, Connector};

    DiagramItem(DiagramType diagramType, QMenu *contextMenu, QGraphicsItem *parent = 0);
    DiagramItem(DiagramType diagramType, int index, QMenu *contextMenu,QGraphicsItem * parent = 0);

    void removeArrow(Arrow *arrow);
    void removeArrows();
    DiagramType diagramType() const { return myDiagramType; }
    QPolygonF polygon() const { return myPolygon; }
    void addArrow(Arrow *arrow);
    QPixmap image() const;
    int type() const Q_DECL_OVERRIDE { return Type;}
    int index() const {return itemIndex;}
    qint32 rot=0;
    void PoligonSizeChange(double r);
    void PoligoneRotationChange(double r);

    //friend QDataStream & operator << (QDataStream & out,QGraphicsItem & item);
    //friend QDataStream & operator >> (QDataStream & in, QGraphicsItem & item);
  QList<Arrow *> arrows;
   QPolygonF myPolygon;
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
    void mousePressEvent(QGraphicsSceneMouseEvent *pe)  Q_DECL_OVERRIDE;
     void mouseReleaseEvent(QGraphicsSceneMouseEvent *pe) Q_DECL_OVERRIDE;
private:
    DiagramType myDiagramType;

    QMenu *myContextMenu;

    int itemIndex = 0;
   /* inline QDataStream & operator << (QDataStream & out, QGraphicsItem & item){
        out << qgraphicsitem_cast<DiagramItem>(item).diagramType();
        out << item.pos();
        out << qgraphicsitem_cast<DiagramItem>(item).brush().color();
        //out << qgraphicsitem_cast<DiagramScene*>(item)->lineColor();
        return out;
    }*/

   /* inline QDataStream & operator >> (QDataStream & in, QGraphicsItem & item){
        DiagramItem::DiagramType _Type;
        QPointF position;
        QColor color;
        //QColor lineColor;
        in >> _Type >> position >> color;
        item.diagramType = _Type;
        item.setPos(position);
        item.brush().setColor(color);
        //qgraphicsitem_cast<DiagramScene*>(item)->setLineColor(lineColor);
        return in;
    }*/
};
//! [0]

#endif // DIAGRAMITEM_H
