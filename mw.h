#ifndef MW_H
#define MW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QProgressBar>

namespace Ui {
class MW;
}

class MW : public QMainWindow
{
    Q_OBJECT

public slots:

    bool load_tree_view();

    void statusBar_set_ProgressBar(QString text, QProgressBar *pb);

    void statusBar_show_message(QString text);

    void statusBar_remove_ProgressBar();


private slots:

    void show_dW_filter();

    void show_dW_doc_managering();

    void show_dW_receiving();

    void show_dW_extradition();

    void show_receiving_form();

    void on_trw_statistic_expanded(const QModelIndex &index);

    void on_trw_statistic_clicked(const QModelIndex &index);

    void on_trw_statistic_doubleClicked(const QModelIndex &index);

    void slotCustomMenuRequested(QPoint pos);

    void resize_to_content();

public:
    explicit MW(QWidget *parent = 0);
    ~MW();

    QWidget *filter_form;

    QMenu *menu0;

    QSqlQueryModel *medals = new QSqlQueryModel();

    QStandardItemModel *sim = new QStandardItemModel;

    QSortFilterProxyModel *proxy;// = new QSortFilterProxyModel(this);

private:
    Ui::MW *ui;
};

#endif // MW_H
