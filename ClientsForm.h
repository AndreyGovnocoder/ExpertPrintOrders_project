#pragma once

#include <QDialog>
#include "ui_ClientsForm.h"
#include "MainForm.h"

class ClientsForm : public QDialog, public Ui::ClientsForm
{
	Q_OBJECT

public:
	ClientsForm(QWidget *parent = Q_NULLPTR);
	~ClientsForm() = default;

private:
	std::vector<Client> _foundClientList;
	void setClientsTable(const std::vector<Client>& clientList);
	void addItemToClientsTable(const Client& client, const int orw);
	void setWidowSize();

private slots:
	void customMenuRequestedSlot(QPoint pos);
	void addClientSlot();
	void editClientSlot();
	void removeClientSlot();
	void searchClientSlot();
};
