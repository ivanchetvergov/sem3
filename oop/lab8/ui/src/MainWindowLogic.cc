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

    if (isInEditMode_) {
        if (mapper_->submit()) {
            QMessageBox::information(this, "Успех", "Данные успешно отредактированы.");
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить изменения.");
            return; // Останавливаемся, если не удалось сохранить
        }
    } else {
        tableModel_->addContact(contact);
        QMessageBox::information(this, "Успех", "Контакт успешно добавлен.");
    }

    contactManager_.saveToFile("contacts.json");
    setEditingMode(false);
}

void MainWindow::onRemoveButtonClicked() {
    int currentRow = tableView_->currentIndex().row();
    if (currentRow >= 0) {
        tableModel_->removeContact(proxyModel_->mapToSource(tableView_->currentIndex()).row());
        contactManager_.saveToFile("contacts.json");
    } else {
        QMessageBox::information(this, "Удаление", "Пожалуйста, выберите запись для удаления.");
    }
}

void MainWindow::onEditButtonClicked() {
    int currentRow = tableView_->currentIndex().row();
    if (currentRow < 0) {
        QMessageBox::information(this, "Редактирование", "Пожалуйста, выберите запись для редактирования.");
        return;
    }

    setEditingMode(true);
    // Заполняем поля, используя маппер с корректным индексом из прокси-модели
    QModelIndex sourceIndex = proxyModel_->mapToSource(tableView_->currentIndex());
    mapper_->setCurrentIndex(sourceIndex.row());
    
    QMessageBox::information(this, "Редактирование", "Теперь вы можете редактировать данные.");
}

void MainWindow::onCancelButtonClicked() {
    mapper_->revert(); // Откатываем все изменения в полях, сделанные маппером
    setEditingMode(false);
}

void MainWindow::onSearchTextChanged(const QString& text) {
    proxyModel_->setFilterRegularExpression(text);
}

void MainWindow::onSelectionChanged() {
    bool rowIsSelected = (tableView_->currentIndex().row() >= 0);
    removeButton_->setEnabled(rowIsSelected);
    editButton_->setEnabled(rowIsSelected);
}

void MainWindow::clearInputFields() {
    firstNameInput_->clear();
    lastNameInput_->clear();
    middleNameInput_->clear();
    addressInput_->clear();
    birthDateInput_->setDate(QDate::currentDate());
    emailInput_->clear();
    phoneInput_->clear();
}

Contact MainWindow::getCurrentContactFromForm() const {
    Contact contact;
    contact.firstName_ = firstNameInput_->text().trimmed();
    contact.lastName_ = lastNameInput_->text().trimmed();
    contact.middleName_ = middleNameInput_->text().trimmed();
    contact.adress_ = addressInput_->text().trimmed();
    contact.birthDate_ = birthDateInput_->date();
    contact.email_ = emailInput_->text().trimmed();
    contact.phoneNumbers_["Рабочий"] = phoneInput_->text().trimmed();
    return contact;
}

void MainWindow::setEditingMode(bool isEditing) {
    isInEditMode_ = isEditing;
    if (isInEditMode_) {
        addButton_->setText("Сохранить");
        removeButton_->hide();
        editButton_->hide();
        cancelButton_->show();
        tableView_->setEnabled(false);
    } else {
        addButton_->setText("Добавить");
        removeButton_->show();
        editButton_->show();
        cancelButton_->hide();
        tableView_->setEnabled(true);
        clearInputFields();
        tableView_->clearSelection();
    }
}

void MainWindow::onSaveButtonClicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить файл", "", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        if (contactManager_.saveToFile(filePath)) {
            QMessageBox::information(this, "Сохранение", "Данные успешно сохранены.");
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл.");
        }
    }
}

void MainWindow::onLoadButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Загрузить файл", "", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        if (contactManager_.loadFromFile(filePath)) {
            tableModel_->resetTable();
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
