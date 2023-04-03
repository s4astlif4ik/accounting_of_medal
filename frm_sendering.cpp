#include "frm_sendering.h"
#include "ui_frm_sendering.h"
#include "mw.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QSpinBox>
#include <QDate>


FRM_sendering *g_sendering;
extern MW *g_main_window;

FRM_sendering::FRM_sendering(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FRM_sendering)
{
    ui->setupUi(this);
    ::g_sendering = this;
//модель для выпадающего списка документов
    model_incoming_doc->setQuery("SELECT DISTINCT inputnumber, '№ ' || inputnumber || ' от ' || inputdate\
        FROM documents WHERE type = 'Получение наград' ORDER BY inputnumber");
    ui->cB_incoming_doc->setModel(model_incoming_doc);
    ui->cB_incoming_doc->setModelColumn(1);
    ui->cB_incoming_doc->setCurrentIndex(-1);
//модель для выпадающего списка документов
    model_outgoing_doc->setQuery("SELECT DISTINCT inputnumber, '№ ' || inputnumber || ' от ' || inputdate\
        FROM documents WHERE type = 'Выдача наград' ORDER BY inputnumber");
    ui->cB_outgoing_doc->setModel(model_outgoing_doc);
    ui->cB_outgoing_doc->setModelColumn(1);
    ui->cB_outgoing_doc->setCurrentIndex(-1);
//установка текущей даты
    ui->de_outgoing_date->setMinimumDate(QDate::currentDate());
//выравнивание столбцов в таблице при обновлении модели данных
    //connect(model_awards_in, SIGNAL(layoutChanged()), ui->tV_awards_in, SLOT(resizeColumnsToContents()));
    //connect(model_awards_out, SIGNAL(layoutChanged()), ui->tV_awards_out, SLOT(resizeColumnsToContents()));
}

FRM_sendering::~FRM_sendering()
{
    delete ui;
}

void FRM_sendering::on_cB_incoming_doc_currentIndexChanged(int index)
{
    model_awards_in->setQuery(QString("SELECT DISTINCT a.award_id AS \"Идентификатор\", d_t.text AS \"Тип\",\
        d_a.text AS \"Название\", a.number AS \"№ награды\",\
        (SELECT COUNT(count.award_id) FROM awards AS count WHERE count.award_id = a.award_id AND count.number = a.number\
        AND count.incoming_doc_id = a.incoming_doc_id AND count.storage = a.storage AND count.outgoing_doc_id IS NULL) AS \"Кол-во\",\
        a.storage AS \"Расположение\"\
        FROM awards AS a\
        INNER JOIN (SELECT DISTINCT razdel, kod, text FROM distionary WHERE razdel = 25) AS d_t ON (a.type_id = d_t.kod)\
        INNER JOIN (SELECT DISTINCT razdel, kod, text FROM distionary WHERE razdel = 1) AS d_a ON (a.award_id = d_a.kod)\
        WHERE incoming_doc_id = %1 AND outgoing_doc_id IS NULL ORDER BY \"Категория\", \"Тип\", \"Название\";")
        .arg(ui->cB_incoming_doc->model()->index(index, 0).data().toString()));
    ui->tV_awards_in->setModel(model_awards_in);
    ui->tV_awards_in->hideColumn(0);
    //ui->tV_awards_in->resizeColumnsToContents();
}

void FRM_sendering::on_cB_outgoing_doc_currentIndexChanged(int index)
{
    model_awards_out->setQuery(QString("SELECT DISTINCT a.award_id AS \"Идентификатор\", d_t.text AS \"Тип\",\
        d_a.text AS \"Название\", a.number AS \"№ награды\",\
        (SELECT COUNT(count.award_id) FROM awards AS count WHERE count.award_id = a.award_id AND count.number = a.number\
        AND count.incoming_doc_id = a.incoming_doc_id AND count.outgoing_doc_id = a.outgoing_doc_id AND count.storage = a.storage) AS \"Кол-во\",\
        a.storage AS \"Расположение\",\
            (SELECT COALESCE((SELECT 'не выдано' FROM awards AS no WHERE no.award_id = a.award_id AND no.number = a.number\
            AND no.outgoing_doc_id = a.outgoing_doc_id AND no.storage = a.storage AND no.outgoing_date IS NULL),\
            (SELECT 'выдано' FROM awards AS yes WHERE yes.award_id = a.award_id AND yes.number = a.number\
            AND yes.outgoing_doc_id = a.outgoing_doc_id AND yes.storage = a.storage AND yes.outgoing_date IS NOT NULL), '')) AS \"Статус\"\
        FROM awards AS a\
        INNER JOIN (SELECT DISTINCT razdel, kod, text FROM distionary WHERE razdel = 25) AS d_t ON (a.type_id = d_t.kod)\
        INNER JOIN (SELECT DISTINCT razdel, kod, text FROM distionary WHERE razdel = 1) AS d_a ON (a.award_id = d_a.kod)\
        WHERE outgoing_doc_id = %1 ORDER BY \"Категория\", \"Тип\", \"Название\";")
        .arg(ui->cB_incoming_doc->model()->index(index, 0).data().toString()));
    ui->tV_awards_out->setModel(model_awards_out);
    ui->tV_awards_out->hideColumn(0);
    //ui->tV_awards_out->resizeColumnsToContents();
}

void FRM_sendering::on_pushButton_clicked()
{
    QModelIndexList selectedrows = ui->tV_awards_in->selectionModel()->selectedRows();

    foreach (QModelIndex index, selectedrows)
    {
        //QMessageBox::information(this, QString::number(selectedrows.count()), ui->tV_awards_in->model()->index(index.row(), 5).data().toString());
        add_outgoing_award(index);
    }
    model_awards_in->setQuery(model_awards_in->query().lastQuery());
    model_awards_out->setQuery(model_awards_out->query().lastQuery());
}

void FRM_sendering::on_tV_awards_in_doubleClicked(const QModelIndex &index)
{
    add_outgoing_award(index);
    model_awards_in->setQuery(model_awards_in->query().lastQuery());
    model_awards_out->setQuery(model_awards_out->query().lastQuery());
}

bool FRM_sendering::add_outgoing_award(QModelIndex index)
{
//объявление запроса на добавление наград в резерв
    int count_award(1);
    QSqlQuery query_add_award;
//проверка документа на выдачу
    if(ui->cB_outgoing_doc->currentText().isEmpty())
    {
        return false;
    }
    bool ok;
    if(ui->tV_awards_in->model()->index(index.row(), 4).data().toInt() > 1)
    {
        count_award = QInputDialog::getInt(this, ui->tV_awards_in->model()->index(index.row(), 2).data().toString(),
            QString("Укажите количество резервируемых медалей (доступно: %1)").arg(ui->tV_awards_in->model()->index(index.row(), 4).data().toString()), QSpinBox().value(),
            1, ui->tV_awards_in->model()->index(index.row(), 4).data().toInt(),1, &ok);
        if(!ok)
        {
            return false;
        }
    }
    for (int k = 0; k < count_award; k++)
    {
        if(!query_add_award.exec(QString("UPDATE awards SET outgoing_doc_id = %1\
            WHERE rowid IN (SELECT rowid FROM awards WHERE incoming_doc_id = %2 AND award_id = '%3' AND number = '%4'\
            AND storage = '%5' AND outgoing_doc_id IS NULL AND outgoing_date IS NULL LIMIT 1);")
            .arg(model_outgoing_doc->index(ui->cB_outgoing_doc->currentIndex(), 0).data().toString())
            .arg(model_incoming_doc->index(ui->cB_incoming_doc->currentIndex(), 0).data().toString())
            .arg(ui->tV_awards_in->model()->index(index.row(), 0).data().toString())
            .arg(ui->tV_awards_in->model()->index(index.row(), 3).data().toString())
            .arg(ui->tV_awards_in->model()->index(index.row(), 5).data().toString())))
        {
            QMessageBox::warning(this, "Ошибка при выполнении запроса", query_add_award.lastError().databaseText());
            return false;
        }
    }
    //model_awards_in->setQuery(model_awards_in->query().lastQuery());
    //model_awards_out->setQuery(model_awards_out->query().lastQuery());
    //::g_main_window->load_tree_view();
    return true;
}

void FRM_sendering::update_models()
{
    model_awards_in->setQuery(model_awards_in->query().lastQuery());
    model_awards_out->setQuery(model_awards_out->query().lastQuery());
    model_incoming_doc->setQuery(model_incoming_doc->query().lastQuery());
    model_outgoing_doc->setQuery(model_outgoing_doc->query().lastQuery());
}

void FRM_sendering::on_bnt_outgoing_save_clicked()
{
    QSqlQuery query_save_award;
    QMessageBox::StandardButton reply;
    if(!model_incoming_doc->index(ui->cB_incoming_doc->currentIndex(), 0).data().toString().isEmpty() &&
        !model_outgoing_doc->index(ui->cB_outgoing_doc->currentIndex(), 0).data().toString().isEmpty())
    {
        reply = QMessageBox::question(this, "Внимание", "Награды из таблицы будут выданы. Продолжить?..", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            if(!query_save_award.exec(QString("UPDATE awards SET outgoing_date = '%1'\
                WHERE outgoing_doc_id = %2;")
                .arg(ui->de_outgoing_date->date().toString(Qt::ISODate))
                .arg(model_outgoing_doc->index(ui->cB_outgoing_doc->currentIndex(), 0).data().toString())))
            {
                QMessageBox::warning(this, "Ошибка при выполнении запроса", query_save_award.lastError().databaseText());
            }
        model_awards_out->setQuery(model_awards_out->query().lastQuery());
        //update_models();
        }
    }
}

void FRM_sendering::on_bnt_print_report_clicked()
{
    QMessageBox::information(this, "Информация", "Когда нибудь здесь будет формировать реест, но на данный момент данная кнопка задействована для отката изменений в ходе демонстрации формы... ;-)");
    QSqlQuery query_rem_award;
    if(!model_incoming_doc->index(ui->cB_incoming_doc->currentIndex(), 0).data().toString().isEmpty() &&
            !model_outgoing_doc->index(ui->cB_outgoing_doc->currentIndex(), 0).data().toString().isEmpty())
    {
        if(!query_rem_award.exec(QString("UPDATE awards SET outgoing_doc_id = NULL, outgoing_date = NULL\
            WHERE outgoing_doc_id = %1;")
            .arg(model_outgoing_doc->index(ui->cB_outgoing_doc->currentIndex(), 0).data().toString())))
        {
            QMessageBox::warning(this, "Ошибка при выполнении запроса", query_rem_award.lastError().databaseText());
        }
        update_models();
    }
}

