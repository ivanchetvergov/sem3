#pragma once

#include "shape_types.h"
#include <QPainter>
#include <memory>

namespace domain {

/*
 * @brief Базовый абстрактный класс для всех геометрических фигур.
 *
 * Реализует паттерн Полиморфизма, определяя общий интерфейс(API).
 */
class Shape {
public:
    explicit Shape(const ShapeData& data);
    virtual ~Shape() = default;

    // * --- (Disabled Copy, Enabled Move) ---
    Shape(const Shape&) = delete;
    Shape& operator=(const Shape&) = delete;
    Shape(Shape&&) = default;
    Shape& operator=(Shape&&) = default;

    // *  контракт интерфейса геттеров:
    // 1. [[nodiscard]]: обязывает использовать возвращаемое значение.
    // 2. noexcept: гарантирует, что метод не генерирует исключений
    [[nodiscard]] int id() const noexcept { return m_data.id; }
    [[nodiscard]] ShapeType type() const noexcept { return m_data.type; }
    [[nodiscard]] const QPointF& position() const noexcept { return m_data.position; }
    [[nodiscard]] bool isVisible() const noexcept { return m_data.isVisible; }
    [[nodiscard]] const std::vector<int>& connectedShapes() const noexcept { return m_data.connectedShapeIds; }
    [[nodiscard]] const ShapeData& data() const noexcept { return m_data; }

    // * --- сеттеры и управление связями ---
    void setPosition(const QPointF& pos) { m_data.position = pos; }
    void setVisible(bool visible) { m_data.isVisible = visible; }
    void addConnection(int shapeId);
    void removeConnection(int shapeId);

    virtual void draw(QPainter& painter) const = 0;
    virtual QRectF boundingRect() const = 0;

protected:
    ShapeData m_data;
};

/*
 * @brief Реализация фигуры: Эллипс.
 */
class Ellipse : public Shape {
public:
    explicit Ellipse(const ShapeData& data) : Shape(data) {}
    void draw(QPainter& painter) const override;
    QRectF boundingRect() const override;
};

/*
 * @brief Реализация фигуры: Прямоугольник.
 */
class Rectangle : public Shape {
public:
    explicit Rectangle(const ShapeData& data) : Shape(data) {}
    void draw(QPainter& painter) const override;
    QRectF boundingRect() const override;
};

/*
 * @brief Реализация фигуры: Многоугольник.
 */
class Polygon : public Shape {
public:
    explicit Polygon(const ShapeData& data) : Shape(data) {}
    void draw(QPainter& painter) const override;
    QRectF boundingRect() const override;
};

} // namespace domain