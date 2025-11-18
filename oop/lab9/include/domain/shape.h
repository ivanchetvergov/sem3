#pragma once

#include "shape_types.h"
#include <QGraphicsObject>       // Используем QGraphicsObject для будущей поддержки сигналов
#include <QGraphicsEllipseItem>  // Требуется для Ellipse
#include <QGraphicsRectItem>     // Требуется для Rectangle
#include <QGraphicsPolygonItem>  // Требуется для Polygon
#include <QPainter>
#include <memory>
#include <QGraphicsLineItem>

namespace domain {

/*
 * @brief Базовый абстрактный класс для всех геометрических фигур.
 *
 * Теперь наследуется от QGraphicsObject для интеграции с Qt Framework.
 * Позволяет использовать сигналы/слоты.
 */
class Shape : public QGraphicsObject { // !!! QGraphicsObject !!!
    Q_OBJECT 
public:
    explicit Shape(const ShapeData& data);
    virtual ~Shape() = default;

    // * --- (Disable) ---
    Shape(const Shape&) = delete;
    Shape& operator=(const Shape&) = delete;
    Shape(Shape&&) = delete;
    Shape& operator=(Shape&&) = delete;

    // * контракт интерфейса геттеров:
    // 1. [[nodiscard]]: обязывает использовать возвращаемое значение.
    // 2. noexcept: гарантирует, что метод не генерирует исключений
    [[nodiscard]] int id() const noexcept { return m_data.id; }
    [[nodiscard]] ShapeType shapeType() const noexcept { return m_data.type; }    
    [[nodiscard]] QPointF position() const noexcept { return pos(); }
    [[nodiscard]] bool isVisible() const noexcept { return m_data.isVisible; }
    [[nodiscard]] const std::vector<int>& connectedShapes() const noexcept { return m_data.connectedShapeIds; }
    [[nodiscard]] const ShapeData& data() const noexcept { return m_data; }

    // * --- сеттеры и управление связями ---
    void setPosition(const QPointF& pos) { setPos(pos); }
    void setShapeVisible(bool visible);
    void addConnection(int shapeId);
    void removeConnection(int shapeId);

    // * --- QGraphicsItem обязательные методы (должны быть реализованы в потомках) ---
    // !!! Больше не чисто виртуальные, но переопределяются для делегирования
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
               QWidget* widget = nullptr) override;

    // переопределяем itemChange для обработки изменения позиции
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    // * --- Дополнительные методы для взаимодействия ---
    enum { Type = UserType + 1 };
    int type() const override { return Type; }

    // виртуальный метод для уведомления об изменении позиции
    virtual void notifyConnectionsOfPositionChange();

signals:
    // сигнал, который может слушать GraphicsView
    void positionChanged(int shapeId, const QPointF& newPos);

protected:
    ShapeData m_data;
    QGraphicsItem* m_graphicsDelegate = nullptr;
    // вспомогательный метод для подсветки при наведении/выделении
    void drawSelectionHighlight(QPainter* painter) const;

    // Виртуальные методы для получения Pen и Brush, чтобы потомки могли их использовать
    virtual QPen shapePen() const { return QPen(Qt::black, 2); }
    virtual QBrush shapeBrush() const = 0; // Теперь эта логика стала абстрактной!
};

/*
 * @brief Реализация фигуры: Эллипс. Делегирует отрисовку QGraphicsEllipseItem.
 */
class Ellipse : public Shape, private QGraphicsEllipseItem { // !!! ИЗМЕНЕНО: Приватное наследование !!!
public:
    explicit Ellipse(const ShapeData& data);
    
    // !!! Методы boundingRect() и paint() наследуются от QGraphicsEllipseItem
    // Мы явно переопределяем их для делегирования
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
               QWidget* widget = nullptr) override;
    
    enum { Type = UserType + 2 };
    int type() const override { return Type; }
protected:
    QBrush shapeBrush() const override;
};

/*
 * @brief Реализация фигуры: Прямоугольник. Делегирует отрисовку QGraphicsRectItem.
 */
class Rectangle : public Shape, private QGraphicsRectItem { // !!! ИЗМЕНЕНО: Приватное наследование !!!
public:
    explicit Rectangle(const ShapeData& data);
    
    // !!! Методы boundingRect() и paint() наследуются от QGraphicsRectItem
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
               QWidget* widget = nullptr) override;
    
    enum { Type = UserType + 3 };
    int type() const override { return Type; }
protected:
    QBrush shapeBrush() const override;
};

/*
 * @brief Реализация фигуры: Многоугольник. Делегирует отрисовку QGraphicsPolygonItem.
 * Оставим Polygon как есть, но используем делегирование, чтобы не дублировать код.
 */
class Polygon : public Shape {
public:
    explicit Polygon(const ShapeData& data);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, 
               QWidget* widget = nullptr) override;
    
    enum { Type = UserType + 4 };
    int type() const override { return Type; }

private:
    QPolygonF getPolygon() const; // вспомогательный метод для получения полигона
protected:
    QBrush shapeBrush() const override;
};


class ConnectionItem : public QGraphicsLineItem {
public:
    ConnectionItem(int fromId, int toId, Shape* fromShape, Shape* toShape,
                   QGraphicsItem* parent = nullptr);
    
    [[nodiscard]] int fromShapeId() const noexcept { return m_fromId; }
    [[nodiscard]] int toShapeId() const noexcept { return m_toId; }
    
    void updatePosition();
    
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;
    
    enum { Type = UserType + 10 };
    int type() const override { return Type; }

private:
    int m_fromId;
    int m_toId;
    Shape* m_fromShape; 
    Shape* m_toShape;   
    
    QPointF getEdgePoint(const QRectF& bounds, const QLineF& line, bool isStart) const;
};

} // namespace domain