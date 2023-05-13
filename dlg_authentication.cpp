#include "dlg_authentication.h"
#include "ui_dlg_authentication.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QTime>


static int l_authentification_error_counter(3); //количество ошибок входа
int connection_db (QString type_connection, QString login, QString pass); //функция проверки пользователя

dlg_authentication::dlg_authentication(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_authentication)
{
    ui->setupUi(this);
//наполняем выпадающий список тип подключения
    ui->cB_type_connection->addItem("Локальное подключение");
    ui->cB_type_connection->addItem("Сетевое подключение");
//включаем рамку с данными для аутентификации
    ui->fr_login->setVisible(true);
//скрываем прогресс бар
    ui->pb_login->setVisible(false);
//фокус на тип подключения
    ui->cB_type_connection->setFocus();
//устанавливаем прозрачность главной формы
    setWindowOpacity(0.95);
}

dlg_authentication::~dlg_authentication()
{
    delete ui;
}

void dlg_authentication::on_btn_login_clicked()
{
    QString type_connection;
////текущей файл базы данных для локального подключения
//    QString path = QDir::currentPath() + "/accounting_of_medal_db.sqlite";
////если текущей файл базы данных не существует
//    if(!QFileInfo(path).exists())
//    {
//        QString filters("Файл SQLite (*.sqlite);;Все файлы (*.*)");
//        QString defaultFilter("Файл SQLite (*.sqlite)");
//        QString defaultfilename(QString("./accounting_of_medal_db.sqlite"));
//        path = QFileDialog::getOpenFileName(0, "База данных не найдена!", defaultfilename, filters, &defaultFilter);
//        if (path.isEmpty() && path.isNull())
//        {
//            on_btn_close_clicked();
//        }
//    }
    if(ui->cB_type_connection->currentIndex() == 0)
    {
        type_connection = "local";
    }
    if(ui->cB_type_connection->currentIndex() == 1)
    {
        type_connection = "network";
    }

    if(::connection_db(type_connection, ui->le_login->text(), ui->le_pass->text()) == 0) //если функция
    {
        load_sps();
        this->close();
    }
    else
    {
        if(::l_authentification_error_counter > 0)
        {
        ui->lbl_info->setStyleSheet("background-color: rgba(255, 255, 255, 0); color: rgb(255, 0, 0);");
        ui->lbl_info->setText(QString("...неправильный логин/пароль.\n...осталось %1 попыток ").arg(QString::number(::l_authentification_error_counter - 1)));
        ::l_authentification_error_counter = ::l_authentification_error_counter - 1;
        }
        else
        {
            QMessageBox msb;
            msb.warning(this, "Отказ в доступе!", "Логин/пароль не соответствует.\nПриложение будет закрыто!");
            this->close();
        }

    }
}

void dlg_authentication::load_sps()
{
    QTime time;
    ui->lbl_info->setStyleSheet("background-color: rgba(255, 255, 255, 0); color: rgb(0, 0, 0);");
    ui->lbl_info->setText("");
    ui->fr_login->setVisible(false);
    ui->pb_login->setVisible(true);

    for(int i = 0; i <= 100;i++)
    {
        if(i == 1) ui->lbl_info->setText(QString("%1\n%2")
                                        .arg(ui->lbl_info->text(), "...аутентификация пользователя"));
        if(i == 31) ui->lbl_info->setText(QString("%1\n%2")
                                        .arg(ui->lbl_info->text(), "...проверка целостности СПС"));
        if(i == 67) ui->lbl_info->setText(QString("%1\n%2")
                                        .arg(ui->lbl_info->text(), "...подключение базы данных"));
        if(i == 95) ui->lbl_info->setText(QString("%1\n%2")
                                        .arg(ui->lbl_info->text(), "...запуск СПС"));
        time.start();
        for(; time.elapsed() <= 50;)
        {
            ui->pb_login->setValue(i);
        }
    }
}

void dlg_authentication::on_btn_close_clicked()
{
    this->close();
}


