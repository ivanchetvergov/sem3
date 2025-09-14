// FiguresWidget.h
#pragma once

#include <QWidget>
#include <QVector>
#include <QMouseEvent>
#include <QPointF>

#include "Figure.h"

class FiguresWidget : public QWidget {
    Q_OBJECT

public:
    explicit FiguresWidget(QWidget* parent = nullptr);
    ~FiguresWidget() override;

    void addFigure(Figure* figure);
    void removeLastFigure();
    void clearFigures();

    void removeFigure();
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QVector<Figure*> figures_;
    Figure* activeFigure_ = nullptr;
    QPointF lastMousePos_;
};