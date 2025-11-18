#include "presentation/graphics_view.h"
#include "domain/shape_factory.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>

namespace presentation {

GraphicsView::GraphicsView(std::shared_ptr<infrastructure::ShapeRepository> repository,
                           QWidget* parent)
    : QGraphicsView(parent)
    , m_repository(repository) {
    
    // создаём и настраиваем сцену
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(-500, -300, 1300, 500);
    setScene(m_scene);
    
    // настройки рендеринга
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // режим взаимодействия
    setDragMode(QGraphicsView::RubberBandDrag); // резиновое выделение
    
    // подключаем сигналы репозитория
    m_repository->shapeAdded.connect([this](int id) { onShapeAdded(id); });
    m_repository->shapeRemoved.connect([this](int id) { onShapeRemoved(id); });
    
    // !!! ИСПРАВЛЕНО: QGraphicsScene::changed подключается только ОДИН раз (для оптимизации)
    connect(m_scene, &QGraphicsScene::changed, this, &GraphicsView::onShapeMoved);
    
    loadAllShapes();
}

void GraphicsView::addShape(domain::Shape* shape) {
    if (!shape) {
        return;
    }
    
    int id = shape->id();
    
    // добавляем фигуру на сцену (сцена становится владельцем)
    m_scene->addItem(shape);
    m_shapes[id] = shape;
    
    // !!! УДАЛЕНО: Redundant connect, теперь он в конструкторе
    
    // обновляем связи после добавления новой фигуры
    updateConnections();
}

void GraphicsView::refresh() {
    clearShapes();
    loadAllShapes();
}

void GraphicsView::clearShapes() {
    // удаляем все линии связей
    for (auto* connection : m_connections) {
        m_scene->removeItem(connection);
        delete connection;
    }
    m_connections.clear();
    
    // удаляем все фигуры
    for (auto& [id, shape] : m_shapes) {
        m_scene->removeItem(shape);
        delete shape;
    }
    m_shapes.clear();
}

void GraphicsView::updateShapeVisibility(int shapeId, bool visible) {
    auto it = m_shapes.find(shapeId);
    if (it != m_shapes.end() && it->second) {
        it->second->setShapeVisible(visible);
        
        // обновляем связи, т.к. видимость фигуры изменилась
        updateConnections();
    }
}

void GraphicsView::drawBackground(QPainter* painter, const QRectF& rect) {
    QGraphicsView::drawBackground(painter, rect);
    
    // рисуем сетку для удобства позиционирования
    painter->setPen(QPen(QColor(220, 220, 220), 1));
    
    const int gridSize = 50;
    int left = static_cast<int>(rect.left()) - (static_cast<int>(rect.left()) % gridSize);
    int top = static_cast<int>(rect.top()) - (static_cast<int>(rect.top()) % gridSize);
    
    // вертикальные линии
    for (int x = left; x < rect.right(); x += gridSize) {
        painter->drawLine(x, static_cast<int>(rect.top()), 
                         x, static_cast<int>(rect.bottom()));
    }
    
    // горизонтальные линии
    for (int y = top; y < rect.bottom(); y += gridSize) {
        painter->drawLine(static_cast<int>(rect.left()), y, 
                         static_cast<int>(rect.right()), y);
    }
}

void GraphicsView::onShapeAdded(int id) {
    auto shapeData = m_repository->findById(id);
    if (shapeData) {
        // создаём фигуру через фабрику
        auto shapePtr = domain::ShapeFactory::create(*shapeData);
        
        // добавляем на сцену (передаём владение)
        addShape(shapePtr.release());
    }
}

void GraphicsView::onShapeRemoved(int id) {
    auto it = m_shapes.find(id);
    if (it != m_shapes.end()) {
        domain::Shape* shape = it->second;
        
        // удаляем со сцены
        m_scene->removeItem(shape);
        delete shape;
        
        // удаляем из кэша
        m_shapes.erase(it);
        
        // обновляем связи
        updateConnections();
    }
}

void GraphicsView::onShapeMoved() {
    // обновляем позиции всех линий связей при перемещении любой фигуры
    for (auto* connection : m_connections) {
        connection->updatePosition();
    }
}

void GraphicsView::loadAllShapes() {
    auto allShapes = m_repository->findAll();
    
    for (const auto& shapeData : allShapes) {
        auto shapePtr = domain::ShapeFactory::create(shapeData);
        if (shapePtr) {
            addShape(shapePtr.release());
        }
    }
    
    // после загрузки всех фигур создаём линии связей
    createConnectionItems();
}

void GraphicsView::updateConnections() {
    // удаляем старые линии
    for (auto* connection : m_connections) {
        m_scene->removeItem(connection);
        delete connection;
    }
    m_connections.clear();
    
    // создаём новые
    createConnectionItems();
}

void GraphicsView::createConnectionItems() {
    // проходим по всем фигурам и создаём линии связей
    for (const auto& [fromId, fromShape] : m_shapes) {
        if (!fromShape || !fromShape->isVisible()) {
            continue; // пропускаем невидимые фигуры
        }
        
        const auto& connections = fromShape->connectedShapes();
        
        for (int toId : connections) {
            auto toIt = m_shapes.find(toId);
            if (toIt != m_shapes.end() && toIt->second && toIt->second->isVisible()) {
                // создаём линию связи
                auto* connectionItem = new domain::ConnectionItem(
                    fromId, toId, fromShape, toIt->second
                );
                
                // добавляем на сцену
                m_scene->addItem(connectionItem);
                m_connections.push_back(connectionItem);
            }
        }
    }
}

void GraphicsView::mousePressEvent(QMouseEvent* event) {
    m_isMousePressed = true;
    m_lastMousePos = event->pos();
    
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    if (m_isMousePressed) {
        // проверяем, была ли перемещена какая-то фигура
        QGraphicsItem* item = itemAt(event->pos());
        
        if (item && item->type() >= QGraphicsItem::UserType) {
            // это наша фигура
            domain::Shape* shape = dynamic_cast<domain::Shape*>(item);
            if (shape) {
                // сохраняем новую позицию в БД
                auto shapeOpt = m_repository->findById(shape->id());
                if (shapeOpt) {
                    auto shapeData = *shapeOpt;
                    shapeData.position = shape->scenePos();
                    m_repository->update(shapeData);
                    
                    emit shapePositionChanged(shape->id(), shapeData.position);
                }
            }
        }
        
        m_isMousePressed = false;
    }
    
    QGraphicsView::mouseReleaseEvent(event);
}

} // namespace presentation