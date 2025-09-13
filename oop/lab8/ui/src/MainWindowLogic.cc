// MainWindowLogic.cc
#include "MainWindow.h"

#include "Contact.h"
#include "Validator.h"
#include "ContactTableModel.h" 

#include <QMessageBox>
#include <QFileDialog>
#include <QDateEdit>
#include <QLineEdit>
#include <QTableView>
#include <QDataWidgetMapper>
#include <QPushButton> 

void MainWindow::onAddButtonClicked() {
    Contact contact = getCurrentContactFromForm();
    if (!validateContact(contact)) {
        return;
    }

    if (m_isInEditMode) {
        if (m_mapper->submit()) {
            QMessageBox::information(this, "Успех", "Данные успешно отредактированы.");
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изменения.");
            return; // Останавливаемся, если не удалось сохранить
        }
    } else {
        m_tableModel->addContact(contact);
        QMessageBox::information(this, "Успех", "Контакт успешно добавлен.");
    }

    m_contactManager.saveToFile("contacts.json");
    setEditingMode(false);
}

void MainWindow::onRemoveButtonClicked() {
    int currentRow = m_tableView->currentIndex().row();
    if (currentRow >= 0) {
        m_tableModel->removeContact(m_proxyModel->mapToSource(m_tableView->currentIndex()).row());
        m_contactManager.saveToFile("contacts.json");
    } else {
        QMessageBox::information(this, "Удаление", "Пожалуйста, выберите запись для удаления.");
    }
}

void MainWindow::onEditButtonClicked() {
    int currentRow = m_tableView->currentIndex().row();
    if (currentRow < 0) {
        QMessageBox::information(this, "Редактирование", "Пожалуйста, выберите запись для редактирования.");
        return;
    }

    setEditingMode(true);
    // Заполняем поля, используя маппер с корректным индексом из прокси-модели
    QModelIndex sourceIndex = m_proxyModel->mapToSource(m_tableView->currentIndex());
    m_mapper->setCurrentIndex(sourceIndex.row());
    
    QMessageBox::information(this, "Редактирование", "Теперь вы можете редактировать данные.");
}

void MainWindow::onCancelButtonClicked() {
    m_mapper->revert(); // Откатываем все изменения в полях, сделанные маппером
    setEditingMode(false);
}

void MainWindow::onSearchTextChanged(const QString& text) {
    m_proxyModel->setFilterRegularExpression(text);
}

void MainWindow::onSelectionChanged() {
    bool rowIsSelected = (m_tableView->currentIndex().row() >= 0);
    m_removeButton->setEnabled(rowIsSelected);
    m_editButton->setEnabled(rowIsSelected);
}

void MainWindow::clearInputFields() {
    m_firstNameInput->clear();
    m_lastNameInput->clear();
    m_middleNameInput->clear();
    m_addressInput->clear();
    m_birthDateInput->setDate(QDate::currentDate());
    m_emailInput->clear();
    m_phoneInput->clear();
}

Contact MainWindow::getCurrentContactFromForm() const {
    Contact contact;
    contact.firstName_ = m_firstNameInput->text().trimmed();
    contact.lastName_ = m_lastNameInput->text().trimmed();
    contact.middleName_ = m_middleNameInput->text().trimmed();
    contact.adress_ = m_addressInput->text().trimmed();
    contact.birthDate_ = m_birthDateInput->date();
    contact.email_ = m_emailInput->text().trimmed();
    contact.phoneNumbers_["Рабочий"] = m_phoneInput->text().trimmed();
    return contact;
}

void MainWindow::setEditingMode(bool isEditing) {
    m_isInEditMode = isEditing;
    if (m_isInEditMode) {
        m_addButton->setText("Сохранить");
        m_removeButton->hide();
        m_editButton->hide();
        m_cancelButton->show();
        m_tableView->setEnabled(false);
    } else {
        m_addButton->setText("Добавить");
        m_removeButton->show();
        m_editButton->show();
        m_cancelButton->hide();
        m_tableView->setEnabled(true);
        clearInputFields();
        m_tableView->clearSelection();
    }
}

void MainWindow::onSaveButtonClicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл", "", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        if (m_contactManager.saveToFile(filePath)) {
            QMessageBox::information(this, "Сохранение", "Данные успешно сохранены.");
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл.");
        }
    }
}

void MainWindow::onLoadButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Загрузить файл", "", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        if (m_contactManager.loadFromFile(filePath)) {
            m_tableModel->resetTable();
            QMessageBox::information(this, "Загрузка", "Данные успешно загружены.");
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить файл.");
        }
    }
}

bool MainWindow::validateContact(const Contact& contact) {
    if (!Validator::isValidName(contact.firstName_) ||
        !Validator::isValidName(contact.lastName_) ||
        (!contact.middleName_.isEmpty() && !Validator::isValidName(contact.middleName_)) ||
        !Validator::isValidBirthDate(contact.birthDate_) ||
        !Validator::isValidEmail(contact.email_) ||
        !Validator::isValidPhoneNumber(contact.phoneNumbers_.value("Рабочий"))) {
        QMessageBox::warning(this, "Ошибка валидации", "Проверьте введенные данные.");
        return false;
    }
    return true;
}
