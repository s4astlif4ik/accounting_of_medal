#ifndef FRM_RECEIWING_H
#define FRM_RECEIWING_H

#include <QWidget>
#include <QAction>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QCompleter>
#include <QDebug>
#include <QProgressBar>
#include <QSortFilterProxyModel>
#include <QTime>

namespace Ui {
class FRM_receiwing;
}

class FRM_receiwing : public QWidget
{
    Q_OBJECT
public slots:

    void update_models();

public:
    explicit FRM_receiwing(QWidget *parent = 0);
    ~FRM_receiwing();

    QCompleter *comp = new QCompleter();

    QIcon button_ok;

    QIcon combobox_icon;

    QTime t;

private slots:
    void on_le_award_name_returnPressed();

    //void f_ls_for_award_type();

    void f_ls_for_award_name();

    int on_bnt_save_award_clicked();

    void on_bnt_clear_award_clicked();

    void on_cB_doc_currentIndexChanged(int index);

    void on_rb_numbers_toggled(bool checked);

    void on_cB_award_category_currentIndexChanged(int index);

    void on_le_award_first_num_editingFinished();

    bool add_awards(QString awards_number);

    void on_cB_award_type_currentIndexChanged(int index);

    void clear_le_award_name();

    int awards_count();

    void on_le_award_last_num_textChanged(const QString &arg1);

    int on_bnt_print_report_clicked();

private:
    Ui::FRM_receiwing *ui;

    QAction *act_award_type;
    QAction *act_award_name;

    QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
    QSqlQueryModel *model_awards = new QSqlQueryModel();
    QSqlQueryModel *model_incoming_doc = new QSqlQueryModel();
    QSqlQueryModel *model_type_award = new QSqlQueryModel();
    QSqlQueryModel *model_name_award = new QSqlQueryModel();
    QSqlQueryModel *model_storage = new QSqlQueryModel();
};

#endif // FRM_RECEIWING_H
