#ifndef DEVICEADD_H
#define DEVICEADD_H

#include <QDialog>

namespace Ui {
class DeviceAdd;
}

class DeviceAdd : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceAdd(QWidget *parent = 0);
    ~DeviceAdd();
    void SetWindowTitle(QString title);

private slots:
    void on_btnClose_clicked();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::DeviceAdd *ui;

    QPoint mousePoint;
    bool mousePressed;
};

#endif // DEVICEADD_H
