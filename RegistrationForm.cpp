#include "RegistrationForm.h"

RegistrationForm::RegistrationForm(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	setPositionCBox();
	setLineEditsLengths();

	_keyEnter = new QShortcut(this);
	_keyEnter->setKey(Qt::Key_Enter);

	connect(_keyEnter, SIGNAL(activated()), this, SLOT(keyEnterPressSlot()));
}

bool RegistrationForm::checkInputs()
{
	if (nameLineEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "Внимание", "Не заполнено поле \"Имя\"");
		return false;
	}
	else if (surnameLineEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "Внимание", "Не заполнено поле \"Фамилия\"");
		return false;
	}
	else if (positionCBox->currentIndex() < 0)
	{
		QMessageBox::warning(this, "Внимание", "Не выбрана должность");
		return false;
	}
	else if (loginLineEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "Внимание", "Не заполнено поле \"Логин\"");
		return false;
	}
	else if (checkLogin(loginLineEdit->text()))
	{
		QMessageBox::warning(this, "Внимание", "Такой логин уже существует");
		return false;
	}
	else if (passwordLineEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "Внимание", "Не заполнено поле \"Пароль\"");
		return false;
	}
	else if (passwordLineEdit->text() != passRepeatLineEdit->text())
	{
		QMessageBox::warning(this, "Внимание", "Пароли должны совпадать");
		return false;
	}

	return true;
}

bool RegistrationForm::checkLogin(const QString& login)
{
	auto it = std::find_if(MainForm::_accountList.begin(), MainForm::_accountList.end(), [&login](const Account& account) {return account.get_login() == login; });
	
	return (it == MainForm::_accountList.end()) ? false : true;
}

void RegistrationForm::setLineEditsLengths()
{
	nameLineEdit->setMaxLength(20);
	surnameLineEdit->setMaxLength(20);
	loginLineEdit->setMaxLength(50);
	passwordLineEdit->setMaxLength(50);
	passRepeatLineEdit->setMaxLength(50);
}

void RegistrationForm::setPositionCBox()
{
	int index = 0;
	for (const auto& position : MainForm::_positionList)
	{
		QVariant var;
		var.setValue(position.get_id());
		positionCBox->addItem(position.get_position());
		positionCBox->setItemData(index, var);
		++index;
	}

	positionCBox->setCurrentIndex(-1);
}

void RegistrationForm::showHideSlot()
{
	if (passwordLineEdit->echoMode() == QLineEdit::EchoMode::Password)
	{
		passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Normal);
		passRepeatLineEdit->setEchoMode(QLineEdit::EchoMode::Normal);
		showHideBtn->setText("Скрыть");
	}
	else
	{
		passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);
		passRepeatLineEdit->setEchoMode(QLineEdit::EchoMode::Password);
		showHideBtn->setText("Показать");
	}
}

void RegistrationForm::registrationSlot()
{
	if (checkInputs())
		accept();
}
