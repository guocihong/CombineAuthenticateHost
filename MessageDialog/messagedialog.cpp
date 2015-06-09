#include "messagedialog.h"
#include "ui_messagedialog.h"
#include "QDesktopWidget"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QLayout>

MessageDialog::MessageDialog(enum MsgType MsgTypeFlag, QString text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    if(MsgTypeFlag == MessageDialog::information){
        ui->label_title->setText(tr("提示"));
        ui->label_info_icon->setStyleSheet(tr("border-image:url(:/image/images/information.png)"));
        ui->btnCancel->setVisible(false);
    }else if(MsgTypeFlag == MessageDialog::critical){
        ui->label_title->setText(tr("错误"));
        ui->label_info_icon->setStyleSheet(tr("border-image:url(:/image/images/critical.png)"));
        ui->btnCancel->setVisible(false);
    }else if(MsgTypeFlag == MessageDialog::question){
        ui->label_title->setText(tr("询问"));
        ui->label_info_icon->setStyleSheet(tr("border-image:url(:/image/images/question.png)"));
        ui->btnCancel->setVisible(true);
    }

    ui->label_info->setText(text);

    this->installEventFilter(this);
    ui->label_title->installEventFilter(this);
}

MessageDialog::~MessageDialog()
{
    delete ui;
}

void MessageDialog::on_btnOK_clicked()
{
    done(1);
    this->close();
}

void MessageDialog::on_btnCancel_clicked()
{
    done(0);
    this->close();
}

bool MessageDialog::eventFilter(QObject *obj, QEvent *event)
{
    QMouseEvent *MouseEvent = static_cast<QMouseEvent*>(event);
    if(obj == ui->label_title){
        if(MouseEvent->buttons() == Qt::LeftButton){
            if(event->type() == QEvent::MouseButtonPress){
                mousePressed = true;
                mousePoint = MouseEvent->globalPos() - this->pos();
                return true;
            }else if(event->type() == QEvent::MouseMove){
                if(mousePressed){
                    this->move(MouseEvent->globalPos() - mousePoint);
                    return true;
                }
            }else if(event->type() == QEvent::MouseButtonRelease){
                mousePressed = false;
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj,event);
}
