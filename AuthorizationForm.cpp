#include "AuthorizationForm.h"

AuthorizationForm::AuthorizationForm(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);

	_keyEnter = new QShortcut(this);
	_keyEnter->setKey(Qt::Key_Enter);

	connect(_keyEnter, SIGNAL(activated()), this, SLOT(keyEnterPressSlot()));
}

void AuthorizationForm::createAccountSlot()
{
	RegistrationForm registrationForm;
	registrationForm.setWindowTitle("Регистрация аккаунта");
	registrationForm.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
	if (!registrationForm.exec())
		return;

	Account account;
	account.set_name(registrationForm.nameLineEdit->text());
	account.set_surname(registrationForm.nameLineEdit->text());
	account.set_position(registrationForm.positionCBox->currentData().toInt());
	account.set_name(registrationForm.nameLineEdit->text());
	account.set_surname(registrationForm.nameLineEdit->text());
	account.set_login(registrationForm.loginLineEdit->text());
	account.set_password(registrationForm.passwordLineEdit->text());
	account.set_active(true);

	if (Database::addAccount(account))
	{
		QMessageBox::information(this, "Регистрация аккаунта", "Регистрация прошла успешно");
		account.set_id(Database::getLastId(Database::TABLE_ACCOUNTS));
		MainForm::_accountList.push_back(std::move(account));
	}
	else
		QMessageBox::critical(this, "Регистрация аккаунта", "Ошибка регистрации");

}

void AuthorizationForm::checkLoginPassSlot()
{
	Account* account = MainForm::findAccount(loginLineEdit->text());

	if (!account)
	{
		QMessageBox::warning(this, "Авторизация", "Неверный логин");
		return;
	}	

	if (passwordLineEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "Авторизация", "Введите пароль");
		return;
	}

	if (account->get_password() != passwordLineEdit->text())
	{
		QMessageBox::warning(this, "Авторизация", "Неверный пароль");
		return;
	}	

	_currentAccount = account->get_id();
	accept();
}

void AuthorizationForm::showHidePasswordSlot()
{
	if (passwordLineEdit->echoMode() == QLineEdit::EchoMode::Password)
	{
		passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Normal);
		showHideBtn->setText("Скрыть");
	}
	else
	{
		passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);
		showHideBtn->setText("Показать");
	}
}