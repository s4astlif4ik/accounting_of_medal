#ifndef DLG_REPORT_H
#define DLG_REPORT_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QSqlQueryModel>

namespace Ui {
class DLG_report;
}

class DLG_report : public QDialog
{
    Q_OBJECT

public slots:

    QString create_HTML(QString content);

    QString insert_paragraph(QString text = "", QString align = "left", int font_size = 14, int size = 4,
                             QString color = "#000000", QString font = "Times New Roman");

    QString insert_centered_paragraph(QString content);

    QString insert_left_underlined_paragraph(QString content);

    QString insert_signature_paragraph(QString position, QString full_name);

    QString insert_docnumber(QString docnumber);

    QString insert_table(QString row_content);

    QString insert_full_content_row_for_table(QString content);

    QString insert_row_for_table(QStringList content_list);

    QString int2string(int value);

    void load_report();

public:
    explicit DLG_report(QWidget *parent = 0);
    ~DLG_report();
    QSqlQueryModel model_report_receiver;

private slots:
    void on_buttonBox_accepted();

    void on_bnt_outgoing_save_clicked();

    void on_bnt_outgoing_save_2_clicked();

private:
    Ui::DLG_report *ui;
};

#endif // DLG_REPORT_H
