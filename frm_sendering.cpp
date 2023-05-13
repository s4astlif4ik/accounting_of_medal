#include "frm_sendering.h"
#include "ui_frm_sendering.h"
#include "dlg_report.h"
#include "frm_filter.h"
#include "mw.h"
#include "report.h"

#include <QDate>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSpinBox>


FRM_sendering *g_sendering;
extern MW *g_main_window;
extern FRM_filter *g_filter;
//extern REPORT *g_report;

FRM_sendering::FRM_sendering(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FRM_sendering)
{
    ui->setupUi(this);
    ::g_sendering = this;
//модель для выпадающего списка документов
//наполнение выпадающего списка
    QIcon state_awards_icon;
    state_awards_icon.addFile(":/resource/Images/Герой Российской Федерации.png");
    QIcon miliraty_awards_icon;
    miliraty_awards_icon.addFile(":/resource/Images/За воинскую доблесть I степени.png");
    ui->cB_award_category->addItem(state_awards_icon, "Государственные награды");
    ui->cB_award_category->addItem(miliraty_awards_icon, "Ведомственные награды");
    //ui->cB_award_category->addItem("Иные награды");
    ui->cB_award_category->setCurrentIndex(-1);

//    model_award_type->setQuery("SELECT DISTINCT category FROM awards ORDER BY category");
//    ui->cB_award_type->setModel(model_award_type);
//    ui->cB_award_type->setModelColumn(0);
//    ui->cB_award_type->setCurrentIndex(-1);
////модель для выпадающего списка документов
//    model_outgoing_doc->setQuery("SELECT DISTINCT inputnumber, '№ ' || inputnumber || ' от ' || inputdate\
//        FROM documents WHERE type = 'Выдача наград' ORDER BY inputnumber");
//    ui->cB_outgoing_doc->setModel(model_outgoing_doc);
//    ui->cB_outgoing_doc->setModelColumn(1);
//    ui->cB_outgoing_doc->setCurrentIndex(-1);
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

void FRM_sendering::on_cB_award_category_currentIndexChanged(int index)
{
//загрузка списка невыданных наград
    model_awards_in->setQuery(QString("SELECT DISTINCT a.award_id AS \"Идентификатор\", d_t.text AS \"Тип\",\
        d_a.text AS \"Название\", a.number AS \"№ награды\",\
        (SELECT COUNT(count.award_id) FROM awards AS count WHERE count.award_id = a.award_id AND count.number = a.number\
        AND count.storage = a.storage AND count.outgoing_doc_id IS NULL) AS \"Кол-во\",\
        a.storage AS \"Расположение\"\
        FROM awards AS a\
        INNER JOIN (SELECT DISTINCT razdel, kod, text FROM distionary WHERE razdel = 25) AS d_t ON (a.type_id = d_t.kod)\
        INNER JOIN (SELECT DISTINCT razdel, kod, text FROM distionary WHERE razdel = 1) AS d_a ON (a.award_id = d_a.kod)\
        WHERE category = %1 AND outgoing_doc_id IS NULL ORDER BY \"Тип\", \"Название\";")
        .arg(index));
        //.arg(ui->cB_award_category->model()->index(index, 0).data().toString()));
    ui->tV_awards_in->setModel(model_awards_in);
    ui->tV_awards_in->hideColumn(0);
    ui->tV_awards_in->resizeColumnsToContents();

//модель для выпадающего списка документов
    model_outgoing_doc->setQuery(QString("SELECT DISTINCT inputnumber, '№ ' || inputnumber || ' от ' || inputdate\
        FROM documents WHERE awards_category = %1 AND type = 1 ORDER BY inputnumber").arg(index));
    ui->cB_outgoing_doc->setModel(model_outgoing_doc);
    ui->cB_outgoing_doc->setModelColumn(1);
    ui->cB_outgoing_doc->setCurrentIndex(-1);
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
        .arg(ui->cB_outgoing_doc->model()->index(index, 0).data().toString()));
    ui->tV_awards_out->setModel(model_awards_out);
    ui->tV_awards_out->hideColumn(0);
    ui->tV_awards_out->resizeColumnsToContents();
}

void FRM_sendering::on_btn_add_selected_awards_clicked()
{
    QModelIndexList selectedrows = ui->tV_awards_in->selectionModel()->selectedRows();

    foreach (QModelIndex index, selectedrows)
    {
        //QMessageBox::information(this, QString::number(selectedrows.count()), ui->tV_awards_in->model()->index(index.row(), 5).data().toString());
        add_outgoing_award(index);
    }
    model_awards_in->setQuery(model_awards_in->query().lastQuery());
    ui->tV_awards_in->resizeColumnsToContents();
    model_awards_out->setQuery(model_awards_out->query().lastQuery());
    ui->tV_awards_out->resizeColumnsToContents();
}

void FRM_sendering::on_tV_awards_in_doubleClicked(const QModelIndex &index)
{
    add_outgoing_award(index);
    model_awards_in->setQuery(model_awards_in->query().lastQuery());
    ui->tV_awards_in->resizeColumnsToContents();
    model_awards_out->setQuery(model_awards_out->query().lastQuery());
    ui->tV_awards_out->resizeColumnsToContents();
}

//добавление наград в документ на выдачу
bool FRM_sendering::add_outgoing_award(QModelIndex index)
{
//объявление запроса на добавление наград в резерв
    QProgressBar *pb = new QProgressBar;
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
    pb->setMaximum(count_award);
    ::g_main_window->statusBar_set_ProgressBar("Резервирование наград", pb);
    for (int k = 0; k < count_award; k++)
    {
        if(!query_add_award.exec(QString("UPDATE awards SET outgoing_doc_id = %1\
            WHERE rowid IN (SELECT rowid FROM awards WHERE award_id = '%3' AND number = '%4'\
            AND storage = '%5' AND outgoing_doc_id IS NULL AND outgoing_date IS NULL LIMIT 1);")
            .arg(model_outgoing_doc->index(ui->cB_outgoing_doc->currentIndex(), 0).data().toString())
            .arg(ui->tV_awards_in->model()->index(index.row(), 0).data().toString())
            .arg(ui->tV_awards_in->model()->index(index.row(), 3).data().toString())
            .arg(ui->tV_awards_in->model()->index(index.row(), 5).data().toString())))
        {
            QMessageBox::warning(this, "Ошибка при выполнении запроса", query_add_award.lastError().databaseText());
            return false;
        }
    pb->setValue(k);
    }
    //model_awards_in->setQuery(model_awards_in->query().lastQuery());
    //model_awards_out->setQuery(model_awards_out->query().lastQuery());
    pb->hide();
    ::g_main_window->statusBar_show_message("Награды зарезервированы...");
    ::g_filter->set_btn_update_tree_info(false);
    return true;
}

void FRM_sendering::update_models()
{
    model_awards_in->setQuery(model_awards_in->query().lastQuery());
    model_awards_out->setQuery(model_awards_out->query().lastQuery());
    model_award_type->setQuery(model_award_type->query().lastQuery());
    model_outgoing_doc->setQuery(model_outgoing_doc->query().lastQuery());
}

//нажатие кнопки выдачи наград по документу
void FRM_sendering::on_bnt_outgoing_save_clicked()
{
    QSqlQuery query_save_award;
    QMessageBox::StandardButton reply;
    if(!model_outgoing_doc->index(ui->cB_outgoing_doc->currentIndex(), 0).data().toString().isEmpty())
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

void FRM_sendering::on_bnt_clean_clicked()
{
    QMessageBox::information(this, "Информация", "Когда нибудь здесь будет формировать реест, но на данный момент данная кнопка задействована для отката изменений в ходе демонстрации формы... ;-)");
    QSqlQuery query_rem_award;
    if(!model_outgoing_doc->index(ui->cB_outgoing_doc->currentIndex(), 0).data().toString().isEmpty())
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

void FRM_sendering::on_bnt_print_report_clicked()
{
    REPORT *report = new REPORT;
    report->load_outgoing_doc_report(get_outgoing_doc_details());
    if(ui->cB_outgoing_doc->currentIndex() >= 0)
    {
        QString filters("Текстовые файлы (*.doc);;Текстовые файлы (*.odt);;Веб файлы (*.html);;Все файлы (*.*)");
        QString defaultFilter("Текстовые файлы (*.doc)");
        QString defaultfilename(QString("/report_%1.doc").arg(QDate::currentDate().toString(Qt::ISODate)));
        QString file_name = QFileDialog::getSaveFileName(this, "Сохранить наряд", QDir::currentPath() + defaultfilename, filters, &defaultFilter);
        if(!file_name.isEmpty())\
        {
            QFile file(file_name);
            if(file.open(QIODevice::WriteOnly | QIODevice::Append))
            {
               file.write(report->load_outgoing_doc_report(get_outgoing_doc_details()).toUtf8());
               file.close();
            }
        }
    }
}

QStringList FRM_sendering::get_outgoing_doc_details()
    {
        QStringList document_details;
           QSqlQuery query_doc_details;
        if(query_doc_details.exec(QString("SELECT inputnumber AS \"№\", inputdate AS \"Дата отправки\",\
            (SELECT fullname FROM organizations WHERE id = sender_id) AS \"Отправитель\",\
            (SELECT fullname FROM organizations WHERE id = receiver_id) AS \"Получатель\"  FROM\
            (SELECT inputnumber, receiver_id, sender_id, inputdate\
            FROM documents WHERE awards_category = %1 AND type = 1 AND inputnumber = %2 LIMIT 1)\
            AS sql;")
                                          .arg(ui->cB_award_category->currentIndex())
                                          .arg(ui->cB_outgoing_doc->model()->index(
            ui->cB_outgoing_doc->currentIndex(), 0).data().toString())))
        {
            while (query_doc_details.next())
            {
                document_details.append(query_doc_details.value(0).toString());
                document_details.append(QDate(query_doc_details.value(1).toDate()).toString("\"d\" MMMM yyyy г."));
                document_details.append(query_doc_details.value(2).toString());
                document_details.append(query_doc_details.value(3).toString());
            }
        }
        return document_details;
    }

