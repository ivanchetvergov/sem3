#include "presentation/graphics_view.h"
#include "domain/shape_factory.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <cmath>

namespace presentation {

GraphicsView::GraphicsView(std::shared_ptr<infrastructure::ShapeRepository> repository,
                           QWidget* parent)
    : QGraphicsView(parent)
    , m_repository(repository) {
    
    m_scene = new QGraphicsScene(this);

    m_scene->setSceneRect(-500, -300, 1300, 500);
    setScene(m_scene);
    
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setMouseTracking(true);
    
    // разрешаем перетаскивание холста
    setDragMode(QGraphicsView::NoDrag);
    
    // подключаем сигналы репозитория (подписка на сигналы Boost)
    m_repository->shapeAdded.connect([this](int id) { onShapeAdded(id); });
    m_repository->shapeRemoved.connect([this](int id) { onShapeRemoved(id); });
    
    loadAllShapes();
}

void GraphicsView::addShape(std::unique_ptr<domain::Shape> shape) {
    if (!shape) {
        return;
    }
    
    int id = shape->id();
    m_shapes[id] = std::move(shape);
    viewport()->update();
}

void GraphicsView::refresh() {
    clearShapes();
    loadAllShapes();
}

void GraphicsView::clearShapes() {
    m_shapes.clear();
    viewport()->update();
}

void GraphicsView::updateShapeVisibility(int shapeId, bool visible) {
    auto it = m_shapes.find(shapeId);
    if (it != m_shapes.end() && it->second) {
        it->second->setVisible(visible);
        viewport()->update();
    }
}

void GraphicsView::drawBackground(QPainter* painter, const QRectF& rect) {
    QGraphicsView::drawBackground(painter, rect);
    
    // рисуем сетку
    painter->setPen(QPen(QColor(220, 220, 220), 0.8));
    
    const int gridSize = 50;
    int left = static_cast<int>(rect.left()) - (static_cast<int>(rect.left()) % gridSize);
    int top = static_cast<int>(rect.top()) - (static_cast<int>(rect.top()) % gridSize);
    
    for (int x = left; x < rect.right(); x += gridSize) {
        painter->drawLine(x, static_cast<int>(rect.top()), 
                         x, static_cast<int>(rect.bottom()));
    }
    
    for (int y = top; y < rect.bottom(); y += gridSize) {
        painter->drawLine(static_cast<int>(rect.left()), y, 
                         static_cast<int>(rect.right()), y);
    }
}

void GraphicsView::drawForeground(QPainter* painter, const QRectF& rect) {
    Q_UNUSED(rect)
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    // сначала рисуем все связи (только для видимых фигур)
    drawConnections(painter);
    
    // затем рисуем все фигуры
    for (const auto& [id, shape] : m_shapes) {
        if (!shape || !shape->isVisible()) {
            continue; // пропускаем невидимые фигуры
        }
        
        // подсвечиваем перетаскиваемую фигуру
        if (m_isDragging && id == m_draggedShapeId) {
            painter->save();
            painter->setOpacity(0.7);
            shape->draw(*painter);
            painter->restore();
            
            // рисуем контур вокруг перетаскиваемой фигуры
            painter->setPen(QPen(QColor(255, 165, 0), 3, Qt::DashLine));
            painter->setBrush(Qt::NoBrush);
            painter->drawRect(shape->boundingRect().adjusted(-5, -5, 5, 5));

        } else {
            shape->draw(*painter);
        }
        
        // отображаем ID при наведении
        if (id == m_hoveredShapeId && !m_isDragging) {
            painter->setPen(QPen(Qt::black));
            painter->setBrush(QBrush(QColor(255, 255, 200, 220)));
            painter->setFont(QFont("Arial", 9, QFont::Bold));
            
            QString idText = QString("ID: %1").arg(id);
            QRectF textBounds = painter->fontMetrics().boundingRect(idText);
            textBounds.adjust(-4, -2, 4, 2);
            textBounds.moveCenter(shape->position() + QPointF(0, -shape->boundingRect().height() / 2 - 15));
            
            painter->drawRoundedRect(textBounds, 3, 3);
            painter->drawText(textBounds, Qt::AlignCenter, idText);
        }
    }
}

void GraphicsView::onShapeAdded(int id) {
    auto shapeData = m_repository->findById(id);
    if (shapeData) {
        auto shape = domain::ShapeFactory::create(*shapeData);
        addShape(std::move(shape));
    }
}

void GraphicsView::onShapeRemoved(int id) {
    m_shapes.erase(id);
    viewport()->update();
}

void GraphicsView::loadAllShapes() {
    auto allShapes = m_repository->findAll();
    
    for (const auto& shapeData : allShapes) {
        auto shape = domain::ShapeFactory::create(shapeData);
        if (shape) {
            addShape(std::move(shape));
        }
    }
}

void GraphicsView::drawConnections(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing);
    
    for (const auto& [id, shape] : m_shapes) {
        if (!shape || !shape->isVisible()) {
            continue;
        }
        
        QPointF startPos = shape->position();
        QRectF startBounds = shape->boundingRect();
        
        for (int connectedId : shape->connectedShapes()) {
            auto it = m_shapes.find(connectedId);
            if (it != m_shapes.end() && it->second && it->second->isVisible()) {
                QPointF endPos = it->second->position();
                QRectF endBounds = it->second->boundingRect();
                
                // вычисляем точки на границах фигур
                QLineF connectionLine(startPos, endPos);
                
                // находим точки пересечения с границами
                QPointF startEdge = getEdgePoint(startBounds, connectionLine, true);
                QPointF endEdge = getEdgePoint(endBounds, connectionLine, false);
                
                // рисуем линию между границами с градиентом
                QLineF edgeLine(startEdge, endEdge);
                
                // основная линия
                QPen linePen(QColor(255, 255, 255), 2.5, Qt::SolidLine);
                linePen.setCapStyle(Qt::RoundCap);
                painter->setPen(linePen);
                painter->drawLine(edgeLine);
                
                // рисуем стрелку на конце линии
                double angle = std::atan2(-edgeLine.dy(), edgeLine.dx());
                
                const double arrowSize = 14.0;
                QPointF arrowP1 = endEdge - QPointF(std::sin(angle + M_PI / 3) * arrowSize,
                                                     std::cos(angle + M_PI / 3) * arrowSize);
                QPointF arrowP2 = endEdge - QPointF(std::sin(angle + M_PI - M_PI / 3) * arrowSize,
                                                     std::cos(angle + M_PI - M_PI / 3) * arrowSize);
                
                QPolygonF arrowHead;
                arrowHead << endEdge << arrowP1 << arrowP2;
                
                painter->setBrush(QColor(255, 255, 255));
                painter->setPen(QPen(QColor(255, 255, 255), 1.5));
                painter->drawPolygon(arrowHead);
            }
        }
    }
}

QPointF GraphicsView::getEdgePoint(const QRectF& bounds, const QLineF& line, bool isStart) const {
    QPointF center = bounds.center();
    QPointF target = isStart ? line.p2() : line.p1();
    
    // направление от центра к целевой точке
    QLineF direction(center, target);
    double angle = direction.angle() * M_PI / 180.0;
    
    // полуоси прямоугольника
    double halfWidth = bounds.width() / 2.0;
    double halfHeight = bounds.height() / 2.0;
    
    // вычисляем точку на эллипсе/прямоугольнике
    double dx = std::cos(angle);
    double dy = -std::sin(angle);
    
    // для прямоугольных границ используем простое пересечение
    double t = std::min(
        halfWidth / std::abs(dx),
        halfHeight / std::abs(dy)
    );
    // 0.85 для небольшого отступа
    return center + QPointF(dx * t * 0.85, dy * t * 0.85); 
}

QPointF GraphicsView::getShapeCenter(int shapeId) const {
    auto it = m_shapes.find(shapeId);
    if (it != m_shapes.end() && it->second) {
        return it->second->position();
    }
    return QPointF();
}

void GraphicsView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->pos());
        int shapeId = findShapeAtPosition(scenePos);
        
        if (shapeId != -1) {
            m_draggedShapeId = shapeId;
            m_dragStartPos = event->pos();
            m_shapeOriginalPos = m_shapes[shapeId]->position();
            m_isDragging = true;
            setCursor(Qt::ClosedHandCursor);
            viewport()->update();
            return;
        }
    }
    
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event) {
    if (m_isDragging && m_draggedShapeId != -1) {
        QPointF currentPos = event->pos();
        QPointF delta = mapToScene(currentPos.toPoint()) - mapToScene(m_dragStartPos.toPoint());
        
        QPointF newPos = m_shapeOriginalPos + delta;
        
        // ограничиваем перемещение границами сцены
        QRectF sceneRect = m_scene->sceneRect();
        newPos.setX(qBound(sceneRect.left() + 50.0, newPos.x(), sceneRect.right() - 50.0));
        newPos.setY(qBound(sceneRect.top() + 50.0, newPos.y(), sceneRect.bottom() - 50.0));
        
        m_shapes[m_draggedShapeId]->setPosition(newPos);
        viewport()->update();
        return;
    }
    
    // изменяем курсор и отслеживаем наведение
    QPointF scenePos = mapToScene(event->pos());
    int shapeId = findShapeAtPosition(scenePos);
    
    if (shapeId != m_hoveredShapeId) {
        m_hoveredShapeId = shapeId;
        viewport()->update();
    }
    
    if (shapeId != -1 && !m_isDragging) {
        setCursor(Qt::OpenHandCursor);
    } else if (!m_isDragging) {
        setCursor(Qt::ArrowCursor);
    }
    
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && m_isDragging && m_draggedShapeId != -1) {
        // сохраняем новую позицию в базу данных
        auto shapeOpt = m_repository->findById(m_draggedShapeId);
        if (shapeOpt) {
            auto shapeData = *shapeOpt;
            shapeData.position = m_shapes[m_draggedShapeId]->position();
            m_repository->update(shapeData);
            
            emit shapePositionChanged(m_draggedShapeId, shapeData.position);
        }
        
        m_isDragging = false;
        m_draggedShapeId = -1;
        setCursor(Qt::ArrowCursor);
        viewport()->update();
        return;
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

int GraphicsView::findShapeAtPosition(const QPointF& pos) const {
    for (auto const& [id, shape] : m_shapes) { 
        if (shape && shape->isVisible() && shape->boundingRect().contains(pos)) {
            return id; 
        }
    }
    return -1;
}

} // namespace presentation