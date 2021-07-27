#pragma once

#include <QDialog>
#include "ui_ClientDialog.h"
#include <QtWidgets/qmessagebox.h>
#include "MainForm.h"

class ClientDialog : public QDialog, public Ui::ClientDialog
{
	Q_OBJECT

public:
	ClientDialog(QWidget *parent = Q_NULLPTR);
	~ClientDialog();

private:
	QShortcut* _keyEnter;
	QShortcut* _keyEsc;

	void setLineEditsLengths();
	bool checkInputs();
	void setWindowSize();
	void closeEvent(QCloseEvent* event);

private slots:
	void addClientSlot();
	void saveChangesSlot();
	void keyEnterPressSlot();
	void keyEscSlot() { close(); };
};
