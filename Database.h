#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <QSqlDatabase>
#include <QFile>
#include <QDate>
#include <QtWidgets/qmessagebox.h>
#include <vector>
#include <qstring.h>
#include "MainForm.h"

class Database
{
public:
    Database() = default;
    ~Database() = default;

    static const QString TABLE_ACCOUNTS;
    static const QString TABLE_CLIENTS;
    static const QString TABLE_ORDERS;
    static const QString TABLE_POSITIONS;
    static const QString TABLE_PRODUCT_TYPES;
    static const QString TABLE_STATUS;

    static QSqlDatabase getDatabase();
    static bool checkConnect();
    static bool initDatabase(const QString& DB_NAME);
    static int getLastId(const QString& table);
    static int getOrdersCount();
    static bool getMessage(const int accountId);
    static bool setMessage(const int accountId, bool message);
    static bool setOrderIsRead(const int orderId, bool isRead);
    static std::vector<Client> getClientsList();
    static bool addClient(const Client& newClient);
    static bool editClient(const Client& client);
    static std::vector<Account> getAccountList();
    static bool addAccount(const Account& newAccount);
    static std::vector<Position> getPositionList();
    static std::vector<ProductType> getProductTypeList();
    static std::vector<Status> getStatusList();
    static std::vector<Order> getOrdersList();
    static bool addOrder(const Order& newOrder);
    static bool editOrder(const Order& order);
    static bool editStatusInOrder(const int orderId, const int status);
    static bool removeObject(int id, const QString& table);
    static bool setActive(int id, const QString& table, bool active);
};

#endif  DATABASE_H