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
    ui->cB_award_category->blockSignals(true);
    ui->cB_outgoing_doc->blockSignals(true);
    miliraty_awards_icon.addFile(":/resource/Images/За воинскую доблесть I степени.png");
    ui->cB_award_category->addItem(state_awards_icon, "Государственные награды");
    ui->cB_award_category->addItem(miliraty_awards_icon, "Ведомственные награды");
    //ui->cB_award_category->addItem("Иные награды");
    ui->cB_award_category->setCurrentIndex(-1);
    ui->cB_outgoing_doc->setCurrentIndex(-1);
    ui->cB_award_category->blockSignals(false);
    ui->cB_outgoing_doc->blockSignals(false);
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
    if(index != -1)
    {
        //ВНИМАНИЕ требуется оптимизация запроса, долго выполняется
//загрузка списка невыданных наград
        model_awards_in->setQuery(QString("SELECT DISTINCT TOP 100 a.award_id AS \"Идентификатор\", d_t.text AS \"Тип\",\
            d_a.text AS \"Название\", a.number AS \"№ награды\",\
            (SELECT COUNT(cnt.award_id) FROM awards AS cnt WHERE cnt.award_id = a.award_id AND cnt.number = a.number\
            AND cnt.storage = a.storage AND cnt.outgoing_doc_id IS NULL) AS \"Кол-во\",\
            a.storage AS \"Расположение\"\
            FROM awards AS a\
            LEFT JOIN (SELECT DISTINCT razdel, kod, text FROM SLOVAR WHERE razdel = 25) AS d_t ON (a.type_id = d_t.kod)\
            LEFT JOIN (SELECT DISTINCT razdel, kod, text FROM SLOVAR WHERE razdel = 1) AS d_a ON (a.award_id = d_a.kod)\
            WHERE category = %1 AND outgoing_doc_id IS NULL ORDER BY \"Тип\", \"Название\";")
            .arg(index));
            //.arg(ui->cB_award_category->model()->index(index, 0).data().toString()));
        ui->tV_awards_in->setModel(model_awards_in);
        ui->tV_awards_in->hideColumn(0);
        ui->tV_awards_in->resizeColumnsToContents();

//модель для выпадающего списка документов
        model_outgoing_doc->setQuery(QString("SELECT DISTINCT ID, '№ ' + CONVERT(nvarchar(50), inputnumber) + ' от '\
             + CONVERT(nvarchar(50), inputdate), inputnumber\
            FROM documents WHERE awards_category = %1 AND type = 1 ORDER BY inputnumber").arg(index));
        ui->cB_outgoing_doc->setModel(model_outgoing_doc);
        ui->cB_outgoing_doc->setModelColumn(1);
        ui->cB_outgoing_doc->setCurrentIndex(-1);
    }
}

void FRM_sendering::on_cB_outgoing_doc_currentIndexChanged(int index)
{
    model_awards_out->setQuery(QString("SELECT DISTINCT \"Идентификатор\", \"Тип\", \"Название\", \"№ награды\",\
                                       (SELECT COUNT(cnt.award_id) FROM awards AS cnt\
                                        WHERE (cnt.id = \"Идентификатор изделия\" AND outgoing_doc_id IS NULL)\
                                        OR\
                                        (cnt.award_id = \"Идентификатор\" AND cnt.number = \"№ награды\"\
                                        AND cnt.incoming_doc_id = \"Индекс документа получения на склад\"\
                                        AND cnt.outgoing_doc_id = \"Индекс резервирующего документа\"\
                                        AND cnt.storage = \"Расположение\")) AS  \"Кол-во\", \"Расположение\",\
                                        (SELECT COALESCE((SELECT 'не выдано' FROM awards AS no\
                                        WHERE no.id = \"Идентификатор изделия\" AND no.outgoing_date IS NULL),\
                                        (SELECT 'выдано' FROM awards AS yes WHERE yes.id = \"Идентификатор изделия\"\
                                        AND yes.outgoing_date IS NOT NULL), '')) AS  \"Статус\"\
                                        FROM\
                                        (SELECT '%1' AS \"Индекс резервирующего документа\",\
                                        a.ID AS \"Идентификатор изделия\", a.incoming_doc_id AS \"Индекс документа получения на склад\",\
                                        a.award_id AS \"Идентификатор\", d_t.text AS  \"Тип\", d_a.text AS \"Название\",\
                                        a.number AS \"№ награды\", a.storage AS  \"Расположение\"\
                                        FROM awards AS a\
                                        INNER JOIN (SELECT DISTINCT razdel, kod, text FROM SLOVAR WHERE razdel = 25)\
                                            AS d_t ON (a.type_id = d_t.kod)\
                                        INNER JOIN (SELECT DISTINCT razdel, kod, text FROM SLOVAR WHERE razdel = 1)\
                                            AS d_a ON (a.award_id = d_a.kod)\
                                        WHERE (EXISTS\
                                        (SELECT [klnagruk], [nomnagr] FROM OSNAGR AS gn\
                                        WHERE [klprotpr] = 1\
                                        AND [nomisx] = (SELECT CONVERT(nvarchar(5), 'рн ') + CONVERT(nvarchar(10), inputnumber)\
                                        FROM documents WHERE ID = '%1')\
                                        AND [datotpr]  = (SELECT CONVERT(nvarchar(10), inputdate, 104)\
                                        FROM documents WHERE ID = '%1') AND a.award_id = gn.klnagruk AND a.number = gn.nomnagr))\
                                        OR\
                                        (outgoing_doc_id = '%1')) AS sql;")
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
    QProgressBar *pb = new QProgressBar;
    pb->setMaximum(count_award);
    ::g_main_window->statusBar_set_ProgressBar("Резервирование наград", pb);
    for (int k = 0; k < count_award; k++)
    {
        if(!query_add_award.exec(QString("UPDATE awards SET outgoing_doc_id = '%1'\
            WHERE id IN (SELECT TOP 1 id FROM awards WHERE award_id = '%3' AND number = '%4'\
            AND storage = '%5' AND outgoing_doc_id IS NULL AND outgoing_date IS NULL);")
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
                WHERE outgoing_doc_id = '%2';")
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
            WHERE outgoing_doc_id = '%1';")
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
        if(query_doc_details.exec(QString("SELECT TOP 1 id AS \"Идентификатор\", inputnumber AS \"№\", inputdate AS \"Дата отправки\",\
                                          (SELECT text FROM SLOVAR WHERE razdel = 3 AND kod = sender_id) AS \"Отправитель\",\
                                          (SELECT text FROM SLOVAR WHERE razdel = 3 AND kod = receiver_id) AS \"Получатель\" FROM\
                                          (SELECT id, inputnumber, receiver_id, sender_id, inputdate\
                                          FROM documents WHERE awards_category = %1 AND type = 1 AND id = '%2') AS sql;")
                                          .arg(ui->cB_award_category->currentIndex())
                                          .arg(ui->cB_outgoing_doc->model()->index(
            ui->cB_outgoing_doc->currentIndex(), 0).data().toString())))
        {
            while (query_doc_details.next())
            {
                document_details.append(query_doc_details.value(0).toString());
                document_details.append(query_doc_details.value(1).toString());
                document_details.append(QDate(query_doc_details.value(2).toDate()).toString("\"d\" MMMM yyyy г."));
                document_details.append(query_doc_details.value(3).toString());
                document_details.append(query_doc_details.value(4).toString());
            }
        }
        return document_details;
    }

