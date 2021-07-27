#include "OrderDialog.h"

OrderDialog::OrderDialog(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);	
	setUiData();
	setWindowSize();
	_keyEsc = new QShortcut(this);
	_keyEsc->setKey(Qt::Key_Escape);
	connect(_keyEsc, SIGNAL(activated()), this, SLOT(keyEscSlot()));
}

void OrderDialog::setUiData()
{
	addNewClientBtn->setToolTip("Добавить нового клиента");	
	ordersDateEdit->setDate(QDate::currentDate());
	productTimeDateEdit->setDate(QDate::currentDate());

	setClientCBox();
	setDesignerCBox();	
}

void OrderDialog::setClientCBox()
{
	clientsCBox->clear();
	int indexClient = 0;
	for (const Client& client : MainForm::_clientList)
	{
		if (!client.is_active())
			continue;

		QVariant var;
		var.setValue(client.get_id());
		clientsCBox->addItem(client.get_name());
		clientsCBox->setItemData(indexClient, var);
		++indexClient;
	}

	clientsCBox->setCurrentIndex(-1);
}

void OrderDialog::setDesignerCBox()
{
	int indexDesigner = 0;
	for (const Account& designer : MainForm::_accountList)
	{
		if (designer.get_position() != 2 || !designer.is_active())
			continue;

		QVariant var;
		var.setValue(designer.get_id());
		designerCBox->addItem(designer.get_surname() + " " + designer.get_name()[0].toUpper() + ".");
		designerCBox->setItemData(indexDesigner, var);
		++indexDesigner;
	}

	designerCBox->setCurrentIndex(-1);
}

bool OrderDialog::checkInputs()
{
	if (ordersDateEdit->date() > QDate::currentDate())
	{
		QMessageBox::warning(this, "Внимание", "Дата заказа не может превышать текущую.");
		return false;
	}
	else if (clientsCBox->currentIndex() < 0)
	{
		QMessageBox::warning(this, "Внимание", "Не указан клиент");
		return false;
	}
	else if (!productTaskGBox->isChecked() && !mockUpTaskGBox->isChecked())
	{
		QMessageBox::warning(this, "Внимание", "Необходимо выбрать \"Макетирование\" и(или) \"Производство\"");
		return false;
	}
	else if (mockUpTaskGBox->isChecked() && designerCBox->currentIndex() < 0)
	{
		QMessageBox::warning(this, "Внимание", "Не указан дизайнер");
		return false;
	}
	else if (mockUpTaskGBox->isChecked() && mockupTaskTextEdit->toPlainText().isEmpty())
	{
		QMessageBox::warning(this, "Внимание", "Если выбрано \"Макетирование\" необходимо ввести текст задания");
		return false;
	}
	else if (productTaskGBox->isChecked() && productTaskTextEdit->toPlainText().isEmpty())
	{
		QMessageBox::warning(this, "Внимание", "Если выбрано \"Производство\" необходимо ввести текст задания");
		return false;
	}

	return true;
}

void OrderDialog::setWindowSize()
{
	QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
	settings.beginGroup("sizes");
	const int height = settings.value("OrderDialog_height", this->height()).toInt();
	const int width = settings.value("OrderDialog_width", this->width()).toInt();
	settings.endGroup();
	this->resize(width, height);
}

void OrderDialog::closeEvent(QCloseEvent* event)
{
	if (MainForm::askForClose(this->windowTitle(),"Данные заказа не добавлены в базу данных. Все равно выйти?"))
		event->accept();
	else
		event->ignore();
}

void OrderDialog::addOrderSlot()
{
	if (checkInputs())
		accept();
}

void OrderDialog::saveChangesSlot()
{
	if (checkInputs())
		accept();
}

void OrderDialog::addNewClientSlot()
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
		dialog.mailLineEdit->text(), true);

	if (Database::addClient(client))
	{
		MainForm::_clientList.push_back(std::move(client));
		QMessageBox::information(this, "Добавление нового клиента", "Клиент успешно добавлен в базу данных");
		setClientCBox();
		clientsCBox->setCurrentIndex(clientsCBox->count() - 1);
	}
	else
		QMessageBox::critical(this, "Добавление нового клиента", "Ошибка добавления клиента в базу данных");
}
