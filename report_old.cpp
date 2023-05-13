#include "report_old.h"

#include <QList>
#include <QTextStream>
#include <QTextDocument>
#include <QTextBrowser>
#include <QPrinter>
#include <QPrintDialog>
#include <QDebug>
#include <QPlainTextEdit>
#include <QFileDialog>
#include <QPainter>


//REPORT *g_report;

REPORT::REPORT()
{
//    g_report = this;
}

//QString REPORT::create_HTML(QString content)
//{
//    return QString("\
//        <!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.0 Transitional//EN'>\
//        <html>\
//        <head>\
//            <meta http-equiv='content-type' content='text/html; charset=utf-8'/>\
//            <title></title>\
//            <meta name='generator' content='LibreOffice 6.0.5.2 (Linux)'/>\
//            <style type=\"text/css\">\
//                @page { margin-left: '3cm'; margin-right: '1cm'; margin-top: '2cm'; margin-bottom: '1,5cm' }\
//                p { margin-bottom: '0cm'; line-height: '100% '}\
//                td p { margin-bottom: '0cm'}\
//                th p { margin-bottom: '0cm'}\
//            </style>\
//        </head>\
//        <body lang='ru-RU' dir='ltr'>\
//        %1\
//        </body>\
//        </html>\
//        ").arg(content);
//}

//QString REPORT::create_centered_paragraph(QString content)
//{
//    return QString("\
//        <p align='center' style='margin-bottom: 0cm; line-height: 100%'>\
//        <font size='3' style='font-size: 14pt'>%1</font></p>\
//    ").arg(content);
//}

//QString REPORT::create_left_underlined_paragraph(QString content)
//{
//    return QString("\
//        <p align='justify' style='margin-bottom: 0cm; line-height: 100%; background: transparent'>\
//        <font size='3' style='font-size: 14pt'><i><u>%1</u></i></font></p>\
//        <br>\
//    ").arg(content);
//}

//QString REPORT::create_signature_paragraph(QString position, QString full_name)
//{
//    return QString("\
//        <table width='100%' cellpadding='0' cellspacing='0'>\
//           <col width='128*'>\
//           <col width='128*'>\
//           <tr valign='top'>\
//               <td width='50%' style='border: none; padding: 0cm'>\
//                   <p align='left'><font size='3' style='font-size: 12pt'>%1</font></p>\
//               </td>\
//               <td width='50%' style='border: none; padding: 0cm'>\
//                   <p align='right'><font size='3' style='font-size: 12pt'>%2</font></p>\
//               </td>\
//           </tr>\
//        </table>\
//    ").arg(position).arg(full_name);
//}

//QString REPORT::create_docnumber(QString docnumber)
//{
//    return QString("\
//    </p>\
//    <p align='center' style='margin-bottom: 0cm; line-height: 100%; background: transparent'>\
//    <font size='4' style='font-size: 14pt'><i>РАСХОДНАЯ НАКЛАДНАЯ № %1</i></font></p>\
//    <p align='center' style='margin-bottom: 0cm; line-height: 100%; background: transparent'>\
//    ").arg(docnumber);
//}

//QString REPORT::create_table(QString row_content)
//{
//    return QString("\
//        <br>\
//        <table width=100% border=1 cellpadding=4 cellspacing=0>\
//           <col width=17*>\
//           <col width=64*>\
//           <col width=41*>\
//           <col width=47*>\
//           <col width=88*>\
//           <thead>\
//               <tr>\
//                   <th rowspan='2' width='7%' style='vertical-align: middle; border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//                       <p><font size='3' style='font-size: 12pt'>№<br>п/п</font></p>\
//                   </th>\
//\
//                   <th rowspan='2' width='25%' style='vertical-align: middle; border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//                       <p><font size='3' style='font-size: 12pt'>Наменование</font></p>\
//                   </th>\
//\
//                   <th colspan='2' width='34%' style='vertical-align: middle; border-top: 1px solid #000000; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//                       <p><font size='3' style='font-size: 12pt'>Количество, шт.</font></p>\
//                   </th>\
//\
//                   <th rowspan='2' width='34%' style='vertical-align: middle; border: 1px solid #000000; padding: 0.1cm'>\
//                       <p><font size='3' style='font-size: 12pt'>Номера наград</font></p>\
//                   </th>\
//\
//               </tr>\
//               <tr>\
//                   <th width='16%' style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//                       <p><font size='3' style='font-size: 12pt'>Отпустить</font></p>\
//                   </th>\
//                   <th width='18%' style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//                       <p><font size='3' style=font-size: 12pt'>Фактически отпущено</font></p>\
//                   </th>\
//               </tr>\
//           </thead>\
//           <tbody>\
//              %1\
//            </tbody>\
//        </table>\
//        <p align='justify' style='margin-bottom: 0cm; line-height: 100%; background: transparent'>\
//    ").arg(row_content);
//}

//QString REPORT::create_full_content_row_for_table(QString content)
//{
//    return QString("\
//        <tr>\
//           <td colspan='5' width='100%'\
//               style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//               <p align='center'>\
//                   <font size='4' style='font-size: 14pt'><b>%1</b></font>\
//               </p>\
//           </td>\
//        </tr>\
//    ").arg(content);
//}

//QString REPORT::create_row_for_table(QStringList content_list)
//{
////    QString content;
////    QTextStream out(&content);
////    int columnCount = list_content.count();
////    for(int column = 0; column < columnCount; column++)
////    {
////        out << list_content.at(column);
////    }
//    return QString("\
//        <tr>\
//        <td width='7%'\
//            style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//            <p align='center'>\
//                <font size='3' style='font-size: 12pt'>%1</font>\
//            </p>\
//        </td>\
//        <td width='33%'\
//            style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//            <p align='justify'>\
//                <font size='3' style='font-size: 12pt'>%2</font>\
//            </p>\
//        </td>\
//        <td width='16%'\
//            style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//            <p align='center'>\
//                <font size='3' style='font-size: 12pt'>%3</font>\
//            </p>\
//        </td>\
//        <td width='18%'\
//            style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: none; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//            <p align='center'>\
//                <font size='3' style='font-size: 12pt'>%4</font>\
//            </p>\
//        </td>\
//        <td width='27%'\
//            style='vertical-align: middle; border-top: none; border-bottom: 1px solid #000000; border-left: 1px solid #000000; border-right: 1px solid #000000; padding-top: 0.1cm; padding-bottom: 0.1cm; padding-left: 0.1cm; padding-right: 0.1cm'>\
//            <p align='center'>\
//                <font size='3' style='font-size: 12pt'>%5</font>\
//            </p>\
//        </td>\
//    </tr>\
//    ").arg(content_list.at(0))
//        .arg(content_list.at(1))
//        .arg(content_list.at(2))
//        .arg(content_list.at(3))
//        .arg(content_list.at(4));
//}

//void REPORT::print_report()
//{
//    //if(!ui->tV_awards->model()) return 1;

//    QString strStream;
//    QTextStream out(&strStream);
//    QStringList columns_text;
//    columns_text.append("1.");
//    columns_text.append("Медаль Суворова");
//    columns_text.append("1");
//    columns_text.append("одну");
//    columns_text.append("М13932");
//    QString table_text;
//    table_text.append(create_full_content_row_for_table("Указ ПРФ от 23.03.2023 г. №717"));
//    table_text.append(create_row_for_table(columns_text));
//    out << create_centered_paragraph("Главное управление кадров<br>Министерства обороны Российской Федерации");
//    out << create_centered_paragraph("\"28\" января 2023 г.");
//    out << create_docnumber("27 гн");
//    out << create_left_underlined_paragraph("Отпустить (кому): Западный военный округ");
//    out << create_left_underlined_paragraph("Получатель: Романов М.А.");
//    out << create_table(table_text);
//    out << create_signature_paragraph("Ведущий консультан", "М.Шрамко");



////    const int rowCount = ui->tV_awards->model()->rowCount();
////    const int columnCount = ui->tV_awards->model()->columnCount();




//////установка разметки и кодировки документа
////    out << "<html>\n" << "<head>\n" << "meta Content=\"Text/html;charset=Windows-1251\n" <<
////           QString("<title>%1</title>\n").arg("Report") <<
////           "</head>\n"
////           "<body bgcolor = #ffffff link=#500A0>\n"
////           "<table border=1 cellspacing=0 cellpading=2>\n";

////    out << "<thead><tr bgcolor=#f0f0f0>";
////    for(int column = 0; column < columnCount; column++)
////    {
////        if(!ui->tV_awards->isColumnHidden(column)) out << QString("<th>%1</th>").arg(ui->tV_awards->model()->headerData(column, Qt::Horizontal).toString());
////    }
////    out << "</tr></thead>\n";

////    for(int row = 0; row < rowCount; row++)
////    {
////        out << "<tr>";
////        for(int column = 0; column < columnCount; column++)
////        {
////            if(!ui->tV_awards->isColumnHidden(column))
////            {
////                QString data = ui->tV_awards->model()->data(ui->tV_awards->model()->index(row, column)).toString().simplified();
////                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty())? data:QString("&nbsp;"));
////            }
////        }
////        out << "</tr>\n";
////    }
////    out << "</table\n>""</body>\n""</html>\n";
////    qDebug() << strStream;
//    QTextDocument* document = new QTextDocument();
//    qDebug() << document->size();
//    document->setHtml(create_HTML(strStream));

//    QTextEdit *pte = new QTextEdit;
//    pte->setDocument(document);
//    //pte->setHtml(strStream);
//    pte->setStyleSheet("background-color: rgb(255, 255, 255);");
//    pte->showMaximized();
//    //QTextBrowser *browser = new QTextBrowser;
//    //browser->setText(strStream);
//    //browser->showMaximized();
////создание диалога сохранения файла
//    QString filters("Текстовые файлы (*.odt);;Веб файлы (*.html);;Все файлы (*.*)");
//    QString defaultFilter("Текстовые файлы (*.odt)");
//    QFileDialog fileDialog(0, "Сохранить наряд", QDir::currentPath(), filters);
//    fileDialog.selectNameFilter(defaultFilter);
//    fileDialog.exec();
//    QString file_name = fileDialog.selectedFiles().last();
//    //QString file_name = QFileDialog::getSaveFileName(0, "Сохранить наряд", QDir::currentPath(), filters, &defaultFilter);
//    if(!file_name.isEmpty())\
//    {
//        QFile file(file_name);
//        if(file.open(QIODevice::WriteOnly | QIODevice::Append))
//        {

//            file.write(document->toHtml().toLocal8Bit().data());
//            file.close();
//        }
//    }
////    QPrinter printer(QPrinter::PrinterResolution);
//    //QPageSize A4;
//    //A4 = QPageSize::A4;
//    //printer.setPageSize(A4);
//    //printer.setOutputFileName("1111.pdf");
//    //printer.setPageMargins(QMargins(0, 0, 0, 0));

////    QPrintDialog *dialog = new QPrintDialog(&printer, 0);
////    if(dialog->exec() == QDialog::Accepted)
////    {
////        pte->print(&printer);
////        //document->print(&printer);
////        pte->hide();
////        //browser->hide();
////    }
//    delete document;
////    }
//    //::g_main_window->statusBar_show_message("Отчет сформирован");
//}
