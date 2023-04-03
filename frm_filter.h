#ifndef FRM_FILTER_H
#define FRM_FILTER_H

#include <QWidget>
#include <QTreeView>

namespace Ui {
class FRM_filter;
}

class FRM_filter : public QWidget
{
    Q_OBJECT

public:
    explicit FRM_filter(QWidget *parent = 0);
    ~FRM_filter();

private slots:
    void on_lE_text_filter_textChanged(const QString &arg1);

    void on_cB_period_filter_currentIndexChanged(int index);

    void on_btn_update_tree_clicked();

private:
    Ui::FRM_filter *ui;
};

#endif // FRM_FILTER_H
