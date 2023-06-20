#include "form_statistic.h"
#include "ui_form_statistic.h"

Form_statistic::Form_statistic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_statistic)
{
    ui->setupUi(this);
}

Form_statistic::~Form_statistic()
{
    delete ui;
}
