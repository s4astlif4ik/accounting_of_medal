#ifndef FRM_DOCUMENT_MANAGERING_H
#define FRM_DOCUMENT_MANAGERING_H

#include <QWidget>
#include <QCompleter>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QUuid>

namespace Ui {
class FRM_document_managering;
}

class FRM_document_managering : public QWidget
{
    Q_OBJECT
public slots:

    void models_update();

public:
    explicit FRM_document_managering(QWidget *parent = 0);
    ~FRM_document_managering();

private slots:
    int on_bnt_save_doc_clicked();

    void on_bnt_clear_doc_clicked();

    void on_tV_documents_doubleClicked(const QModelIndex &index);

    void load_documents_table(int awards_category, int type);

    void on_le_receiver_name_editingFinished();

    int find_organizations_id(QString fullname);

    void on_le_sender_name_editingFinished();

    void on_cB_awards_category_currentIndexChanged(int index);

    void on_cB_type_operation_currentIndexChanged(int index);

    int get_free_inputnumber();

    void slotCustomMenuRequested(QPoint pos);

    void set_filter_from_table();

private:
    Ui::FRM_document_managering *ui;

    QSqlQueryModel *model_documents = new QSqlQueryModel();

    QSqlQueryModel *model_organization = new QSqlQueryModel();

};

#endif // FRM_DOCUMENT_MANAGERING_H
