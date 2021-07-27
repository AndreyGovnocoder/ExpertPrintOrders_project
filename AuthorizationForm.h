#pragma once

#include <QDialog>
#include "ui_AuthorizationForm.h"
#include "MainForm.h"
#include <QShortcut>

class AuthorizationForm : public QDialog, public Ui::AuthorizationForm
{
	Q_OBJECT

public:
	AuthorizationForm(QWidget *parent = Q_NULLPTR);
	~AuthorizationForm() = default;

	int get_currentAccount() { return _currentAccount; }

private:
	int _currentAccount = -1;
	QShortcut* _keyEnter;

private slots:
	void createAccountSlot();
	void checkLoginPassSlot();
	void showHidePasswordSlot();
	void keyEnterPressSlot() { checkLoginPassSlot(); };
};
