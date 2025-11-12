#pragma once

#include "domain/shape.h"
#include "infrastructure/shape_repository.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <memory>
#include <unordered_map>

namespace presentation {
/*
 * @brief Пользовательское представление для отображения фигур (QGraphicsView).
 *
 * Отвечает за визуализацию всех объектов-фигур, их связей и обработку
 * пользовательского взаимодействия в графической области. Подписывается
 * на сигналы репозитория для автоматического обновления.
 */
class GraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit GraphicsView(std::shared_ptr<infrastructure::ShapeRepository> repository,
                         QWidget* parent = nullptr);

    void addShape(std::unique_ptr<domain::Shape> shape);
    void refresh();         // принудительно перезагружает все фигуры из репозитория
    void clearShapes();     // очищает локальный кэш фигур (m_shapes)
    void updateShapeVisibility(int shapeId, bool visible); // меняет флаг видимости фигуры

signals:
    void shapeSelected(int shapeId);                                    // сигнал при выборе фигуры
    void shapePositionChanged(int shapeId, const QPointF& newPosition); // сигнал при изменении ее позиции

protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override; // рисует фон
    void drawForeground(QPainter* painter, const QRectF& rect) override; // рисует поверх всего
    void mousePressEvent(QMouseEvent* event) override;      // нажатие кнопки мыши 
    void mouseMoveEvent(QMouseEvent* event) override;       // передвижение мыши
    void mouseReleaseEvent(QMouseEvent* event) override;    // отпускание кнопки мыши

private slots:
    void onShapeAdded(int id);      // обрабатывает сигнал от репозитория о доб. фигуры
    void onShapeRemoved(int id);    // сигнал об удалении фигуры

private:
    void loadAllShapes();           // загружает все фигуры из репозитория в кэш m_shapes
    void drawConnections(QPainter* painter); 
    QPointF getShapeCenter(int shapeId) const;
    QPointF getEdgePoint(const QRectF& bounds, const QLineF& line, bool isStart) const;
    int findShapeAtPosition(const QPointF& pos) const;

    // * UI / Core
    QGraphicsScene* m_scene; // графическая сцена
    std::shared_ptr<infrastructure::ShapeRepository> m_repository;      // репозиторий для доступа к БД
    std::unordered_map<int, std::unique_ptr<domain::Shape>> m_shapes;   // кэш всех фигур отс. по ID
    
    // * Drag and drop state (Состояние перетаскивания)
    int m_draggedShapeId{-1};
    QPointF m_dragStartPos;
    QPointF m_shapeOriginalPos;
    bool m_isDragging{false};
    int m_hoveredShapeId{-1};
};

} // namespace presentation