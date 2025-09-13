// main.cc
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Телефонный справочник");
    window.resize(1000, 600);
    window.show();

    return app.exec();
}