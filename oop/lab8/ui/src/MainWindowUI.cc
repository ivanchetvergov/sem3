// MainWindowUI.cc
#include "MainWindow.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QTableView>
#include <QHeaderView>
#include <QDataWidgetMapper>

void MainWindow::setupUi()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout_ = new QVBoxLayout(centralWidget);
    
    QGridLayout* inputLayout = new QGridLayout();
    
    lastNameInput_ = new QLineEdit();
    firstNameInput_ = new QLineEdit();
    middleNameInput_ = new QLineEdit();
    addressInput_ = new QLineEdit();
    birthDateInput_ = new QDateEdit(QDate::currentDate());
    birthDateInput_->setDisplayFormat("dd.MM.yyyy");
    emailInput_ = new QLineEdit();
    phoneInput_ = new QLineEdit();
    
    inputLayout->addWidget(new QLabel("Фамилия:"), 0, 0);
    inputLayout->addWidget(lastNameInput_, 0, 1);
    inputLayout->addWidget(new QLabel("Имя:"), 1, 0);
    inputLayout->addWidget(firstNameInput_, 1, 1);
    inputLayout->addWidget(new QLabel("Отчество:"), 2, 0);
    inputLayout->addWidget(middleNameInput_, 2, 1);
    inputLayout->addWidget(new QLabel("Адрес:"), 3, 0);
    inputLayout->addWidget(addressInput_, 3, 1);
    inputLayout->addWidget(new QLabel("Дата рождения:"), 4, 0);
    inputLayout->addWidget(birthDateInput_, 4, 1);
    inputLayout->addWidget(new QLabel("Email:"), 5, 0);
    inputLayout->addWidget(emailInput_, 5, 1);
    inputLayout->addWidget(new QLabel("Телефон:"), 6, 0);
    inputLayout->addWidget(phoneInput_, 6, 1);
    
    mainLayout_->addLayout(inputLayout);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    addButton_ = new QPushButton("Добавить");
    removeButton_ = new QPushButton("Удалить");
    editButton_ = new QPushButton("Редактировать");
    cancelButton_ = new QPushButton("Отмена");

    removeButton_->setEnabled(false);
    editButton_->setEnabled(false);
    cancelButton_->hide();
    
    buttonLayout->addWidget(addButton_);
    buttonLayout->addWidget(removeButton_);
    buttonLayout->addWidget(editButton_);
    buttonLayout->addWidget(cancelButton_);
    mainLayout_->addLayout(buttonLayout);

    QHBoxLayout* searchLayout = new QHBoxLayout();
    searchInput_ = new QLineEdit();
    searchInput_->setPlaceholderText("Поиск по имени, фамилии или email...");
    searchLayout->addWidget(new QLabel("Поиск:"));
    searchLayout->addWidget(searchInput_);
    mainLayout_->addLayout(searchLayout);

    tableView_ = new QTableView();
    tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers); 
    mainLayout_->addWidget(tableView_);
}