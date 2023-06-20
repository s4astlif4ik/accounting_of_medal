#include "frm_receiwing.h"
#include "ui_frm_receiwing.h"
#include "frm_document_managering.h"
#include "frm_sendering.h"
#include "mw.h"
#include "frm_filter.h"
#include "dlg_report.h"

#include <QDebug>

//ссылка на базу данных
extern QSqlDatabase db;
//ссылка на основную форму (статистика)
extern MW *g_main_window;
extern FRM_filter *g_filter;
//глобальная ссылка на форму получения наград
FRM_receiwing *g_receiving;
//ссылка на форму выдачи документов
extern FRM_sendering *g_sendering;
//идентификатор типа награды
int l_name_award_id(0);
//комплитер награды
QCompleter *l_comp_name_award_id = new QCompleter();
//создание комплитера
extern QCompleter* createCompleter(QSqlQueryModel* model);
//комплитер с моделью для фильтрации
QCompleter* createCompleter_filter(QSortFilterProxyModel* model)
{
    QCompleter* completer = new QCompleter();
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setModel(model);
    completer->setCompletionColumn(0);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    return completer;
}

FRM_receiwing::FRM_receiwing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FRM_receiwing)
{
    ui->setupUi(this);
//получение глобальной ссылки на текущую форму
    ::g_receiving = this;
//наполнение выпадающего списка
    QIcon state_awards_icon;
    state_awards_icon.addFile(":/resource/Images/Герой Российской Федерации.png");
    QIcon miliraty_awards_icon;
    miliraty_awards_icon.addFile(":/resource/Images/За воинскую доблесть I степени.png");
    ui->cB_award_category->addItem(state_awards_icon, "Государственные награды");
    ui->cB_award_category->addItem(miliraty_awards_icon, "Ведомственные награды");
    //ui->cB_award_category->addItem("Иные награды");
    ui->cB_award_category->setCurrentIndex(0);
//установка валидатора на поле с количеством наград
    ui->le_award_count->setValidator(new QRegExpValidator(QRegExp("[1-9]\\d{0,2}"), this));
//блокировка выпадающего списка тип награды
    //ui->cB_award_type->setEnabled(false);
//блокировка текстового поля наименование награды
    ui->le_award_name->setEnabled(false);
//создание локальной ссылки на комплитер тип награды
    proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(model_name_award);
    proxy->setFilterCaseSensitivity(Qt::CaseSensitive); //определяет тип фильтрации
    proxy->setFilterKeyColumn(0);
    proxy->setRecursiveFilteringEnabled(true);
//оформление текстового поля наименование награды
    combobox_icon = QIcon(":/resource/Images/combobox-icon.png");
    button_ok = QIcon(":/img/button_ok.png");
    ::l_comp_name_award_id = createCompleter_filter(proxy);
    ui->le_award_name->setCompleter(::l_comp_name_award_id);
    QAction *clear_le_award_name = ui->le_award_name->addAction(QIcon(":/img/editdelete.png"), QLineEdit::TrailingPosition);
    act_award_name = ui->le_award_name->addAction(combobox_icon, QLineEdit::TrailingPosition);
    connect(clear_le_award_name, SIGNAL(triggered(bool)), this, SLOT(clear_le_award_name()));
    connect(act_award_name, SIGNAL(triggered(bool)), this, SLOT(f_ls_for_award_name()));
    connect(::l_comp_name_award_id, QOverload<const QModelIndex &>::of(&QCompleter::activated),[=](const QModelIndex &index)
    {
        ::l_name_award_id = ::l_comp_name_award_id->completionModel()->index(index.row(), 1).data().toInt();
        act_award_name->setIcon(button_ok);
        ui->le_award_name->setReadOnly(true);
    });
    //qDebug("Загрузка формы получения документов: %d ms", t.elapsed());
}

FRM_receiwing::~FRM_receiwing()
{
    delete ui;
}

void FRM_receiwing::on_le_award_name_returnPressed() //открытие выпадающего списка наименований наград при нажатии "Enter"
{
    if(ui->le_award_name->cursorPosition() == 0)
    {
        ui->le_award_name->completer()->setCompletionPrefix("");
        ui->le_award_name->completer()->complete();
    }
}

void FRM_receiwing::f_ls_for_award_name()
{
    ::l_comp_name_award_id->setCompletionPrefix("");
    ::l_comp_name_award_id->complete();
}

int FRM_receiwing::on_bnt_save_award_clicked()
{
//объявление переменной номера награды
    QString award_number;
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
//    if(ui->cB_award_category->currentIndex() == 0 && ui->le_award_first_num->text().isEmpty())
//    {
//        QMessageBox::warning(this, "Внимание!", "Не указан номер награды...");
//        return 1;
//    }
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
        //db.transaction();
//определение количества наград
        int kk = ui->le_award_count->text().toInt();
//вызов статус бара для индикации добавления наград
        ::g_main_window->statusBar_set_ProgressBar("Добавление медалей на хранение...", pb);
        pb->setMaximum(kk - 1);
//цикл добавления наград
        for (int k = 0; k < kk; k++)
        {
            pb->setValue(k);
//если указан номер награды
            if(!ui->le_award_first_num->text().isEmpty()) //если указан начальный номер
            {
                award_number = QString("%1%2")
                    .arg(ui->le_award_first_num->text().left(ui->le_award_first_num->text().length() - 3))
                    .arg(ui->le_award_first_num->text().right(3).toInt() + k);
            }
//если номер награды не указан для наград без номера
            else //если начальный номер не указан
            {
                award_number = "-";
            }
//вызов функции добавления наград
            if(!add_awards(award_number))
            {
                //db.rollback();
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
    model_storage->setQuery(model_storage->query().lastQuery());
//выравнивание столбцов таблицы под содержимое
    ui->tV_awards->resizeColumnsToContents();
//обновление дерева статистики

    ::g_filter->set_btn_update_tree_info(false);
//    ::g_main_window->load_tree_view();
    ::g_sendering->update_models();
//очистка полей с номерами наград
    ui->le_award_first_num->clear();
    ui->le_award_last_num->clear();
    return 0;
}

bool FRM_receiwing::add_awards(QString awards_number)
{
//объявление запроса на добавление записей
    QSqlQuery add_award;
//проверка выполнения запроса на добавление записей
    if(!add_award.exec(QString("INSERT INTO awards (category, type_id, award_id, number, storage, note, incoming_doc_id)\
        VALUES  (%1, %2, %3, '%4', '%5', '%6', '%7');")
            .arg(ui->cB_award_category->currentIndex())
            .arg(ui->cB_award_type->model()->index(ui->cB_award_type->currentIndex(), 1).data().toString())
            .arg(::l_name_award_id)
            .arg(awards_number)
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

void FRM_receiwing::clear_le_award_name()
{
    ui->le_award_name->setReadOnly(false);
    ui->le_award_name->clear();
    ui->le_award_storage->clear();
    //act_award_name->setIcon(combobox_icon);
    ::l_name_award_id = 0;
}

void FRM_receiwing::on_bnt_clear_award_clicked()
{
    if(::l_name_award_id != 0)
    {
        clear_le_award_name();
    }
    ui->cB_award_category->setCurrentIndex(0);
    ui->le_award_first_num->clear();
    ui->le_award_last_num->clear();
    ui->le_award_count->setText("1");
    ui->le_award_storage->clear();
    ui->le_award_note->clear();
}

void FRM_receiwing::on_cB_doc_currentIndexChanged(int index)
{
//очистка формы при выборе другого документа
    //FRM_receiwing::on_bnt_clear_award_clicked();
//    model_awards->setQuery(QString("SELECT DISTINCT '%1' AS \"Категория\", d_t.text AS \"Тип\",\
//                                   d_a.text AS \"Название\", a.number AS \"№ награды\", (SELECT COUNT(count.award_id) FROM awards AS count\
//                                   WHERE count.award_id = a.award_id AND count.number = a.number AND count.category = a.category\
//                                   AND count.incoming_doc_id = a.incoming_doc_id) AS \"Кол-во\",\
//                                   (SELECT DISTINCT GROUP_CONCAT(aw.storage, ', ') FROM awards AS aw\
//                                   WHERE aw.award_id = a.award_id AND aw.number = a.number AND aw.category = a.category\
//                                   AND aw.incoming_doc_id = a.incoming_doc_id) AS \"Расположение\"\
//                                   FROM awards AS a\
//                                   INNER JOIN (SELECT DISTINCT razdel, kod, text FROM SLOVAR WHERE razdel = 25) AS d_t ON\
//                                   (a.type_id = d_t.kod)\
//                                   INNER JOIN (SELECT DISTINCT razdel, kod, text FROM SLOVAR WHERE razdel = 1) AS d_a ON\
//                                   (a.award_id = d_a.kod)\
//                                   WHERE category = %2 AND incoming_doc_id = %3 ORDER BY \"Категория\", \"Тип\", \"Название\";")

    model_awards->setQuery(QString("SELECT DISTINCT '%1' AS \"Категория\", d_t.text AS \"Тип\",\
                                   d_a.text AS \"Название\", a.number AS \"№ награды\", (SELECT COUNT(count.award_id) FROM awards AS count\
                                   WHERE count.award_id = a.award_id AND count.number = a.number AND count.category = a.category\
                                   AND count.incoming_doc_id = a.incoming_doc_id) AS \"Кол-во\",\
                                   STUFF(\
                                   (SELECT DISTINCT ', ' + aw.storage FROM awards AS aw\
                                   WHERE aw.award_id = a.award_id AND aw.number = a.number AND aw.category = a.category\
                                   AND aw.incoming_doc_id = a.incoming_doc_id FOR XML PATH('')), 1, 1, '') AS \"Расположение\"\
                                   FROM awards AS a\
                                   INNER JOIN (SELECT DISTINCT razdel, kod, text FROM SLOVAR WHERE razdel = 25) AS d_t ON\
                                   (a.type_id = d_t.kod)\
                                   INNER JOIN (SELECT DISTINCT razdel, kod, text FROM SLOVAR WHERE razdel = 1) AS d_a ON\
                                   (a.award_id = d_a.kod)\
                                   WHERE category = %2 AND incoming_doc_id = '%3' ORDER BY \"Категория\", \"Тип\", \"Название\";")
    .arg(ui->cB_award_category->currentText())
    .arg(QString::number(ui->cB_award_category->currentIndex()))
    .arg(ui->cB_doc->model()->index(index, 0).data().toString()));
    ui->tV_awards->setModel(model_awards);
    ui->tV_awards->resizeColumnsToContents();
//наполнение модели данных выпадающего списка типов наград
//    ui->cB_award_type->setEnabled(true);
    model_type_award->setQuery("SELECT DISTINCT text, kod FROM SLOVAR WHERE razdel = 25 ORDER BY text;");
    ui->cB_award_type->setModel(model_type_award);
    ui->cB_award_type->setModelColumn(0);
    ui->cB_award_type->setCurrentIndex(-1);
//разблокировка выпадающего списка с категорией награды

//установка комплитера для поля место хранения
    model_storage->setQuery("SELECT DISTINCT storage FROM awards ORDER BY storage;");
    ui->le_award_storage->setCompleter(createCompleter(model_storage));

//сброс категории награды
    //ui->cB_award_category->setCurrentIndex(-1);
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
            ui->le_award_count->setEnabled(true);
            ui->le_award_count->setText("1");
        }
    }
//если выбран раздел с номерами наград
    if(checked)
    {
        ui->le_award_first_num->setEnabled(true);
        ui->le_award_last_num->setEnabled(true);
        ui->le_award_count->setEnabled(false);
    }
}

void FRM_receiwing::on_cB_award_category_currentIndexChanged(int index)
{

//при выборе категории получаемых наград загружаем модель документов в выпадающий список
//    model_incoming_doc->setQuery(QString("SELECT DISTINCT inputnumber, '№ ' || inputnumber || ' от ' || inputdate\
//        FROM documents WHERE type = 0 AND awards_category = %1 ORDER BY inputnumber;").arg(index));
    model_incoming_doc->setQuery(QString("SELECT DISTINCT ID, '№ ' + CONVERT(nvarchar(10),  inputnumber) + ' от ' + CONVERT(nvarchar(10), inputdate),\
        inputnumber FROM documents WHERE type = 0 AND awards_category = %1 ORDER BY inputnumber;").arg(index));
    ui->cB_doc->setModel(model_incoming_doc);
    ui->cB_doc->setModelColumn(1);
    //ui->cB_doc->blockSignals(false);
    ui->cB_doc->setCurrentIndex(-1);
//выбран раздел государственные награды
    if(index == 0)
    {
        ui->rb_numbers->setChecked(true);
        model_name_award->setQuery(QString("SELECT DISTINCT text, kod FROM SLOVAR WHERE razdel = 1 AND kod < 8000 ORDER BY text;"));
        //ui->cB_award_type->setEnabled(true);
    }
//выбран раздел ведомственные награды
    if(index == 1)
    {
        ui->rb_count->setChecked(true);
        model_name_award->setQuery(QString("SELECT DISTINCT text, kod FROM SLOVAR WHERE razdel = 1 AND kod > 8000 ORDER BY text"));
        //ui->cB_award_type->setEnabled(true);
    }
}

void FRM_receiwing::on_le_award_first_num_editingFinished()
{
//установка условия по оформлению конечного номера награды
    QString validator_content;

    for(int k = 0; k < ui->le_award_first_num->text().length() - 4; k++)
    {
        validator_content.append(QString("[%1]").arg(ui->le_award_first_num->text().at(k)));
    }
    //validator_content.append("\\d{3}");
    //validator_content.append("[0-9][0-9][0-9]");
    validator_content.append("[0-9][0-9][0-9][0-9]");
    ui->le_award_last_num->setValidator(new QRegExpValidator(QRegExp(validator_content)));
//здесь будет функция по подсчету наград
        ui->le_award_count->setText(QString::number(awards_count()));
}

void FRM_receiwing::on_le_award_last_num_textChanged(const QString &/*arg1*/)
{
    ui->le_award_count->setText(QString::number(awards_count()));
}

int FRM_receiwing::awards_count()
{
    if(!ui->le_award_first_num->text().isEmpty())
    {
        if(!ui->le_award_last_num->text().isEmpty())
        {
            if(ui->le_award_first_num->text().length() == ui->le_award_last_num->text().length())
            {
                int a = ui->le_award_first_num->text().right(3).toInt();
                int b = ui->le_award_last_num->text().right(3).toInt();
                if(a < b)
                {
                    return b - a + 1;
                }
            }
        }
    }
    if(ui->le_award_first_num->text().isEmpty())
    {
        ui->le_award_last_num->text().clear();
    }
    return 1;
}

void FRM_receiwing::on_cB_award_type_currentIndexChanged(int index) //выбор выпадающего списка тип награды
{
    if(index == -1)
    {
        clear_le_award_name();
    }
    if(index >= 0)
    {
        ui->le_award_name->setEnabled(true);
        //proxy->setFilterFixedString(ui->cB_award_type->currentText().left(5));
    }
}

int FRM_receiwing::on_bnt_print_report_clicked()
{
    DLG_report report;
    report.setModal(true);
    report.exec();
    //::g_report->print_report();
    //REPORT::print_report();
//    if(!ui->tV_awards->model()) return 1;

//    QString strStream;
//    QTextStream out(&strStream);

//    const int rowCount = ui->tV_awards->model()->rowCount();
//    const int columnCount = ui->tV_awards->model()->columnCount();
////установка разметки и кодировки документа
//    out << "<html>\n" << "<head>\n" << "meta Content=\"Text/html;charset=Windows-1251\n" <<
//           QString("<title>%1</title>\n").arg("Report") <<
//           "</head>\n"
//           "<body bgcolor = #ffffff link=#500A0>\n"
//           "<table border=1 cellspacing=0 cellpading=2>\n";

//    out << "<thead><tr bgcolor=#f0f0f0>";
//    for(int column = 0; column < columnCount; column++)
//    {
//        if(!ui->tV_awards->isColumnHidden(column)) out << QString("<th>%1</th>").arg(ui->tV_awards->model()->headerData(column, Qt::Horizontal).toString());
//    }
//    out << "</tr></thead>\n";

//    for(int row = 0; row < rowCount; row++)
//    {
//        out << "<tr>";
//        for(int column = 0; column < columnCount; column++)
//        {
//            if(!ui->tV_awards->isColumnHidden(column))
//            {
//                QString data = ui->tV_awards->model()->data(ui->tV_awards->model()->index(row, column)).toString().simplified();
//                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty())? data:QString("&nbsp;"));
//            }
//        }
//        out << "</tr>\n";
//    }
//    out << "</table\n>""</body>\n""</html>\n";

//    QTextDocument* document = new QTextDocument();
//    document->setHtml(strStream);
//    QTextBrowser *browser = new QTextBrowser;
//    browser->setText(strStream);
//    browser->show();
//    QPrinter printer;

//    QPrintDialog *dialog = new QPrintDialog(&printer, 0);
//    if(dialog->exec() == QDialog::Accepted)
//    {
//        document->print(&printer);
//        browser->hide();
//    }
//    delete document;
//    ::g_main_window->statusBar_show_message("Отчет сформирован");
    return 0;
}


