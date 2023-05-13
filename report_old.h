#ifndef REPORT_H
#define REPORT_H

#include <QString>
#include <QList>

class REPORT
{

public:
    REPORT();

    QString create_HTML(QString content);

    QString create_centered_paragraph(QString content);

    QString create_left_underlined_paragraph(QString content);

    QString create_signature_paragraph(QString position, QString full_name);

    QString create_docnumber(QString docnumber);

    QString create_table(QString row_content);

    QString create_full_content_row_for_table(QString content);

    QString create_row_for_table(QStringList content_list);

    void print_report();
};

#endif // REPORT_H
