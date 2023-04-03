#include "frm_filter.h"
#include "ui_frm_filter.h"
#include "frm_sendering.h"
#include "mw.h"


QLineEdit *g_le_filter_text;
QDateEdit *g_firstdate_filter;
QDateEdit *g_lastdate_filter;
extern MW *g_main_window; //ссылка на главную форму
extern FRM_sendering *g_sendering;
extern QSortFilterProxyModel *g_proxy; //ссылка на прокси модель статистики
extern QTreeView *g_tree; //ссылка на дерево статистики


FRM_filter::FRM_filter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FRM_filter)
{
    ui->setupUi(this);
//получение ссылки на поле фильтрации
    ::g_le_filter_text = ui->lE_text_filter;
//наполнение выпадающего списка периодов
    ui->cB_period_filter->addItem("весь период");
    ui->cB_period_filter->addItem("настраиваемый период");
    ui->cB_period_filter->addItem("за последнюю неделю");
    ui->cB_period_filter->addItem("за последний месяц");
    ui->cB_period_filter->addItem("За последний год");
    ui->cB_period_filter->setCurrentIndex(0);
//наполнение выпадающего списка типа наград
    ui->cb_filter_award_category->addItem("");
    ui->cb_filter_award_category->addItem("Государственные награды");
    ui->cb_filter_award_category->addItem("Ведомственные награды");
    ui->cb_filter_award_category->addItem("Иные награды");
    ui->cb_filter_award_category->setCurrentIndex(0);
    ui->cb_filter_award_category->hide();
}

FRM_filter::~FRM_filter()
{
    delete ui;
}

void FRM_filter::on_lE_text_filter_textChanged(const QString &arg1)
{
    ::g_proxy->setFilterFixedString(arg1);
    //if(!arg1.isEmpty())
    ::g_tree->expandAll();
}

void FRM_filter::on_cB_period_filter_currentIndexChanged(int index)
{
    QDate firstdate;
    ::g_firstdate_filter = ui->dE_firstdate_filter;
    ::g_lastdate_filter = ui->dE_lastdate_filter;
    QSqlQuery firstdate_query;
    if(firstdate_query.exec("SELECT MIN(inputdate) FROM documents LIMIT 1;"))
    {
        while (firstdate_query.next())
        {
            ui->dE_firstdate_filter->setDate(firstdate_query.value(0).toDate());
            firstdate = firstdate_query.value(0).toDate();
        }
    }
    if(index == 0) //фильтр по периоду отключен
    {
        ui->dE_lastdate_filter->setEnabled(false);
        ui->dE_lastdate_filter->setDate(QDate::currentDate());
        ui->dE_firstdate_filter->setEnabled(false);
        ui->dE_firstdate_filter->setDate(firstdate);
    }
    if(index == 1) //фильтр по периоду настраиваемый
    {
        ui->dE_lastdate_filter->setEnabled(true);
        ui->dE_lastdate_filter->setDate(QDate::currentDate());
        ui->dE_firstdate_filter->setEnabled(true);
        ui->dE_firstdate_filter->setDate(firstdate);
    }
    if(index == 2) //последняя неделя
    {
        ui->dE_lastdate_filter->setEnabled(false);
        ui->dE_lastdate_filter->setDate(QDate::currentDate());
        ui->dE_firstdate_filter->setEnabled(false);
        ui->dE_firstdate_filter->setDate(QDate::currentDate().addDays(-7));
    }
    if(index == 3) //последний месяц
    {
        ui->dE_lastdate_filter->setEnabled(false);
        ui->dE_lastdate_filter->setDate(QDate::currentDate());
        ui->dE_firstdate_filter->setEnabled(false);
        ui->dE_firstdate_filter->setDate(QDate::currentDate().addMonths(-1));
    }
    if(index == 4) //последний месяц
    {
        ui->dE_lastdate_filter->setEnabled(false);
        ui->dE_lastdate_filter->setDate(QDate::currentDate());
        ui->dE_firstdate_filter->setEnabled(false);
        ui->dE_firstdate_filter->setDate(QDate::currentDate().addYears(-1));
    }
}

void FRM_filter::on_btn_update_tree_clicked()
{
    ::g_main_window->load_tree_view();
}

