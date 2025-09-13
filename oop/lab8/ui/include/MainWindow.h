// MainWindow.h
#pragma once

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include "ContactManager.h"
#include "ContactTableModel.h"

class QPushButton;
class QLineEdit;
class QDateEdit;
class QComboBox;
class QVBoxLayout;
class QTableView;
class QDataWidgetMapper;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onEditButtonClicked();
    void onSearchTextChanged(const QString& text);
    void onSelectionChanged();
    void onSaveButtonClicked();
    void onLoadButtonClicked();
    void onCancelButtonClicked();

private:
    void setupUi();
    void setupMapper();
    void clearInputFields();
    Contact getCurrentContactFromForm() const;
    bool validateContact(const Contact& contact);

    ContactManager m_contactManager;
    ContactTableModel* m_tableModel; 
    QTableView* m_tableView;         

    QDataWidgetMapper* m_mapper;     

    QLineEdit* m_firstNameInput;
    QLineEdit* m_lastNameInput;
    QLineEdit* m_middleNameInput;
    QLineEdit* m_addressInput;
    QDateEdit* m_birthDateInput;
    QLineEdit* m_emailInput;
    QLineEdit* m_phoneInput;

    QPushButton* m_addButton;
    QPushButton* m_removeButton;
    QPushButton* m_editButton;
    QPushButton* m_cancelButton;

    bool m_isInEditMode = false;
    void setEditingMode(bool isInEditMode);
    
    // поиск и сортировка
    QLineEdit* m_searchInput;
    QComboBox* m_sortComboBox;

    QVBoxLayout* m_mainLayout;
    QSortFilterProxyModel* m_proxyModel;
};
