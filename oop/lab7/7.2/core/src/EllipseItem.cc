// EllipseItem.cc
#include "EllipseItem.h"
#include "FiguresScene.h"

EllipseItem::EllipseItem(const QPointF& position, 
                         const QColor& color, 
                         qreal width, qreal height,
                         QGraphicsItem* parent)
    : QGraphicsEllipseItem(0, 0, width, height, parent), color_(color) 
{
    // Устанавливаем позицию
    setPos(position);
    
    // Включаем флаги для взаимодействия
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    setAcceptHoverEvents(true);
    
    // Устанавливаем внешний вид
    updateAppearance(false);
}

void EllipseItem::updateAppearance(bool isActive) {
    setBrush(QBrush(color_));
    
    if (isActive) {
        setPen(QPen(Qt::blue, 2, Qt::DashLine));
    } else {
        setPen(QPen(Qt::black, 2));
    }
}

QPainterPath EllipseItem::shape() const {
    // Точные границы эллипса (не прямоугольник!)
    QPainterPath path;
    path.addEllipse(rect());
    return path;
}

void EllipseItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
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
    QGraphicsEllipseItem::mousePressEvent(event);
}

void EllipseItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (isDragging_) {
        QPointF delta = event->scenePos() - lastMousePos_;
        moveBy(delta.x(), delta.y());
        lastMousePos_ = event->scenePos();
    }
}

void EllipseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isDragging_ = false;
        updateAppearance(false);
        update();
    }
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}
