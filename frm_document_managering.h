#ifndef FRM_DOCUMENT_MANAGERING_H
#define FRM_DOCUMENT_MANAGERING_H

#include <QWidget>
#include <QCompleter>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

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

    void on_cB_type_operation_currentTextChanged(const QString &arg1);

    void on_tV_documents_doubleClicked(const QModelIndex &index);

    void load_documents_table(QString text);

private:
    Ui::FRM_document_managering *ui;
    QSqlQueryModel *model_documents = new QSqlQueryModel();
    QSqlQueryModel *model_organization = new QSqlQueryModel();
    QSqlQueryModel *model_receiver = new QSqlQueryModel();
    QSqlQueryModel *model_sender = new QSqlQueryModel();
};

#endif // FRM_DOCUMENT_MANAGERING_H
