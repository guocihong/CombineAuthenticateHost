#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>

namespace Ui {
class MessageDialog;
}

class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    enum MsgType{
        information,
        critical,
        question
    };

    explicit MessageDialog(enum MsgType MsgTypeFlag,QString text,QWidget *parent = 0);
    ~MessageDialog();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_btnOK_clicked();
    void on_btnCancel_clicked();

public:
    Ui::MessageDialog *ui;

    QPoint mousePoint;
    bool mousePressed;
};

#endif // MESSAGEDIALOG_H
