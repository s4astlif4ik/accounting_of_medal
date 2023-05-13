#ifndef FRM_STATISTIC_H
#define FRM_STATISTIC_H

#include <QWidget>

namespace Ui {
class FRM_statistic;
}

class FRM_statistic : public QWidget
{
    Q_OBJECT

public:
    explicit FRM_statistic(QWidget *parent = 0);
    ~FRM_statistic();

private:
    Ui::FRM_statistic *ui;
};

#endif // FRM_STATISTIC_H
