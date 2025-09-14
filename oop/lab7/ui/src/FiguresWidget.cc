// FigureWidget.cc
#include "FiguresWidget.h"
#include <QPainter>

FiguresWidget::FiguresWidget(QWidget* parent) : QWidget(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

FiguresWidget::~FiguresWidget() {
    clearFigures();
}

//--- Управление коллекцией ---

void FiguresWidget::addFigure(Figure* figure) {
    figures_.append(figure);
    update();
}

// Новый метод для удаления последнего элемента
void FiguresWidget::removeFigure() {
    if (!figures_.isEmpty()) {
        Figure* lastFigure = figures_.last();
        // Если последний элемент активен, сбрасываем activeFigure_
        if (lastFigure == activeFigure_) {
            activeFigure_ = nullptr;
        }
        figures_.removeLast(); // Удаляем последний элемент из вектора
        delete lastFigure; // Освобождаем память
        update();
    }
}

void FiguresWidget::clearFigures() {
    qDeleteAll(figures_);
    figures_.clear();
    activeFigure_ = nullptr;
    update();
}

//--- Обработка событий ---

void FiguresWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // рисуем все фигуры в порядке их добавления
    for (Figure* figure : figures_) {
        figure->draw(&painter);
    }
}

void FiguresWidget::mousePressEvent(QMouseEvent* event) {
    // Деактивируем предыдущую фигуру, если она есть
    if (activeFigure_) {
        activeFigure_->isActive_ = false;
        activeFigure_ = nullptr;
    }

    // Ищем фигуру в обратном порядке для выбора верхнего элемента
    for (int i = figures_.size() - 1; i >= 0; --i) {
        if (figures_[i]->contains(event->pos())) {
            activeFigure_ = figures_[i];
            activeFigure_->isActive_ = true;
            
            // Удаляем фигуру из текущей позиции
            figures_.removeAt(i);
            
            // Добавляем её в конец вектора, чтобы она рисовалась поверх остальных
            figures_.append(activeFigure_);
            
            break; // Останавливаем поиск
        }
    }
    
    lastMousePos_ = event->pos();
    update();
}

void FiguresWidget::mouseMoveEvent(QMouseEvent* event) {
    if (activeFigure_) {
        QPointF delta = event->pos() - lastMousePos_;
        activeFigure_->move(delta);
        lastMousePos_ = event->pos();
        update();
    }
}

void FiguresWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (activeFigure_) {
        activeFigure_->isActive_ = false;
        activeFigure_ = nullptr;
        update();
    }
}