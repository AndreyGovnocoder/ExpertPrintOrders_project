#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainForm.h"
#include <QTimer>
#include <QCloseEvent>
#include "Account.h"
#include "Client.h"
#include "Order.h"
#include "Position.h"
#include "ProductType.h"
#include "Status.h"
#include "Database.h"
#include "AuthorizationForm.h"
#include "OrderDialog.h"
#include "ClientDialog.h"
#include "ClientsForm.h"
#include "RegistrationForm.h"
#include "AboutForm.h"

class MainForm : public QMainWindow
{
    Q_OBJECT

public:
    MainForm(QWidget *parent = Q_NULLPTR);
    ~MainForm() = default;

    static QString _pathToDatabase;
    static std::vector<Account> _accountList;
    static std::vector<Client> _clientList;
    static std::vector<Order> _orderList;
    static std::vector<Position> _positionList;
    static std::vector<ProductType> _productTypeList;
    static std::vector<Status> _statusList;    

    void initDataFromDB();  
    void setManagerData();
    void setDesignerData();
    void setWorkshopData();
    void startTimer() { _timer->start(); };
    void set_currentAccount(int accountId) { _currentAccount = accountId; }
    int get_currentAccount() const { return _currentAccount; }
    static bool askForClose(const QString& titleText, const QString& askText);
    static Order* findOrder(const int orderId);
    static Account* findAccount(const QString& login);
    static Account* findAccount(const int accountId);
    static Client* findClient(const int clientId);
    static ProductType* findProductType(const int typeId);
    static Status* findStatus(const int statusId);
    static bool checkClientInOrders(const int clientId);
    QAction* get_showClientsAction() { return _showClientsAction; }
    QMenu* get_ordersTableOptionMenu() { return _ordersTableOptionMenu; }
    QAction* get_onlyMyOrdersAction() { return _onlyMyOrdersAction; }
    QAction* get_hideCompletedOrdersAction() { return _hideCompletedOrdersAction; }
    QMenu* get_ordersFilterMenu() { return _ordersFilterMenu; }
    QAction* get_mockUpFilterAction() { return _mockUpFilterAction; }
    QAction* get_approvalFilterAction() { return _approvalFilterAction; }
    QAction* get_productionFilterAction() { return _productionFilterAction; }
    QAction* get_waitingFilterAction() { return _waitingFilterAction; }

private:
    Ui::MainFormClass ui;

    int _currentAccount;
    std::vector<Order> _foundOrderList;
    QTimer* _timer;
    QShortcut* _keyF9;
    QShortcut* _keyF5;
    QShortcut* _keyEsc;
    QLabel* _isMessageIndicator;
    QLabel* _newTaskMessage;
    QAction* _showClientsAction = new QAction("Клиенты", this);
    QMenu* _ordersTableOptionMenu = new QMenu("Таблица заказов", this);
    QAction* _onlyMyOrdersAction = new QAction("Только мои заказы", this);
    QAction* _hideCompletedOrdersAction = new QAction("Скрыть выполненые заказы", this);
    QMenu* _ordersFilterMenu = new QMenu("Фильтр по статусу", this);
    QAction* _mockUpFilterAction = new QAction("Макетирование", this);
    QAction* _approvalFilterAction = new QAction("Согласование макета", this);
    QAction* _productionFilterAction = new QAction("Производство", this);
    QAction* _waitingFilterAction = new QAction("Ожидание", this);
    QAction* _aboutAction = new QAction("О программе", this);

    void set_accountList() { _accountList = Database::getAccountList(); };
    void set_clientList() { _clientList = Database::getClientsList(); };
    void set_orderList() { _orderList = Database::getOrdersList(); };
    void set_positionList() { _positionList = Database::getPositionList(); };
    void set_statusList() { _productTypeList = Database::getProductTypeList(); };
    void set_productTypeList();
    void setMenuBar();
    void setOrdersTable();
    void setOrdersTable(const std::vector<Order>& orderList);
    void setOrdersTableByFilter(const std::vector<Order>& orderList);
    void addItemToOrdersTable(const Order& order, const int row);
    bool setOrdersStatus(const int orderId, const int status);
    void clearOrderDetails();
    void setWindowSize();
    static int randomBetween(int low, int high);
    void closeEvent(QCloseEvent* event);

private slots:
    void managerMenuRequestedSlot(QPoint pos);
    void designerMenuRequestedSlot(QPoint pos);
    void workshopMenuRequestedSlot(QPoint pos);
    void showClientsFormSlot();
    void addOrderSlot();
    void editOrderSlot();
    void removeOrderSlot();
    void sendToMockUpSlot();
    void sendToProductionSlot();
    void setStatusCompletedSlot();
    void sendToApprovalSlot();
    void notApprovalSlot();
    void selectOrderSlot();
    void setOrdersTableByFilterSlot();
    void searchOrdersByClientSlot();
    void keyEscSlot() { close(); };
    void fillDBSlot();
    void checkMessageSlot();
    void updateData();
    void showAboutSlot();
    void newTasksSlot();
};
