#ifndef DLG_RECEIVING_H
#define DLG_RECEIVING_H

#include <QDialog>

namespace Ui {
class DLG_receiving;
}

class DLG_receiving : public QDialog
{
    Q_OBJECT

public:
    explicit DLG_receiving(QWidget *parent = 0);
    ~DLG_receiving();

private:
    Ui::DLG_receiving *ui;
};

#endif // DLG_RECEIVING_H
