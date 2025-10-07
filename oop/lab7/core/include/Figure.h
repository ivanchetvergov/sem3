// Figure.h
#pragma once

#include <QPointF>
#include <QPainter>
#include <QColor>

class Figure {
    Q_GADGET // Отсутствует поддержка сигналов и слотов
public:  
    virtual ~Figure() = default;

    virtual void draw(QPainter* painter) = 0;
    virtual bool contains(const QPointF& point) const = 0;

    virtual void move(const QPointF& delta){
        pos_ += delta;
    }

    QPointF pos_;      
    QColor color_;  
    bool isActive_ = false; 
};