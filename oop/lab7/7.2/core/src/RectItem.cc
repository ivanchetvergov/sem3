// RectItem.cc
#include "RectItem.h"
#include "FiguresScene.h"

RectItem::RectItem(const QPointF& position, 
                   const QColor& color, 
                   qreal width, qreal height,
                   QGraphicsItem* parent)
    : QGraphicsRectItem(0, 0, width, height, parent), color_(color) 
{
    // Устанавливаем позицию
    setPos(position);
    
    // Включаем флаги для взаимодействия
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    
    // Устанавливаем внешний вид
    updateAppearance(false);
}

void RectItem::updateAppearance(bool isActive) {
    setBrush(QBrush(color_));
    
    if (isActive) {
        setPen(QPen(Qt::blue, 2, Qt::DashLine));
    } else {
        setPen(QPen(Qt::black, 2));
    }
}

QPainterPath RectItem::shape() const {
    // Точные границы прямоугольника
    QPainterPath path;
    path.addRect(rect());
    return path;
}

void RectItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isDragging_ = true;
        lastMousePos_ = event->scenePos();
        
        // Поднимаем фигуру на передний план через сцену
        if (FiguresScene* figScene = qobject_cast<FiguresScene*>(scene())) {
            figScene->bringToFront(this);
        }
        
        updateAppearance(true);
        update();
    }
    QGraphicsRectItem::mousePressEvent(event);
}

void RectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (isDragging_) {
        QPointF delta = event->scenePos() - lastMousePos_;
        moveBy(delta.x(), delta.y());
        lastMousePos_ = event->scenePos();
    }
}

void RectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isDragging_ = false;
        updateAppearance(false);
        update();
    }
    QGraphicsRectItem::mouseReleaseEvent(event);
}
