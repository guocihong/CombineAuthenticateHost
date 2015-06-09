#include "systemsetting.h"
#include "ui_systemsetting.h"
#include "MessageDialog/messagedialog.h"

SystemSetting::SystemSetting(QWidget *parent) :
    QDialog(parent,Qt::Dialog),
    ui(new Ui::SystemSetting)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->label_title->installEventFilter(this);

    connect(ui->ServerIP,SIGNAL(textChanged(QString)),this,SLOT(slotChangeButtonState(QString)));
    connect(ui->ServerListenPort,SIGNAL(textChanged(QString)),this,SLOT(slotChangeButtonState(QString)));
    connect(ui->DevelopBoardIP,SIGNAL(textChanged(QString)),this,SLOT(slotChangeButtonState(QString)));
    connect(ui->DevelopBoardMask,SIGNAL(textChanged(QString)),this,SLOT(slotChangeButtonState(QString)));
    connect(ui->DevelopBoardGateway,SIGNAL(textChanged(QString)),this,SLOT(slotChangeButtonState(QString)));
    connect(ui->DevelopBoardDNS,SIGNAL(textChanged(QString)),this,SLOT(slotChangeButtonState(QString)));

    LoadDefaultConfig();
}

SystemSetting::~SystemSetting()
{
    delete ui;
}

void SystemSetting::on_btnSave_clicked()
{
    QString ServerIP = ui->ServerIP->text();
    QString ServerListenPort = ui->ServerListenPort->text();

    QString DevelopBoardIP = ui->DevelopBoardIP->text();
    QString DevelopBoardMask = ui->DevelopBoardMask->text();
    QString DevelopBoardGateway = ui->DevelopBoardGateway->text();
    QString DevelopBoardDNS = ui->DevelopBoardDNS->text();

    CommonSetting::WriteSettings("/bin/config.ini","ServerNetwork/IP",ServerIP);
    CommonSetting::WriteSettings("/bin/config.ini","ServerNetwork/PORT",ServerListenPort);

    QStringList DevelopConfigureInfo;
    DevelopConfigureInfo
            << tr("IP=%1\n").arg(DevelopBoardIP)
            << tr("Mask=%1\n").arg(DevelopBoardMask)
            << tr("Gateway=%1\n").arg(DevelopBoardGateway)
            << tr("DNS=%1\n").arg(DevelopBoardDNS);

    QString ConfigureInfo = DevelopConfigureInfo.join("");

    QString fileName("/etc/eth0-setting");
    CommonSetting::WriteCommonFile(fileName,ConfigureInfo);

    MessageDialog message_dialog(MessageDialog::information,tr("配置成功,系统自动重启"));
    message_dialog.exec();
    system("reboot");
}

void SystemSetting::LoadDefaultConfig()
{
    QString ServerIP = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/IP");
    QString ServerListenPort = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/PORT");

    ui->ServerIP->setText(ServerIP);
    ui->ServerListenPort->setText(ServerListenPort);

    QString DevelopBoardIP,DevelopBoardGateway,
            DevelopBoardMask,DevelopBoardDNS;

    QFile file("/etc/eth0-setting");
    if(file.open(QFile::ReadOnly)){
        QTextStream in(&file);
        DevelopBoardIP = in.readLine().mid(3);
        DevelopBoardMask = in.readLine().mid(5);
        DevelopBoardGateway = in.readLine().mid(8);
        DevelopBoardDNS = in.readLine().mid(4);
        file.close();
    }

    ui->DevelopBoardIP->setText(DevelopBoardIP);
    ui->DevelopBoardMask->setText(DevelopBoardMask);
    ui->DevelopBoardGateway->setText(DevelopBoardGateway);
    ui->DevelopBoardDNS->setText(DevelopBoardDNS);
}

void SystemSetting::on_btnClose_clicked()
{
    this->close();
    emit signalControlStateEnable();
}

void SystemSetting::slotChangeButtonState(QString str)
{
    if(ui->ServerIP->text().isEmpty() || ui->ServerListenPort->text().isEmpty() || ui->DevelopBoardIP->text().isEmpty() || ui->DevelopBoardMask->text().isEmpty() || ui->DevelopBoardGateway->text().isEmpty()){
        ui->btnSave->setDisabled(true);
    }else{
        ui->btnSave->setEnabled(true);
    }
}

bool SystemSetting::eventFilter(QObject *obj, QEvent *event)
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
