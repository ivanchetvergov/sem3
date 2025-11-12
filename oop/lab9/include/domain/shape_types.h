#pragma once

#include <QString>
#include <QVector>
#include <QPointF>
#include <optional>
#include <vector>
#include <memory>

namespace domain {  
/*
 * @brief Перечисление типов геометрических фигур.
*/
enum class ShapeType {
    Ellipse,
    Rectangle,
    Polygon
};

/*
* @brief Структура данных, хранящая состояние фигуры.
*
* Содержит все параметры, необходимые для создания, хранения и отображения.
*/
struct ShapeData {
    int id{-1}; // значение по умолчанию, чтобы избежать UB
    ShapeType type{ShapeType::Rectangle};
    QPointF position{0.0, 0.0};
    double width{100.0};
    double height{100.0};
    std::vector<QPointF> polygonPoints;
    std::vector<int> connectedShapeIds;
    bool isVisible{true};

    // ! Правило пяти (The Rule of Five)
    // явное объявление =default предотвращает ошибку компилятора/STL,
    // которая приводила к неверному побитовому перемещению (memcpy)
    ShapeData() = default; 
    ShapeData(const ShapeData& other) = default;
    ShapeData& operator=(const ShapeData& other) = default;
    ShapeData(ShapeData&& other) noexcept;
    ShapeData& operator=(ShapeData&& other) noexcept;
    ~ShapeData() = default;
};

// * --- Методы для преобразования фигуры ---
// inline предотвращает ошибки линковки и ускоряет код
inline QString shapeTypeToString(ShapeType type) {
    switch (type) {
        case ShapeType::Ellipse: return "Ellipse";
        case ShapeType::Rectangle: return "Rectangle";
        case ShapeType::Polygon: return "Polygon";
    }
    return "Unknown";
}

inline std::optional<ShapeType> stringToShapeType(const QString& str) {
    if (str == "Ellipse") return ShapeType::Ellipse;
    if (str == "Rectangle") return ShapeType::Rectangle;
    if (str == "Polygon") return ShapeType::Polygon;
    return std::nullopt;
}

} // namespace domain