#include "report.h"
#include "frm_sendering.h"

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QDate>
#include <QFile>
#include <QFileDialog>
#include <QDebug>


REPORT::REPORT()
{
}

QString REPORT::create_HTML(QString content)
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
            <style type='text/css'>\
                @page { margin-left: \"3cm\"; margin-right: \"1cm\"; margin-top: \"2cm\"; margin-bottom: \"1,5cm\" }\
                p { margin: 0; font-variant: normal; color: #000000; font-family: 'Times New Roman'; font-size: 14pt; font-style: normal;\
                font-weight: normal; margin-top: 0cm; margin-bottom: 0cm; line-height: 100%}\
                td p { font-variant: normal; color: #000000; font-family: 'Times New Roman'; font-size: 12pt; font-style: normal;\
                font-weight: normal; margin-bottom: \"0cm\"}\
                th p { margin-bottom: \"0cm\"}\
            </style>\
        </head>\
        <body lang='ru-RU' dir='ltr'>\
        %1\
        </body>\
        </html>\
        ").arg(content);
}

QString REPORT::insert_paragraph(QString text, QString align, int font_size, int size, QString font, QString color)
{
    if(!text.isEmpty())
    {
        return QString("<p align='%2'><font color='%6'><font face='%7'>\
            <font size='%4' style='margin-top: 0.1cm; margin-bottom: 0cm; margin-left: 0cm; margin-right: 0cm; padding: 0cm;\
            font-size: %3pt'>%1</font></font></font></p>")
        .arg(text).arg(align).arg(font_size).arg(size).arg(font).arg(color);
    }
    return "";
}

QString REPORT::insert_signature_paragraph(QString position, QString full_name)
{
//    <col width='128*'>\
//    <col width='128*'>\

//вставка в шаблон строки с подписью должностного лица
    return QString("\
        <table width='100%' cellpadding='0' cellspacing='0'>\
           <tr valign='top'>\
                <td width='1%'\
                    style='vertical-align: middle; border: none; padding: 0cm'>%1</td>\
                <td width='98%'\
                    style='vertical-align: middle; border-top: none; border-bottom: 2px solid #000000; border-left: none;\
                    border-right: none; padding: 0cm'></td>\
                <td width='1%'\
                    style='vertical-align: middle; border: none; padding: 0cm'>%2</td>\
           </tr>\
        </table>\
    ").arg(insert_paragraph(position, "left"), insert_paragraph(full_name, "rigth"));
}

QString REPORT::insert_table(QString row_content)
{
//добавление таблицы в шаблон
    return QString("\
        <table width=100% border=1 cellpadding=1 cellspacing=0 style = {border-collapse: separate; border-spacing: 0em;}>\
           <col width=17*>\
           <col width=64*>\
           <col width=41*>\
           <col width=47*>\
           <col width=88*>\
           <thead>\
               <tr>\
                   <th rowspan='2' width='7%' style='vertical-align: middle; border-top: 1px solid #000000;\
                        border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none;\
                        padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>%2</th>\
                   <th rowspan='2' width='25%' style='vertical-align: middle; border-top: 1px solid #000000;\
                        border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none;\
                        padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\%3</th>\
                   <th colspan='2' width='34%' style='vertical-align: middle; border-top: 1px solid #000000;\
                        border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none;\
                        padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>%4</th>\
                   <th rowspan='2' width='34%' style='vertical-align: middle; border: 1px solid #000000; padding: 0.1cm'>%5</th>\
               </tr>\
               <tr>\
                   <th width='16%' style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000;\
                        border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm;\
                        padding-left: 0.1cm; padding-right: 0.1cm'>%6</th>\
                   <th width='18%' style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000;\
                        border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm;\
                        padding-left: 0.1cm; padding-right: 0.1cm'>%7</th>\
               </tr>\
           </thead>\
           <tbody>\
              %1\
            </tbody>\
        </table>\
        <p align='justify' style = 'margin-top: 0; margin-bottom: 0cm; line-height: 100%; background: transparent'>\
    ").arg(row_content, insert_paragraph("№<br>п/п", "center", 14, 3), insert_paragraph("Наименование", "center", 14, 3),
            insert_paragraph("Количество, шт.", "center", 14, 3), insert_paragraph("Номера наград", "center", 14, 3),
            insert_paragraph("Отпустить", "center", 14, 3), insert_paragraph("Фактически отпущено", "center", 14, 3));
}

QString REPORT::insert_full_content_row_for_table(QString content)
{
//добавление в таблицу строки с одной ячейкой
    return QString("\
        <tr>\
           <td colspan='5' width='100%'\
               style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000;\
                    border-left: 1px solid #000000; border-right: 1px solid #000000; padding-top: 0.1cm;\
                    padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'><b>%1</b></td>\
        </tr>\
    ").arg(insert_paragraph(content, "center", 12, 2));
}

QString REPORT::insert_row_for_table(QStringList content_list)
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
            border-right: 1px solid #000000; padding-top: 0cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right:Фактически отпущено 0.1cm'>\
            <p align='left' style = 'margin-top: 0'>\
                <font size='2' style='font-size: 12pt'>%5</font>\
            </p>\
        </td>\
    </tr>\
    ").arg(insert_paragraph(content_list.at(0), "center", 12, 2), insert_paragraph(content_list.at(1), "left", 12, 2),
            insert_paragraph(content_list.at(2), "center", 12, 2), insert_paragraph(content_list.at(3), "center", 12, 2),
            insert_paragraph(content_list.at(4), "left", 12, 2));
}

QString REPORT::load_outgoing_doc_report(QStringList document_details)
{
//загрузка модели данных таблицы
//    model_report_receiver->setQuery(QString("SELECT\
//        'ГОСУДАРСТВЕННЫЕ НАГРАДЫ РОССИЙСКОЙ ФЕДЕРАЦИИ' AS \"Категория\",\
//        (SELECT direction FROM documents AS doc WHERE\
//        doc.inputnumber = a.incoming_doc_id AND awards_category = 0 AND doc.type = 0) AS \"Основание\",\
//        (SELECT text FROM SLOVAR AS d WHERE d.kod = a.award_id) AS \"Название\",\
//        (SELECT  COUNT(award_id) FROM awards AS ca WHERE ca.award_id = a.award_id\
//        AND ca.incoming_doc_id = a.incoming_doc_id AND ca.outgoing_doc_id = a.outgoing_doc_id) AS \"Количество\",\
//        (SELECT GROUP_CONCAT(number, ', ') FROM awards AS na WHERE na.award_id = a.award_id\
//        AND na.incoming_doc_id = a.incoming_doc_id AND na.outgoing_doc_id = a.outgoing_doc_id\
//        AND na.number <>  '-' ORDER BY number) AS \"Номера\"\
//        FROM (SELECT incoming_doc_id, outgoing_doc_id, award_id FROM awards WHERE category = 0 AND outgoing_doc_id = %1\
//        ORDER BY incoming_doc_id) AS a GROUP BY \"Основание\", \"Название\"\
//        UNION ALL\
//        SELECT\
//        'ВЕДОМСТВЕННЫЕ НАГРАДЫ РОССИЙСКОЙ ФЕДЕРАЦИИ' AS \"Категория\",\
//        (SELECT direction FROM documents AS doc WHERE\
//        doc.inputnumber = a.incoming_doc_id AND awards_category = 1 AND doc.type = 0) AS \"Основание\",\
//        (SELECT text FROM SLOVAR AS d WHERE d.kod = a.award_id) AS \"Название\",\
//        (SELECT  COUNT(award_id) FROM awards AS ca WHERE ca.award_id = a.award_id\
//        AND ca.incoming_doc_id = a.incoming_doc_id AND ca.outgoing_doc_id = a.outgoing_doc_id) AS \"Количество\",\
//        (SELECT GROUP_CONCAT(number, ', ') FROM awards AS na WHERE na.award_id = a.award_id\
//        AND na.incoming_doc_id = a.incoming_doc_id AND na.outgoing_doc_id = a.outgoing_doc_id\
//        AND na.number <>  '-' ORDER BY number) AS \"Номера\"\
//        FROM (SELECT incoming_doc_id, outgoing_doc_id, award_id FROM awards WHERE category = 1 AND outgoing_doc_id = %1\
//        ORDER BY incoming_doc_id) AS a GROUP BY \"Основание\", \"Название\"\
//        ;").arg(document_details.at(0)));



    model_report_receiver->setQuery(QString("SELECT DISTINCT\
    'ГОСУДАРСТВЕННЫЕ НАГРАДЫ РОССИЙСКОЙ ФЕДЕРАЦИИ' AS \"Категория\",\
    (SELECT direction FROM documents AS d WHERE d.ID = (SELECT incoming_doc_id FROM awards AS doc WHERE doc.ID = a.ID)) AS \"Основание\",\
    (SELECT text FROM SLOVAR AS s WHERE s.kod = (SELECT award_id FROM awards AS name WHERE name.ID = a.ID)) AS \"Название\",\
    COUNT(a.ID) AS \"Количество\",\
    STUFF((SELECT ', ' + number FROM awards AS num WHERE EXISTS\
    (SELECT [klnagruk], [nomnagr] FROM OSNAGR AS gn\
    WHERE [klprotpr] = 1\
    AND [nomisx] = (SELECT CONVERT(nvarchar(5), 'рн ') + CONVERT(nvarchar(10), inputnumber)\
    FROM documents WHERE ID = '%1')\
    AND [datotpr]  = (SELECT CONVERT(nvarchar(10), inputdate, 104)\
    FROM documents WHERE ID = '%1') AND award_id = gn.klnagruk AND number = gn.nomnagr)\
    OR\
    (outgoing_doc_id = '%1') FOR XML PATH('')), 1, 1, '') AS \"Номера\"\
    FROM\
    (SELECT ID FROM awards WHERE EXISTS\
    (SELECT [klnagruk], [nomnagr] FROM OSNAGR AS gn\
    WHERE [klprotpr] = 1\
    AND [nomisx] = (SELECT CONVERT(nvarchar(5), 'рн ') + CONVERT(nvarchar(10), inputnumber)\
    FROM documents WHERE ID = '%1')\
    AND [datotpr]  = (SELECT CONVERT(nvarchar(10), inputdate, 104)\
    FROM documents WHERE ID = '%1') AND award_id = gn.klnagruk AND number = gn.nomnagr)\
    OR\
    (outgoing_doc_id = '%1')) AS a GROUP BY a.ID;").arg(document_details.at(0)));
//перенос на новую строку МО РФ и ВС РФ
    QString sender;
    sender = document_details.at(3);
    sender = QString(sender)
        .replace(" Министерства обороны Российской Федерации","<br>Министерства обороны Российской Федерации");
    sender = QString(sender)
        .replace(" Вооруженных Сил Российской Федерации","<br>Вооруженных Сил Российской Федерации");
    QString receiver;
    receiver = document_details.at(4);
    receiver = QString(receiver)
        .replace(" Министерства обороны Российской Федерации","<br>Министерства обороны Российской Федерации");
    receiver = QString(receiver)
        .replace(" Вооруженных Сил Российской Федерации","<br>Вооруженных Сил Российской Федерации");
//создаем переменную под шаблон
    QString strStream;
//определение количества строк в модели
    int rowCount = model_report_receiver->rowCount();
//создание транслятора для наполнения html страницы
    QTextStream out(&strStream);
    QString table_text;
//добавление организации в шаблон
    out << insert_paragraph(QString("%1<br>%2").arg(sender, document_details.at(2)), "center");
    out << insert_paragraph(QString("&nbsp;"));
    //out << insert_paragraph(QString("<br>"));
//добавление номера документа в шаблон
    out << insert_paragraph(QString("<i>РАСХОДНАЯ НАКЛАДНАЯ № %1</i>").arg(document_details.at(1)), "center");
    out << insert_paragraph(QString("&nbsp;"));
    //out << insert_paragraph(QString("<br>"));
//добавление организации получателя в шаблон
    out << insert_paragraph(QString("<u><i>Отпустить (кому): </i>%1</u>").arg(receiver));
    out << insert_paragraph(QString("&nbsp;"));
    //out << insert_paragraph(QString("<br>"));
//создание таблицы
    QString award_category;
    QString award_direction;
//цикл наполнение таблицы
    for(int row = 0; row < rowCount; row++)
    {
//отслеживаем изменение категории и меняем при несоответствии
//здесь будет функция формирования таблицы
        QStringList row_content;
        if(award_category != model_report_receiver->index(row, 0).data().toString())
        {
            award_category = model_report_receiver->index(row, 0).data().toString();
            table_text.append(insert_full_content_row_for_table(award_category));
            award_direction = model_report_receiver->index(row, 1).data().toString();
            table_text.append(insert_full_content_row_for_table(award_direction));
        }
//отслеживаем основание и меняем при несоответствиии
        if(award_direction != model_report_receiver->index(row, 1).data().toString())
        {
            award_direction = model_report_receiver->index(row, 1).data().toString();
            table_text.append(insert_full_content_row_for_table(award_direction));
        }
//наполняем строки данными из модели
        row_content.append(QString::number(row + 1));
        row_content.append(model_report_receiver->index(row, 2).data().toString());
        row_content.append(model_report_receiver->index(row, 3).data().toString());
//добавить функцию преобразования числа в текст
        row_content.append(int2string(model_report_receiver->index(row, 3).data().toInt()));
        row_content.append(model_report_receiver->index(row, 4).data().toString());
        table_text.append(insert_row_for_table(row_content));
    }
//добавление таблицы с наградами в шаблон
    out << insert_table(table_text);
    out << insert_paragraph(QString("&nbsp;"));
    //out << insert_paragraph(QString("<br>"));
    out << insert_paragraph(QString("&nbsp;"));
    //out << insert_paragraph(QString("<br>"));
//добавление подписи руководителя
    out << insert_signature_paragraph("Начальник&nbsp;отдела", "А.&nbsp;Клишов");
    out << insert_paragraph(QString("&nbsp;"));
    //out << insert_paragraph(QString("<br>"));
//добавление подписанта в шаблон
    out << insert_signature_paragraph("Главный&nbsp;эксперт", "Е.&nbsp;Краева");
    out << insert_paragraph(QString("&nbsp;"));
    //out << insert_paragraph(QString("<br>"));
//добавление подписанта в шаблон
    out << insert_signature_paragraph("Ведущий&nbsp;эксперт", "Н.&nbsp;Софийская");
    out << insert_paragraph(QString("&nbsp;"));
//вывод сформированного документа
    return create_HTML(strStream);

//    QString filters("Текстовые файлы (*.doc);;Текстовые файлы (*.odt);;Веб файлы (*.html);;Все файлы (*.*)");
//    QString defaultFilter("Текстовые файлы (*.doc)");
//    QString defaultfilename(QString("/report_%1.odt").arg(QDate::currentDate().toString(Qt::ISODate)));
//    QString file_name = QFileDialog::getSaveFileName(0, "Сохранить наряд", QDir::currentPath() + defaultfilename, filters, &defaultFilter);
//    if(!file_name.isEmpty())\
//    {
//        QFile file(file_name);
//        if(file.open(QIODevice::WriteOnly | QIODevice::Append))
//        {
//           file.write(create_HTML(strStream).toUtf8());
//           file.close();
//        }
//    }

}

QString REPORT::int2string(int value)
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

