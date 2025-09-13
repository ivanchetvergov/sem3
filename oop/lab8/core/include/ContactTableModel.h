// ContactTableModel.h
#pragma once

#include "Contact.h"
#include "ContactManager.h"

#include <QAbstractTableModel>
#include <QList>
#include <QVariant>

class ContactTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit ContactTableModel(ContactManager* contactManager, QObject* parent = nullptr);

    // обязательные методы для QAbstractTableModel
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // вспомогательные методы
    const Contact& getContact(int row) const;
    void addContact(const Contact& contact);
    void removeContact(int index);
    void updateContact(int index, const Contact& contact);

public slots:
    void resetTable();

private:
    ContactManager* m_contactManager;
};