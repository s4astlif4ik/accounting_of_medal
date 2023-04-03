#ifndef FORM_STATISTIC_H
#define FORM_STATISTIC_H

#include <QWidget>

namespace Ui {
class Form_statistic;
}

class Form_statistic : public QWidget
{
    Q_OBJECT

public:
    explicit Form_statistic(QWidget *parent = 0);
    ~Form_statistic();

private:
    Ui::Form_statistic *ui;
};

#endif // FORM_STATISTIC_H
