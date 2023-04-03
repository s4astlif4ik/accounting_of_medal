#include "frm_receiwing.h"
#include "ui_frm_receiwing.h"
#include "frm_document_managering.h"
#include "frm_sendering.h"
#include "mw.h"

extern QSqlDatabase db;
extern FRM_sendering *g_sendering;

//глобальная ссылка на форму получения наград
    FRM_receiwing *g_receiving;
//объявление глобальной ссылки на основную форму (статистика)
    extern MW *g_main_window;
//идентификатор типа награды
    int l_type_award_id;
//комплитер типа награды
    QCompleter *l_comp_type_award_id = new QCompleter();
//идентификатор награды
    int l_name_award_id;
//комплитер награды
    QCompleter *l_comp_name_award_id = new QCompleter();
//функция создания комплитеров
extern QCompleter* createCompleter(QSqlQueryModel* model);

FRM_receiwing::FRM_receiwing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FRM_receiwing)
{
    ui->setupUi(this);
    g_receiving = this;
//наполнение комбобокса типа наград
    ui->cB_award_category->addItem("Государственные награды");
    ui->cB_award_category->addItem("Ведомственные награды");
    ui->cB_award_category->addItem("Иные награды");
    ui->cB_award_category->setCurrentIndex(-1);
//выбор заполнения количества медалей по умолчанию
    ui->rb_sum->setChecked(true);
//установка валидатора на поле с количеством наград
    //ui->le_award_sum->setValidator(new QIntValidator(2, 500, this));
    ui->le_award_sum->setValidator(new QRegExpValidator(QRegExp("[1-9]\\d{0,2}"), this));
//установка валидатора на поля с номерами наград
    //ui->le_award_first_num->setValidator(new QIntValidator(2, 999999, this));
    ui->le_award_first_num->setValidator(new QRegExpValidator(QRegExp("^([1-9]\\d{0,5})"), this));
    //ui->le_award_last_num->setValidator(new QIntValidator(2,999999, this));
    ui->le_award_last_num->setValidator(new QRegExpValidator(QRegExp("^([1-9]\\d{0,5})"), this));
//наполнение моделей данных для выпадающих списков !!!
    model_incoming_doc->setQuery("SELECT DISTINCT inputnumber, '№ ' || inputnumber || ' от ' || inputdate\
        FROM documents WHERE type = 'Получение наград' ORDER BY inputnumber");
    //connect(model_awards, SIGNAL(layoutChanged()), ui->tV_awards, SLOT(resizeColumnsToContents()));
    model_type_award->setQuery("SELECT DISTINCT text, kod FROM distionary WHERE razdel = 25 ORDER BY text");
    model_name_award->setQuery("SELECT DISTINCT text, kod FROM distionary WHERE razdel = 1 ORDER BY text");
//модель для выпадающего списка документов
    ui->cB_doc->setModel(model_incoming_doc);
    ui->cB_doc->setModelColumn(1);
    ui->cB_doc->setCurrentIndex(-1);
//отправление текстового поля тип награды
    ::l_comp_type_award_id = createCompleter(model_type_award);
    ui->le_award_type->setCompleter(::l_comp_type_award_id);
    /*QAction **/act_award_type = ui->le_award_type->addAction(QIcon(":/resource/Images/combobox-icon.png"), QLineEdit::TrailingPosition);
    connect(act_award_type, SIGNAL(triggered()), this, SLOT(f_ls_for_award_type()));
    connect(::l_comp_type_award_id, QOverload<const QModelIndex &>::of(&QCompleter::activated),[=](const QModelIndex &index)
    {
        ::l_type_award_id = ::l_comp_type_award_id->completionModel()->index(index.row(), 1).data().toInt();
        act_award_type->setIcon(QIcon(":/img/button_ok.png"));
    });
    connect(act_award_type, SIGNAL(triggered()), this, SLOT(f_ls_for_award_type()));
//оформление текстового поля наименование награды
    ::l_comp_name_award_id = createCompleter(model_name_award);
    ui->le_award_name->setCompleter(::l_comp_name_award_id);
    /*QAction */act_award_name = ui->le_award_name->addAction(QIcon(":/resource/Images/combobox-icon.png"), QLineEdit::TrailingPosition);
    connect(act_award_name, SIGNAL(triggered(bool)), this, SLOT(f_ls_for_award_name()));
    connect(::l_comp_name_award_id, QOverload<const QModelIndex &>::of(&QCompleter::activated),[=](const QModelIndex &index)
    {
        ::l_name_award_id = ::l_comp_name_award_id->completionModel()->index(index.row(), 1).data().toInt();
        act_award_name->setIcon(QIcon(":/img/button_ok.png"));
    });
}

FRM_receiwing::~FRM_receiwing()
{
    delete ui;
}

void FRM_receiwing::on_le_award_name_returnPressed()
{
    if(ui->le_award_name->cursorPosition() == 0)
    {
        ui->le_award_name->completer()->setCompletionPrefix("");
        ui->le_award_name->completer()->complete();
    }
}

void FRM_receiwing::f_ls_for_award_type()
{
    ::l_comp_type_award_id->setCompletionPrefix("");
    ::l_comp_type_award_id->complete();
}

void FRM_receiwing::f_ls_for_award_name()
{
    ::l_comp_name_award_id->setCompletionPrefix("");
    ::l_comp_name_award_id->complete();
}

//void FRM_receiwing::on_le_award_name_editingFinished()
//{


//    int row;
//    QString key;
//    QModelIndex  index = ui->le_award_name->completer()->currentIndex();

//    if (index.isValid())
//    {
//        row = index.row();
//        key = comp->completionModel()->index(row, 0).data().toString();
//    }
//    QMessageBox::information(this, QString::number(row) + "/" + QString::number(ui->le_award_name->completer()->currentRow()), key);
//}

int FRM_receiwing::on_bnt_save_award_clicked()
{
//объявление переменной номера награды
    int award_number;
//объявление переменных
    QProgressBar *pb = new QProgressBar;
//диалоговое окно
    QMessageBox::StandardButton reply;
//проверка выбора документа
    if(ui->cB_doc->currentIndex() == -1)
    {
        QMessageBox::warning(this, "Внимание!", "Не выбран документ");
        return 1;
    }
//проверка выбора типа награды
    if(::l_type_award_id == 0)
    {
        QMessageBox::warning(this, "Внимание!", "Не указан тип награды");
        return 1;
    }
//валидация наименования награды
    if(ui->le_award_name->text().isEmpty())
    {
        QMessageBox::warning(this, "Внимание!", "Не указано наименование награды");
        return 1;
    }
//валидация места хранения награды
    if(ui->le_award_storage->text().isEmpty())
    {
        QMessageBox::warning(this, "Внимание!", "Не указано место хранения награды");
        return 1;
    }
//проверка номера награды
    if(ui->cB_award_category->currentIndex() == 0 && ui->le_award_first_num->text().isEmpty())
    {
        QMessageBox::warning(this, "Внимание!", "Не указан номер награды...");
        return 1;
    }
//отказ от записи
    reply = QMessageBox::question(this, "Внимание!", "Указанные награды будут добавлены на хранение! Продолжить?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
    {
        return 1;
    }
//подтверждение записи
    if (reply == QMessageBox::Yes)
    {
//цикл добавления определенного количества медалей
        db.transaction();
//определение количества наград
        int kk = ui->le_award_sum->text().toInt();
//вызов статус бара для индикации добавления наград
        ::g_main_window->statusBar_set_ProgressBar("Добавление медалей на хранение...", pb);
        pb->setMaximum(kk - 1);
//цикл добавления наград
        for (int k = 0; k < kk; k++)
        {
            pb->setValue(k);
//если указан номер награды
            if(ui->le_award_first_num->text().toInt() > 0)
            {
                award_number = ui->le_award_first_num->text().toInt() + k;
            }
//если номер награды не указан для наград без номера
            else
            {
                award_number = 0;
            }
//вызов функции добавления наград
            if(!add_awards(QString::number(award_number)))
            {
                db.rollback();
                QMessageBox::warning(this, "Внимание", "Произошла ошибка добавление медалей, процесс добавления будет остановлен!!!");
                return 1;
            }
        }
    }
//обновление модели данных дерева статистики
    pb->hide();
//закрытие транзакции
    db.commit();
//обновление модели наград на форме добавления
    model_awards->setQuery(model_awards->query().lastQuery());
    //ui->tV_awards->resizeColumnsToContents();
//обновление дерева статистики
    ::g_main_window->load_tree_view();
    ::g_sendering->update_models();
//очистка полей с номерами наград
    ui->le_award_first_num->clear();
    ui->le_award_last_num->clear();
    return 0;
}

bool FRM_receiwing::add_awards(QString awards_num)
{
//объявление запроса на добавление записей
    QSqlQuery add_award;
//проверка выполнения запроса на добавление записей
    if(!add_award.exec(QString("INSERT INTO awards (category, type_id, award_id, number, storage, note, incoming_doc_id)\
        VALUES  ('%1', %2, %3, '%4', '%5', '%6', %7);")
            .arg(ui->cB_award_category->currentText())
            .arg(::l_type_award_id)
            .arg(::l_name_award_id)
            .arg(awards_num)
            .arg(ui->le_award_storage->text())
            .arg(ui->le_award_note->text())
            .arg(ui->cB_doc->model()->index(ui->cB_doc->currentIndex(), 0).data().toString())))
    {
//раздел если запрос не выполнен
        QMessageBox::warning(this, "Ошибка выполнения запроса", add_award.lastError().databaseText());
        qWarning() << add_award.lastError().databaseText();
        return false;
    }
//возврат если функция выполнила запрос
    return true;
}

void FRM_receiwing::on_bnt_clear_award_clicked()
{
    ui->cB_award_category->setCurrentIndex(-1);
    ui->le_award_type->clear();
    ui->le_award_name->clear();
    ui->le_award_first_num->clear();
    ui->le_award_last_num->clear();
    ui->le_award_sum->setText("1");
    ui->le_award_storage->clear();
    ui->le_award_note->clear();
}

void FRM_receiwing::on_le_award_type_editingFinished()
{
    if (::l_type_award_id == 0)
    {
        ui->le_award_type->setText("");
    }
}

void FRM_receiwing::on_le_award_name_editingFinished()
{
    if (::l_name_award_id == 0)
    {
        ui->le_award_name->setText("");
    }
}

void FRM_receiwing::on_cB_doc_currentIndexChanged(int index)
{
    FRM_receiwing::on_bnt_clear_award_clicked();
    model_awards->setQuery(QString("SELECT DISTINCT a.category AS \"Категория\", d_t.text AS \"Тип\",\
        d_a.text AS \"Название\", a.number AS \"№ награды\", (SELECT COUNT(count.award_id) FROM awards AS count\
        WHERE count.award_id = a.award_id AND count.number = a.number AND count.incoming_doc_id = a.incoming_doc_id) AS \"Кол-во\",\
        (SELECT GROUP_CONCAT(DISTINCT aw.storage) FROM awards AS aw\
        WHERE aw.award_id = a.award_id AND aw.number = a.number AND aw.incoming_doc_id = a.incoming_doc_id) AS \"Расположение\"\
        FROM awards AS a\
        INNER JOIN (SELECT DISTINCT razdel, kod, text FROM distionary WHERE razdel = 25) AS d_t ON (a.type_id = d_t.kod)\
        INNER JOIN (SELECT DISTINCT razdel, kod, text FROM distionary WHERE razdel = 1) AS d_a ON (a.award_id = d_a.kod)\
        WHERE incoming_doc_id = %1 ORDER BY \"Категория\", \"Тип\", \"Название\";")
        .arg(ui->cB_doc->model()->index(index, 0).data().toString()));
    ui->tV_awards->setModel(model_awards);
    ui->tV_awards->resizeColumnsToContents();
}

void FRM_receiwing::update_models()
{
    model_awards->setQuery(model_awards->query().lastQuery());
    model_type_award->setQuery(model_type_award->query().lastQuery());
    model_name_award->setQuery(model_name_award->query().lastQuery());
    model_incoming_doc->setQuery(model_incoming_doc->query().lastQuery());
}

void FRM_receiwing::on_rb_numbers_toggled(bool checked)
{
    if(ui->cB_award_category->currentIndex() == 0)
    {
//активация полей с номерами при выборе государственных наград
        ui->rb_numbers->setChecked(true);
    }
//в других случаях активация поля с количеством
    else
    {
        if(!checked)
        {
            ui->le_award_first_num->setEnabled(false);
            ui->le_award_first_num->clear();
            ui->le_award_last_num->setEnabled(false);
            ui->le_award_last_num->clear();
            ui->le_award_sum->setEnabled(true);
            ui->le_award_sum->setText("1");
        }
    }
//если выбран раздел с номерами наград
    if(checked)
    {
        ui->le_award_first_num->setEnabled(true);
        ui->le_award_last_num->setEnabled(true);
        ui->le_award_sum->setEnabled(false);
    }
}


void FRM_receiwing::on_cB_award_category_currentIndexChanged(int index)
{
//активация полей с номерами наград при выборе государственных наград
    if(index == 0)
    {
        ui->rb_numbers->setChecked(true);
    }
}

void FRM_receiwing::on_le_award_first_num_editingFinished()
{
    if(ui->le_award_first_num->text().toInt() < ui->le_award_last_num->text().toInt())
    {
        ui->le_award_sum->setText(QString::number(ui->le_award_last_num->text().toInt() - ui->le_award_first_num->text().toInt() + 1));
    }
    if(ui->le_award_first_num->text().toInt() > ui->le_award_last_num->text().toInt())
    {
        ui->le_award_last_num->clear();
        ::g_main_window->statusBar_show_message("Нарушение диапазона нумерации наград...");
    }
}

void FRM_receiwing::on_le_award_last_num_editingFinished()
{
    if(ui->le_award_first_num->text().toInt() < ui->le_award_last_num->text().toInt())
    {
        ui->le_award_sum->setText(QString::number(ui->le_award_last_num->text().toInt() - ui->le_award_first_num->text().toInt() + 1));
    }
    if(ui->le_award_first_num->text().toInt() > ui->le_award_last_num->text().toInt())
    {
        ui->le_award_last_num->clear();
        ::g_main_window->statusBar_show_message("Нарушение диапазона нумерации наград...");
    }
}
