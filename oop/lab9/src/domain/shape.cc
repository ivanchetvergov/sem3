#include "domain/shape.h"
#include <algorithm>
#include <QPen>
#include <QBrush>
#include <QStyleOptionGraphicsItem>
#include <cmath>
#include <QGraphicsScene>
#include <QGraphicsRectItem> // Нужно для делегирования

namespace domain {

// * --- Shape (базовый класс) ---
Shape::Shape(const ShapeData& data) : QGraphicsObject(), m_data(data) { 
    setPos(data.position);
    setFlag(QGraphicsItem::ItemIsMovable, true);      
    setFlag(QGraphicsItem::ItemIsSelectable, true);   
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true); 
    setAcceptHoverEvents(true);                       
    setVisible(data.isVisible);                       
}

void Shape::setShapeVisible(bool visible) {
    m_data.isVisible = visible;
    setVisible(visible);
}

void Shape::addConnection(int shapeId) {
    if (std::find(m_data.connectedShapeIds.begin(), 
                  m_data.connectedShapeIds.end(), 
                  shapeId) == m_data.connectedShapeIds.end()) {
        m_data.connectedShapeIds.push_back(shapeId);
    }
}

void Shape::removeConnection(int shapeId) {
    auto it = std::find(m_data.connectedShapeIds.begin(), 
                        m_data.connectedShapeIds.end(), 
                        shapeId);
    if (it != m_data.connectedShapeIds.end()) {
        m_data.connectedShapeIds.erase(it);
    }
}

void Shape::drawSelectionHighlight(QPainter* painter) const {
    if (isSelected()) {
        painter->setPen(QPen(QColor(255, 165, 0), 3, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect().adjusted(-5, -5, 5, 5));
    }
}

// *** ДЕЛЕГИРОВАНИЕ ***
// В базовом классе эти методы просто делегируют вызов, если делегат установлен.
QRectF Shape::boundingRect() const { 
    if (m_graphicsDelegate) {
        // boundingRect() должен возвращать прямоугольник в локальных координатах
        return m_graphicsDelegate->boundingRect();
    }
    return QRectF();
}

void Shape::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) { 
    if (m_graphicsDelegate) {
        // Перед отрисовкой делегата устанавливаем для него наше перо и кисть
        // Предполагаем, что делегат — это QAbstractGraphicsShapeItem или его потомок.
        if (auto shapeItem = qgraphicsitem_cast<QAbstractGraphicsShapeItem*>(m_graphicsDelegate)) {
             shapeItem->setPen(shapePen());
             shapeItem->setBrush(shapeBrush());
        }
        
        // Рисуем сам делегат
        m_graphicsDelegate->paint(painter, option, widget);
        
        // Рисуем нашу дополнительную логику
        drawSelectionHighlight(painter);
    }
}
// *********************


QVariant Shape::itemChange(GraphicsItemChange change, const QVariant& value) {
    
    // 1. Ограничение движения (ItemPositionChange)
    if (change == ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();
        QRectF sceneRect = scene()->sceneRect();
        
        // Получаем ограничивающий прямоугольник элемента (уже делегирован)
        QRectF itemRect = boundingRect(); 
        
        qreal minX = sceneRect.left() + itemRect.width() / 2.0;
        qreal minY = sceneRect.top() + itemRect.height() / 2.0;
        qreal maxX = sceneRect.right() - itemRect.width() / 2.0;
        qreal maxY = sceneRect.bottom() - itemRect.height() / 2.0;
        
        newPos.setX(std::max(minX, std::min(maxX, newPos.x())));
        newPos.setY(std::max(minY, std::min(maxY, newPos.y())));
        
        return newPos;
    }

    // 2. Уведомление об изменении (ItemPositionHasChanged)
    if (change == ItemPositionHasChanged) {
        emit positionChanged(m_data.id, pos());
        notifyConnectionsOfPositionChange();
    }

    return QGraphicsObject::itemChange(change, value); 
}

void Shape::notifyConnectionsOfPositionChange() {
    // Эта функция остается, чтобы соответствовать старому интерфейсу, 
    // но ее логику теперь полностью заменяет сигнал positionChanged.
}


// * --- Ellipse ---
Ellipse::Ellipse(const ShapeData& data) 
    : Shape(data) {
    
    // !!! Создаём делегата, передавая 'this' как родителя.
    // Родитель (Shape) автоматически становится владельцем и управляет памятью.
    m_graphicsDelegate = new QGraphicsEllipseItem(
        -data.width / 2, 
        -data.height / 2, 
        data.width, 
        data.height, 
        this // QGraphicsItem* parent
    );
    
    // Устанавливаем положение делегата в (0,0) относительно родителя (Shape)
    m_graphicsDelegate->setPos(0, 0);
}

QBrush Ellipse::shapeBrush() const {
    return QBrush(QColor(100, 149, 237, 180));
}

// * --- Rectangle ---
Rectangle::Rectangle(const ShapeData& data) 
    : Shape(data) {
    
    // !!! Создаём делегата
    m_graphicsDelegate = new QGraphicsRectItem(
        -data.width / 2, 
        -data.height / 2, 
        data.width, 
        data.height, 
        this // QGraphicsItem* parent
    );

    m_graphicsDelegate->setPos(0, 0);
}
// Методы paint и boundingRect делегированы в базовом классе Shape

QBrush Rectangle::shapeBrush() const {
    return QBrush(QColor(144, 238, 144, 180));
}

// * --- Polygon --- (Обновлен для использования делегата)
Polygon::Polygon(const ShapeData& data) : Shape(data) {
    
    // !!! Создаём делегата
    QGraphicsPolygonItem* polygonItem = new QGraphicsPolygonItem(
        getPolygon(), 
        this // QGraphicsItem* parent
    );
    
    m_graphicsDelegate = polygonItem;
    m_graphicsDelegate->setPos(0, 0);
}

QBrush Polygon::shapeBrush() const {
    return QBrush(QColor(255, 182, 193, 180));
}

// Методы Polygon::boundingRect/paint делегированы в Shape::boundingRect/paint

QPolygonF Polygon::getPolygon() const {
    if (m_data.polygonPoints.empty()) {
        QPolygonF triangle;
        triangle << QPointF(0, -50)
                 << QPointF(-50, 50)
                 << QPointF(50, 50);
        return triangle;
    } else {
        QPolygonF poly;
        for (const auto& pt : m_data.polygonPoints) {
            poly << pt;
        }
        return poly;
    }
}


// * --- ConnectionItem (линия связи) ---
ConnectionItem::ConnectionItem(int fromId, int toId, Shape* fromShape, Shape* toShape,
                               QGraphicsItem* parent)
    : QGraphicsLineItem(parent)
    , m_fromId(fromId)
    , m_toId(toId)
    , m_fromShape(fromShape)
    , m_toShape(toShape) {
    
    setZValue(-1);
    setPen(QPen(QColor(255, 255, 255), 2.5, Qt::SolidLine, Qt::RoundCap));
    updatePosition();
}
// ... (остальные методы ConnectionItem без изменений) ...

void ConnectionItem::updatePosition() {
    if (!m_fromShape || !m_toShape) {
        return;
    }
    
    QPointF startPos = m_fromShape->scenePos();
    QPointF endPos = m_toShape->scenePos();
    
    // Используем делегированный boundingRect()
    QRectF startBounds = m_fromShape->boundingRect(); 
    startBounds.translate(startPos);
    
    QRectF endBounds = m_toShape->boundingRect();
    endBounds.translate(endPos);
    
    QLineF connectionLine(startPos, endPos);
    
    QPointF startEdge = getEdgePoint(startBounds, connectionLine, true);
    QPointF endEdge = getEdgePoint(endBounds, connectionLine, false);
    
    setLine(QLineF(startEdge, endEdge));
}

void ConnectionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    painter->setPen(pen());
    painter->drawLine(line());
    
    QLineF edgeLine = line();
    double angle = std::atan2(-edgeLine.dy(), edgeLine.dx());
    
    const double arrowSize = 14.0;
    QPointF endPoint = edgeLine.p2();
    
    QPointF arrowP1 = endPoint - QPointF(std::sin(angle + M_PI / 3) * arrowSize,
                                         std::cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = endPoint - QPointF(std::sin(angle + M_PI - M_PI / 3) * arrowSize,
                                         std::cos(angle + M_PI - M_PI / 3) * arrowSize);
    
    QPolygonF arrowHead;
    arrowHead << endPoint << arrowP1 << arrowP2;
    
    painter->setBrush(QColor(255, 255, 255));
    painter->setPen(QPen(QColor(255, 255, 255), 1.5));
    painter->drawPolygon(arrowHead);
}

QPointF ConnectionItem::getEdgePoint(const QRectF& bounds, const QLineF& line, bool isStart) const {
    QPointF center = bounds.center();
    QPointF target = isStart ? line.p2() : line.p1();
    
    QLineF direction(center, target);
    double angle = direction.angle() * M_PI / 180.0;
    
    double halfWidth = bounds.width() / 2.0;
    double halfHeight = bounds.height() / 2.0;
    
    double dx = std::cos(angle);
    double dy = -std::sin(angle);
    
    double t = std::min(
        halfWidth / std::abs(dx),
        halfHeight / std::abs(dy)
    );
    
    return center + QPointF(dx * t * 0.85, dy * t * 0.85);
}

} // namespace domain