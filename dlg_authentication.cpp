#include "dlg_authentication.h"
#include "ui_dlg_authentication.h"
//#include "mw_gl.h"
#include <QMessageBox>
#include <QTime>


//static const int LOAD_TIME_MSEC = 5 * 1000;
//static const int PROGRESS_X_PX = 272;
//static const int PROGRESS_Y_PX = 333;
//static const int PROGRESS_WIDTH_PX = 310;
//static const int PROGRESS_HEIGHT_PX = 28;

static int l_authentification_error_counter(3); //количество ошибок входа
int connection_db (QString login, QString pass); //функция проверки пользователя

dlg_authentication::dlg_authentication(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_authentication)
{
    ui->setupUi(this);

    ui->fr_login->setVisible(true);
    ui->pb_login->setVisible(false);
    ui->le_login->setFocus(); //фокус на поле ввода логина
    setWindowOpacity(0.95);
}

dlg_authentication::~dlg_authentication()
{
    delete ui;
}

void dlg_authentication::on_btn_login_clicked()
{
    if(::connection_db(ui->le_login->text(), ui->le_pass->text()) == 0) //если функция
    //if(::connection_db(ui->le_login->text(), ui->le_pass->text())) //для подключения по учетным данным базы данных
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
                                        .arg(ui->lbl_info->text())
                                        .arg("...аутентификация пользователя"));

        if(i == 31) ui->lbl_info->setText(QString("%1\n%2")
                                        .arg(ui->lbl_info->text())
                                        .arg("...проверка целостности СПС"));
        if(i == 67) ui->lbl_info->setText(QString("%1\n%2")
                                        .arg(ui->lbl_info->text())
                                        .arg("...подключение базы данных"));
        if(i == 95) ui->lbl_info->setText(QString("%1\n%2")
                                        .arg(ui->lbl_info->text())
                                        .arg("...запуск СПС"));
        time.start();
        for(; time.elapsed()<=50;)
        {
            ui->pb_login->setValue(i);
        }
    }
}

void dlg_authentication::on_btn_close_clicked()
{
    this->close();
}
