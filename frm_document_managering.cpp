#include "frm_document_managering.h"
#include "ui_frm_document_managering.h"
#include "frm_receiwing.h"
#include "frm_sendering.h"
#include "mw.h"

int g_current_document_id;
extern MW *g_main_window;
extern FRM_receiwing *g_receiving;
extern FRM_sendering *g_sendering;
extern QCompleter* createCompleter(QSqlQueryModel* model);

FRM_document_managering::FRM_document_managering(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FRM_document_managering)
{
    ui->setupUi(this);
//уставновка текущей даты
    ui->de_inputdate->setDate(QDate::currentDate());
    ui->de_outputdate->setDate(QDate::currentDate());
//наполнение выпадающего списка
    ui->cB_type_operation->addItem("Получение наград");
    ui->cB_type_operation->addItem("Выдача наград");
//заполнение моделей данных
    model_organization->setQuery("SELECT DISTINCT \"Организация\" FROM(SELECT DISTINCT receiver AS \"Организация\" FROM documents\
        UNION ALL SELECT DISTINCT sender AS \"Организация\" FROM documents) ORDER BY \"Организация\";");
//установка комплитеров на текстовые поля
    ui->le_receiver_name->setCompleter(createCompleter(model_organization));
    ui->le_sender_name->setCompleter(createCompleter(model_organization));
    //connect(model_documents, SIGNAL(layoutChanged()), ui->tV_documents, SLOT(resizeColumnsToContents()));
}

FRM_document_managering::~FRM_document_managering()
{
    delete ui;
}

int FRM_document_managering::on_bnt_save_doc_clicked()
{
//объявление переменных
    QSqlQuery add_doc;
    QString add_doc_text;
    QMessageBox::StandardButton reply;
//валидация обязательных полей
    if(ui->le_inputnumber->text().isEmpty())
    {
        QMessageBox::warning(this, "Внимание!", "Обязательные поля не заполнены...");
        return 1;
    }
//подтерждение записи
    reply = QMessageBox::question(this, "Внимание!", "Документ будет добавлен в базу данных! Продолжить?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
    {
        return 1;
    }
    if (reply == QMessageBox::Yes)
    {
        if(ui->le_inputnumber->text().toInt() == ui->tV_documents->model()->index(0, 8).data().toInt())
        {
            add_doc_text = QString("UPDATE documents SET receiver = '%1', inputdate = '%3', sender = '%4',\
                outputnumber = '%5', outputdate = '%6', mail_type = '%7', direction = '%8'\
                WHERE inputnumber = '%2';")
                    .arg(ui->le_receiver_name->text())
                    .arg(ui->le_inputnumber->text())
                    .arg(ui->de_inputdate->date().toString(Qt::ISODate))
                    .arg(ui->le_sender_name->text())
                    .arg(ui->le_outputnumber->text())
                    .arg(ui->de_outputdate->date().toString(Qt::ISODate))
                    .arg(ui->le_mail_type->text())
                    .arg(ui->le_direction->text());
        }
        if(ui->le_inputnumber->text().toInt() != ui->tV_documents->model()->index(0, 8).data().toInt())
        {
            add_doc_text = QString("INSERT INTO documents (type, receiver,inputnumber, \
                inputdate, sender, outputnumber, outputdate, mail_type, direction)\
                VALUES ('%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                    .arg(ui->cB_type_operation->currentText())
                    .arg(ui->le_receiver_name->text())
                    .arg(ui->le_inputnumber->text())
                    .arg(ui->de_inputdate->date().toString(Qt::ISODate))
                    .arg(ui->le_sender_name->text())
                    .arg(ui->le_outputnumber->text())
                    .arg(ui->de_outputdate->date().toString(Qt::ISODate))
                    .arg(ui->le_mail_type->text())
                    .arg(ui->le_direction->text());
        }
        if (!add_doc.exec(add_doc_text))
        {
            QMessageBox::warning(this, "Ошибка выполнения запроса", add_doc.lastError().databaseText());
            qWarning() << add_doc.lastError().databaseText();
            return 1;
        }
    }
//обновление модели документов
    load_documents_table(ui->cB_type_operation->currentText());
    model_organization->setQuery(model_organization->query().lastQuery());
    ::g_receiving->update_models();
    ::g_sendering->update_models();
    ::g_main_window->statusBar_show_message("Документ успешно зарегистрирован...");
    return 0;
}

void FRM_document_managering::on_bnt_clear_doc_clicked()
{
    ui->le_receiver_name->clear();
    ui->le_inputnumber->setText(QString::number(model_documents->index(0, 8).data().toInt() + 1));
    ui->de_inputdate->setDate(QDate::currentDate());
    ui->le_sender_name->clear();
    ui->le_outputnumber->clear();
    ui->de_outputdate->setDate(QDate::currentDate());
    ui->le_mail_type->clear();
    ui->le_direction->clear();
    ui->bnt_save_doc->setText("Зарегистрировать");
}

void FRM_document_managering::on_cB_type_operation_currentTextChanged(const QString &arg1)
{
    load_documents_table(arg1);
}

void FRM_document_managering::load_documents_table(QString text)
{
    FRM_document_managering::on_bnt_clear_doc_clicked();
    model_documents->setQuery(QString("SELECT receiver AS \"Получатель\", inputnumber AS \"Входящий №\",\
        inputdate AS \"Дата вх.\", sender AS \"Отправитель\", outputnumber AS \"Исходящий №\",\
        outputdate AS \"Дата исх.\", mail_type AS \"Тип передачи\", direction AS \"Основание\",\
        (SELECT MAX(inputnumber) FROM documents WHERE type = '%1') AS \"Последний документ\"\
        FROM documents WHERE type = '%1' ORDER BY inputnumber;").arg(text));
    ui->tV_documents->setModel(model_documents);
    ui->tV_documents->hideColumn(8);
    //ui->tV_documents->resizeColumnsToContents();
    ui->le_inputnumber->setText(QString::number(ui->tV_documents->model()->index(0, 8).data().toInt() + 1));
}

void FRM_document_managering::on_tV_documents_doubleClicked(const QModelIndex &index)
{
    ::g_current_document_id = ui->tV_documents->model()->index(index.row(), 1).data().toInt();
    ui->le_receiver_name->setText(ui->tV_documents->model()->index(index.row(), 0).data().toString());
    ui->le_inputnumber->setText(ui->tV_documents->model()->index(index.row(), 1).data().toString());
    ui->de_inputdate->setDate(ui->tV_documents->model()->index(index.row(), 2).data().toDate());
    ui->le_sender_name->setText(ui->tV_documents->model()->index(index.row(), 3).data().toString());
    ui->le_outputnumber->setText(ui->tV_documents->model()->index(index.row(), 4).data().toString());
    ui->de_outputdate->setDate(ui->tV_documents->model()->index(index.row(), 5).data().toDate());
    ui->le_mail_type->setText(ui->tV_documents->model()->index(index.row(), 6).data().toString());
    ui->le_direction->setText(ui->tV_documents->model()->index(index.row(), 7).data().toString());
    ui->bnt_save_doc->setText("Обновить информацию");
}

void FRM_document_managering::models_update()
{
    model_documents->setQuery(model_documents->query().lastQuery());
    model_organization->setQuery(model_organization->query().lastQuery());
}

