#ifndef REPORT_H
#define REPORT_H

#include <QString>
#include <QStringList>
#include <QSqlQueryModel>


class REPORT
{
public slots:

    QString create_HTML(QString content);

    QString insert_paragraph(QString text = "", QString align = "left", int font_size = 14, int size = 4,
                             QString color = "#000000", QString font = "Times New Roman");

    QString insert_signature_paragraph(QString position, QString full_name);

    QString insert_table(QString row_content);

    QString insert_full_content_row_for_table(QString content);

    QString insert_row_for_table(QStringList content_list);

    QString int2string(int value);

    QString load_outgoing_doc_report(QStringList document_details);

public:
    REPORT();

    QSqlQueryModel *model_report_receiver = new QSqlQueryModel;
};

#endif // REPORT_H
