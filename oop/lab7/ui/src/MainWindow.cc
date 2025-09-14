// MainWindow.cc
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <ctime>

#include "Rect.h"
#include "Triag.h"
#include "Ellipse.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    std::srand(std::time(nullptr));
    setupUi();
}

void MainWindow::setupUi() {
    setWindowTitle("Qt Figures");
    setMinimumSize(800, 600);

    figuresWidget_ = new FiguresWidget(this);

    figureTypeComboBox_ = new QComboBox(this);
    figureTypeComboBox_->addItem("Rectangle");
    figureTypeComboBox_->addItem("Triangle");
    figureTypeComboBox_->addItem("Ellipse");

    addButton_ = new QPushButton("Add Figure", this);
    removeButton_ = new QPushButton("Remove Figure", this);

    QHBoxLayout* controlLayout = new QHBoxLayout;
    controlLayout->addWidget(figureTypeComboBox_);
    controlLayout->addWidget(addButton_);
    controlLayout->addWidget(removeButton_);
    controlLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(figuresWidget_);

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(addButton_, &QPushButton::clicked, this, &MainWindow::on_add_button_clicked);
    connect(removeButton_, &QPushButton::clicked, this, &MainWindow::on_remove_button_clicked);
}

void MainWindow::on_add_button_clicked() {
    QString selectedType = figureTypeComboBox_->currentText();
    Figure* newFigure = nullptr;
    
    QPointF pos(std::rand() % (figuresWidget_->width() - 50), std::rand() % (figuresWidget_->height() - 50));
    QColor color(std::rand() % 256, std::rand() % 256, std::rand() % 256);

    if (selectedType == "Rectangle") {
        newFigure = new Rect(pos, color, 100, 70);
    } else if (selectedType == "Triangle") {
        QPointF p1 = pos;
        QPointF p2(pos.x() + 100, pos.y());
        QPointF p3(pos.x() + 50, pos.y() - 100);
        newFigure = new Triag(color, p1, p2, p3);
    } else if (selectedType == "Ellipse") {
        newFigure = new Ellipse(pos, color, 100, 70);
    }

    if (newFigure) {
        figuresWidget_->addFigure(newFigure);
    }
}

void MainWindow::on_remove_button_clicked() {
    figuresWidget_->removeFigure();
}