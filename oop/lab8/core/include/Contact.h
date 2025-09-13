// Contact.h
#pragma once 

#include <QString>
#include <QDate>
#include <QMap>

struct Contact {
    QString firstName_;
    QString lastName_;
    QString middleName_;
    QString adress_;
    QDate birthDate_;
    QString email_;
    QMap<QString, QString> phoneNumbers_;
};