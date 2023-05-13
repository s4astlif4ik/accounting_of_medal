#include "dlg_report.h"
#include "ui_dlg_report.h"
#include "frm_receiwing.h"
#include "frm_sendering.h"

#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QDate>
#include <QStringList>
#include <QString>

#include <QInputDialog>


extern FRM_sendering *g_sendering;

DLG_report::DLG_report(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DLG_report)
{
    ui->setupUi(this);
//загрузка отчета
    load_report();
}

DLG_report::~DLG_report()
{
    delete ui;
}

QString DLG_report::create_HTML(QString content)
{
//создание структуры HTML
    return QString("\
        <!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0 Transitional//EN'>\
        <title></title>\
        <html>\
        <head>\
            <meta http-equiv='content-type' content='text/html; charset=utf-8'/>\
            <title></title>\
            <meta name='generator' content='LibreOffice 6.0.5.2 (Linux)'/>\
            <style type=\"text/css\">\
                @page { margin-left: \"3cm\"; margin-right: \"1cm\"; margin-top: \"2cm\"; margin-bottom: \"1,5cm\" }\
                p { font-variant: normal; color: #000000; font-family: 'Times New Roman'; font-size: 10pt; font-style: normal;\
                font-weight: normal; margin-top: 0cm; margin-bottom: 0cm; line-height: 100%}\
                td p { font-variant: normal; color: #000000; font-family: 'Times New Roman'; font-size: 10pt; font-style: normal;\
                font-weight: normal; margin-bottom: \"0cm\"}\
                th p { margin-bottom: '0cm'}\
            </style>\
        </head>\
        <body lang='ru-RU' dir='ltr'>\
        %1\
        </body>\
        </html>\
        ").arg(content);
}

QString DLG_report::insert_paragraph(QString text, QString align, int font_size, int size, QString font, QString color)
{
    if(!text.isEmpty())
    {
        return QString("<p align='%2'><font color='%6'><font face='%5'>\
        <font size='%4' style='font-size: %3pt; margin-top: 0.1cm'>%1</font></font></font></p>")
        .arg(text).arg(align).arg(font_size).arg(size).arg(font).arg(color);
    }
    return "";
}

QString DLG_report::insert_centered_paragraph(QString content)
{
//вставка в шаблон строки с равнением по центру
    return QString("\
        <p align='center' style = 'margin-top: 0; margin-bottom: 0cm; line-height: 100%'>\
        <font size='3' style='font-size: 12pt'>%1</font></p>\
    ").arg(content);
}

QString DLG_report::insert_left_underlined_paragraph(QString content)
{
//вставка в шаблон подчеркнутой строки по центру
    return QString("\
        <p align='left' style = 'margin-top: 0; margin-bottom: 0cm; line-height: 100%; background: transparent'>\
        <font size='3' style='font-size: 12pt'><i><u>%1</u></i></font></p>\
    ").arg(content);
}

QString DLG_report::insert_signature_paragraph(QString position, QString full_name)
{
//вставка в шаблон строки с подписью должностного лица
    return QString("\
        <table width='100%' cellpadding='0' cellspacing='0'>\
           <col width='128*'>\
           <col width='128*'>\
           <tr valign='top'>\
                <td width='70%'\
                    style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 0px solid #000000;\
                    border-right: none; padding-top: 0cm; padding-bottom: 0cm; padding-left: 0cm; padding-right: 0cm'>\
                    <p align='left' style = 'while-space: nowrap; margin-top: 0'>\
                        <font size='2' style='font-size: 14pt'>%1</font>\
                    </p>\
                </td>\
                <td width='25%'\
                    style='vertical-align: middle; border-top: none; border-bottom: 2px solid #000000; border-left: 0px solid #000000;\
                    border-right: none; padding-top: 0cm; padding-bottom: 0cm; padding-left: 0cm; padding-right: 0cm'>\
                    <p align='center' style = 'margin-top: 0'>\
                        <font size='2' style='font-size: 14pt'></font>\
                    </p>\
                </td>\
                <td width='5%'\
                    style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 0px solid #000000;\
                    border-right: none; padding-top: 0cm; padding-bottom: 0cm; padding-left: 0cm; padding-right: 0cm'>\
                    <p align='right' style = 'while-space: nowrap; margin-top: 0'>\
                        <font size='2' style='font-size: 14pt'>%2</font>\
                    </p>\
                </td>\
           </tr>\
        </table>\
    ").arg(position, full_name);
}

QString DLG_report::insert_docnumber(QString docnumber)
{
//вставка в шаблон строки курсивом в верхнем регистре (используется для указания номера документа)
    return QString("\
    </p>\
    <p align='center' style = 'margin-top: 0; margin-bottom: 0cm; line-height: 100%; background: transparent'>\
    <font size='3' style='font-size: 12pt'><i>%1</i></font></p>\
    <p align='center' style = 'margin-top: 0; margin-bottom: 0cm; line-height: 100%; background: transparent'>\
    ").arg(docnumber);
}

QString DLG_report::insert_table(QString row_content)
{
//добавление таблицы в шаблон
    return QString("\
        <table width=10cm border=1 cellpadding=1 cellspacing=0>\
           <col width=17*>\
           <col width=64*>\
           <col width=41*>\
           <col width=47*>\
           <col width=88*>\
           <thead>\
               <tr>\
                   <th rowspan='2' width='7%' style='vertical-align: middle; border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
                       <p><font size='3' style='font-size: 12pt'>№<br>п/п</font></p>\
                   </th>\
                   <th rowspan='2' width='25%' style='vertical-align: middle; border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
                       <p><font size='3' style='font-size: 12pt'>Наименование</font></p>\
                   </th>\
                   <th colspan='2' width='34%' style='vertical-align: middle; border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
                       <p><font size='3' style='font-size: 12pt'>Количество, шт.</font></p>\
                   </th>\
                   <th rowspan='2' width='34%' style='vertical-align: middle; border: 1px solid #000000; padding: 0.1cm'>\
                       <p><font size='3' style='font-size: 12pt'>Номера наград</font></p>\
                   </th>\
               </tr>\
               <tr>\
                   <th width='16%' style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
                       <p><font size='3' style='font-size: 12pt'>Отпустить</font></p>\
                   </th>\
                   <th width='18%' style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
                       <p><font size='3' style=font-size: 12pt'>Фактически отпущено</font></p>\
                   </th>\
               </tr>\
           </thead>\
           <tbody>\
              %1\
            </tbody>\
        </table>\
        <p align='justify' style = 'margin-top: 0; margin-bottom: 0cm; line-height: 100%; background: transparent'>\
    ").arg(row_content);
}

QString DLG_report::insert_full_content_row_for_table(QString content)
{
//добавление в таблицу строки с одной ячейкой
    return QString("\
        <tr>\
           <td colspan='5' width='100%'\
               style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
               <p align='center' style = 'margin-top: 0'>\
                   <font size='3' style='font-size: 12pt'><b>%1</b></font>\
               </p>\
           </td>\
        </tr>\
    ").arg(content);
}

QString DLG_report::insert_row_for_table(QStringList content_list)
{
//добавление строки в таблицу
    return QString("\
        <tr>\
        <td width='7%'\
            style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000;\
            border-right: none; padding-top: 0cm; padding-bottom: 0cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
            <p align='center' style = 'margin-top: 0'>\
                <font size='2' style='font-size: 12pt'>%1</font>\
            </p>\
        </td>\
        <td width='33%'\
            style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000;\
            border-right: none; padding-top: 0cm; padding-bottom: 0cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
            <p align='left' style = 'margin-top: 0'>\
                <font size='2' style='font-size: 12pt'>%2</font>\
            </p>\
        </td>\
        <td width='16%'\
            style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000;\
            border-right: none; padding-top: 0cm; padding-bottom: 0cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
            <p align='center' style = 'margin-top: 0'>\
                <font size='2' style='font-size: 12pt'>%3</font>\
            </p>\
        </td>\
        <td width='18%'\
            style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000;\
            border-right: none; padding-top: 0cm; padding-bottom: 0cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
            <p align='center' style = 'margin-top: 0'>\
                <font size='2' style='font-size: 12pt'>%4</font>\
            </p>\
        </td>\
        <td width='27%'\
            style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000;\
            border-right: 1px solid #000000; padding-top: 0cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
            <p align='left' style = 'margin-top: 0'>\
                <font size='2' style='font-size: 12pt'>%5</font>\
            </p>\
        </td>\
    </tr>\
    ").arg(content_list.at(0), content_list.at(1), content_list.at(2), content_list.at(3), content_list.at(4));
}

void DLG_report::load_report()
{
//список реквизитов документа
    QStringList document_details = ::g_sendering->get_outgoing_doc_details();
//загрузка модели данных таблицы
    model_report_receiver.setQuery(QString("SELECT\
        'ГОСУДАРСТВЕННЫЕ НАГРАДЫ РОССИЙСКОЙ ФЕДЕРАЦИИ' AS \"Категория\",\
        (SELECT direction FROM documents AS doc WHERE\
        doc.inputnumber = a.incoming_doc_id AND doc.type = 'Получение наград') AS \"Основание\",\
        (SELECT text FROM distionary AS d WHERE d.kod = a.award_id) AS \"Название\",\
        (SELECT  COUNT(award_id) FROM awards AS ca WHERE ca.award_id = a.award_id\
        AND ca.incoming_doc_id = a.incoming_doc_id AND ca.outgoing_doc_id = a.outgoing_doc_id) AS \"Количество\",\
        (SELECT GROUP_CONCAT(number, ', ') FROM awards AS na WHERE na.award_id = a.award_id\
        AND na.incoming_doc_id = a.incoming_doc_id AND na.outgoing_doc_id = a.outgoing_doc_id\
        AND na.number <>  '-' ORDER BY number) AS \"Номера\"\
        FROM (SELECT incoming_doc_id, outgoing_doc_id, award_id FROM awards WHERE category = 'Государственные награды' AND outgoing_doc_id = %1\
        ORDER BY incoming_doc_id) AS a GROUP BY \"Основание\", \"Название\"\
        UNION ALL\
        SELECT\
        'ВЕДОМСТВЕННЫЕ НАГРАДЫ РОССИЙСКОЙ ФЕДЕРАЦИИ' AS \"Категория\",\
        (SELECT direction FROM documents AS doc WHERE\
        doc.inputnumber = a.incoming_doc_id AND doc.type = 'Получение наград') AS \"Основание\",\
        (SELECT text FROM distionary AS d WHERE d.kod = a.award_id) AS \"Название\",\
        (SELECT  COUNT(award_id) FROM awards AS ca WHERE ca.award_id = a.award_id\
        AND ca.incoming_doc_id = a.incoming_doc_id AND ca.outgoing_doc_id = a.outgoing_doc_id) AS \"Количество\",\
        (SELECT GROUP_CONCAT(number, ', ') FROM awards AS na WHERE na.award_id = a.award_id\
        AND na.incoming_doc_id = a.incoming_doc_id AND na.outgoing_doc_id = a.outgoing_doc_id\
        AND na.number <>  '-' ORDER BY number) AS \"Номера\"\
        FROM (SELECT incoming_doc_id, outgoing_doc_id, award_id FROM awards WHERE category = 'Ведомственные награды' AND outgoing_doc_id = %1\
        ORDER BY incoming_doc_id) AS a GROUP BY \"Основание\", \"Название\"\
        UNION ALL\
        SELECT\
        'ДРУГИЕ НАГРАДЫ' AS \"Категория\",\
        (SELECT direction FROM documents AS doc WHERE\
        doc.inputnumber = a.incoming_doc_id AND doc.type = 'Получение наград') AS \"Основание\",\
        (SELECT text FROM distionary AS d WHERE d.kod = a.award_id) AS \"Название\",\
        (SELECT  COUNT(award_id) FROM awards AS ca WHERE ca.award_id = a.award_id\
        AND ca.incoming_doc_id = a.incoming_doc_id AND ca.outgoing_doc_id = a.outgoing_doc_id) AS \"Количество\",\
        (SELECT GROUP_CONCAT(number, ', ') FROM awards AS na WHERE na.award_id = a.award_id\
        AND na.incoming_doc_id = a.incoming_doc_id AND na.outgoing_doc_id = a.outgoing_doc_id\
        AND na.number <>  '-' ORDER BY number) AS \"Номера\"\
        FROM (SELECT incoming_doc_id, outgoing_doc_id, award_id FROM awards WHERE category = 'Иные награды' AND outgoing_doc_id = %1\
        ORDER BY incoming_doc_id) AS a GROUP BY \"Основание\", \"Название\";").arg(document_details.at(0)));
//перенос на новую строку МО РФ и ВС РФ
    QString sender;
    sender = document_details.at(2);
    sender = QString(sender)
        .replace(" Министерства обороны Российской Федерации","<br>Министерства обороны Российской Федерации");
    sender = QString(sender)
        .replace(" Вооруженных Сил Российской Федерации","<br>Вооруженных Сил Российской Федерации");
    QString receiver = document_details.at(3);
    receiver = QString(receiver)
        .replace(" Министерства обороны Российской Федерации","<br>Министерства обороны Российской Федерации");
    receiver = QString(receiver)
        .replace(" Вооруженных Сил Российской Федерации","<br>Вооруженных Сил Российской Федерации");
//проверка что модель заполнена
//    if(model_report_receiver == nullptr) return 1;
//создаем переменную под шаблон
    QString strStream;

//определение количества строк в модели
    const int rowCount = model_report_receiver.rowCount();
//создание транслятора для наполнения html страницы
    QTextStream out(&strStream);
    QString table_text;
//добавление организации в шаблон
    out << insert_centered_paragraph(QString("%1<br>%2")
        .arg(sender, document_details.at(1)));
    out << insert_centered_paragraph(QString("<br>"));
//добавление номера документа в шаблон
    out << insert_docnumber(QString("РАСХОДНАЯ НАКЛАДНАЯ № %1")
        .arg(document_details.at(0)));
    out << insert_centered_paragraph(QString("<br>"));
//добавление организации получателя в шаблон
    out << insert_left_underlined_paragraph(QString("<i>Отпустить (кому): </i>%1<i>").arg(receiver));
    out << insert_centered_paragraph(QString("<br>"));
//создание таблицы
    QString award_category;
    QString award_direction;
//цикл наполнение таблицы
    for(int row = 0; row < rowCount; row++)
    {
//отслеживаем изменение категории и меняем при несоответствии
//здесь будет функция формирования таблицы
        QStringList row_content;
        if(award_category != model_report_receiver.index(row, 0).data().toString())
        {
            award_category = model_report_receiver.index(row, 0).data().toString();
            table_text.append(insert_full_content_row_for_table(award_category));
            award_direction = model_report_receiver.index(row, 1).data().toString();
            table_text.append(insert_full_content_row_for_table(award_direction));
        }
//отслеживаем основание и меняем при несоответствиии
        if(award_direction != model_report_receiver.index(row, 1).data().toString())
        {
            award_direction = model_report_receiver.index(row, 1).data().toString();
            table_text.append(insert_full_content_row_for_table(award_direction));
        }
//наполняем строки данными из модели
        row_content.append(QString::number(row + 1));
        row_content.append(model_report_receiver.index(row, 2).data().toString());
        row_content.append(model_report_receiver.index(row, 3).data().toString());
//добавить функцию преобразования числа в текст
        row_content.append(int2string(model_report_receiver.index(row, 3).data().toInt()));
        row_content.append(model_report_receiver.index(row, 4).data().toString());
        table_text.append(insert_row_for_table(row_content));
    }
//добавление таблицы с наградами в шаблон
    out << insert_table(table_text);
    out << insert_centered_paragraph(QString("<br>"));
    out << insert_centered_paragraph(QString("<br>"));
//добавление подписи руководителя
    if(!ui->le_supervisor_position->text().isEmpty() || !ui->le_supervisor_fullname->text().isEmpty())
    {
        out << insert_signature_paragraph(ui->le_supervisor_position->text(), ui->le_supervisor_fullname->text());
        out << insert_centered_paragraph(QString("<br>"));
    }
//добавление подписанта в шаблон
    if(!ui->le_manager_position->text().isEmpty() || !ui->le_manager_fullname->text().isEmpty())
    {
        out << insert_signature_paragraph(ui->le_manager_position->text(), ui->le_manager_fullname->text());
        out << insert_centered_paragraph(QString("<br>"));
    }
//добавление подписанта в шаблон
    if(!ui->le_keeper_position->text().isEmpty() || !ui->le_keeper_fullname->text().isEmpty())
    {
        out << insert_signature_paragraph(ui->le_keeper_position->text(), ui->le_keeper_fullname->text());
        out << insert_centered_paragraph(QString("<br>"));
    }
//загрузка шаблона в редактор
    QTextDocument* report = new QTextDocument;
    report->setHtml(create_HTML(strStream));
    ui->tE_report->setDocument(report);// ->setHtml(create_HTML(strStream));
}

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
//    qDebug() << strStream;

//    QTextDocument* document = new QTextDocument();
//    qDebug() << document->size();
//    document->setHtml(create_HTML(strStream));

//    QTextEdit *pte = new QTextEdit;
//    pte->setDocument(document);
    //pte->setHtml(strStream);
//    pte->setStyleSheet("background-color: rgb(255, 255, 255);");
//    pte->showMaximized();
    //QTextBrowser *browser = new QTextBrowser;
    //browser->setText(strStream);
    //browser->showMaximized();
//создание диалога сохранения файла

//    QPrinter printer(QPrinter::PrinterResolution);
    //QPageSize A4;
    //A4 = QPageSize::A4;
    //printer.setPageSize(A4);
    //printer.setOutputFileName("1111.pdf");
    //printer.setPageMargins(QMargins(0, 0, 0, 0));

//    QPrintDialog *dialog = new QPrintDialog(&printer, 0);
//    if(dialog->exec() == QDialog::Accepted)
//    {
//        pte->print(&printer);
//        //document->print(&printer);
//        pte->hide();
//        //browser->hide();
//    }

void DLG_report::on_buttonBox_accepted()
{
    QString filters("Текстовые файлы (*.doc);;Текстовые файлы (*.odt);;Веб файлы (*.html);;Все файлы (*.*)");
    QString defaultFilter("Текстовые файлы (*.doc)");
    QString defaultfilename(QString("/report_%1.odt").arg(QDate::currentDate().toString(Qt::ISODate)));
//    QFileDialog fileDialog(0, "Сохранить наряд", QDir::currentPath(), filters)
//    QFileDialog *fileDialog = new QFileDialog();
//    fileDialog->selectNameFilter(defaultFilter);
//    fileDialog->selectFile(QString("отчет от %1.odt").arg(QDate::currentDate().toString(Qt::ISODate)));
//    fileDialog->exec();
//    QString file_name = fileDialog->selectedFiles().last();

    QString file_name = QFileDialog::getSaveFileName(this, "Сохранить наряд", QDir::currentPath() + defaultfilename, filters, &defaultFilter);
    if(!file_name.isEmpty())\
    {
        QFile file(file_name);
        if(file.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            file.write(ui->tE_report->document()->toHtml().toLocal8Bit().data());
            file.close();
        }
    }
}

void DLG_report::on_bnt_outgoing_save_clicked()
{
    load_report();
}

QString DLG_report::int2string(int value)
{
    QString in_text;
    QString out_text;
    QTextStream add_string(&out_text);
    in_text = QString::number(value);

    if(value > 10000) return "";

    if(in_text.length() == 0) in_text = "0000" + in_text;
    if(in_text.length() == 1) in_text = "000" + in_text;
    if(in_text.length() == 2) in_text = "00" + in_text;
    if(in_text.length() == 3) in_text = "0" + in_text;

    if(in_text[0] == "0") add_string << QString("").toUtf8();
    if(in_text[0] == "1") add_string << QString("одна тысяча ").toUtf8();
    if(in_text[0] == "2") add_string << QString("две тысячи ").toUtf8();
    if(in_text[0] == "3") add_string << QString("три тысячи ").toUtf8();
    if(in_text[0] == "4") add_string << QString("четыре тысячи ").toUtf8();
    if(in_text[0] == "5") add_string << QString("пять тысяч ").toUtf8();
    if(in_text[0] == "6") add_string << QString("шесть тысяч ").toUtf8();
    if(in_text[0] == "7") add_string << QString("семь тысяч ").toUtf8();
    if(in_text[0] == "8") add_string << QString("восемь тысяч ").toUtf8();
    if(in_text[0] == "9") add_string << QString("девять тысяч ").toUtf8();

    if(in_text[1] == "0") add_string << QString("").toUtf8();
    if(in_text[1] == "1") add_string << QString("сто ").toUtf8();
    if(in_text[1] == "2") add_string << QString("двести ").toUtf8();
    if(in_text[1] == "3") add_string << QString("триста ").toUtf8();
    if(in_text[1] == "4") add_string << QString("четыреста ").toUtf8();
    if(in_text[1] == "5") add_string << QString("пятьсот ").toUtf8();
    if(in_text[1] == "6") add_string << QString("шестьсот ").toUtf8();
    if(in_text[1] == "7") add_string << QString("семьсот ").toUtf8();
    if(in_text[1] == "8") add_string << QString("восемьсот ").toUtf8();
    if(in_text[1] == "9") add_string << QString("девятьсот ").toUtf8();

    if(in_text[2] == "1")
    {
        if(in_text.right(2) == "10") add_string << QString("десять").toUtf8();
        if(in_text.right(2) == "11") add_string << QString("одиннадцать").toUtf8();
        if(in_text.right(2) == "12") add_string << QString("двенадцать").toUtf8();
        if(in_text.right(2) == "13") add_string << QString("тринадцать").toUtf8();
        if(in_text.right(2) == "14") add_string << QString("четырнадцать").toUtf8();
        if(in_text.right(2) == "15") add_string << QString("пятнадцать").toUtf8();
        if(in_text.right(2) == "16") add_string << QString("шестнадцать").toUtf8();
        if(in_text.right(2) == "17") add_string << QString("семнадцать").toUtf8();
        if(in_text.right(2) == "18") add_string << QString("восемнадцать").toUtf8();
        if(in_text.right(2) == "19") add_string << QString("девятнадцать").toUtf8();
    }

    if(in_text[2] != "1")
    {
        if(in_text[2] == "2") add_string << QString("двадцать ").toUtf8();
        if(in_text[2] == "3") add_string << QString("тридцать ").toUtf8();
        if(in_text[2] == "4") add_string << QString("сорок ").toUtf8();
        if(in_text[2] == "5") add_string << QString("пятьдесят ").toUtf8();
        if(in_text[2] == "6") add_string << QString("шестьдесят ").toUtf8();
        if(in_text[2] == "7") add_string << QString("семьдесят ").toUtf8();
        if(in_text[2] == "8") add_string << QString("восемьдесят ").toUtf8();
        if(in_text[2] == "9") add_string << QString("девяносто ").toUtf8();

        if(in_text[3] == "1") add_string << QString("одна").toUtf8();
        if(in_text[3] == "2") add_string << QString("две").toUtf8();
        if(in_text[3] == "3") add_string << QString("три").toUtf8();
        if(in_text[3] == "4") add_string << QString("четыре").toUtf8();
        if(in_text[3] == "5") add_string << QString("пять").toUtf8();
        if(in_text[3] == "6") add_string << QString("шесть").toUtf8();
        if(in_text[3] == "7") add_string << QString("семь").toUtf8();
        if(in_text[3] == "8") add_string << QString("восемь").toUtf8();
        if(in_text[3] == "9") add_string << QString("девять").toUtf8();
    }

    return out_text;
}


void DLG_report::on_bnt_outgoing_save_2_clicked()
{
    bool ok;
    int count_award = QInputDialog::getInt(this, "Преобразование числа в текст", "Выберите число до 4 символов", 0, 0, 9999,1, &ok);
    if(ok)
    {
        QMessageBox::information(this, QString::number(count_award), int2string(count_award));
    }
}
