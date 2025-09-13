// ContactManager.h
#pragma once 

#include <QList>
#include "Contact.h"

class ContactManager {
public:
    ContactManager() = default;
    ~ContactManager() = default;

    void addContact(const Contact& contact);
    void removeContact(int index);
    void updateContact(int index, const Contact& contact);

    const QList<Contact>& getContacts() const;
    QList<Contact>& getContactsMutable();

    bool loadFromFile(const QString& filePath);
    bool saveToFile(const QString& filePath) const;

    Contact* getContact(int index); 
    int contactCount() const;

private:
    QList<Contact> m_contacts;
};