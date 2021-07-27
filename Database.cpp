#include "Database.h"


const QString Database::TABLE_ACCOUNTS("accounts");
const QString Database::TABLE_CLIENTS("clients");
const QString Database::TABLE_ORDERS("orders");
const QString Database::TABLE_POSITIONS("positions");
const QString Database::TABLE_PRODUCT_TYPES("productTypes");
const QString Database::TABLE_STATUS("status");

QSqlDatabase Database::getDatabase()
{
    return QSqlDatabase::database(MainForm::_pathToDatabase);
}

bool Database::checkConnect()
{
    QSqlQuery query(getDatabase());
    query.exec("SELECT _id FROM " + TABLE_ORDERS);

    return query.exec();
}

bool Database::initDatabase(const QString& DB_NAME)
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DB_NAME);
    const bool ok = db.open();
    if (!ok)
    {
        QString error = db.lastError().text();
        QMessageBox::critical(0, "Ошибка", "Ошибка подключения к базе данных: " + error);
    }
    return ok;
}

int Database::getLastId(const QString& table)
{
    int lastId = -1;

    QSqlQuery query(getDatabase());
    query.exec("SELECT _id FROM " + table + " ORDER BY _id DESC LIMIT 1");

    if (query.first())
        lastId = query.value(0).toInt();

    return query.exec() ? lastId : -1;
}

int Database::getOrdersCount()
{
    int rowCount = -1;

    QSqlQuery query(getDatabase());
    query.exec("SELECT _id FROM " + TABLE_ORDERS);

    query.last();
    rowCount = query.at();
    if (rowCount > 0)
        ++rowCount;

    return query.exec() ? rowCount : -1;
}

bool Database::getMessage(const int accountId)
{
    QSqlQuery query(getDatabase());

    query.exec("SELECT message FROM " + TABLE_ACCOUNTS + " WHERE _id=" + QString::number(accountId));
    
    bool msg = false;

    while (query.next())
    {
        msg = query.value(0).toBool();
    }

    return msg;
}

bool Database::setMessage(const int accountId, bool message)
{
    QSqlQuery query(getDatabase());
    query.prepare("UPDATE " + TABLE_ACCOUNTS +
        " SET message = ? " +
        " WHERE _id=" + QString::number(accountId));
    query.addBindValue(message);

    return query.exec();
}

bool Database::setOrderIsRead(const int orderId, bool isRead)
{
    QSqlQuery query(getDatabase());
    query.prepare("UPDATE " + TABLE_ORDERS +
        " SET isRead = ? " +
        " WHERE _id=" + QString::number(orderId));
    query.addBindValue(isRead);

    return query.exec();
}

std::vector<Client> Database::getClientsList()
{
    int rowCount = 0;
    QSqlQuery query(getDatabase());

    query.exec("SELECT * FROM " + TABLE_CLIENTS);
    query.last();
    rowCount = query.at();
    std::vector<Client> clientsList;
    if (rowCount > 0)
        clientsList.reserve(++rowCount);

    query.first();
    query.previous();

    while (query.next())
    {
        Client client(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toBool());
        clientsList.push_back(std::move(client));
    }

    return clientsList;
}

bool Database::addClient(const Client& newClient)
{
    QSqlQuery query(getDatabase());
    query.prepare("INSERT INTO " + TABLE_CLIENTS + " (name, phone, mail, active)"
        "VALUES (:name, :phone, :mail, :active);");
    query.bindValue(":name", newClient.get_name());
    query.bindValue(":phone", newClient.get_phone());
    query.bindValue(":mail", newClient.get_mail());
    query.bindValue(":active", newClient.is_active());

    return query.exec();
}

bool Database::editClient(const Client& client)
{
    QSqlQuery query(getDatabase());
    query.prepare("UPDATE " + TABLE_CLIENTS +
        " SET name = ?, phone = ?, mail = ?, active = ? " +
        " WHERE _id=" + QString::number(client.get_id()));
    query.addBindValue(client.get_name());
    query.addBindValue(client.get_phone());
    query.addBindValue(client.get_mail());
    query.addBindValue(client.is_active());

    return query.exec();
}

std::vector<Account> Database::getAccountList()
{
    int rowCount = 0;
    QSqlQuery query(getDatabase());

    query.exec("SELECT * FROM " + TABLE_ACCOUNTS);
    query.last();
    rowCount = query.at();
    std::vector<Account> accountList;
    if (rowCount > 0)
        accountList.reserve(++rowCount);

    query.first();
    query.previous();
    
    while (query.next())
    {
        Account account;
        account.set_id(query.value(0).toInt());
        account.set_name(query.value(1).toString());
        account.set_surname(query.value(2).toString());
        account.set_position(query.value(3).toInt());
        account.set_login(query.value(4).toString());
        account.set_password(query.value(5).toString());
        account.set_active(query.value(6).toBool());

        accountList.push_back(std::move(account));
    }

    return accountList;
}

bool Database::addAccount(const Account& newAccount)
{
    QSqlQuery query(getDatabase());
    query.prepare("INSERT INTO " + TABLE_ACCOUNTS + " (name, surname, position, login, password, active)"
        "VALUES (:name, :surname, :position, :login, :password, :active);");
    query.bindValue(":name", newAccount.get_name());
    query.bindValue(":surname", newAccount.get_surname());
    query.bindValue(":position", newAccount.get_position());
    query.bindValue(":login", newAccount.get_login());
    query.bindValue(":password", newAccount.get_password());
    query.bindValue(":active", newAccount.is_active());

    return query.exec();
}

std::vector<Position> Database::getPositionList()
{
    int rowCount = 0;
    QSqlQuery query(getDatabase());

    query.exec("SELECT * FROM " + TABLE_POSITIONS);
    query.last();
    rowCount = query.at();
    std::vector<Position> positionList;
    if (rowCount > 0)
        positionList.reserve(++rowCount);

    query.first();
    query.previous();

    while (query.next())
    {
        Position position(
            query.value(0).toInt(),
            query.value(1).toString());
        positionList.push_back(std::move(position));
    }

    return positionList;
}

std::vector<ProductType> Database::getProductTypeList()
{
    int rowCount = 0;
    QSqlQuery query(getDatabase());

    query.exec("SELECT * FROM " + TABLE_PRODUCT_TYPES);
    query.last();
    rowCount = query.at();
    std::vector<ProductType> productTypeList;
    if (rowCount > 0)
        productTypeList.reserve(++rowCount);

    query.first();
    query.previous();

    while (query.next())
    {
        ProductType type(
            query.value(0).toInt(),
            query.value(1).toString());
        productTypeList.push_back(std::move(type));
    }

    return productTypeList;
}

std::vector<Status> Database::getStatusList()
{
    int rowCount = 0;
    QSqlQuery query(getDatabase());

    query.exec("SELECT * FROM " + TABLE_STATUS);
    query.last();
    rowCount = query.at();
    std::vector<Status> statusList;
    if (rowCount > 0)
        statusList.reserve(++rowCount);

    query.first();
    query.previous();

    while (query.next())
    {
        Status status;
        status.set_id(query.value(0).toInt());
        status.set_status(query.value(1).toString());
        statusList.push_back(std::move(status));
    }

    return statusList;
}

std::vector<Order> Database::getOrdersList()
{
    int rowCount = 0;
    QSqlQuery query(getDatabase());

    query.exec("SELECT * FROM " + TABLE_ORDERS + " ORDER BY date ASC");
    query.last();
    rowCount = query.at();
    std::vector<Order> orderList;
    if (rowCount > 0)
        orderList.reserve(++rowCount);

    query.first();
    query.previous();

    while (query.next())
    {
        Order order;
        order.set_id(query.value(0).toInt());
        order.set_date(query.value(1).toDate());
        order.set_client(query.value(2).toInt());
        order.set_productType(query.value(3).toInt());
        order.set_manager(query.value(4).toInt());
        order.set_designer(query.value(5).toInt());
        order.set_productionTime(query.value(6).toDate());
        order.set_status(query.value(7).toInt());
        order.set_mockUpTask(query.value(8).toString());
        order.set_productionTask(query.value(9).toString());
        order.set_isRead(query.value(10).toBool());

        orderList.push_back(std::move(order));
    }

    return orderList;
}

bool Database::addOrder(const Order& newOrder)
{
    QSqlQuery query(getDatabase());
    query.prepare("INSERT INTO " + TABLE_ORDERS +
        " (date, client, productType, manager, designer, productionTime, status, mockUpTask, productionTask)"
        " VALUES (:date, :client, :productType, :manager, :designer, :productionTime, :status, :mockUpTask, :productionTask);");
    query.bindValue(":date", newOrder.get_date());
    query.bindValue(":client", newOrder.get_client());
    query.bindValue(":productType", newOrder.get_productType());
    query.bindValue(":manager", newOrder.get_manager());
    query.bindValue(":designer", newOrder.get_designer());
    query.bindValue(":productionTime", newOrder.get_productionTime());
    query.bindValue(":status", newOrder.get_status());
    query.bindValue(":mockUpTask", newOrder.get_mockUpTask());
    query.bindValue(":productionTask", newOrder.get_productionTask());

    return query.exec();
}

bool Database::editOrder(const Order& order)
{
    QSqlQuery query(getDatabase());
    query.prepare("UPDATE " + TABLE_ORDERS +
        " SET date=?, client=?, productType=?, manager=?, designer=?, productionTime=?, status=?, mockUpTask=?, productionTask=?"
        " WHERE _id=" + QString::number(order.get_id()));
    query.addBindValue(order.get_date());
    query.addBindValue(order.get_client());
    query.addBindValue(order.get_productType());
    query.addBindValue(order.get_manager());
    query.addBindValue(order.get_designer());
    query.addBindValue(order.get_productionTime());
    query.addBindValue(order.get_status());
    query.addBindValue(order.get_mockUpTask());
    query.addBindValue(order.get_productionTask());

    return query.exec();
}

bool Database::editStatusInOrder(const int orderId, const int status)
{
    QSqlQuery query(getDatabase());
    query.prepare("UPDATE " + TABLE_ORDERS +
        " SET status=?"
        " WHERE _id=" + QString::number(orderId));
    query.addBindValue(status);

    return query.exec();
}

bool Database::removeObject(int id, const QString& table)
{
    QSqlQuery query(getDatabase());
    return query.exec("DELETE FROM " + table + " WHERE _id=" + QString::number(id));        
}

bool Database::setActive(int id, const QString& table, bool active)
{
    QSqlQuery query(getDatabase());
    query.prepare("UPDATE " + table +
        " SET active=?"
        " WHERE _id=" + QString::number(id));
    query.addBindValue(active);
    return query.exec();
}
