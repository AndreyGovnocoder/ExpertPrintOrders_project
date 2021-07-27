#include "ClientsForm.h"

ClientsForm::ClientsForm(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	setClientsTable(MainForm::_clientList);
	connect(clientsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequestedSlot(QPoint)));
	setWidowSize();
}

void ClientsForm::setClientsTable(const std::vector<Client>& clientList)
{
	clientsTable->clearContents();
	clientsTable->setRowCount(0);

	for (const auto& client : clientList)
	{
		if (client.is_active())
			addItemToClientsTable(client, clientsTable->rowCount());
	}

	clientsTable->hideColumn(0);
	clientsTable->resizeColumnsToContents();
	clientsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	clientsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	clientsTable->setContextMenuPolicy(Qt::CustomContextMenu);
	clientsTable->scrollToBottom();
}

void ClientsForm::addItemToClientsTable(const Client& client, const int row)
{
	QTableWidgetItem* clientIdItem = new QTableWidgetItem(QString::number(client.get_id()));
	clientIdItem->setData(Qt::UserRole, client.get_id());

	clientsTable->insertRow(row);
	clientsTable->setItem(row, 0, clientIdItem);
	clientsTable->setItem(row, 1, new QTableWidgetItem(client.get_name()));
	clientsTable->setItem(row, 2, new QTableWidgetItem(client.get_phone()));
	clientsTable->setItem(row, 3, new QTableWidgetItem(client.get_mail()));
}

void ClientsForm::setWidowSize()
{
	QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("sizes");
	const int height = settings.value("ClientsForm_height", this->height()).toInt();
	const int width = settings.value("ClientsForm_width", this->width()).toInt();
	settings.endGroup();
	this->resize(width, height);
}

void ClientsForm::addClientSlot()
{
	ClientDialog dialog(this);
	dialog.setWindowTitle("Добавление нового клиента");
	dialog.setWindowFlags(Qt::Window);
	dialog.saveButton->setVisible(false);
	if (!dialog.exec())
		return;

	Client client(
		dialog.nameLineEdit->text(),
		dialog.phoneLineEdit->text(),
		dialog.mailLineEdit->text(), 
		true);

	if (Database::addClient(client))
	{
		MainForm::_clientList.push_back(std::move(client));
		QMessageBox::information(this, "Добавление нового клиента", "Клиент успешно добавлен в базу данных");
		setClientsTable(MainForm::_clientList);
		searchLineEdit->text().clear();
	}
	else
		QMessageBox::critical(this, "Добавление нового клиента", "Ошибка добавления клиента в базу данных");
}

void ClientsForm::editClientSlot()
{
	const int currRow = clientsTable->currentRow();
	if (currRow < 0)
		return;

	const int clientId = clientsTable->item(currRow, 0)->data(Qt::UserRole).toInt();
	Client* edClient = MainForm::findClient(clientId);

	if (!edClient)
		return;

	ClientDialog dialog(this);
	dialog.setWindowTitle("Редактирование клиента");
	dialog.setWindowFlags(Qt::Window);
	dialog.addButton->setVisible(false);
	dialog.nameLineEdit->setText(edClient->get_name());
	dialog.phoneLineEdit->setText(edClient->get_phone());
	dialog.mailLineEdit->setText(edClient->get_mail());

	if (!dialog.exec())
		return;

	Client editedClient(
		edClient->get_id(),
		dialog.nameLineEdit->text(),
		dialog.phoneLineEdit->text(),
		dialog.mailLineEdit->text(), true);
	if (Database::editClient(editedClient))
	{
		std::replace_if(
			MainForm::_clientList.begin(),
			MainForm::_clientList.end(),
			[editedClient](const Client& client) { return editedClient.get_id() == client.get_id(); },
			editedClient);
		QMessageBox::information(this, "Редактирование клиента", "Изменения успешно сохранены");
		setClientsTable(MainForm::_clientList);
		searchLineEdit->text().clear();
	}
	else
		QMessageBox::critical(this, "Редактирование клиента", "Ошибка сохранения изменений");
	
}

void ClientsForm::removeClientSlot()
{
	const int currRow = clientsTable->currentRow();
	if (currRow < 0)
		return;

	const int clientId = clientsTable->item(currRow, 0)->data(Qt::UserRole).toInt();
	QMessageBox messageBox(QMessageBox::Question,
		"Удаление клиента",
		"Удалить выделенного клиента?",
		QMessageBox::Yes | QMessageBox::No,
		this);
	messageBox.setButtonText(QMessageBox::Yes, "Да");
	messageBox.setButtonText(QMessageBox::No, "Нет");
	if (messageBox.exec() != QMessageBox::Yes)
		return;

	if (MainForm::checkClientInOrders(clientId))
	{
		if (Client* client = MainForm::findClient(clientId))
		{
			if (Database::setActive(clientId, Database::TABLE_CLIENTS, false))
			{
				client->set_active(false);
				setClientsTable(MainForm::_clientList);
				searchLineEdit->text().clear();
				QMessageBox::information(this, "Удаление клиента", "Клиент успешно удален");
			}
		}
	}
	else
	{
		if (Database::removeObject(clientId, Database::TABLE_CLIENTS))
		{
			auto itClient = std::remove_if(MainForm::_clientList.begin(), MainForm::_clientList.end(), [&clientId](const Client& client) {return client.get_id() == clientId; });
			MainForm::_clientList.erase(itClient, MainForm::_clientList.end());
			clientsTable->removeRow(currRow);
			QMessageBox::information(this, "Удаление клиента", "Клиент успешно удален");
		}
		else
			QMessageBox::critical(this, "Удаление клиента", "Ошибка удаления клиента");
	}
}

void ClientsForm::customMenuRequestedSlot(QPoint pos)
{
	QMenu* menu = new QMenu(this);
	QAction* addClientAction = new QAction("Добавить", this);
	QAction* editClientAction = new QAction("Редактировать", this);
	QAction* removeClientAction = new QAction("Удалить", this);

	connect(addClientAction, SIGNAL(triggered()), this, SLOT(addClientSlot()));
	connect(editClientAction, SIGNAL(triggered()), this, SLOT(editClientSlot()));
	connect(removeClientAction, SIGNAL(triggered()), this, SLOT(removeClientSlot()));

	const int row = clientsTable->selectionModel()->currentIndex().row();

	if (row >= 0)
	{
		menu->addAction(addClientAction);
		menu->addAction(editClientAction);
		menu->addAction(removeClientAction);
	}
	else
	{
		menu->addAction(addClientAction);
	}

	menu->popup(clientsTable->viewport()->mapToGlobal(pos));
}

void ClientsForm::searchClientSlot()
{
	if (searchLineEdit->text().isEmpty())
	{
		setClientsTable(MainForm::_clientList);
		return;
	}

	const size_t one = 1;
	QString searchClient = searchLineEdit->text().toLower();
	_foundClientList.clear();

	for (const Client& client : MainForm::_clientList)
	{
		if (!client.is_active())
			continue;

		const QString clientStr = client.get_name().toLower();
		for (size_t i = 0; i < clientStr.size(); ++i)
		{
			if (i == clientStr.size())
				break;
			if (searchClient.at(i) != clientStr.at(i))
				break;
			if (i == searchClient.size() - one)
				_foundClientList.push_back(client);
		}
	}

	for (const Client& client : MainForm::_clientList)
	{
		if (!client.is_active())
			continue;

		const int clientId = client.get_id();
		const QString clientStr = client.get_name().toLower();
		const QStringList clientWordsList = clientStr.split(" ");

		for (size_t n = 1; n < clientWordsList.size(); ++n)
		{
			for (size_t i = 0; i < searchClient.size(); ++i)
			{
				if (i == clientWordsList[n].size())
					break;
				if (searchClient.at(i) != clientWordsList[n].at(i))
					break;
				if (i == searchClient.size() - one)
				{
					bool duplicate = false;
					auto it = std::find_if(_foundClientList.begin(), _foundClientList.end(), [&clientId](const Client& client) {return client.get_id() == clientId; });
					duplicate = (it == _foundClientList.end()) ? false : true;

					if (!duplicate)
					{
						_foundClientList.push_back(std::move(client));
					}
				}
			}
		}
	}

	setClientsTable(_foundClientList);
}