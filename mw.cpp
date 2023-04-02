#include "mw.h"
#include "ui_mw.h"
#include "frm_document_managering.h"
#include "frm_receiwing.h"
#include "frm_sendering.h"
#include "dlg_receiving.h"
#include "frm_filter.h"

#include <QMessageBox>
#include <QLabel>
#include <QList>
#include <QDateEdit>
#include <QLineEdit>


MW *g_main_window; //ссылка на главную форму
QSortFilterProxyModel *g_proxy; //ссылка на прокси модель статистики
QTreeView *g_tree;
extern QLineEdit *g_le_filter_text;
extern QDateEdit *g_firstdate_filter;
extern QDateEdit *g_lastdate_filter;

QCompleter* createCompleter(QSqlQueryModel* model)
{
    QCompleter* completer = new QCompleter();
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setModel(model);
    completer->setCompletionColumn(0);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    return completer;
}

MW::MW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MW)
{
    ui->setupUi(this);
    ::g_main_window = this;
    ::g_tree = ui->trw_statistic;
    //sb = ui->statusbar;
    filter_form =new FRM_filter;
    ui->toolBar->addWidget(filter_form);
//главная
    QMenu *main = new QMenu("Главная");
    if(ui->menubar->addMenu(main)) menu0 = main;
    QAction *doc_managering = new QAction("Управление документами");
    doc_managering->setCheckable(true);
    doc_managering->setChecked(false);
    main->addAction(doc_managering);
    QAction *receiving = new QAction("Получение наград");
    receiving->setCheckable(true);
    receiving->setChecked(false);
    main->addAction(receiving);
    QAction *extradition = new QAction("Выдача наград");
    extradition->setCheckable(true);
    extradition->setChecked(false);
    main->addAction(extradition);
//фильтр
    QAction *filter = new QAction("Статистика");
    ui->menubar->addAction(filter);
//настройки
    QAction *settings = new QAction("Настройки");
    ui->menubar->addAction(settings);
//настройки
    QAction *about = new QAction("О программе");
    ui->menubar->addAction(about);
//объявления сигналов
    connect(filter, SIGNAL(triggered()), this, SLOT(show_dW_filter()));
    connect(doc_managering, SIGNAL(triggered()), this, SLOT(show_dW_doc_managering()));
    connect(receiving, SIGNAL(triggered()), this, SLOT(show_dW_receiving()));
    connect(extradition, SIGNAL(triggered()), this, SLOT(show_dW_extradition()));
//установка контекстного меню на таблицу статистики
    ui->trw_statistic->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->trw_statistic, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
//скрытие докера получения наград
    ui->dW_doc_managering->hide();
//создание ссылки на виджет получения наград
    QWidget *doc_managering_form = new FRM_document_managering;
    ui->dW_doc_managering->setWidget(doc_managering_form);
//скрытие докера получения наград
    ui->dW_receiving->hide();
//создание ссылки на виджет получения наград
    QWidget *receiving_form = new FRM_receiwing;
    ui->dW_receiving->setWidget(receiving_form);
//скрытие докера получения наград
    ui->dW_extradition->hide();
//создание ссылки на виджет получения наград
    QWidget *extradition_form = new FRM_sendering;
    ui->dW_extradition->setWidget(extradition_form);
//скрытие докера с фильтрами
    ui->dW_filter->hide();
//отображение заголовка
    ui->trw_statistic->header()->setVisible(true);
//загрузка таблицы статистики
    load_tree_view();
    //connect(sim, SIGNAL(layoutChanged()), this, SLOT(resize_to_content()));
}

bool MW::load_tree_view()
{
    QProgressBar *pb = new QProgressBar();
    sim->setColumnCount(6);//задаем таблице 5 столбцов
    QIcon icon_sub_item = QIcon();
    QIcon standard_icon_item = this->windowIcon();
    QPixmap outpixmap;
    QSqlQuery query_pb_max;//запрос на вывод ведомств
    QSqlQuery query_root_item;//запрос на вывод ведомств
    QSqlQuery query_item;//запрос на вывод типов наград
    QSqlQuery query_sub_item;//запрос на вывод медалей по типам наград
    //QList<QStandardItem*> ls_root_item;//список элементов верхнего уровня
    //QList<QStandardItem*> ls_item;//списов элементов среднего уровня
    //QList<QStandardItem*> ls_sub_item;//список элементов среднего уровня
    int root_k = 0;//индекс для счетчика строки верхнего уровня
    int sub_k = 0;
    //icon_sub_item = this->windowIcon();//QIcon.addFile("./resource/Images/guk.ico");//добавление иконки на медаль
//запрос на вывод количества наименований медалей для прогресс бара
    if (query_pb_max.exec("SELECT COUNT (DISTINCT award_id) FROM awards LIMIT 1;"))
    {
        while (query_pb_max.next())
        {
            pb->setMaximum(query_pb_max.value(0).toInt() - 1);
            ::g_main_window->statusBar_set_ProgressBar("Обновление таблицы статистики...", pb);
        }
    }
//запрос на вывод строк верхнего уровня
    if (query_root_item.exec("SELECT DISTINCT category FROM awards;"))
    {
        while (query_root_item.next())
        {
            QString category = query_root_item.value(0).toString();
            QStandardItem *root_item = new QStandardItem(category);
//установка стиля элементов верхнего уровня
            root_item->setFont(QFont("Times New Roman", 11, 10, true));
            //root_item->setBackground(QBrush(QColor(255,155,155)));
            root_item->setForeground(QBrush(QColor(255, 50, 50)));
//вывод типов наград
            query_item.setForwardOnly(true);
            if (query_item.exec(QString("SELECT DISTINCT a.type_id, d.text FROM awards AS a\
                INNER JOIN (SELECT DISTINCT kod, text FROM distionary WHERE razdel = 25) as d ON (d.kod = a.type_id)\
                INNER JOIN (SELECT inputnumber, inputdate FROM documents) AS d ON (a.incoming_doc_id = d.inputnumber)\
                WHERE category = '%1';")
                .arg(category)))
            {
                while (query_item.next())
                {
//сохранение типа текущей награды для запроса
                    int type_id = query_item.value(0).toInt();
//создание списка элементов строки среднего уровня
                    QList<QStandardItem*> ls_item;
                    //QStandardItem *item = new QStandardItem(type);
//присвоение значений строке элементов среднего уровня
                    auto item_0 = new QStandardItem(query_item.value(1).toString());
                    auto item_1 = new QStandardItem("");
                    auto item_2 = new QStandardItem("");
                    auto item_3 = new QStandardItem("");
                    auto item_4 = new QStandardItem("");
                    auto item_5 = new QStandardItem("");
//определения стиля элементов строки среднего уровня
                    item_1->setTextAlignment(Qt::AlignCenter);
                    item_2->setTextAlignment(Qt::AlignCenter);
                    item_3->setTextAlignment(Qt::AlignCenter);
                    item_4->setTextAlignment(Qt::AlignCenter);
                    item_0->setFont(QFont("Times New Roman", 11, 10, true));
                    item_1->setFont(QFont("Times New Roman", 11, 10, true));
                    item_2->setFont(QFont("Times New Roman", 11, 10, true));
                    item_3->setFont(QFont("Times New Roman", 11, 10, true));
                    item_4->setFont(QFont("Times New Roman", 11, 10, true));
                    item_5->setFont(QFont("Times New Roman", 11, 10, true));
                    //root_item->setBackground(QBrush(QColor(255,155,155)));
                    item_0->setForeground(QBrush(QColor(0, 85, 127)));
//вывод наград определенного ведомства и типа
                    if (query_sub_item.exec(QString("SELECT \"Название\", \"Изображение\",\
                                (SELECT COUNT(indoc.incoming_doc_id) FROM awards AS indoc WHERE EXISTS (SELECT d1.inputnumber\
                                FROM documents AS d1 WHERE indoc.incoming_doc_id = d1.inputnumber AND d1.type = 'Получение наград')\
                                AND kod = indoc.award_id AND indoc.outgoing_date IS NULL) AS \"Наличие\",\
                    (SELECT COUNT(indoc.incoming_doc_id) FROM awards AS indoc WHERE EXISTS (SELECT d1.inputnumber\
                    FROM documents AS d1 WHERE indoc.incoming_doc_id = d1.inputnumber AND d1.type = 'Получение наград'\
                    AND '%1' <= d1.inputdate AND d1.inputdate <= '%2')\
                    AND kod = indoc.award_id) AS \"Наличие\",\
                        (SELECT COUNT(outdoc.outgoing_doc_id) FROM awards AS outdoc WHERE EXISTS (SELECT d2.inputnumber\
                        FROM documents AS d2 WHERE outdoc.outgoing_doc_id = d2.inputnumber AND d2.type = 'Выдача наград'\
                        AND '%1' <= d2.inputdate AND d2.inputdate <= '%2')\
                        AND kod = outdoc.award_id AND outdoc.outgoing_date IS NULL) AS \"Резерв\",\
                        (SELECT COUNT(outdate.outgoing_doc_id) FROM awards AS outdate WHERE EXISTS (SELECT d3.inputnumber\
                        FROM documents AS d3 WHERE outdate.outgoing_doc_id = d3.inputnumber AND d3.type ='Выдача наград')\
                        AND '%1' <= outdate.outgoing_date AND outdate.outgoing_date <= '%2'\
                        AND kod = outdate.award_id AND outdate.outgoing_date IS NOT NULL) AS \"Выдано\",\
                        (SELECT group_concat(storage) FROM\
                        (SELECT DISTINCT storage, award_id, incoming_doc_id, outgoing_date FROM awards) AS st\
                        WHERE EXISTS (SELECT dst.inputnumber FROM documents AS dst WHERE st.incoming_doc_id = dst.inputnumber\
                        AND dst.type = 'Получение наград' )\
                        AND kod = st.award_id AND st.outgoing_date IS NULL) AS \"Хранилище\"\
                        FROM (SELECT dn.text AS \"Название\", dn.file AS \"Изображение\", dn.kod AS kod FROM distionary AS dn\
                        WHERE EXISTS (SELECT a.award_id FROM awards AS a WHERE dn.kod = a.award_id\
                        AND a.category = '%3' AND type_id = %4));")
                        .arg(::g_firstdate_filter->date().toString(Qt::ISODate),::g_lastdate_filter->date().toString(Qt::ISODate),
                             category, QString::number(type_id))))
                    {
                        while (query_sub_item.next())
                        {
//создание списка элементов строки нижнего уровня
                            QList<QStandardItem*> ls_sub_item;
                            icon_sub_item = standard_icon_item;
                            if (outpixmap.loadFromData(query_sub_item.value(1).toByteArray()))
                            {
                                icon_sub_item.addPixmap(outpixmap);
                            }
//присвоение значений строке элементов нижнего уровня
                            auto sub_item_0 = new QStandardItem(icon_sub_item, query_sub_item.value(0).toString());
                            auto sub_item_1 = new QStandardItem(query_sub_item.value(3).toString() + " шт.");
//добавить исключение при пустом значении!!!
                            auto sub_item_2 = new QStandardItem(query_sub_item.value(4).toString() + " шт.");
                            auto sub_item_3 = new QStandardItem(query_sub_item.value(5).toString() + " шт.");
                            auto sub_item_4 = new QStandardItem(query_sub_item.value(2).toString() + " шт.");
                            auto sub_item_5 = new QStandardItem(query_sub_item.value(6).toString());
//определение стиля элементов строки нижнего уровня
                            sub_item_1->setTextAlignment(Qt::AlignCenter);
                            sub_item_2->setTextAlignment(Qt::AlignCenter);
                            sub_item_3->setTextAlignment(Qt::AlignCenter);
                            sub_item_4->setTextAlignment(Qt::AlignCenter);
//подсветка если значение на критической отметке
                            if(query_sub_item.value(2).toInt() < 5)
                            {
                                sub_item_4->setForeground(QBrush(QColor(255, 50, 50)));
                                            //->setBackground(QColor::fromRgb(255, 0, 0, 100));
                            }
//запись элементов нижнего уровня в список элементов сроки
                            //ls_sub_item.append(new QStandardItem(icon, query_sub_item.value(0).toString()));
                            ls_sub_item.append(sub_item_0);
                            ls_sub_item.append(sub_item_1);
                            ls_sub_item.append(sub_item_2);
                            ls_sub_item.append(sub_item_3);
                            ls_sub_item.append(sub_item_4);
                            ls_sub_item.append(sub_item_5);
//добавление списка элементов строки нижнего уровня в элемент среднего уровня
                            pb->setValue(sub_k);
                        //QMessageBox::information(this, query_sub_item.value(0).toString(), QString::number(sub_k));
                            sub_k++;
                            item_0->appendRow(ls_sub_item);
//очистка списка для заполнения элементами новой строки
                            ls_sub_item.clear();
                        }
//добавление элементов среднего уровня в список элементов строки
                        ls_item.append(item_0);
                        ls_item.append(item_1);
                        ls_item.append(item_2);
                        ls_item.append(item_3);
                        ls_item.append(item_4);
                        ls_item.append(item_5);
                    }
//добавление списка элементов строки среднего уровня в элемент верхнего уровня
                    //root_item->appendRow(item);
                    root_item->appendRow(ls_item);
//очистка списка для заполнения элементами новой строки
                    ls_item.clear();
                }
//размещение строки в модели данных
                sim->setItem(root_k, 0, root_item);
                sim->setData(sim->index(root_k, 0), QSize(250, 65), Qt::SizeHintRole);
//увеличение коэффициента строки в модели данных
                root_k++;
            }
        }
    }
//присвоение наименований заголовкам
    //sim->setHeaderData(0, Qt::Horizontal, "Награды");
    auto header_0 = new QStandardItem("Награды");
    header_0->setTextAlignment(Qt::AlignCenter);
    sim->setHorizontalHeaderItem(0, header_0);
    auto header_1 = new QStandardItem("Получено");
    header_1->setTextAlignment(Qt::AlignCenter);
    sim->setHorizontalHeaderItem(1, header_1);
    auto header_2 = new QStandardItem("Резерв");
    header_2->setTextAlignment(Qt::AlignCenter);
    sim->setHorizontalHeaderItem(2, header_2);
    auto header_3 = new QStandardItem("Выдано");
    header_3->setTextAlignment(Qt::AlignCenter);
    sim->setHorizontalHeaderItem(3, header_3);
    auto header_4 = new QStandardItem("На складе (в н.в.)");
    header_4->setTextAlignment(Qt::AlignCenter);
    sim->setHorizontalHeaderItem(4, header_4);
    auto header_5 = new QStandardItem("Расположение (в н.в.)");
    header_5->setTextAlignment(Qt::AlignCenter);
    sim->setHorizontalHeaderItem(5, header_5);
    pb->hide();
    ::g_main_window->statusBar_show_message("Таблица статистики обновлена...");
//определение ширины столбцов таблицы
    proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(sim);
    proxy->setFilterCaseSensitivity(Qt::CaseInsensitive); //определяет тип фильтрации
    proxy->setFilterKeyColumn(-1);
    proxy->setRecursiveFilteringEnabled(true);
    proxy->setFilterFixedString(::g_le_filter_text->text());
    ::g_proxy = proxy;
    ui->trw_statistic->setModel(proxy);
    ui->trw_statistic->expandAll();
    ui->trw_statistic->resizeColumnToContents(0);
    ui->trw_statistic->setColumnWidth(1, 100);
    ui->trw_statistic->setColumnWidth(2, 100);
    ui->trw_statistic->setColumnWidth(3, 100);
    ui->trw_statistic->setColumnWidth(4, 150);
    return true;
}

MW::~MW()
{
    delete ui;
}

void MW::on_trw_statistic_expanded(const QModelIndex &/*index*/)//раскрытие веток дерева статистики
{
    ui->trw_statistic->resizeColumnToContents(0);//автоматический подгон первого столбца дерева под содержимое
}

void MW::on_trw_statistic_clicked(const QModelIndex &index)//раскрытие ветки при одинарном клике
{
    ui->trw_statistic->expand(index);//раскрытие ветки
    show_dW_filter();
}

void MW::show_dW_doc_managering()
{
    if(menu0->actions().at(0)->isChecked())
    {
        ui->dW_doc_managering->show();
    }
    else
    {
        ui->dW_doc_managering->hide();
        menu0->actions().at(0)->setChecked(false);
    }
}

void MW::show_dW_receiving()
{
    if(menu0->actions().at(1)->isChecked())
    {
        ui->dW_receiving->show();
    }
    else
    {
        ui->dW_receiving->hide();
        menu0->actions().at(1)->setChecked(false);
    }
}

void MW::show_dW_extradition()
{
    if(menu0->actions().at(2)->isChecked())
    {
        ui->dW_extradition->show();
    }
    else
    {
        ui->dW_extradition->hide();
        menu0->actions().at(2)->setChecked(false);
    }
}

void MW::show_dW_filter()
{
    if (!ui->dW_filter->isHidden()) ui->dW_filter->hide();
    else ui->dW_filter->show();
}

void MW::show_receiving_form()
{
//объявление диалогового окна получения наград
    DLG_receiving receiving_form;
    receiving_form.setModal(true);
    receiving_form.showMaximized();
    receiving_form.exec();
}

void MW::on_trw_statistic_doubleClicked(const QModelIndex &/*index*/)
{
    //QMessageBox::information(this, "", "");
}

void MW::statusBar_set_ProgressBar(QString text, QProgressBar *pb)
{
    ui->statusbar->clearMessage();
    ui->statusbar->showMessage(text);
    ui->statusbar->addPermanentWidget(pb);
}

void MW::statusBar_remove_ProgressBar()
{
    ui->statusbar-> clearMessage();// removeWidget(ui->statusbar->children());
}

void MW::statusBar_show_message(QString text)
{
    ui->statusbar->clearMessage();
    ui->statusbar->showMessage(text);
}

void MW::slotCustomMenuRequested(QPoint pos)//создание контекстного меню для таблиц
{
    QMenu * menu = new QMenu(this); //обьект контектсного меню

    QAction * load_tree_view = new QAction(tr("Обновить"), this); //действия для контестного меню
//    QAction * editDevice = new QAction(tr("Редактировать"), this); //действия для контестного меню
//    QAction * deleteDevice = new QAction(tr("Удалить"), this);
//    QAction * reportDevice = new QAction(tr("Импорт таблицы"), this); //действия для контестного меню

    connect(load_tree_view, SIGNAL(triggered()), this, SLOT(load_tree_view())); //слот вызов диалога редактирования
//    connect(editDevice, SIGNAL(triggered()), this, SLOT(slotEditRecord())); //слот вызов диалога редактирования
//    connect(deleteDevice, SIGNAL(triggered()), this, SLOT(slotRemoveRecord())); //слот удаление записи из таблицы
//    connect(reportDevice, SIGNAL(triggered()), this, SLOT(slotImportTable())); //слот выгрузка неформализованного отчета

    menu->addAction(load_tree_view); //пункт добавление новой строки контекстного меню
//    menu->addAction(editDevice); //пункт редактирование строки контекстного меню
//    menu->addAction(deleteDevice); //пункт удаление строки контекстного меню
//    menu->addAction(reportDevice); //пункт выгрузка таблицы контекстного меню

    menu->popup(ui->trw_statistic->viewport()->mapToGlobal(pos));//установка координат контекстного меню рядом с курсором
}

void MW::resize_to_content()
{
    ui->trw_statistic->resizeColumnToContents(0);
}
