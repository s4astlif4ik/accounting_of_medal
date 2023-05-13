#ifndef FRM_SENDERING_H
#define FRM_SENDERING_H

#include <QWidget>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QItemSelectionModel>

namespace Ui {
class FRM_sendering;
}

class FRM_sendering : public QWidget
{
    Q_OBJECT

public slots:

    void update_models();

    QStringList get_outgoing_doc_details();

public:
    explicit FRM_sendering(QWidget *parent = 0);
    ~FRM_sendering();

private slots:
    void on_cB_award_category_currentIndexChanged(int index);

    void on_cB_outgoing_doc_currentIndexChanged(int index);

    void on_btn_add_selected_awards_clicked();

    bool add_outgoing_award(QModelIndex index);

    void on_tV_awards_in_doubleClicked(const QModelIndex &index);

    void on_bnt_outgoing_save_clicked();

    void on_bnt_clean_clicked();

    void on_bnt_print_report_clicked();

private:
    Ui::FRM_sendering *ui;

    QSqlQueryModel *model_award_type = new QSqlQueryModel();

    QSqlQueryModel *model_outgoing_doc = new QSqlQueryModel();

    QSqlQueryModel *model_awards_in = new QSqlQueryModel();

    QSqlQueryModel *model_awards_out = new QSqlQueryModel();

    QItemSelectionModel *select = new QItemSelectionModel;
};

#endif // FRM_SENDERING_H
