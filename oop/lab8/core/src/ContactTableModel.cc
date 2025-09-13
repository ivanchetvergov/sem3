// ContactTableModel.cc 
#include "ContactTableModel.h"

ContactTableModel::ContactTableModel(ContactManager* contactManager, QObject* parent)
    : QAbstractTableModel(parent),
      m_contactManager(contactManager) {}

int ContactTableModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return m_contactManager->getContacts().size();
}

int ContactTableModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return 7;
}

QVariant ContactTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_contactManager->getContacts().size()) {
        return QVariant();
    }

    const QList<Contact>& contacts = m_contactManager->getContacts();
    const Contact& contact = contacts.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case 0: return contact.firstName_;
            case 1: return contact.lastName_;
            case 2: return contact.middleName_;
            case 3: return contact.adress_;
            case 4: return contact.birthDate_;
            case 5: return contact.email_;
            case 6: return contact.phoneNumbers_.value("Рабочий");
        }
    }
    return QVariant();
}

QVariant ContactTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
        return QVariant();
    }
    
    switch (section) {
        case 0: return "Имя";
        case 1: return "Фамилия";
        case 2: return "Отчество";
        case 3: return "Адрес";
        case 4: return "Дата рождения";
        case 5: return "Email";
        case 6: return "Телефон";
        default: return QVariant();
    }
}

bool ContactTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (index.isValid() && role == Qt::EditRole) {
        // Получаем ссылку на контакт, а не копию
        QList<Contact>& contacts = m_contactManager->getContactsMutable(); 
        Contact& contact = contacts[index.row()];
        
        switch (index.column()) {
            case 0: contact.firstName_ = value.toString(); break;
            case 1: contact.lastName_ = value.toString(); break;
            case 2: contact.middleName_ = value.toString(); break;
            case 3: contact.adress_ = value.toString(); break;
            case 4: contact.birthDate_ = value.toDate(); break;
            case 5: contact.email_ = value.toString(); break;
            case 6: contact.phoneNumbers_["Рабочий"] = value.toString(); break;
            default: return false;
        }

        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

Qt::ItemFlags ContactTableModel::flags(const QModelIndex& index) const {
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

void ContactTableModel::addContact(const Contact& contact) {
    int newRow = m_contactManager->getContacts().size();
    beginInsertRows(QModelIndex(), newRow, newRow);
    m_contactManager->addContact(contact);
    endInsertRows();
}

void ContactTableModel::removeContact(int index) {
    if (index >= 0 && index < m_contactManager->getContacts().size()) {
        beginRemoveRows(QModelIndex(), index, index);
        m_contactManager->removeContact(index);
        endRemoveRows();
    }
}

void ContactTableModel::updateContact(int index, const Contact& contact) {
    if (index >= 0 && index < m_contactManager->getContacts().size()) {
        m_contactManager->updateContact(index, contact);
        emit dataChanged(this->index(index, 0), this->index(index, columnCount() - 1));
    }
}

const Contact& ContactTableModel::getContact(int row) const {
    return *m_contactManager->getContact(row);
}

void ContactTableModel::resetTable() {
    beginResetModel();
    endResetModel();
}