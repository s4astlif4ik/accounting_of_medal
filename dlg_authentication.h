#ifndef DLG_AUTHENTICATION_H
#define DLG_AUTHENTICATION_H

#include <QDialog>

namespace Ui {
class dlg_authentication;
}

class dlg_authentication : public QDialog
{
    Q_OBJECT

public:
    explicit dlg_authentication(QWidget *parent = 0);
    ~dlg_authentication();

private slots:
    void on_btn_login_clicked();
    void load_sps();

    void on_btn_close_clicked();

private:
    Ui::dlg_authentication *ui;
};

#endif // DLG_AUTHENTICATION_H
