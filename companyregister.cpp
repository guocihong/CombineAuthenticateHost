#include "companyregister.h"
#include "ui_companyregister.h"
#include <QMouseEvent>

CompanyRegister::CompanyRegister(QWidget *parent) :
    QDialog(parent,Qt::Dialog),
    ui(new Ui::CompanyRegister)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->lab_Title->installEventFilter(this);
}

CompanyRegister::~CompanyRegister()
{
    delete ui;
}

void CompanyRegister::on_btnClose_clicked()
{
    this->close();
}

void CompanyRegister::SetWindowTitle(QString title)
{
    ui->lab_Title->setText(title);
}

bool CompanyRegister::eventFilter(QObject *obj, QEvent *event)
{
    QMouseEvent *MouseEvent = static_cast<QMouseEvent*>(event);
    if(obj == ui->lab_Title){
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
