#include "mw.h"
#include "dlg_authentication.h"
//#include "loggingcategories.h"

#include <QApplication>
#include <QtGui>
#include <QtSql/QSqlDatabase>
#include <QPixmap>
#include <QtSql>
#include <QSqlQuery>
#include <QTableView>
#include <QSqlTableModel>
#include <QSplashScreen>
#include <QPainter>
#include <QTime>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QScopedPointer>
#include <QTextStream>
#include <QDateTime>
#include <QLoggingCategory>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QInputDialog>


#include <QLineEdit>
#include <QTextEdit>


QSqlDatabase db;
int docindex;

//умный указатель на файл логирования
QScopedPointer<QFile>   m_logFile;

//объявляение обработчика
//void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
//Установка файла логирования
    m_logFile.reset(new QFile("/var/log/crypton/log"));
//Открытие файла логирования
    m_logFile.data()->open(QFile::Append | QFile::Text);
//Установка обработчика
//    qInstallMessageHandler(messageHandler);
//выбор драйвера базы данных
    //db = QSqlDatabase::addDatabase("QPSQL");

//запуск формы загрузки приложения
    QApplication a( argc, argv ); //объявление класса приложения
    a.processEvents();
//запуск формы аутентификации
    dlg_authentication authentication;
    authentication.setWindowFlags(Qt::FramelessWindowHint);
    authentication.setModal(true);
    if(authentication.exec() == QDialog::Rejected && !db.open()) return 0;
    MW w; //объявление главной формы
    w.showMaximized();
    w.show(); //отображение главной формы
    return a.exec();
}

//реализация обработчика
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
//открытие потока записи в файл
    QTextStream out(m_logFile.data());
//запись даты создания записи
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
//по типу определяем, к какому уровню относится сообщение
    switch (type)
    {
    case QtInfoMsg:     out << "INF "; break;
    case QtDebugMsg:    out << "DBG "; break;
    case QtWarningMsg:  out << "WRN "; break;
    case QtCriticalMsg: out << "CRT "; break;
    case QtFatalMsg:    out << "FTL "; break;
    }
//запись вывода, категории сообщения и самого сообщения
    out << context.category << ": "
        << msg << endl;
//очистка буферизированных данных
    out.flush();
}

int connection_db (QString type_connection, QString login, QString pass) //функция подключения к базе данных
{
    if(type_connection == "local")
    {
    //текущей файл базы данных для локального подключения
        QString path = QDir::currentPath() + "/accounting_of_medal_db.sqlite";
    //если текущей файл базы данных не существует
        if(!QFileInfo(path).exists())
        {
            QString filters("Файл SQLite (*.sqlite);;Все файлы (*.*)");
            QString defaultFilter("Файл SQLite (*.sqlite)");
            QString defaultfilename(QString("./accounting_of_medal_db.sqlite"));
            path = QFileDialog::getOpenFileName(0, "База данных не найдена!", defaultfilename, filters, &defaultFilter);
            if (path.isEmpty() && path.isNull())
            {
                return 1;
            }
        }
        db = QSqlDatabase::addDatabase("QSQLITE"); //выбрать драйвер
        db.setDatabaseName(path);
    }
    if(type_connection == "network")
    {
        qDebug() << "Прием настроек сетевого подключения базы данных";
        db = QSqlDatabase::addDatabase("QODBC3"); //выбрать драйвер
        bool ok;
        QString connection_string = QInputDialog::getText(0, "Подключение к базе данных",
        QString("Укажите данные для подключения..."), QLineEdit::Normal, "DRIVER={SQL SERVER}; SERVER=127.0.0.1; DATABASE=accounting_of_medal_db; Trusted_Connection=yes;", &ok);
//при положительном решение...
        if(ok)
        {
            db.setDatabaseName(connection_string);
        }

//        db.setDatabaseName("DRIVER={SQL SERVER};SERVER=DESKTOP-J3OBSRS;DATABASE=accounting_of_medal_db;Trusted_Connection=yes;");
//        db.setDatabaseName("DRIVER={SQL SERVER};SERVER=127.0.0.1;DATABASE=accounting_of_medal_db;Trusted_Connection=yes;");
//        db.setHostName("127.0.0.1");
//        db.setHostName("localhost");
//        db.setDatabaseName("accounting_of_medal_db");
//        db.setPort(5432);
//        db.setUserName("sa");
//        db.setPassword("12345678");
    }

    if(db.open())
    {
        qDebug() << "Открытие базы данных";
        //начало раздела аутентификация по учетным данным таблицы в базе данных
        QSqlQuery authentication;
        if (authentication.exec(QString("SELECT DISTINCT usertype FROM users WHERE login = '%1' AND pass = '%2';").arg(login, pass)))
        {
            while (authentication.next())
            {
                if(authentication.value(0).toInt() > 0) return 0;
            }
        }
        //конец раздела аутентификация по учетным данным таблицы в базе данных
        qInfo() << QString("%1 подключился к базе данных").arg(login); //убрать или добавить имя пользователя
    }
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(QSqlDatabase::defaultConnection);
    qDebug() << QString("Ошибка подключения к базе данных пользователя %1, неправильный логин/пароль").arg(login);
    return 1;
}


