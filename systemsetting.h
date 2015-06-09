#ifndef SYSTEMSETTING_H
#define SYSTEMSETTING_H

#include "CommonSetting.h"

namespace Ui {
class SystemSetting;
}

class SystemSetting : public QDialog
{
    Q_OBJECT

public:
    explicit SystemSetting(QWidget *parent = 0);
    ~SystemSetting();
    void LoadDefaultConfig();

private slots:
    void on_btnSave_clicked();
    void on_btnClose_clicked();
    void slotChangeButtonState(QString str);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

signals:
    void signalControlStateEnable();

private:
    Ui::SystemSetting *ui;

    QPoint mousePoint;
    bool mousePressed;
};

#endif // SYSTEMSETTING_H
