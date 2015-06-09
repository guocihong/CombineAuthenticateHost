#ifndef COMPANYREGISTER_H
#define COMPANYREGISTER_H

#include <QDialog>

namespace Ui {
class CompanyRegister;
}

class CompanyRegister : public QDialog
{
    Q_OBJECT

public:
    explicit CompanyRegister(QWidget *parent = 0);
    ~CompanyRegister();
    void SetWindowTitle(QString title);

private slots:
    void on_btnClose_clicked();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::CompanyRegister *ui;

    QPoint mousePoint;
    bool mousePressed;
};

#endif // COMPANYREGISTER_H
