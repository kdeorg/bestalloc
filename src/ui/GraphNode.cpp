/*! ---------------------------------------------------------------
 * \file   GraphNode.cpp
 * \brief  GraphNode class implementation
 * \author Edward Ryabikov
 *
 * PROJ: bestalloc
 * ---------------------------------------------------------------- */

#include "GraphNode.h"
#include "GraphWidget.h"
#include "GraphEdge.h"
#include "Constants.h"
using namespace bestalloc;

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>

GraphNode::GraphNode(const QPixmap& nodePicture, GraphWidget* widget)
    : QGraphicsItem(), m_nodePicture(QPixmap(nodePicture)), m_widget(widget)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

void GraphNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void GraphNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);

    if (m_widget != NULL) {
        m_widget->resizeToFit();
    }
}

QVariant GraphNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
        case ItemPositionHasChanged:
            foreach (GraphEdge *edge, m_edgeList) {
                edge->adjust();
            }

            break;

        default:
            break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void GraphNode::addEdge(GraphEdge *edge)
{
    m_edgeList << edge;
    edge->adjust();
}

QList<GraphEdge*> GraphNode::getEdges() const
{
    return m_edgeList;
}

QPixmap GraphNode::getNodePicture() const
{
    return m_nodePicture;
}

void GraphNode::setNodePicture(const QPixmap& value)
{
    m_nodePicture = value;
}


QPointF GraphNode::getPosition() const
{
    return m_position;
}

void GraphNode::setPosition(const QPointF& value)
{
    m_position = value;
}

bool GraphNode::advance()
{
    if (m_position == pos())
        return false;

    setPos(m_position);
    return true;
}

QRectF GraphNode::boundingRect() const
{
    qreal adjust = 5;
    return QRectF(-100 - adjust, -130 - adjust, 100 + adjust, 130 + adjust);
}

QPainterPath GraphNode::shape() const
{
    QPainterPath path;
    path.addEllipse(-150, -150, 150, 150);
    return path;
}

void GraphNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->drawPixmap(-100, -100, 100, 100, m_nodePicture);
}

void GraphNode::removeEdge(GraphEdge *edge)
{
    m_edgeList.removeAll(edge);
}

GraphNode::~GraphNode()
{
}
