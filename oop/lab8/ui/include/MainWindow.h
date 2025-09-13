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

    ContactManager contactManager_;
    ContactTableModel* tableModel_; 
    QTableView* tableView_;         

    QDataWidgetMapper* mapper_;     

    QLineEdit* firstNameInput_;
    QLineEdit* lastNameInput_;
    QLineEdit* middleNameInput_;
    QLineEdit* addressInput_;
    QDateEdit* birthDateInput_;
    QLineEdit* emailInput_;
    QLineEdit* phoneInput_;

    QPushButton* addButton_;
    QPushButton* removeButton_;
    QPushButton* editButton_;
    QPushButton* cancelButton_;

    bool isInEditMode_ = false;
    void setEditingMode(bool isInEditMode);
    
    // поиск и сортировка
    QLineEdit* searchInput_;
    QComboBox* sortComboBox_;

    QVBoxLayout* mainLayout_;
    QSortFilterProxyModel* proxyModel_;
};
