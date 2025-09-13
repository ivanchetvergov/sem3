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

    m_mainLayout = new QVBoxLayout(centralWidget);
    
    QGridLayout* inputLayout = new QGridLayout();
    
    m_lastNameInput = new QLineEdit();
    m_firstNameInput = new QLineEdit();
    m_middleNameInput = new QLineEdit();
    m_addressInput = new QLineEdit();
    m_birthDateInput = new QDateEdit(QDate::currentDate());
    m_birthDateInput->setDisplayFormat("dd.MM.yyyy");
    m_emailInput = new QLineEdit();
    m_phoneInput = new QLineEdit();
    
    inputLayout->addWidget(new QLabel("Фамилия:"), 0, 0);
    inputLayout->addWidget(m_lastNameInput, 0, 1);
    inputLayout->addWidget(new QLabel("Имя:"), 1, 0);
    inputLayout->addWidget(m_firstNameInput, 1, 1);
    inputLayout->addWidget(new QLabel("Отчество:"), 2, 0);
    inputLayout->addWidget(m_middleNameInput, 2, 1);
    inputLayout->addWidget(new QLabel("Адрес:"), 3, 0);
    inputLayout->addWidget(m_addressInput, 3, 1);
    inputLayout->addWidget(new QLabel("Дата рождения:"), 4, 0);
    inputLayout->addWidget(m_birthDateInput, 4, 1);
    inputLayout->addWidget(new QLabel("Email:"), 5, 0);
    inputLayout->addWidget(m_emailInput, 5, 1);
    inputLayout->addWidget(new QLabel("Телефон:"), 6, 0);
    inputLayout->addWidget(m_phoneInput, 6, 1);
    
    m_mainLayout->addLayout(inputLayout);
    
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("Добавить");
    m_removeButton = new QPushButton("Удалить");
    m_editButton = new QPushButton("Редактировать");
    m_cancelButton = new QPushButton("Отмена");

    m_removeButton->setEnabled(false);
    m_editButton->setEnabled(false);
    m_cancelButton->hide();
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_cancelButton);
    m_mainLayout->addLayout(buttonLayout);

    QHBoxLayout* searchLayout = new QHBoxLayout();
    m_searchInput = new QLineEdit();
    m_searchInput->setPlaceholderText("Поиск по имени, фамилии или email...");
    searchLayout->addWidget(new QLabel("Поиск:"));
    searchLayout->addWidget(m_searchInput);
    m_mainLayout->addLayout(searchLayout);

    m_tableView = new QTableView();
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); 
    m_mainLayout->addWidget(m_tableView);
}