#include "MainForm.h"

QString MainForm::_pathToDatabase;
std::vector<Account> MainForm::_accountList;
std::vector<Client> MainForm::_clientList;
std::vector<Order> MainForm::_orderList;
std::vector<Position> MainForm::_positionList;
std::vector<ProductType> MainForm::_productTypeList;
std::vector<Status> MainForm::_statusList;

MainForm::MainForm(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.pushButton->setVisible(false);
    ui.ordersTable->setContextMenuPolicy(Qt::CustomContextMenu); 
    setWindowSize();
    QSize sizeLabel;
    sizeLabel.setHeight(15);
    sizeLabel.setWidth(15);
    _timer = new QTimer();
    _timer->setInterval(2000);
    _keyF9 = new QShortcut(this);
    _keyF9->setKey(Qt::Key_F9);
    _keyF5 = new QShortcut(this);
    _keyF5->setKey(Qt::Key_F5);
    _keyEsc = new QShortcut(this);
    _keyEsc->setKey(Qt::Key_Escape);

    _isMessageIndicator = new QLabel();
    _isMessageIndicator->setFixedSize(sizeLabel);
    _isMessageIndicator->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::green);
    _isMessageIndicator->setPalette(palette);
    _isMessageIndicator->setVisible(false);
    _newTaskMessage = new QLabel("Есть новые задачи");
    _newTaskMessage->setVisible(false);

    ui.statusBar->insertWidget(0, _isMessageIndicator);
    ui.statusBar->insertWidget(1, _newTaskMessage);
    ui.statusBar->setStyleSheet("QStatusBar::item {border: None;}");

    connect(_timer, SIGNAL(timeout()), this, SLOT(checkMessageSlot()));  
    connect(_keyF9, SIGNAL(activated()), this, SLOT(fillDBSlot()));
    connect(_keyF5, SIGNAL(activated()), this, SLOT(updateData()));
    connect(_keyEsc, SIGNAL(activated()), this, SLOT(keyEscSlot()));
}

void MainForm::initDataFromDB()
{
    set_accountList();
    set_clientList();
    set_orderList();
    set_positionList();
    set_productTypeList(); 
    set_statusList();    
}

void MainForm::setMenuBar()
{
    _onlyMyOrdersAction->setCheckable(true);
    _hideCompletedOrdersAction->setCheckable(true);
    _mockUpFilterAction->setCheckable(true);
    _approvalFilterAction->setCheckable(true);
    _productionFilterAction->setCheckable(true);
    _waitingFilterAction->setCheckable(true);

    QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
    settings.beginGroup("menuBar");
    _onlyMyOrdersAction->setChecked(settings.value("onlyMyOrdersAction", true).toBool());
    _hideCompletedOrdersAction->setChecked(settings.value("hideCompletedOrdersAction", false).toBool());
    _mockUpFilterAction->setChecked(settings.value("mockUpFilterAction", false).toBool());
    _approvalFilterAction->setChecked(settings.value("approvalFilterAction", true).toBool());
    _productionFilterAction->setChecked(settings.value("productionFilterAction", false).toBool());
    _waitingFilterAction->setChecked(settings.value("waitingFilterAction", true).toBool());
    settings.endGroup();

    connect(_onlyMyOrdersAction, &QAction::triggered, this, &MainForm::setOrdersTableByFilterSlot);
    connect(_hideCompletedOrdersAction, &QAction::triggered, this, &MainForm::setOrdersTableByFilterSlot);
    connect(_mockUpFilterAction, &QAction::triggered, this, &MainForm::setOrdersTableByFilterSlot);
    connect(_approvalFilterAction, &QAction::triggered, this, &MainForm::setOrdersTableByFilterSlot);
    connect(_productionFilterAction, &QAction::triggered, this, &MainForm::setOrdersTableByFilterSlot);
    connect(_waitingFilterAction, &QAction::triggered, this, &MainForm::setOrdersTableByFilterSlot);
    connect(_aboutAction, &QAction::triggered, this, &MainForm::showAboutSlot);

    if (findAccount(_currentAccount)->get_position() == 1)
    {
        _ordersTableOptionMenu->addAction(_onlyMyOrdersAction);
        _ordersTableOptionMenu->addAction(_hideCompletedOrdersAction);
        _ordersTableOptionMenu->addMenu(_ordersFilterMenu);
        _ordersFilterMenu->addAction(_mockUpFilterAction);
        _ordersFilterMenu->addAction(_approvalFilterAction);
        _ordersFilterMenu->addAction(_productionFilterAction);
        _ordersFilterMenu->addAction(_waitingFilterAction);
        ui.menuBar->addMenu(_ordersTableOptionMenu);
    }

    ui.menuBar->addAction(_aboutAction);
}

void MainForm::setOrdersTable()
{
    ui.ordersTable->clearContents();
    ui.ordersTable->setRowCount(0);

    switch (findAccount(_currentAccount)->get_position())
    {
        case 1: //менеджер
            setOrdersTableByFilterSlot();
            break;

        case 2://дизайнер
            for (const auto& order : _orderList)
            {
                if (order.get_status() == 1 && order.get_designer() == _currentAccount)
                    addItemToOrdersTable(order, ui.ordersTable->rowCount());
            }
            break;

        case 3://цех
            for (const auto& order : _orderList)
            {
                if (order.get_status() == 3)
                    addItemToOrdersTable(order, ui.ordersTable->rowCount());
            }
            break;

        default:
            break;
    }

    ui.ordersTable->scrollToBottom();
    ui.ordersTable->hideColumn(0);
    ui.ordersTable->resizeColumnsToContents();
    ui.ordersTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui.ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.ordersTable->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainForm::setOrdersTable(const std::vector<Order>& orderList)
{
    ui.ordersTable->clearContents();
    ui.ordersTable->setRowCount(0);

    switch (findAccount(_currentAccount)->get_position())
    {
    case 1: //менеджер
        setOrdersTableByFilter(orderList);
        break;

    case 2://дизайнер
        for (const auto& order : orderList)
        {
            if (order.get_status() == 1 && order.get_designer() == _currentAccount)
                addItemToOrdersTable(order, ui.ordersTable->rowCount());
        }
        break;

    case 3://цех
        for (const auto& order : orderList)
        {
            if (order.get_status() == 3)
                addItemToOrdersTable(order, ui.ordersTable->rowCount());
        }
        break;

    default:
        break;
    }

    ui.ordersTable->scrollToBottom();
    ui.ordersTable->hideColumn(0);
    ui.ordersTable->resizeColumnsToContents();
    ui.ordersTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui.ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.ordersTable->setContextMenuPolicy(Qt::CustomContextMenu);
}

void MainForm::setOrdersTableByFilter(const std::vector<Order>& orderList)
{
    clearOrderDetails();
    ui.ordersTable->clearContents();
    ui.ordersTable->setRowCount(0);

    for (const auto& order : orderList)
    {
        if (_onlyMyOrdersAction->isChecked() && order.get_manager() != _currentAccount)
            continue;

        if (!_hideCompletedOrdersAction->isChecked() && order.get_status() == 4)
        {
            addItemToOrdersTable(order, ui.ordersTable->rowCount());
            continue;
        }

        if (_mockUpFilterAction->isChecked() && order.get_status() == 1)
        {
            addItemToOrdersTable(order, ui.ordersTable->rowCount());
            continue;
        }

        if (_approvalFilterAction->isChecked() && order.get_status() == 2)
        {
            addItemToOrdersTable(order, ui.ordersTable->rowCount());
            continue;
        }

        if (_productionFilterAction->isChecked() && order.get_status() == 3)
        {
            addItemToOrdersTable(order, ui.ordersTable->rowCount());
            continue;
        }

        if (_waitingFilterAction->isChecked() && order.get_status() == 5)
        {
            addItemToOrdersTable(order, ui.ordersTable->rowCount());
            continue;
        }
    }

    ui.ordersTable->scrollToBottom();
    ui.ordersTable->resizeColumnsToContents();
    ui.ordersTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}

void MainForm::addItemToOrdersTable(const Order& order, const int row)
{
    QTableWidgetItem* orderIdItem = new QTableWidgetItem(QString::number(order.get_id()));
    orderIdItem->setData(Qt::UserRole, order.get_id());
    QTableWidgetItem* orderDateItem = new QTableWidgetItem(order.get_date().toString("dd.MM.yyyy"));
    QTableWidgetItem* clientItem = new QTableWidgetItem();
    QTableWidgetItem* productTypeItem = new QTableWidgetItem();
    QTableWidgetItem* managerItem = new QTableWidgetItem();
    QTableWidgetItem* designerItem = new QTableWidgetItem();
    QTableWidgetItem* productTimeItem = new QTableWidgetItem(order.get_productionTime().toString("dd.MM.yyyy"));
    QTableWidgetItem* statusItem = new QTableWidgetItem();

    if (!order.isRead())
    {
        switch (findAccount(_currentAccount)->get_position())
        {
        case 1: // менеджер
            if (order.get_manager() == _currentAccount && (order.get_status() == 2 || order.get_status() == 4))
                statusItem->setBackground(QBrush(Qt::green));
            break;

        case 2: // дизайнер
            if (order.get_designer() == _currentAccount && order.get_status() == 1)
                statusItem->setBackground(QBrush(Qt::green));
            break;

        case 3: // цех
            if (order.get_status() == 3)
                statusItem->setBackground(QBrush(Qt::green));
            break;

        default:
            break;
        }
    }

    if (const Client* client = findClient(order.get_client()))
    {
        clientItem->setText(client->get_name());
        clientItem->setData(Qt::UserRole, client->get_id());
    }

    if (const ProductType* type = findProductType(order.get_productType()))
    {
        productTypeItem->setText(type->get_type());
        productTypeItem->setData(Qt::UserRole, type->get_id());
    }

    if (const Account* manager = findAccount(order.get_manager()))
    {
        managerItem->setText(manager->get_surname() + " " + manager->get_name()[0].toUpper() + ".");
        managerItem->setData(Qt::UserRole, manager->get_id());
    }

    if (const Account* designer = findAccount(order.get_designer()))
    {
        designerItem->setText(designer->get_surname() + " " + designer->get_name()[0].toUpper() + ".");
        designerItem->setData(Qt::UserRole, designer->get_id());
    }

    if (const Status* status = findStatus(order.get_status()))
    {
        statusItem->setText(status->get_status());
        statusItem->setData(Qt::UserRole, status->get_id());
    }

    ui.ordersTable->insertRow(row);
    ui.ordersTable->setItem(row, 0, orderIdItem);
    ui.ordersTable->setItem(row, 1, orderDateItem);
    ui.ordersTable->setItem(row, 2, clientItem);
    ui.ordersTable->setItem(row, 3, productTypeItem);
    ui.ordersTable->setItem(row, 4, managerItem);
    ui.ordersTable->setItem(row, 5, designerItem);
    ui.ordersTable->setItem(row, 6, productTimeItem);
    ui.ordersTable->setItem(row, 7, statusItem);
}

bool MainForm::setOrdersStatus(const int orderId, const int status)
{    
    if (Order* order = findOrder(orderId))
    {
        if (Database::editStatusInOrder(order->get_id(), status))
        {
            order->set_status(status);
            return true;
        }
        else
            QMessageBox::critical(this, "Ошибка", "Ошибка базы данных");        
    }

    return false;
}

void MainForm::clearOrderDetails()
{
    ui.mockupTaskTextEdit->clear();
    ui.productTaskTextEdit->clear();
    ui.nameClientLineEdit->clear();
    ui.phoneClientLineEdit->clear();
    ui.mailClientLineEdit->clear();
}

void MainForm::setWindowSize()
{
    QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
    settings.beginGroup("sizes");
    const int height = settings.value("MainForm_height", this->height()).toInt();
    const int width = settings.value("MainForm_width", this->width()).toInt();
    settings.endGroup();
    this->resize(width, height);
}

int MainForm::randomBetween(int low, int high)
{
    return (qrand() % ((high + 1) - low) + low);
}

bool MainForm::askForClose(const QString& titleText, const QString& askText)
{
    QMessageBox messageBox(QMessageBox::Question,
        titleText,
        askText,
        QMessageBox::Yes | QMessageBox::No,
        0);
    messageBox.setButtonText(QMessageBox::Yes, "Да");
    messageBox.setButtonText(QMessageBox::No, "Нет");
    if (messageBox.exec() == QMessageBox::No)
        return false;

    return true;
}

void MainForm::closeEvent(QCloseEvent* event)
{
    if (askForClose("Выход из программы", "Вы уверены, что хотите выйти?"))
        event->accept();
    else
        event->ignore();
}

void MainForm::set_statusList()
{
    _statusList = Database::getStatusList();
}

void MainForm::setManagerData()
{
    connect(ui.ordersTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(managerMenuRequestedSlot(QPoint)));
    connect(_showClientsAction, &QAction::triggered, this, &MainForm::showClientsFormSlot);
    ui.menuBar->addAction(_showClientsAction);
    setMenuBar();
    setOrdersTable();
    startTimer();
}

void MainForm::setDesignerData()
{
    connect(ui.ordersTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(designerMenuRequestedSlot(QPoint)));
    setMenuBar();
    setOrdersTable();    
    startTimer();
}

void MainForm::setWorkshopData()
{
    connect(ui.ordersTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(workshopMenuRequestedSlot(QPoint)));    
    setMenuBar();
    setOrdersTable();
    startTimer();
}

Order* MainForm::findOrder(const int orderId)
{
    auto it = std::find_if(_orderList.begin(), _orderList.end(), [&orderId](const Order& order) {return order.get_id() == orderId; });
    return (it == _orderList.end()) ? nullptr : &(*it);
}

Account* MainForm::findAccount(const QString& login)
{
    auto it = std::find_if(_accountList.begin(), _accountList.end(), [&login](const Account& account) {return account.get_login() == login; });
    return (it == _accountList.end()) ? nullptr : &(*it);
}

Account* MainForm::findAccount(const int accountId)
{
    auto it = std::find_if(_accountList.begin(), _accountList.end(), [&accountId](const Account& account) {return account.get_id() == accountId; });
    return (it == _accountList.end()) ? nullptr : &(*it);
}

Client* MainForm::findClient(const int clientId)
{
    auto it = std::find_if(_clientList.begin(), _clientList.end(), [&clientId](const Client& client) {return client.get_id() == clientId; });
    return (it == _clientList.end()) ? nullptr : &(*it);
}

ProductType* MainForm::findProductType(const int typeId)
{
    auto it = std::find_if(_productTypeList.begin(), _productTypeList.end(), [&typeId](const ProductType& type) {return type.get_id() == typeId; });
    return (it == _productTypeList.end()) ? nullptr : &(*it);
}

Status* MainForm::findStatus(const int statusId)
{
    auto it = std::find_if(_statusList.begin(), _statusList.end(), [&statusId](const Status& status) {return status.get_id() == statusId; });
    return (it == _statusList.end()) ? nullptr : &(*it);
}

bool MainForm::checkClientInOrders(const int clientId)
{
    auto it = std::find_if(_orderList.begin(), _orderList.end(), [&clientId](const Order& order) {return order.get_client() == clientId; });
    return (it == _orderList.end()) ? false : true;
}

void MainForm::managerMenuRequestedSlot(QPoint pos)
{
	QMenu* menu = new QMenu(this);
	QAction* addOrderAction = new QAction("Добавить заказ", this);
	QAction* editOrderAction = new QAction("Редактировать заказ", this);
	QAction* removeOrderAction = new QAction("Удалить заказ", this);
    QAction* sendToMockUpAction = new QAction("Отправить на макетирование", this);
    QAction* sendToProductionAction = new QAction("Отправить на производство", this);
    QAction* setCompletedAction = new QAction("Выполнено", this);
    QAction* notApprovalAction = new QAction("Не согласовано", this);

	connect(addOrderAction, SIGNAL(triggered()), this, SLOT(addOrderSlot()));
	connect(editOrderAction, SIGNAL(triggered()), this, SLOT(editOrderSlot()));
	connect(removeOrderAction, SIGNAL(triggered()), this, SLOT(removeOrderSlot()));
    connect(sendToMockUpAction, SIGNAL(triggered()), this, SLOT(sendToMockUpSlot()));
    connect(sendToProductionAction, SIGNAL(triggered()), this, SLOT(sendToProductionSlot()));
    connect(setCompletedAction, SIGNAL(triggered()), this, SLOT(setStatusCompletedSlot()));
    connect(notApprovalAction, SIGNAL(triggered()), this, SLOT(notApprovalSlot()));

    const int currRow = ui.ordersTable->selectionModel()->currentIndex().row();
    
    if (currRow != -1)
    {
        menu->addAction(addOrderAction);
        menu->addAction(editOrderAction);
        menu->addAction(removeOrderAction);
        menu->addSeparator();
        if (Order* order = findOrder(ui.ordersTable->item(currRow, 0)->data(Qt::UserRole).toInt()))
        {
            switch (order->get_productType())
            {
            case 1:     //дизайнерские услуги
                if (order->get_status() == 2)
                {
                    menu->addAction(setCompletedAction);
                    menu->addAction(notApprovalAction);
                }
                else if (order->get_status() == 5)
                    menu->addAction(sendToMockUpAction);
                break;

            case 2:     //производство
                menu->addAction(sendToProductionAction);
                break;

            case 3:     //дизайнерские услуги и производство
                if (order->get_status() == 2)
                {
                    menu->addAction(sendToProductionAction);
                    menu->addAction(notApprovalAction);
                }
                else if (order->get_status() == 5)
                    menu->addAction(sendToMockUpAction);                
                break;

            default:
                break;
            }

            if (order->get_manager() != _currentAccount)
            {
                editOrderAction->setEnabled(false);
                removeOrderAction->setEnabled(false);
                setCompletedAction->setEnabled(false);
                sendToMockUpAction->setEnabled(false);
                sendToProductionAction->setEnabled(false);
                notApprovalAction->setEnabled(false);
            }
        }
        
        menu->popup(ui.ordersTable->viewport()->mapToGlobal(pos));
    }
    else
    {
        menu->addAction(addOrderAction);
        menu->popup(ui.ordersTable->viewport()->mapToGlobal(pos));
    }
}

void MainForm::designerMenuRequestedSlot(QPoint pos)
{
    QMenu* menu = new QMenu(this);
    QAction* sendToApprovalAction = new QAction("Отправить на согласование", this);

    connect(sendToApprovalAction, SIGNAL(triggered()), this, SLOT(sendToApprovalSlot()));

    const int row = ui.ordersTable->selectionModel()->currentIndex().row();
    if (row < 0)
        return;

    menu->addAction(sendToApprovalAction);
    menu->popup(ui.ordersTable->viewport()->mapToGlobal(pos));    
}

void MainForm::workshopMenuRequestedSlot(QPoint pos)
{
    QMenu* menu = new QMenu(this);
    QAction* setCompletedAction = new QAction("Выполнено", this);

    connect(setCompletedAction, SIGNAL(triggered()), this, SLOT(setStatusCompletedSlot()));

    const int row = ui.ordersTable->selectionModel()->currentIndex().row();

    if (row < 0)
        return;
    menu->addAction(setCompletedAction);
    menu->popup(ui.ordersTable->viewport()->mapToGlobal(pos));   
}

void MainForm::showClientsFormSlot()
{
    ClientsForm clientsForm(this);
    clientsForm.setWindowTitle("Клиенты");
    clientsForm.setWindowFlags(Qt::Window);
    clientsForm.exec();
    QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
    settings.beginGroup("sizes");
    settings.setValue("ClientsForm_height", clientsForm.height());
    settings.setValue("ClientsForm_width", clientsForm.width());
    settings.endGroup();
    settings.sync();
}

void MainForm::addOrderSlot()
{
    OrderDialog dialog(this);
    dialog.setWindowTitle("Создание заказа");
    dialog.setWindowFlags(Qt::Window);
    dialog.saveButton->setVisible(false);

    if (!dialog.exec())
    {
        QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
        settings.beginGroup("sizes");
        settings.setValue("OrderDialog_height", dialog.height());
        settings.setValue("OrderDialog_width", dialog.width());
        settings.endGroup();
        settings.sync();
        return;
    }
        
    Order newOrder;
    newOrder.set_date(dialog.ordersDateEdit->date());
    newOrder.set_client(dialog.clientsCBox->currentData().toInt());
    if (dialog.mockUpTaskGBox->isChecked() && dialog.productTaskGBox->isChecked())
        newOrder.set_productType(3);
    else if (dialog.mockUpTaskGBox->isChecked())
        newOrder.set_productType(1);
    else if (dialog.productTaskGBox->isChecked())
        newOrder.set_productType(2);
    newOrder.set_manager(_currentAccount);    
    newOrder.set_productionTime(dialog.productTimeDateEdit->date());
    newOrder.set_status(5);
    if (dialog.mockUpTaskGBox->isChecked())
        newOrder.set_mockUpTask(dialog.mockupTaskTextEdit->toPlainText());
    else
        dialog.designerCBox->setCurrentIndex(-1);

    newOrder.set_designer(dialog.designerCBox->currentData().toInt());

    if (dialog.productTaskGBox->isChecked())
        newOrder.set_productionTask(dialog.productTaskTextEdit->toPlainText());

    if (Database::addOrder(newOrder))
    {
        newOrder.set_id(Database::getLastId(Database::TABLE_ORDERS));
        _orderList.push_back(std::move(newOrder));
        addItemToOrdersTable(newOrder, ui.ordersTable->rowCount());
    }

    ui.ordersTable->selectRow(ui.ordersTable->rowCount() - 1);
    selectOrderSlot();

    QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
    settings.beginGroup("sizes");
    settings.setValue("OrderDialog_height", dialog.height());
    settings.setValue("OrderDialog_width", dialog.width());
    settings.endGroup();
    settings.sync();
}

void MainForm::editOrderSlot()
{
    const int currRow = ui.ordersTable->currentRow();
    const int orderId = ui.ordersTable->item(currRow, 0)->data(Qt::UserRole).toInt();

    if (currRow < 0)
        return;

    OrderDialog dialog(this);
    dialog.setWindowTitle("Редактирование заказа");
    dialog.setWindowFlags(Qt::Window);
    dialog.addButton->setVisible(false);

    if (Order* order = findOrder(orderId))
    {
        dialog.ordersDateEdit->setDate(order->get_date());
        for (size_t i = 0; i <= dialog.clientsCBox->count(); ++i)
        {
            if (order->get_client() == dialog.clientsCBox->itemData(i))
                dialog.clientsCBox->setCurrentIndex(i);
        }
        dialog.productTimeDateEdit->setDate(order->get_productionTime());

        switch (order->get_productType())
        {
        case 1:
            dialog.mockUpTaskGBox->setChecked(true);
            dialog.mockupTaskTextEdit->setText(order->get_mockUpTask());
            for (size_t i = 0; i <= dialog.designerCBox->count(); ++i)
            {
                if (order->get_designer() == dialog.designerCBox->itemData(i))
                    dialog.designerCBox->setCurrentIndex(i);
            }
            break;
        case 2:
            dialog.productTaskGBox->setChecked(true);
            dialog.productTaskTextEdit->setText(order->get_productionTask());
            break;
        case 3:
            dialog.mockUpTaskGBox->setChecked(true);
            dialog.mockupTaskTextEdit->setText(order->get_mockUpTask());
            for (size_t i = 0; i <= dialog.designerCBox->count(); ++i)
            {
                if (order->get_designer() == dialog.designerCBox->itemData(i))
                    dialog.designerCBox->setCurrentIndex(i);
            }
            dialog.productTaskGBox->setChecked(true);
            dialog.productTaskTextEdit->setText(order->get_productionTask());
        default:
            break;
        }

        if (order->get_status() != 5)
        {
            dialog.mockUpTaskGBox->setEnabled(false);
            dialog.productTaskGBox->setEnabled(false);
        }

        if (!dialog.exec())
        {
            QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
            settings.beginGroup("sizes");
            settings.setValue("OrderDialog_height", dialog.height());
            settings.setValue("OrderDialog_width", dialog.width());
            settings.endGroup();
            settings.sync();
            return;
        }

        order->set_date(dialog.ordersDateEdit->date());
        order->set_client(dialog.clientsCBox->currentData().toInt());
        if (dialog.mockUpTaskGBox->isChecked() && dialog.productTaskGBox->isChecked())
            order->set_productType(3);
        else
        {
            if (dialog.mockUpTaskGBox->isChecked())
                order->set_productType(1);
            else
            {
                dialog.mockupTaskTextEdit->clear();
                dialog.designerCBox->setCurrentIndex(-1);
            }

            if (dialog.productTaskGBox->isChecked())
                order->set_productType(2);
            else 
                dialog.productTaskTextEdit->clear();
        }
        order->set_productionTask(dialog.productTaskTextEdit->toPlainText());
        order->set_mockUpTask(dialog.mockupTaskTextEdit->toPlainText());
        order->set_manager(_currentAccount);
        order->set_designer(dialog.designerCBox->currentData().toInt());
        order->set_productionTime(dialog.productTimeDateEdit->date());

        if (Database::editOrder(Order(order)))
        {
            ui.ordersTable->item(currRow, 1)->setText(order->get_date().toString("dd.MM.yyyy"));
            if (Client* client = findClient(order->get_client()))
                ui.ordersTable->item(currRow, 2)->setText(client->get_name());
            else
                ui.ordersTable->item(currRow, 2)->setText("");

            if (ProductType* type = findProductType(order->get_productType()))
                ui.ordersTable->item(currRow, 3)->setText(type->get_type());
            else
                ui.ordersTable->item(currRow, 3)->setText("");

            if (Account* manager = findAccount(order->get_manager()))
                ui.ordersTable->item(currRow, 4)->setText(manager->get_surname() + " " + manager->get_name()[0].toUpper() + ".");
            else
                ui.ordersTable->item(currRow, 4)->setText("");

            if (Account* designer = findAccount(order->get_designer()))
                ui.ordersTable->item(currRow, 5)->setText(designer->get_surname() + " " + designer->get_name()[0].toUpper() + ".");
            else
                ui.ordersTable->item(currRow, 5)->setText("");

            ui.ordersTable->item(currRow, 6)->setText(order->get_productionTime().toString("dd.MM.yyyy"));

            if (Status* status = findStatus(order->get_status()))
                ui.ordersTable->item(currRow, 7)->setText(status->get_status());
            else
                ui.ordersTable->item(currRow, 7)->setText("");
            
            QMessageBox::information(this, "Сохранение изменений", "Изменения успешно сохранены");

            ui.ordersTable->selectRow(currRow);
            selectOrderSlot();

            QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
            settings.beginGroup("sizes");
            settings.setValue("OrderDialog_height", dialog.height());
            settings.setValue("OrderDialog_width", dialog.width());
            settings.endGroup();
            settings.sync();
        }
        else
            QMessageBox::critical(this, "Ошибка", "Ошибка базы данных");
    } 
}

void MainForm::removeOrderSlot()
{
    const int currRow = ui.ordersTable->currentRow();
    const int orderId = ui.ordersTable->item(currRow, 0)->data(Qt::UserRole).toInt();

    if (currRow < 0)
        return;

    QMessageBox messageBox(QMessageBox::Question,
        "Удаление заказа",
        "Вы уверены что хотите удалить выбранный заказ?",
        QMessageBox::Yes | QMessageBox::No,
        this);
    messageBox.setButtonText(QMessageBox::Yes, "Да");
    messageBox.setButtonText(QMessageBox::No, "Нет");
    if (messageBox.exec() == QMessageBox::No)
        return;

    if (Database::removeObject(orderId, Database::TABLE_ORDERS))
    {
        auto itOrder = std::remove_if(MainForm::_orderList.begin(), MainForm::_orderList.end(), [&orderId](const Order& order) {return order.get_id() == orderId; });
        MainForm::_orderList.erase(itOrder, MainForm::_orderList.end());

        ui.ordersTable->removeRow(currRow);
    }

    clearOrderDetails();
}

void MainForm::sendToMockUpSlot()
{
    const int currRow = ui.ordersTable->currentRow();
    const int orderId = ui.ordersTable->item(currRow, 0)->data(Qt::UserRole).toInt();

    if (currRow < 0)
        return;

    if (!setOrdersStatus(orderId, 1))
        return;

    if (!_mockUpFilterAction->isChecked())
        ui.ordersTable->removeRow(currRow);

    clearOrderDetails();

    if (Order* order = findOrder(orderId))
    {
        Database::setMessage(order->get_designer(), true);
        Database::setOrderIsRead(orderId, false);
    }
}

void MainForm::sendToProductionSlot()
{
    const int currRow = ui.ordersTable->currentRow();
    const int orderId = ui.ordersTable->item(currRow, 0)->data(Qt::UserRole).toInt();

    if (currRow < 0)
        return;

    if (!setOrdersStatus(orderId, 3))
        return;
    
    if (!_productionFilterAction->isChecked() || findAccount(_currentAccount)->get_position() == 3)
        ui.ordersTable->removeRow(currRow);
    clearOrderDetails();
    if (Order* order = findOrder(orderId))
    {
        for (Account& worker : _accountList)
        {
            if (worker.get_position() != 3)
                continue;

            Database::setMessage(worker.get_id(), true);
            Database::setOrderIsRead(orderId, false);
        }        
    }
}

void MainForm::setStatusCompletedSlot()
{
    const int currRow = ui.ordersTable->currentRow();
    const int orderId = ui.ordersTable->item(currRow, 0)->data(Qt::UserRole).toInt();

    if (currRow < 0)
        return;

    if (setOrdersStatus(orderId, 4))
    {
        if (findAccount(_currentAccount)->get_position() == 1)
        {
            if (_hideCompletedOrdersAction->isChecked())
                ui.ordersTable->removeRow(currRow);
        }
        else
            ui.ordersTable->removeRow(currRow);
    }
    
    clearOrderDetails();

    if (findAccount(_currentAccount)->get_position() != 3)
        return;

    if (Order* order = findOrder(orderId))
    {
        Database::setMessage(order->get_manager(), true);
        Database::setOrderIsRead(orderId, false);
    }
}

void MainForm::sendToApprovalSlot()
{
    const int currRow = ui.ordersTable->currentRow();
    const int orderId = ui.ordersTable->item(currRow, 0)->data(Qt::UserRole).toInt();

    if (currRow < 0)
        return;

    if (setOrdersStatus(orderId, 2))
        ui.ordersTable->removeRow(currRow);
    clearOrderDetails();

    if (Order* order = findOrder(orderId))
    {
        Database::setMessage(order->get_manager(), true);
        Database::setOrderIsRead(orderId, false);
    }
}

void MainForm::notApprovalSlot()
{
    const int currRow = ui.ordersTable->currentRow();
    const int orderId = ui.ordersTable->item(currRow, 0)->data(Qt::UserRole).toInt();

    if (currRow < 0)
        return;

    if (setOrdersStatus(orderId, 5))
    {
        ui.ordersTable->item(currRow, 7)->setText(findStatus(5)->get_status());
        ui.ordersTable->item(currRow, 7)->setData(Qt::UserRole, 5);
    }
}

void MainForm::selectOrderSlot()
{
    clearOrderDetails();
    const int currRow = ui.ordersTable->currentRow();

    if (currRow < 0)
        return;

    if (Order* order = findOrder(ui.ordersTable->item(currRow, 0)->data(Qt::UserRole).toInt()))
    {
        if (!order->isRead())
        {
            switch (findAccount(_currentAccount)->get_position())
            {
            case 1: // менеджер
                if (order->get_manager() == _currentAccount && (order->get_status() == 2 || order->get_status() == 4))
                {
                    ui.ordersTable->item(currRow, 7)->setBackground(ui.ordersTable->item(currRow, 6)->background());
                    Database::setOrderIsRead(order->get_id(), true);
                    order->set_isRead(true);
                }                    
                break;

            case 2: // дизайнер
                if (order->get_designer() == _currentAccount && order->get_status() == 1)
                {
                    ui.ordersTable->item(currRow, 7)->setBackground(ui.ordersTable->item(currRow, 6)->background());
                    Database::setOrderIsRead(order->get_id(), true);
                    order->set_isRead(true);
                }
                break;

            case 3: // цех
                if (order->get_status() == 3)
                {
                    ui.ordersTable->item(currRow, 7)->setBackground(ui.ordersTable->item(currRow, 6)->background());
                    Database::setOrderIsRead(order->get_id(), true);
                    order->set_isRead(true);
                }
                break;

            default:
                break;
            }
        }
        ui.mockupTaskTextEdit->setText(order->get_mockUpTask());
        ui.productTaskTextEdit->setText(order->get_productionTask());
        if (Client* client = findClient(order->get_client()))
        {
            ui.nameClientLineEdit->setText(client->get_name());
            ui.phoneClientLineEdit->setText(client->get_phone());
            ui.mailClientLineEdit->setText(client->get_mail());
        }
    }
}

void MainForm::setOrdersTableByFilterSlot()
{
    if (!ui.searchLineEdit->text().isEmpty())
        setOrdersTableByFilter(_foundOrderList);
    else
        setOrdersTableByFilter(_orderList);
}

void MainForm::searchOrdersByClientSlot()
{
    clearOrderDetails();
    if (ui.searchLineEdit->text().isEmpty())
    {
        if (ui.newTasksBtn->isChecked())
            newTasksSlot();
        else
            setOrdersTable();
        return;
    }

    const size_t one = 1;
    QString searchText = ui.searchLineEdit->text().toLower();
    _foundOrderList.clear();

    for (const Order& order : _orderList)
    {
        if (const Client* cl = findClient(order.get_client()))
        {
            const QString client = cl->get_name().toLower();
            for (size_t k = 0; k < searchText.size(); ++k)
            {
                if (k == client.size())
                    break;
                if (searchText.at(k) != client.at(k))
                    break;
                if (k == searchText.size() - one)
                {
                    if (ui.newTasksBtn->isChecked() && order.isRead())
                        continue;
                    else
                        _foundOrderList.push_back(std::move(order));
                }
            }
        }
    }

    for (const Order& order : _orderList)
    {
        if (const Client* cl = findClient(order.get_client()))
        {
            const int orderId = order.get_id();
            const QString client = cl->get_name().toLower();
            const QStringList clientWordsList = client.split(" ");

            for (size_t n = 1; n < clientWordsList.size(); ++n)
            {
                for (size_t k = 0; k < searchText.size(); ++k)
                {
                    if (k == clientWordsList[n].size())
                        break;
                    if (searchText.at(k) != clientWordsList[n].at(k))
                        break;
                    if (k == searchText.size() - one)
                    {
                        bool duplicate = false;
                        auto it = std::find_if(_foundOrderList.begin(), _foundOrderList.end(), [&orderId](const Order& order) {return order.get_id() == orderId; });
                        duplicate = (it == _foundOrderList.end()) ? false : true;

                        if (!duplicate)
                        {
                            if (ui.newTasksBtn->isChecked() && order.isRead())
                                continue;
                            else
                                _foundOrderList.push_back(std::move(order));
                        }
                    }
                }
            }
        }
    }

    setOrdersTable(_foundOrderList);
}

void MainForm::fillDBSlot()
{
    //заполняем бд рандомными данными

    QList<QString> phones =
    {
        "89140334466",
        "сот. 8-902-782-6622",
        "москва: 8-966-231-5392, магадан: 8-914-030-5600",
        "сотовый: 89028341203, рабочий: 602213",
        "688922",
        "8-914-037-3313",
        "8(413-42)9-29-49",
        "8(4132)653267 и 89130428423",
        "89008432959 и 89023929933"
    };

    QList<QString> mails =
    {
        "myMail@yandex.ru",
        "eccon@mail.ru",
        "workMail@gmail.com",
        "vostok@iclud.com",
        "kish@yandex.ru",
        "musicHole@mail.ru",
        "fantasticMan@gmail.com",
        "iambeback@iclud.com",
        "noDrugsDoSport@yandex.com"
    };

    QList<QString> designerTasksList = 
    { 
        "Макетирование стенда\n1500 * 1000мм\nрасположить 8 карманов А4, логотип Волна обязательно\nТекст : Приказы, распоряжения , объявления\nоформление на тему бассейна – волны, вода и тд.Преимущественно синий цвет. \n", 
        "Визитка 4+4\n1я сторона : \n Текст : \nСалон обоев\nЦвета : бежевый и белый или серый и белый\n\n2я сторона\nБлинова Татьяна\nдизайнер\n89248556186\nmih.blinow2012@yandex.ru\nул.Коммуны, 11\nинстаграмм : tatidesign49\n", 
        "наклейка\n640 * 440мм, текст в поле 600х400\nоранж фон, текст черный\n    Уважаемые граждане!\n    О своевременном возбуждении\nисполнительных производств вы\nможете узнать на сайте Федеральной\nслужбы судебных приставов\nfssp.gov.ru(синим) \nв разделе «Банк данных\nисполнительных производств» \n",
        "Баннер 2000*500мм\nКрасный фон, белые буквы\nТекст : «Продам 60м.кв. 89148555555» \n",
        "Пластиковая карта 4+4\n1я сторона\nНа фоне - логотип на всю ширину карты; надпись «VIP»; внизу – имя и фамилия держателя карты \n(список будет позже) \n\n2я сторона\n Привилегии держателя карты : \n* приоритетное оповещение о поступлении новых коллекций\n* подарок от магазина в честь вашего дня рождения \n* участие в ежегодных мероприятиях, посвященных Дню клиента\n* возможность капризничать и требовать повышенного внимания в стенах нашего магазина)) \nНиже контактная информация о магазине : \nМагазин BOHO CHIC пр.Карла Маркса 27\nИнстаграм(значок ИГ) bohochicmgd   тел. \n + 7(914) 037 - 97 - 98 Жанна\n  + 7(914) 860 - 29 - 75 Марина\n\nФон - розовый голографический.Либо розовый мрамор с вкраплением золота.Предпочтительная цветовая гамма шрифта - белый, золотой\n",
        "Купон\n100х50мм\nпожеланий конкретных нет\nТекст : \nПри предъявлении купона получите скидку 5 % (выделить жирно) на торжественные костюмы в Брависсимо, ТЦ Осенний вальс, бутик №5\n",
        "Уголок покупателя\n750 * 850мм\nСиний фон, белые буквы\nТекст : \nИнформация для покупателей\nниже размещаем карманы в 2 ряда\nА4 3шт в ряд(плоск.вертик) \nниже 2 кармана А4 плоск.верт.и карман А5 горизонт.объемн.под книгу\n",
        "Визитка  4+0\nТАКСИ 89644560002\n89004070202 Дмитрий \nКруглосуточно трезвый водитель\nтемный фон, желтые / белые буквы\n",
        "Баннер\n6000 * 4000мм\nРазместить изображение  хлеба, все что связано с выпечкой\nТекст : \nВсегда свежая выпечка!\n",
        "Штендер , 600*1200мм квадратный\nМакетирование по брендбуку. \nТекст : \nвсе для Торговли\nтел.: 8 914 850 1685\nХолодильное и торговое оборудование\nОдноразовая посуда и упаковочные материалы\nДачная мебель и садовый инвентарь\nТеплицы и парники из высокопрочных материалов\nПраздничные товары и многое другое\n",
        "Баннер на грузовик\n4160х1840мм\nберем макет баннера, который был на фасаде\n\nубираем : офис 60 - 64 - 22\nсотовый телефон смещаем вправо\nПо середине добавить адрес : \nул.Дзержинского, 26 а\n",
        "Дисконтная пластиковая карта 4+4\n1я сторона\nООО \"Офтальмологический центр \"КРОФТ - Оптика М\"\nг.Магадан, ул.Гагарина, д.12\n8 (4132)и 62 - 85 - 47\nУзнай больше о своих глазах. \n\n2я сторона\nРазовая скидка \nноминал : 10 %, 20 % \n\nХотят на макете глаз или очки, что то связанное с оптикой\n",
        "Табличка 2600*600мм\nТекст: \nПРОДАЖА аренда  8 911 278 47 41; \nСлова продажа крупнее; аренда чуть меньше шрифт\n",
        "плакетка с гравировкой угловая 210*150мм\nплакетка для водителя, поэтому нужно что то на тему машины\nочертание автомобиля марки Санг Йонг Кайрон 2012 г.в. / или колесо / или руль / или след протектора колеса…\nТекст : \nМОГБУ «Автобаза Правительства Магаданской области» выражает искреннюю благодарность Апевалову Вячеславу Сергеевичу, глубокоуважаемому работнику с пятидесятилетним стажем – заботливому, внимательному, чуткому мужу, отцу, деду и просто замечательному человеку, посвятившему свою жизнь созиданию, а также опасной и одновременно романтичной профессии водителя!\nПусть Вас и Вашу семью сопровождают здоровье, долголетие и добродетель!\nВаши коллеги»\nгород Магадан\n2021 год\n",
        "Табличка\n250 * 100мм\nКрасный фон, золотые буквы\nтекст : \nМАЙОР ПОЛИЦИИ\nГОЛУБЦОВ Д.В. \n"
    };

    QList<QString> worksTasksList = 
    {
        "Пленка, без обработки, 1шт \n1230 * 1080мм",
        "Пленка \n500 * 500мм - 2шт(разные), накатка на ПВХ 5мм \n380 * 520мм - 2шт(разные), обрезка",
        "Холст глянец, натяжка на подрамник \n570 * 970мм",
        "Баннер 550гр \n1600 * 600мм, проклейка периметра, люверсы по углам \n5шт",
        "Баннер 550гр \n800 * 2000мм \nОбрезка в край + заправка в конструкцию ролл ап",
        "Цифровая печать визиток 4+4, 300гр 1000шт.",
        "Цифровая печать грамот А4 300гр, 4+0 \n100шт",
        "Изготовление пластик. карт \n4 + 4, нумерация + штрих код, пластик серебро, 100шт",
        "Баннер 6000*3000мм \nОбрезка в край, 1шт",
        "Баннер 12000*3600мм \nПроклейка периметра, сквозная склейка, люверсы по периметру с шагом 30см.",
        "Плакат \nБумага глянец  600 * 400мм, обрезка в  край \n10шт",
        "Табличка \n250 * 120мм пленка + накатка на ПВХ 5мм с заворотом + скотч 2х сторон.",
        "Баннерная сетка \n2750 * 3750мм, проклейка периметра, 1шт",
        "Баннер 550гр \n3000 * 2000мм, проклейка периметра, люверсы с шагом 50см по периметру.",
        "Перфорированная пленка \n1200х980мм, обрезка в край"
    };

    for (int i = 0; i < 1000; ++i)
    {
        int year = randomBetween(2020, 2021);
        int month;
        int day;
        if (year == 2021)
        {
            month = randomBetween(1, 4);
            month == 4 ? day = randomBetween(1, 10) : day = randomBetween(1, 30);
           
        }
        else
        {
            month = randomBetween(1, 12);
            day = randomBetween(1, 30);
        }
        
        QDate date;
        date.setDate(year, month, day);
        int client = randomBetween(10, 49);
        int productType = randomBetween(1, 3);
        int manager = randomBetween(16, 18);
        int designer = -1;
        QDate productionTime = date.addDays(randomBetween(5, 15));
        int status = 5;
        QString mockUpTask = "";
        QString productionTask = "";

        switch (productType)
        {
        case 1:
            designer = randomBetween(14, 15);
            mockUpTask = designerTasksList[randomBetween(0, 14)];
            status = randomBetween(1, 2);
            break;

        case 2:
            productionTask = worksTasksList[randomBetween(0, 14)];
            status = randomBetween(3, 5);
            break;

        case 3:
            designer = randomBetween(14, 15);
            mockUpTask = designerTasksList[randomBetween(0, 14)];
            productionTask = worksTasksList[randomBetween(0, 14)];
            status = randomBetween(1, 5);
            break;

        default:
            break;
        }

        Order order;
        order.set_date(date);
        order.set_client(client);
        order.set_productType(productType);
        order.set_manager(manager);
        order.set_designer(designer);
        order.set_productionTime(productionTime);
        order.set_status(status);
        order.set_mockUpTask(mockUpTask);
        order.set_productionTask(productionTask);

        if (!Database::addOrder(order))
        {
            QMessageBox::critical(this, "Error", "Error add order to DB");
            break;
        }
    }

    QMessageBox::information(this, "", "complete");
}

void MainForm::checkMessageSlot()
{
    if (!Database::checkConnect())
    {
        QMessageBox::critical(this, "Ошибка", "Ошибка подключения к базе данных.\nПрограмма будет закрыта.");
        close();
    }
    
    bool message = Database::getMessage(_currentAccount);
    if (!message)
        return;

    _isMessageIndicator->setVisible(true);
    _newTaskMessage->setVisible(true);

    initDataFromDB();
    setOrdersTable();

    if (findAccount(_currentAccount)->get_position() == 3)
    {
        for (const auto& account : _accountList)
        {
            if (account.get_position() != 3)
                continue;

            Database::setMessage(account.get_id(), false);
        }
    }
    else
        Database::setMessage(_currentAccount, false);
}

void MainForm::updateData()
{
    initDataFromDB();
    setOrdersTable();
    QMessageBox::information(this, "Обновление данных", "Данные обновлены");
}

void MainForm::showAboutSlot()
{
    AboutForm aboutForm;
    aboutForm.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    aboutForm.setWindowTitle("О программе");
    aboutForm.setWindowModality(Qt::WindowModality::ApplicationModal);

    if (QFile(QDir::currentPath() + "/icons/logo.png").exists())
        aboutForm.imageLabel->setPixmap(QPixmap("./icons/logo.png"));
    if (QFile(QDir::currentPath() + "/icons/headNameImage.png").exists())
        aboutForm.headLbl->setPixmap(QPixmap("./icons/headNameImage.png"));
    if (QFile(QDir::currentPath() + "/icons/expertPlogo.png").exists())
        aboutForm.expertPLbl->setPixmap(QPixmap("./icons/expertPlogo.png"));
    if (QFile(QDir::currentPath() + "/icons/goose.png").exists())
        aboutForm.gooseLbl->setPixmap(QPixmap("./icons/goose.png"));

    aboutForm.exec();
}

void MainForm::newTasksSlot()
{
    _foundOrderList.clear();
    _isMessageIndicator->setVisible(false);
    _newTaskMessage->setVisible(false);

    if (!ui.newTasksBtn->isChecked())
    {
        setOrdersTable();
        return;
    }

    for (const Order& order : _orderList)
    {
        if (!order.isRead())
        {
            switch (findAccount(_currentAccount)->get_position())
            {
            case 1: // менеджер
                if (order.get_manager() == _currentAccount && (order.get_status() == 2 || order.get_status() == 4))
                {
                    _foundOrderList.push_back(std::move(order));
                }
                break;

            case 2: // дизайнер
                if (order.get_designer() == _currentAccount && order.get_status() == 1)
                {
                    _foundOrderList.push_back(std::move(order));
                }
                break;

            case 3: // цех
                if (order.get_status() == 3)
                {
                    _foundOrderList.push_back(std::move(order));
                }
                break;

            default:
                break;
            }
        }
    }

    setOrdersTable(_foundOrderList);
}
