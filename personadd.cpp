#include "personadd.h"
#include "ui_personadd.h"

using namespace std;
using namespace Json;

PersonAdd::PersonAdd(QWidget *parent) :
    QDialog(parent,Qt::Dialog),
    ui(new Ui::PersonAdd)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    ui->lab_Title->installEventFilter(this);
    IconHelper::Instance()->SetIcon(ui->lab_Ico, QChar(0xf1a5),14);

    isOver = false;

    ServerIP = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/IP");
    ServerListenPort = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/PORT");

    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(slotReplyFinished(QNetworkReply*)));

    ui->Deadline->setDateTime(QDateTime::currentDateTime());
    getFunctionType();
}

PersonAdd::~PersonAdd()
{
    delete ui;
}

void PersonAdd::on_btnClose_clicked()
{
    this->close();
    emit signalControlStateEnable();
}

void PersonAdd::SetWindowTitle(QString title)
{
    ui->lab_Title->setText(title);
}

void PersonAdd::SetData(QString CardID)
{
//    QUrl getPersonInfo = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getPersonInfo\":[\"CardID=%1\"]}").arg(CardID));
//    reply = manager->get(QNetworkRequest(getPersonInfo));
//    type = PersonAdd::GetBasicInfo;
//    qDebug() << getPersonInfo;

    ui->CardID->setText(CardID);
    on_btnQuery_clicked();
}

bool PersonAdd::eventFilter(QObject *obj, QEvent *event)
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

void PersonAdd::getFunctionType()
{
    QUrl getFunctionType = QUrl(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getFunctionType\":null}");
    reply = manager->get(QNetworkRequest(getFunctionType));
    type = PersonAdd::GetBasicInfo;
//    qDebug() << getFunctionType;
}

void PersonAdd::on_btnQuery_clicked()
{
    ui->CardID->selectAll();
    if(!ui->CardID->text().isEmpty() && ui->CardID->text().size() == 8)
    {
        QUrl getPersonInfo = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getPersonInfo\":[\"CardID=%1\"]}").arg(ui->CardID->text()));
        reply = manager->get(QNetworkRequest(getPersonInfo));
        type = PersonAdd::GetBasicInfo;
//        qDebug() << getPersonInfo;
    }else{
        ui->PersonName->clear();
        ui->Sex->clear();
        ui->BirthDay->clear();
        ui->Nation->clear();
        ui->HomeAddress->clear();
        ui->CompanyCode->clear();
        ui->ValidPeriod->clear();
        ui->IdentityCardNumber->clear();
        ui->IssueAuthority->clear();
        ui->PhoneNumber->clear();
        ui->Deadline->setDateTime(QDateTime::currentDateTime());
        ui->CompanyName->clear();
        ui->PersonID->clear();
        ui->CardID->clear();
        ui->person_pic_label->clear();
    }
}

void PersonAdd::slotReplyFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){
        if(type == PersonAdd::GetBasicInfo){
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            QString JsonData = codec->toUnicode(reply->readAll());
//            qDebug() << JsonData;

            Json::Reader JsonReader;
            Json::Value JsonValue;

            if(JsonReader.parse(JsonData.toStdString(),JsonValue)){
                const Json::Value FunctionTypeArray =
                        JsonValue["FunctionType"];
                if(FunctionTypeArray.type() == Json::arrayValue){
                    for(unsigned int i = 0; i < FunctionTypeArray.size(); i++)
                    {
                        ui->FunctionTypeFilterComboBox->addItem(QString(FunctionTypeArray[i][1].asString().data()) + QString("(") + QString::number(FunctionTypeArray[i][0u].asUInt()) + QString(")"));
                    }
                    connect(ui->FunctionTypeFilterComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotFunctionTypeFilterComboBoxCurrentIndexChanged(int)));

                    isOver = true;
                }

                const Json::Value PersonInfoArray = JsonValue["PersonInfo"];
                if(PersonInfoArray.type() == Json::arrayValue){
                    ui->PersonName->setText(QString(PersonInfoArray[1][0u].asString().data()));
                    ui->Sex->setText(QString(PersonInfoArray[1][8].asString().data()));
                    ui->BirthDay->setText(QString(PersonInfoArray[1][9].asString().data()));
                    ui->Nation->setText(QString(PersonInfoArray[1][10].asString().data()));
                    ui->HomeAddress->setText(QString(PersonInfoArray[1][11].asString().data()));
                    ui->CompanyCode->setText(QString(PersonInfoArray[1][5].asString().data()));
                    ui->ValidPeriod->setText(QString(PersonInfoArray[1][12].asString().data()));
                    ui->IdentityCardNumber->setText(QString(PersonInfoArray[1][2].asString().data()));
                    ui->IssueAuthority->setText(QString(PersonInfoArray[1][13].asString().data()));
                    ui->PhoneNumber->setText(QString(PersonInfoArray[1][3].asString().data()));
                    ui->Deadline->setDateTime(QDateTime::fromString(  QString(PersonInfoArray[1][7].asString().data()),"yyyy-MM-dd"));
                    ui->PersonID->setText(QString::number(PersonInfoArray[1][19].asInt()));
                    ui->CompanyName->setText(QString(PersonInfoArray[1][4].asString().data()));
                    ui->CardID->setText(QString(PersonInfoArray[1][1].asString().data()));
                    ui->CardID->selectAll();

                    PersonPicUrl = QUrl(QString(PersonInfoArray[1][17].asString().data()));

                    const Json::Value PersonTypeArray = PersonInfoArray[1][18];
                    if(PersonTypeArray.type() == Json::arrayValue){
                        FunctionTypeID = QString::number(PersonTypeArray[0u].asInt()).mid(0,1).toInt();
                        FunctionTypeList.clear();
                        for(qint8 i = 0; i < PersonTypeArray.size(); i++)
                        {
                            FunctionTypeList << QString::number(PersonTypeArray[i].asInt());
                        }
                    }
                    reply->deleteLater();

                    ui->FunctionTypeFilterComboBox->setCurrentIndex(FunctionTypeID - 1);
                    if(FunctionTypeID == 1){
                        slotFunctionTypeFilterComboBoxCurrentIndexChanged(0);
                    }
                    return;
                }else if(PersonInfoArray.type() == Json::objectValue){
                    ui->PersonName->clear();
                    ui->Sex->clear();
                    ui->BirthDay->clear();
                    ui->Nation->clear();
                    ui->HomeAddress->clear();
                    ui->CompanyCode->clear();
                    ui->ValidPeriod->clear();
                    ui->IdentityCardNumber->clear();
                    ui->IssueAuthority->clear();
                    ui->PhoneNumber->clear();
                    ui->Deadline->setDateTime(QDateTime::currentDateTime());
                    ui->CompanyName->clear();
                    ui->PersonID->clear();
                    ui->CardID->clear();
                    ui->person_pic_label->clear();
                }

                const Json::Value PersonTypeArray =
                        JsonValue["PersonType"];
                if(PersonTypeArray.type() == Json::arrayValue){
                    if(PersonTypeArray.size()){
                        for(qint8 i = 0; i < cb_list.size(); i++){
                            delete cb_list.at(i);
                        }
                        cb_list.clear();

                        char row = 0,col = 0;
                        QCheckBox *cb;
                        for(unsigned int i = 0; i < PersonTypeArray.size(); i++){
                            cb = new QCheckBox(PersonTypeArray[i][1].asString().data());
                            cb->setFocusPolicy(Qt::NoFocus);

                            for(qint8 index = 0; index < FunctionTypeList.size(); index++){
                                if(QString(PersonTypeArray[i][2].asString().data()) == FunctionTypeList.at(index)){
                                    cb->setChecked(true);
                                }
                            }
                            ui->RoleGridLayout->addWidget(cb,row,col);
                            cb_list.append(cb);
                            col++;
                            if(col == 4){
                                col = 0;
                                row++;
                            }
                        }
                    }
                }

                const Json::Value PersonStateObject =
                        JsonValue["PersonState"];
                if(PersonStateObject.type() == Json::objectValue){
                    if(PersonStateObject.size()){
                        QString setPerson =
                                QString(PersonStateObject["setPerson"].asString().data());
                            MessageDialog message_dialog(MessageDialog::information,setPerson);
                            message_dialog.exec();
                    }
                }

//                if(!PersonPicUrl.isEmpty()){
//                    reply->deleteLater();
//                    reply = manager->get(QNetworkRequest(PersonPicUrl));
//                    type = PersonAdd::GetPersonPic;
//                    qDebug() << PersonPicUrl;
//                    PersonPicUrl.clear();
//                    return;
//                }
            }
        }else if(type == PersonAdd::GetPersonPic){
            QByteArray pic_data = reply->readAll();
            if(!pic_data.isEmpty()){
                QImage image;
                image.loadFromData(pic_data);
                QImage imgScaled = image.scaled(
                            ui->person_pic_label->width(),
                            ui->person_pic_label->height(),
                            Qt::IgnoreAspectRatio);
                ui->person_pic_label->setPixmap(
                            QPixmap::fromImage(imgScaled));
            }
            type = PersonAdd::GetBasicInfo;
        }
    }

    reply->deleteLater();
}

void PersonAdd::slotFunctionTypeFilterComboBoxCurrentIndexChanged(int index)
{
    ui->CardID->selectAll();
    for(qint8 i = 0; i < cb_list.size(); i++){
        delete cb_list.at(i);
    }
    cb_list.clear();
    QUrl getPersonType = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getPersonType\":[%1]}").arg(ui->FunctionTypeFilterComboBox->currentIndex() + 1));
    reply = manager->get(QNetworkRequest(getPersonType));
    type = PersonAdd::GetBasicInfo;
//    qDebug() << getPersonType;
}

void PersonAdd::on_btnSave_clicked()
{
    QString PersonID = ui->PersonID->text();
    qint8 Role = ui->FunctionTypeFilterComboBox->currentIndex() + 1;
    QString ValidPeriod = ui->Deadline->text();

    QStringList type_list;
    for(qint8 i = 0; i < cb_list.size(); i++){
        if(cb_list.at(i)->isChecked()){
            type_list << QString::number(Role) + QString("00") + QString::number(i + 1);
        }
    }


    QUrl setPerson = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"setPerson\":[\"PersonID=%1\",\"Role=%2\",\"Type=%3\",\"ValidPeriod=%4\"]}").arg(PersonID).arg(Role).arg(type_list.join(",")).arg(ValidPeriod));
    reply = manager->get(QNetworkRequest(setPerson));
    type = PersonAdd::GetBasicInfo;
//    qDebug() << setPerson;
}

void PersonAdd::on_btnDownloadPic_clicked()
{
    if(!PersonPicUrl.isEmpty()){
        reply = manager->get(QNetworkRequest(PersonPicUrl));
        type = PersonAdd::GetPersonPic;
//        qDebug() << PersonPicUrl;
        PersonPicUrl.clear();
    }
}
