#pragma once

#include <QDialog>
#include "ui_RegistrationForm.h"
#include <QtWidgets/qmessagebox.h>
#include "MainForm.h"

class RegistrationForm : public QDialog, public Ui::RegistrationForm
{
	Q_OBJECT

public:
	RegistrationForm(QWidget *parent = Q_NULLPTR);
	~RegistrationForm() = default;

	bool checkInputs();
	bool checkLogin(const QString& login);

private:
	QShortcut* _keyEnter;

	void setLineEditsLengths();
	void setPositionCBox();

private slots:
	void registrationSlot();
	void showHideSlot();
	void keyEnterPressSlot() { registrationSlot(); };
};
