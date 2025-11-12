#include "domain/shape_types.h"

namespace domain {

ShapeData& ShapeData::operator=(ShapeData&& other) noexcept {
    if (this != &other) {
        // Сначала очищаем старое состояние, если нужно
        polygonPoints.clear();
        connectedShapeIds.clear();

        // Затем переносим данные
        id = other.id;
        type = other.type;
        position = std::move(other.position);
        width = other.width;
        height = other.height;
        polygonPoints = std::move(other.polygonPoints);
        connectedShapeIds = std::move(other.connectedShapeIds);
        isVisible = other.isVisible;

        // Обнуляем исходник
        other.id = -1;
        other.width = 0;
        other.height = 0;
        other.isVisible = false;
    }
    return *this;
}

ShapeData::ShapeData(ShapeData&& other) noexcept
    : id(other.id)
    , type(other.type)
    , position(std::move(other.position))
    , width(other.width)
    , height(other.height)
    , polygonPoints(std::move(other.polygonPoints))
    , connectedShapeIds(std::move(other.connectedShapeIds))
    , isVisible(other.isVisible)
{
    other.id = -1;
    other.width = 0;
    other.height = 0;
    other.isVisible = false;
}



} // namespace domain