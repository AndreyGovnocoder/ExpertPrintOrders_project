#include "MainForm.h"
#include <QtWidgets/QApplication>
#include <qfile.h>
#include <QtWidgets/qmessagebox.h>
#include <QFileDialog>

QString getPathToDB()
{
    QMessageBox::warning(0, "Внимание", "Отсутствует доспут к базе данных.\nПожалуйста, укажите путь к базе данных.");
    QString path = QFileDialog::getOpenFileName(0,
        "Указать базу данных",
        QDir::currentPath(),
        "Database(*.db)");
    return path;
}

int main(int argc, char *argv[])
{

    QCoreApplication::setOrganizationName("Гулий Андрей");
    QCoreApplication::setOrganizationDomain("Elizgerd@yandex.ru");
    QCoreApplication::setApplicationName("EXPERTprintOrders");

    QApplication app(argc, argv);
    MainForm mainForm;

    if (QFile(QDir::currentPath() + "/icons/icon.ico").exists())
        app.setWindowIcon(QIcon("./icons/icon.ico"));

    QString pathToDB;
    bool rightPath = false;
    do
    {
        if (!QFile("config.ini").exists())
        {
           
            pathToDB = getPathToDB();
            
            if (pathToDB.isEmpty())
                return 0;

            QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
            settings.setValue("pathToDB", pathToDB);            
        }

        QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);

        if (!QFile(settings.value("pathToDB").toString()).exists())
        {
            pathToDB = getPathToDB();

            if (pathToDB.isEmpty())
                return 0;

            settings.setValue("pathToDB", pathToDB);
            continue;
        }
        
        pathToDB = settings.value("pathToDB").toString();
        rightPath = true;

    } while (!rightPath);
        
    mainForm._pathToDatabase = pathToDB;
    Database::initDatabase(mainForm._pathToDatabase);
    mainForm.initDataFromDB();    

    AuthorizationForm authorizationForm;
    authorizationForm.setWindowTitle("Авторизация");
    authorizationForm.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    if (authorizationForm.exec())
    {
        QString winTitle = "Orders 1.0: ";

        switch (MainForm::findAccount(authorizationForm.get_currentAccount())->get_position())
        {
        case 1:
            winTitle += "Менеджер ";
            if (Account* account = MainForm::findAccount(authorizationForm.get_currentAccount()))
                winTitle += account->get_surname() + " " + account->get_name();
            mainForm.set_currentAccount(authorizationForm.get_currentAccount());
            mainForm.setManagerData();
            mainForm.setWindowTitle(winTitle);
            mainForm.show();
            break;

        case 2:
            winTitle += "Дизайнер ";
            if (Account* account = MainForm::findAccount(authorizationForm.get_currentAccount()))
                winTitle += account->get_surname() + " " + account->get_name();
            mainForm.set_currentAccount(authorizationForm.get_currentAccount());
            mainForm.setDesignerData();
            mainForm.setWindowTitle(winTitle);
            mainForm.show();
            break;

        case 3:
            winTitle += "Цех. ";
            if (Account* account = MainForm::findAccount(authorizationForm.get_currentAccount()))
                winTitle += account->get_surname() + " " + account->get_name();
            mainForm.set_currentAccount(authorizationForm.get_currentAccount());
            mainForm.setWorkshopData();
            mainForm.setWindowTitle(winTitle);
            mainForm.show();
            break;

        default:
            QMessageBox::critical(0, "Ошибка", "Ошибка авторизации");
            break;
        }
                
        int end = app.exec();
        QSettings settings(QDir::currentPath() + "/config.ini", QSettings::IniFormat);
        settings.beginGroup("menuBar");
        settings.setValue("onlyMyOrdersAction", mainForm.get_onlyMyOrdersAction()->isChecked());
        settings.setValue("hideCompletedOrdersAction", mainForm.get_hideCompletedOrdersAction()->isChecked());
        settings.setValue("mockUpFilterAction", mainForm.get_mockUpFilterAction()->isChecked());
        settings.setValue("approvalFilterAction", mainForm.get_approvalFilterAction()->isChecked());
        settings.setValue("productionFilterAction", mainForm.get_productionFilterAction()->isChecked());
        settings.setValue("waitingFilterAction", mainForm.get_waitingFilterAction()->isChecked());
        settings.endGroup();
        settings.beginGroup("sizes");
        settings.setValue("MainForm_height", mainForm.height());
        settings.setValue("MainForm_width", mainForm.width());
        settings.endGroup();
        settings.sync();
        return end;
    }
    else
    {
        app.quit();
        return 0;
    }
}
