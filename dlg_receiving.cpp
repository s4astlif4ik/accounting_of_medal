#include "dlg_receiving.h"
#include "ui_dlg_receiving.h"

DLG_receiving::DLG_receiving(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DLG_receiving)
{
    ui->setupUi(this);
}

DLG_receiving::~DLG_receiving()
{
    delete ui;
}
