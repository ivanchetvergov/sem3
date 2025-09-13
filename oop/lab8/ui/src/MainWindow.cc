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

// Конструктор
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();

    m_tableModel = new ContactTableModel(&m_contactManager, this);
    
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_tableModel);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_tableView->setModel(m_proxyModel);
    m_tableView->setSortingEnabled(true);
    
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::onRemoveButtonClicked);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::onEditButtonClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &MainWindow::onCancelButtonClicked);
    connect(m_searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onSelectionChanged);
    
    QPushButton* saveButton = new QPushButton("Сохранить файл");
    QPushButton* loadButton = new QPushButton("Загрузить файл");
    
    QHBoxLayout* fileButtonsLayout = new QHBoxLayout();
    fileButtonsLayout->addWidget(saveButton);
    fileButtonsLayout->addWidget(loadButton);

    m_mainLayout->insertLayout(0, fileButtonsLayout);

    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoadButtonClicked);

    setupMapper();

    if (m_contactManager.loadFromFile("contacts.json")) 
        m_tableModel->resetTable();    
}

void MainWindow::setupMapper() {
    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(m_proxyModel);

    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    
    m_mapper->addMapping(m_firstNameInput, 0);
    m_mapper->addMapping(m_lastNameInput, 1);
    m_mapper->addMapping(m_middleNameInput, 2);
    m_mapper->addMapping(m_addressInput, 3);
    m_mapper->addMapping(m_birthDateInput, 4, "date");
    m_mapper->addMapping(m_emailInput, 5);
    m_mapper->addMapping(m_phoneInput, 6);
}