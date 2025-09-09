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

//--- методы управления коллекцией ---

void FiguresWidget::addFigure(Figure* figure) {
    figures_.append(figure);
    update(); 
}

void FiguresWidget::removeActiveFigure() {
    if (activeFigure_) {
        figures_.removeOne(activeFigure_); 
        delete activeFigure_;           
        activeFigure_ = nullptr;         
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
    // рисуем все фигуры кроме активной.
    // проходимся в обратном порядке
    for (int i = figures_.size() - 1; i >= 0; --i) {
        if (figures_[i] != activeFigure_) {
            figures_[i]->draw(&painter);
        }
    }

    // рисуем активную фигуру последней
    if (activeFigure_) {
        activeFigure_->draw(&painter);
    }
}

void FiguresWidget::mousePressEvent(QMouseEvent* event) {
    // сбрасываем предыдущую активную фигуру
    if (activeFigure_) {
        activeFigure_->isActive_ = false;
        activeFigure_ = nullptr;
    }
    
    // проходим по фигурам в обратном порядке
    for (int i = figures_.size() - 1; i >= 0; --i) {
        if (figures_[i]->contains(event->pos())) {
            activeFigure_ = figures_[i];
            activeFigure_->isActive_ = true;
            // перемещаем активную фигуру в конец вектора
            figures_.append(activeFigure_);
            figures_.removeOne(activeFigure_); 
            break; 
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