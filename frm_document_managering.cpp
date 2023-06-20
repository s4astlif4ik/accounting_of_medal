#include "frm_document_managering.h"
#include "ui_frm_document_managering.h"
#include "frm_receiwing.h"
#include "frm_sendering.h"
#include "mw.h"
#include <QInputDialog>
#include <QMenu>
#include <QContextMenuEvent>


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
    QIcon state_awards_icon;
    state_awards_icon.addFile(":/resource/Images/Герой Российской Федерации.png");
    QIcon miliraty_awards_icon;
    miliraty_awards_icon.addFile(":/resource/Images/За воинскую доблесть I степени.png");
    ui->cB_awards_category->addItem(state_awards_icon, "Государственные награды");
    ui->cB_awards_category->addItem(miliraty_awards_icon, "Ведомственные награды");
    ui->cB_awards_category->setCurrentIndex(-1);
    QIcon receivering_awards;
    receivering_awards.addFile(":/img/db_comit.png");
    QIcon sendering_awards;
    sendering_awards.addFile(":/img/db_update.png");
    ui->cB_type_operation->addItem(receivering_awards, "Получение наград");
    ui->cB_type_operation->addItem(sendering_awards, "Выдача наград");
    ui->cB_type_operation->setCurrentIndex(-1);
//заполнение моделей данных
    model_organization->setQuery("SELECT TEXT AS \"Наименование\", KOD, SOKR AS \"Аббревиатура\"\
                                 FROM SLOVAR\
                                 WHERE RAZDEL = 3;");
//установка комплитеров на текстовые поля
    ui->le_receiver_name->setCompleter(createCompleter(model_organization));
    ui->le_sender_name->setCompleter(createCompleter(model_organization));
//задаем контекстное меню
    ui->tV_documents->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    //ui->tV_documents->setContextMenuPolicy(Qt::CustomContextMenu);
//определяем слоты для контекстного меню
    connect(ui->tV_documents->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));//вызов контекстного меню
}

void FRM_document_managering::slotCustomMenuRequested(QPoint pos)
{
    QMenu* menu = new QMenu(this);
    QAction* set_filter = new QAction(tr("Фильтровать"), this);
    connect(set_filter, SIGNAL(triggered()), this, SLOT(set_filter_from_table()));
    menu->addAction(set_filter);
    menu->popup(ui->tV_documents->horizontalHeader()->viewport()->mapToGlobal(pos));
}

void FRM_document_managering::on_cB_awards_category_currentIndexChanged(int /*index*/)
{
    if(!(ui->cB_awards_category->currentIndex() == -1 || ui->cB_type_operation->currentIndex() == -1))
    {
        load_documents_table(ui->cB_awards_category->currentIndex(), ui->cB_type_operation->currentIndex());
    }
}

void FRM_document_managering::on_cB_type_operation_currentIndexChanged(int /*index*/)
{
    if(!(ui->cB_awards_category->currentIndex() == -1 || ui->cB_type_operation->currentIndex() == -1))
    {
        load_documents_table(ui->cB_awards_category->currentIndex(), ui->cB_type_operation->currentIndex());
    }
}

int FRM_document_managering::find_organizations_id(QString fullname)
{
//    bool ok;
//    QString shortname;
//    QSqlQuery add_organizations;
//составляем список индексов с содержанием значения
    QModelIndexList indexes = model_organization->match(model_organization->index(0, 0),Qt::DisplayRole, fullname);
//если список пуст...
    if(indexes.count() == 0)
    {
////предлагаем добавить значение в классификатор организаций
//        fullname = QInputDialog::getText(this, "Указанная организация отсутствует в классификаторе!",
//            QString("Для привязки сокращенного наименования выберите полное наименование кадрового органа из классификатора..."),
//                                          QLineEdit::Normal, fullname, &ok);
////при положительном решение...
//        if(ok)
//        {
////если строка успешно создана...
//            if(add_organizations.exec(QString("UPDATE [dbo].[SLOVAR] SET [SOKR] = '%1' WHERE [RAZDEL] = 3 AND [TEXT] = '%2';")
//                .arg(shortname, fullname)))
//            {
////обновляем модель данных
//                model_organization->setQuery(model_organization->query().lastQuery());
////заново составляем список индексов
//                indexes = model_organization->match(model_organization->index(0, 0),Qt::DisplayRole, fullname);
//            }
//        }
        return 0;
    }
//если список наполнился...
    if(indexes.count() > 0)
    {
//записываем подсказку в текстовое поле
        //ui->le_receiver_name->setWhatsThis(model_organization->index(indexes.first().row(), 2).data().toString());
//возвращаем идентификатор выбранной организации
        return model_organization->index(indexes.first().row(), 1).data().toInt();
    }
//возвращаем пустой идентификатор если список пуст и пользователь отказался добавлять запись в классификатор
    return 0;
}

void FRM_document_managering::on_le_receiver_name_editingFinished()
{
    if (!ui->le_receiver_name->text().isEmpty())
    {
        if(find_organizations_id(ui->le_receiver_name->text()) == 0)
        {
            ui->le_receiver_name->clear();
        }
    }
}

void FRM_document_managering::on_le_sender_name_editingFinished()
{
    if (!ui->le_sender_name->text().isEmpty())
    {
        if(find_organizations_id(ui->le_sender_name->text()) == 0)
        {
            ui->le_sender_name->clear();
        }
    }
}

int FRM_document_managering::get_free_inputnumber()
{
    QString text_query;
    QSqlQuery free_num_query;
    text_query = QString("SELECT MAX(inputnumber) FROM documents AS d\
                                    WHERE awards_category = %1 AND type = %2;")
                .arg(ui->cB_awards_category->currentIndex()).arg(ui->cB_type_operation->currentIndex());
    if(free_num_query.exec(text_query))
    {
        while (free_num_query.next())
        {
            return free_num_query.value(0).toInt() + 1;
        }
    }
    return 0;
}

int FRM_document_managering::on_bnt_save_doc_clicked()
{
//объявление переменных
    QSqlQuery add_doc;
    QString add_doc_text;
    QMessageBox::StandardButton reply;
//валидация обязательных полей
    if(ui->le_receiver_name->text().isEmpty())
    {
        QMessageBox::warning(this, "Внимание!", "Не указан получатель...");
        return 1;
    }
    if(ui->le_sender_name->text().isEmpty())
    {
        QMessageBox::warning(this, "Внимание!", "Не указан отправитель...");
        return 1;
    }
    if(ui->le_outputnumber->text().isEmpty())
    {
        QMessageBox::warning(this, "Внимание!", "Не указан исходящий номер документа...");
        return 1;
    }
//подтерждение записи
    reply = QMessageBox::question(this, "Внимание!", "Документ будет добавлен в базу данных! Продолжить?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
    {
        return 1;
    }
    if (reply == QMessageBox::Yes)
    {
        if(ui->bnt_save_doc->text() == "Обновить информацию") /*ui->le_inputnumber->text().toInt() <= ui->tV_documents->model()->index(0, 8).data().toInt())*/
        {
            add_doc_text = QString("UPDATE documents SET receiver_id = %3, inputdate = '%5', sender_id = %6,\
                outputnumber = '%7', outputdate = '%8', mail_type = '%9', direction = '%10'\
                WHERE awards_category = %1 AND type = %2 AND inputnumber = '%4';")
                    .arg(ui->cB_awards_category->currentIndex())
                    .arg(ui->cB_type_operation->currentIndex())
                    .arg(find_organizations_id(ui->le_receiver_name->text()))
                    .arg(ui->le_inputnumber->text())
                    .arg(ui->de_inputdate->date().toString(Qt::ISODate))
                    .arg(find_organizations_id(ui->le_sender_name->text()))
                    .arg(ui->le_outputnumber->text())
                    .arg(ui->de_outputdate->date().toString(Qt::ISODate))
                    .arg(ui->le_mail_type->text())
                    .arg(ui->le_direction->text());
        }
        if(ui->bnt_save_doc->text() == "Зарегистрировать") /*ui->le_inputnumber->text().toInt() > ui->tV_documents->model()->index(0, 8).data().toInt())*/
        {
            add_doc_text = QString("INSERT INTO documents (awards_category, type, receiver_id, inputnumber, \
                inputdate, sender_id, outputnumber, outputdate, mail_type, direction)\
                VALUES (%1, %2, %3, '%4', '%5', %6, '%7', '%8', '%9', '%10');")
            .arg(ui->cB_awards_category->currentIndex())
            .arg(ui->cB_type_operation->currentIndex())
            .arg(find_organizations_id(ui->le_receiver_name->text()))
            .arg(ui->le_inputnumber->text())
            .arg(ui->de_inputdate->date().toString(Qt::ISODate))
            .arg(find_organizations_id(ui->le_sender_name->text()))
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
    on_bnt_clear_doc_clicked();
    load_documents_table(ui->cB_awards_category->currentIndex(), ui->cB_type_operation->currentIndex());
    model_organization->setQuery(model_organization->query().lastQuery());
    ::g_receiving->update_models();
    ::g_sendering->update_models();
    ::g_main_window->statusBar_show_message("Документ успешно зарегистрирован...");
    return 0;
}

void FRM_document_managering::on_bnt_clear_doc_clicked()
{
//    ui->cB_awards_category->setCurrentIndex(0);
//    ui->cB_type_operation->setCurrentIndex(0);
    ui->le_receiver_name->clear();
    ui->le_inputnumber->setText(QString::number(get_free_inputnumber()));
    ui->de_inputdate->setDate(QDate::currentDate());
    ui->le_sender_name->clear();
    ui->le_outputnumber->clear();
    ui->de_outputdate->setDate(QDate::currentDate());
    ui->le_mail_type->clear();
    ui->le_direction->clear();
    ui->bnt_save_doc->setText("Зарегистрировать");
//QAction *clear_le_award_name = ui->le_award_name->addAction(QIcon(":/img/editdelete.png"), QLineEdit::TrailingPosition);
    ui->tV_documents->horizontalHeader()->setCornerWidget(new QLineEdit(this));
}

void FRM_document_managering::on_tV_documents_doubleClicked(const QModelIndex &index)
{
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

void FRM_document_managering::load_documents_table(int awards_category, int type)
{
    QString doc_query = QString("SELECT\
                                (SELECT text FROM slovar WHERE RAZDEL = 3 AND KOD = d.receiver_id) AS \"Получатель\",\
                                inputnumber AS \"Входящий №\", inputdate AS \"Дата вх.\", (SELECT TEXT FROM SLOVAR\
                                WHERE RAZDEL = 3 AND KOD = d.sender_id) AS \"Отправитель\", outputnumber AS \"Исходящий №\",\
                                outputdate AS \"Дата исх.\", mail_type AS \"Тип передачи\", direction AS \"Основание\"\
                                FROM [documents] AS d WHERE awards_category = %1 AND type = %2 ORDER BY inputnumber;")
                    .arg(awards_category).arg(type);
    model_documents->setQuery(doc_query);
    ui->tV_documents->setModel(model_documents);
    ui->tV_documents->resizeColumnsToContents();
    ui->le_inputnumber->setText(QString::number(get_free_inputnumber()));
}

void FRM_document_managering::set_filter_from_table()
{
    QMessageBox::information(this, "", "вызов фильтра");
}

FRM_document_managering::~FRM_document_managering()
{
    delete ui;
}

