// MainWindow.cc
#include "MainWindow.h"
#include "ContactTableModel.h"
#include "ContactManager.h"
#include "Contact.h"

#include <QTableView>
#include <QPushButton>
#include <QLineEdit>
#include <QItemSelectionModel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDataWidgetMapper>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateEdit>

// конструктор
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();

    tableModel_ = new ContactTableModel(&contactManager_, this);
    
    proxyModel_ = new QSortFilterProxyModel(this);
    proxyModel_->setSourceModel(tableModel_);
    proxyModel_->setFilterCaseSensitivity(Qt::CaseInsensitive);

    tableView_->setModel(proxyModel_);
    tableView_->setSortingEnabled(true);
    
    connect(addButton_, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);
    connect(removeButton_, &QPushButton::clicked, this, &MainWindow::onRemoveButtonClicked);
    connect(editButton_, &QPushButton::clicked, this, &MainWindow::onEditButtonClicked);
    connect(cancelButton_, &QPushButton::clicked, this, &MainWindow::onCancelButtonClicked);
    connect(searchInput_, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    connect(tableView_->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onSelectionChanged);
    
    QPushButton* saveButton = new QPushButton("Сохранить файл");
    QPushButton* loadButton = new QPushButton("Загрузить файл");
    
    QHBoxLayout* fileButtonsLayout = new QHBoxLayout();
    fileButtonsLayout->addWidget(saveButton);
    fileButtonsLayout->addWidget(loadButton);

    mainLayout_->insertLayout(0, fileButtonsLayout);

    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoadButtonClicked);

    setupMapper();

    if (contactManager_.loadFromFile("contacts.json")) 
        tableModel_->resetTable();    
}

void MainWindow::setupMapper() {
    mapper_ = new QDataWidgetMapper(this);
    mapper_->setModel(proxyModel_);

    // запрещаем автосохранение
    mapper_->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    
    mapper_->addMapping(firstNameInput_, 0);
    mapper_->addMapping(lastNameInput_, 1);
    mapper_->addMapping(middleNameInput_, 2);
    mapper_->addMapping(addressInput_, 3);
    mapper_->addMapping(birthDateInput_, 4, "date");
    mapper_->addMapping(emailInput_, 5);
    mapper_->addMapping(phoneInput_, 6);
}