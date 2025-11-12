#include "domain/shape.h"
#include <algorithm>
#include <QPen>
#include <QBrush>
#include <random>

namespace domain {

// * --- Shape ---
Shape::Shape(const ShapeData& data) : m_data(data) {}

void Shape::addConnection(int shapeId) {
    // * проверка на уникальность айди
    if (std::find(m_data.connectedShapeIds.begin(), 
                  m_data.connectedShapeIds.end(), 
                  shapeId) == m_data.connectedShapeIds.end()) {
        m_data.connectedShapeIds.push_back(shapeId);
    }
}

void Shape::removeConnection(int shapeId) {
    // * проверка на не уникальность айди
    auto it = std::find(m_data.connectedShapeIds.begin(), 
                        m_data.connectedShapeIds.end(), 
                        shapeId);
    if (it != m_data.connectedShapeIds.end()) {
        m_data.connectedShapeIds.erase(it);
    }
}

// * --- Ellipse ---
void Ellipse::draw(QPainter& painter) const {
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(QBrush(QColor(100, 149, 237, 180)));
    
    QRectF rect(m_data.position.x() - m_data.width / 2,
                m_data.position.y() - m_data.height / 2,
                m_data.width,
                m_data.height);
    painter.drawEllipse(rect);
}

QRectF Ellipse::boundingRect() const {
    return QRectF(m_data.position.x() - m_data.width / 2,
                  m_data.position.y() - m_data.height / 2,
                  m_data.width,
                  m_data.height);
}

// * --- Rect ---
void Rectangle::draw(QPainter& painter) const {
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(QBrush(QColor(144, 238, 144, 180)));
    
    QRectF rect(m_data.position.x() - m_data.width / 2,
                m_data.position.y() - m_data.height / 2,
                m_data.width,
                m_data.height);
    painter.drawRect(rect);
}

QRectF Rectangle::boundingRect() const {
    return QRectF(m_data.position.x() - m_data.width / 2,
                  m_data.position.y() - m_data.height / 2,
                  m_data.width,
                  m_data.height);
}

// * --- Polygon ---
void Polygon::draw(QPainter& painter) const {
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(QBrush(QColor(255, 182, 193, 180)));
    
    if (m_data.polygonPoints.empty()) {
        // Default triangle if no points specified
        QPolygonF triangle;
        triangle << QPointF(m_data.position.x(), m_data.position.y() - 50)
                 << QPointF(m_data.position.x() - 50, m_data.position.y() + 50)
                 << QPointF(m_data.position.x() + 50, m_data.position.y() + 50);
        painter.drawPolygon(triangle);
    } else {
        QPolygonF poly;
        for (const auto& pt : m_data.polygonPoints) {
            poly << (m_data.position + pt);
        }
        painter.drawPolygon(poly);
    }
}

QRectF Polygon::boundingRect() const {
    if (m_data.polygonPoints.empty()) {
        return QRectF(m_data.position.x() - 50,
                      m_data.position.y() - 50,
                      100, 100);
    }
    
    QRectF rect;
    for (const auto& pt : m_data.polygonPoints) {
        rect |= QRectF(m_data.position + pt, QSizeF(1, 1));
    }
    return rect;
}

} // namespace domain