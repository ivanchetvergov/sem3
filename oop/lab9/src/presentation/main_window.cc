#include "presentation/main_window.h"
#include "domain/shape_factory.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QToolBar>
#include <QStatusBar>
#include <QHeaderView>
#include <QRandomGenerator>

namespace presentation {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // инициализация базы данных
    m_dbManager = std::make_unique<infrastructure::DatabaseManager>();
    if (!m_dbManager->initialize()) {
        QMessageBox::critical(this, "Ошибка", 
            "Не удалось инициализировать базу данных: " + m_dbManager->lastError());
        return;
    }
    
    m_repository = std::make_shared<infrastructure::ShapeRepository>(*m_dbManager);
    
    setupUi();
    setupConnections();
    
    statusBar()->showMessage("Готов к работе");
}

void MainWindow::setupUi() {
    setWindowTitle("Графические примитивы - Qt Application");
    resize(1400, 800);
    
    // центральный виджет со сплиттером
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(centralWidget);
    
    auto* splitter = new QSplitter(Qt::Vertical, this);
    
    // графическое представление
    m_graphicsView = new GraphicsView(m_repository, this);
    splitter->addWidget(m_graphicsView);
    
    // таблица с данными
    m_tableView = new QTableView(this);
    m_shapeModel = new ShapeModel(m_repository, this);
    m_shapeDelegate = new ShapeDelegate(this);
    
    m_tableView->setModel(m_shapeModel);
    m_tableView->setItemDelegateForColumn(ShapeModel::CountColumn, m_shapeDelegate);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    
    splitter->addWidget(m_tableView);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
    
    mainLayout->addWidget(splitter);
    setCentralWidget(centralWidget);
    
    createToolbar();
}

void MainWindow::createToolbar() {
    auto* toolbar = addToolBar("Tools");
    toolbar->setMovable(false);
    
    // группа добавления фигур
    auto* addGroup = new QGroupBox("Add figure", this);
    auto* addLayout = new QHBoxLayout(addGroup);
    
    addLayout->addWidget(new QLabel("Type:", this));
    m_shapeTypeCombo = new QComboBox(this);
    m_shapeTypeCombo->addItem("Ellipse", static_cast<int>(domain::ShapeType::Ellipse));
    m_shapeTypeCombo->addItem("Rect", static_cast<int>(domain::ShapeType::Rectangle));
    m_shapeTypeCombo->addItem("Polygon", static_cast<int>(domain::ShapeType::Polygon));
    addLayout->addWidget(m_shapeTypeCombo);
    
    addLayout->addWidget(new QLabel("X:", this));
    m_xPosSpin = new QSpinBox(this);
    m_xPosSpin->setRange(-400, 400);
    m_xPosSpin->setValue(0);
    addLayout->addWidget(m_xPosSpin);
    
    addLayout->addWidget(new QLabel("Y:", this));
    m_yPosSpin = new QSpinBox(this);
    m_yPosSpin->setRange(-300, 300);
    m_yPosSpin->setValue(0);
    addLayout->addWidget(m_yPosSpin);
    
    addLayout->addWidget(new QLabel("W:", this));
    m_widthSpin = new QSpinBox(this);
    m_widthSpin->setRange(10, 200);
    m_widthSpin->setValue(80);
    addLayout->addWidget(m_widthSpin);
    
    addLayout->addWidget(new QLabel("H:", this));
    m_heightSpin = new QSpinBox(this);
    m_heightSpin->setRange(10, 200);
    m_heightSpin->setValue(80);
    addLayout->addWidget(m_heightSpin);
    
    auto* addBtn = new QPushButton("Add", this);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddShapeClicked);
    addLayout->addWidget(addBtn);
    
    toolbar->addWidget(addGroup);
    toolbar->addSeparator();
    
    // группа связей
    auto* connectionGroup = new QGroupBox("Links", this);
    auto* connLayout = new QHBoxLayout(connectionGroup);
    
    connLayout->addWidget(new QLabel("ID:", this));
    m_fromShapeIdSpin = new QSpinBox(this);
    m_fromShapeIdSpin->setMinimum(1);
    connLayout->addWidget(m_fromShapeIdSpin);
    
    connLayout->addWidget(new QLabel("ID:", this));
    m_toShapeIdSpin = new QSpinBox(this);
    m_toShapeIdSpin->setMinimum(1);
    connLayout->addWidget(m_toShapeIdSpin);
    
    auto* addConnBtn = new QPushButton("Link", this);
    connect(addConnBtn, &QPushButton::clicked, this, &MainWindow::onAddConnectionClicked);
    connLayout->addWidget(addConnBtn);
    
    toolbar->addWidget(connectionGroup);
    toolbar->addSeparator();
    
    // группа фильтров
    auto* filterGroup = new QGroupBox("Filters", this);
    auto* filterLayout = new QHBoxLayout(filterGroup);
    
    m_filterEllipseCheck = new QCheckBox("Ellipses", this);
    m_filterEllipseCheck->setChecked(true);
    filterLayout->addWidget(m_filterEllipseCheck);
    
    m_filterRectangleCheck = new QCheckBox("Rects", this);
    m_filterRectangleCheck->setChecked(true);
    filterLayout->addWidget(m_filterRectangleCheck);
    
    m_filterPolygonCheck = new QCheckBox("Polygons", this);
    m_filterPolygonCheck->setChecked(true);
    filterLayout->addWidget(m_filterPolygonCheck);
    
    toolbar->addWidget(filterGroup);
    toolbar->addSeparator();
    
    // кнопки управления
    auto* removeBtn = new QPushButton("Remove Selected", this);
    connect(removeBtn, &QPushButton::clicked, this, &MainWindow::onRemoveShapeClicked);
    toolbar->addWidget(removeBtn);
    
    auto* refreshBtn = new QPushButton("Refresh", this);
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    toolbar->addWidget(refreshBtn);
}

void MainWindow::setupConnections() {
    // * QCheckBox
    connect(m_filterEllipseCheck, &QCheckBox::checkStateChanged, 
            this, &MainWindow::onFilterChanged);
    connect(m_filterRectangleCheck, &QCheckBox::checkStateChanged, 
            this, &MainWindow::onFilterChanged);
    connect(m_filterPolygonCheck, &QCheckBox::checkStateChanged, 
            this, &MainWindow::onFilterChanged);
    
    connect(m_shapeModel, &ShapeModel::shapeVisibilityChanged,
            this, &MainWindow::updateGraphicsView);
    
    connect(m_graphicsView, &GraphicsView::shapePositionChanged,
            [this](int shapeId, const QPointF& newPos) {
                statusBar()->showMessage(
                    QString("Shape %1 moved into (%2, %3)")
                        .arg(shapeId)
                        .arg(qRound(newPos.x()))
                        .arg(qRound(newPos.y())),
                    3000);
                m_shapeModel->refresh();
            });
}

void MainWindow::onAddShapeClicked() {
    auto type = static_cast<domain::ShapeType>(m_shapeTypeCombo->currentData().toInt());
    
    auto* generator = QRandomGenerator::global();
    qreal randX = generator->bounded(-450, 551);
    qreal randY = generator->bounded(-250, 151);
        
    // обновляем спин-боксы новым значением
    m_xPosSpin->setValue(qRound(randX));
    m_yPosSpin->setValue(qRound(randY));

    auto shapeData = domain::ShapeFactory::createDefaultShapeData(
        type, 
        QPointF(m_xPosSpin->value(), m_yPosSpin->value())
    );
    
    shapeData.width = m_widthSpin->value();
    shapeData.height = m_heightSpin->value();
    
    auto id = m_repository->save(shapeData);
    if (id) {
        statusBar()->showMessage(QString("Фигура добавлена с ID: %1").arg(*id), 3000);
        m_shapeModel->refresh();
        updateGraphicsView();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить фигуру");
    }
}

void MainWindow::onRemoveShapeClicked() {
    auto selectedIndexes = m_tableView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::information(this, "Информация", "Выберите фигуру для удаления");
        return;
    }
    
    int row = selectedIndexes.first().row();
    auto shapeData = m_shapeModel->getShapeData(row);
    
    auto reply = QMessageBox::question(this, "Подтверждение",
        QString("Удалить фигуру с ID %1?").arg(shapeData.id),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_tableView->selectionModel()->clearSelection(); // cброс выделения
        m_tableView->setCurrentIndex(QModelIndex());     // cброс текущего индекса

        if (m_shapeModel->removeRow(row)) {
            statusBar()->showMessage("Фигура удалена", 3000);
            updateGraphicsView(); 
            m_tableView->selectionModel()->clearSelection();

            if (m_shapeModel->rowCount() > 0) {
                // выбираем новую строку на месте удаленной или последнюю
                int newRow = std::min(row, m_shapeModel->rowCount() - 1);
                
                QModelIndex newIndex = m_shapeModel->index(newRow, 0);
                m_tableView->setCurrentIndex(newIndex);
                m_tableView->selectionModel()->select(
                    newIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows
                );
            }
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить фигуру");
        }
    }
}

void MainWindow::onAddConnectionClicked() {
    int fromId = m_fromShapeIdSpin->value();
    int toId = m_toShapeIdSpin->value();
    
    if (fromId == toId) {
        QMessageBox::warning(this, "Ошибка", "Нельзя связать фигуру саму с собой");
        return;
    }
    
    auto fromShape = m_repository->findById(fromId);
    auto toShape = m_repository->findById(toId);
    
    if (!fromShape || !toShape) {
        QMessageBox::warning(this, "Ошибка", "Одна или обе фигуры не найдены");
        return;
    }
    
    if (m_repository->addConnection(fromId, toId)) {
        statusBar()->showMessage(
            QString("Связь создана: %1 -> %2").arg(fromId).arg(toId), 3000);
        m_shapeModel->refresh();
        updateGraphicsView();
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось создать связь");
    }
}

void MainWindow::onFilterChanged() {
    m_shapeModel->setTypeFilter(domain::ShapeType::Ellipse, 
                                m_filterEllipseCheck->isChecked());
    m_shapeModel->setTypeFilter(domain::ShapeType::Rectangle, 
                                m_filterRectangleCheck->isChecked());
    m_shapeModel->setTypeFilter(domain::ShapeType::Polygon, 
                                m_filterPolygonCheck->isChecked());
    
    // * обновляем видимость фигур на графическом представлении
    auto allShapes = m_repository->findAll();
    for (const auto& shapeData : allShapes) {
        bool shouldBeVisible = true;
        
        // * проверяем фильтры
        if (shapeData.type == domain::ShapeType::Ellipse && !m_filterEllipseCheck->isChecked()) {
            shouldBeVisible = false;
        } else if (shapeData.type == domain::ShapeType::Rectangle && !m_filterRectangleCheck->isChecked()) {
            shouldBeVisible = false;
        } else if (shapeData.type == domain::ShapeType::Polygon && !m_filterPolygonCheck->isChecked()) {
            shouldBeVisible = false;
        }
        
        m_graphicsView->updateShapeVisibility(shapeData.id, shouldBeVisible);
    }
}

void MainWindow::onRefreshClicked() {
    m_shapeModel->refresh();
    updateGraphicsView();
    statusBar()->showMessage("Данные обновлены", 2000);
}

void MainWindow::updateGraphicsView() {
    m_graphicsView->refresh();
}

} // namespace presentation