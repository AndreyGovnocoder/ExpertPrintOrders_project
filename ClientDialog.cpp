#include "ClientDialog.h"


ClientDialog::ClientDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	setLineEditsLengths();
	setWindowSize();
	_keyEnter = new QShortcut(this);
	_keyEnter->setKey(Qt::Key_Enter);
	_keyEsc = new QShortcut(this);
	_keyEsc->setKey(Qt::Key_Escape);

	connect(_keyEnter, SIGNAL(activated()), this, SLOT(keyEnterPressSlot()));
	connect(_keyEsc, SIGNAL(activated()), this, SLOT(keyEscSlot()));
}

ClientDialog::~ClientDialog()
{
	QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("sizes");
	settings.setValue("ClientDialog_height", height());
	settings.setValue("ClientDialog_width", width());
	settings.endGroup();
	settings.sync();
}

void ClientDialog::addClientSlot()
{
	if (checkInputs())
		accept();
}

void ClientDialog::setLineEditsLengths()
{
	nameLineEdit->setMaxLength(50);
	phoneLineEdit->setMaxLength(50);
	mailLineEdit->setMaxLength(20);
}

bool ClientDialog::checkInputs()
{
	if (nameLineEdit->text().isEmpty())
	{
		QMessageBox::warning(this, "Внимание", "Не заполнено поле \"Клиент\"");
		return false;
	}

	return true;
}

void ClientDialog::setWindowSize()
{
	QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("sizes");
	const int height = settings.value("ClientDialog_height", this->height()).toInt();
	const int width = settings.value("ClientDialog_width", this->width()).toInt();
	settings.endGroup();
	this->resize(width, height);
}

void ClientDialog::closeEvent(QCloseEvent* event)
{
	if (MainForm::askForClose(this->windowTitle(), "Данные клиента не внесены в базу данных. Все равно выйти?"))
		event->accept();
	else
		event->ignore();
}

void ClientDialog::saveChangesSlot()
{
	if (checkInputs())
		accept();
}

void ClientDialog::keyEnterPressSlot()
{
	if (checkInputs())
		accept();
}
