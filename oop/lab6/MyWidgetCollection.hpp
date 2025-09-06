// MyWidgetCollection.hpp
#pragma once 

#include <QWidget>
#include <QVector>

// declarations to avoid including full header files
class QLabel;
class QSlider;
class QScrollBar;
class QSpinBox;

class MyWidgetCollection : public QWidget {
    Q_OBJECT // MOC (Meta Obj Comp)

public:
    MyWidgetCollection(QWidget* parent = nullptr);
    void addWidget(const QString& type);

public slots: // a special access specifier for slots
    void connectAllWidgets();

private:
    void setupUi();
    void connectWidgets(QWidget* widget1, QWidget* widget2);
    void debugConnectionInfo();

    QVector<QWidget*> widgets_;
};