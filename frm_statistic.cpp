#include "frm_statistic.h"
#include "ui_frm_statistic.h"

FRM_statistic::FRM_statistic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FRM_statistic)
{
    ui->setupUi(this);
}

FRM_statistic::~FRM_statistic()
{
    delete ui;
}
