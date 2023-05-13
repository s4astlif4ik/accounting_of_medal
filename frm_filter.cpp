#include "frm_filter.h"
#include "ui_frm_filter.h"
#include "frm_sendering.h"
#include "mw.h"


QLineEdit *g_le_filter_text;
QDateEdit *g_firstdate_filter;
QDateEdit *g_lastdate_filter;
FRM_filter *g_filter;
extern MW *g_main_window; //ссылка на главную форму
extern FRM_sendering *g_sendering;
extern QSortFilterProxyModel *g_proxy; //ссылка на прокси модель статистики
extern QTreeView *g_tree; //ссылка на дерево статистики


FRM_filter::FRM_filter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FRM_filter)
{
    ui->setupUi(this);
    g_filter = this;
//получение ссылки на поле фильтрации
    ::g_le_filter_text = ui->lE_text_filter;
//наполнение выпадающего списка периодов
    ui->cB_period_filter->addItem("весь период");
    ui->cB_period_filter->addItem("настраиваемый период");
    ui->cB_period_filter->addItem("за последнюю неделю");
    ui->cB_period_filter->addItem("за последний месяц");
    ui->cB_period_filter->addItem("за последний год");
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
    if(firstdate_query.exec(QString("SELECT \"Наименование\",\
        (SELECT COUNT(award_id) FROM awards AS c WHERE c.award_id = kod AND c.incoming_doc_id = \"doc_id\") AS \"Отпустить\",\
        (SELECT COUNT(award_id) FROM awards AS c WHERE c.award_id = kod AND c.incoming_doc_id = \"doc_id\") AS \"Фактически отпущено\",\
        (SELECT DISTINCT GROUP_CONCAT(number, ', ') FROM awards AS n WHERE n.award_id = kod AND n.incoming_doc_id  = \"doc_id\" ORDER BY number)\
        FROM (SELECT '%1' AS \"doc_id\", kod, text AS \"Наименование\" FROM distionary AS d WHERE\
        EXISTS (SELECT award_id FROM awards AS a WHERE a.award_id = d.kod\
        AND a.incoming_doc_id = \"doc_id\") ORDER BY \"Наименование\") AS sql").arg(1)))
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

void FRM_filter::set_btn_update_tree_info(bool status)
{
    if(!status)
    {
        ui->btn_update_tree->setStyleSheet("background-color: rgba(255, 0, 0, 100);");
        ui->btn_update_tree->setText("Обновить статистику...");
    }
    if(status)
    {
        ui->btn_update_tree->setStyleSheet("background-color: rgba(255, 255, 255, 100);");
        ui->btn_update_tree->setText("Статистика обновлена...");
    }
}

void FRM_filter::on_btn_update_tree_clicked()
{
    ::g_main_window->load_tree_view();
}


void FRM_filter::on_dE_firstdate_filter_dateChanged(const QDate &/*date*/)
{
    set_btn_update_tree_info(false);
}

void FRM_filter::on_dE_lastdate_filter_dateChanged(const QDate &/*date*/)
{
    set_btn_update_tree_info(false);
}
