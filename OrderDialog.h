#pragma once

#include <QDialog>
#include "ui_OrderDialog.h"
#include "MainForm.h"

class OrderDialog : public QDialog, public Ui::OrderDialog
{
	Q_OBJECT

public:
	OrderDialog(QWidget *parent = Q_NULLPTR);
	~OrderDialog() = default;

private:
	QShortcut* _keyEsc;
	void setUiData();
	void setClientCBox();
	void setDesignerCBox();
	bool checkInputs();
	void setWindowSize();
	void closeEvent(QCloseEvent* event);

private slots:
	void addOrderSlot();
	void saveChangesSlot();
	void addNewClientSlot();
	void keyEscSlot() { close(); };
};
