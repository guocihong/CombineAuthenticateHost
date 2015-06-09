#include "mainform.h"
#include "ui_mainform.h"

using namespace std;
using namespace Json;

MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->stackedWidget->setCurrentIndex(0);

    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(slotReplyFinished(QNetworkReply*)));

    CreateCompanyManagerMenu();
    CreatePersonManagerMenu();
    CreateFrontManagerMenu();

    ServerIP = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/IP");
    ServerListenPort = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/PORT");

    type = MainForm::GetOtherInfo;

    getArea();
    getFunctionType();
    getDateTime();

    system_setting = new SystemSetting;
    connect(system_setting,SIGNAL(signalControlStateEnable()),this,SLOT(slotControlStateEnable()));

    SystemDateTimer = new QTimer(this);
    SystemDateTimer->setInterval(1000);
    connect(SystemDateTimer,SIGNAL(timeout()),this,SLOT(slotUpdateSystemDate()));
    SystemDateTimer->start();

    SelectedRowIndex = -1;
}

MainForm::~MainForm()
{
    delete ui;
    delete system_setting;
}

void MainForm::on_btnCompanyManager_clicked()
{
    QPoint point = QPoint(ui->btnCompanyManager->pos().x(),ui->btnCompanyManager->rect().bottom());
    CompanyManagerMenu->exec(this->mapToGlobal(point));
}

void MainForm::on_btnPersonManager_clicked()
{
    QPoint point = QPoint(ui->btnPersonManager->pos().x(),ui->btnPersonManager->rect().bottom());
    PersonManagerMenu->exec(this->mapToGlobal(point));}

void MainForm::on_btnFrontManager_clicked()
{
    QPoint point = QPoint(ui->btnFrontManager->pos().x(),ui->btnFrontManager->rect().bottom());
    FrontManagerMenu->exec(this->mapToGlobal(point));}

void MainForm::on_btnRecordQueryManager_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainForm::on_btnSystemSettingManager_clicked()
{
    system_setting->show();
    slotControlStateDisable();
}

void MainForm::CreateCompanyManagerMenu()
{
    CompanyManagerMenu = new QMenu(this);
    CompanyManagerMenu->setFixedWidth(ui->btnCompanyManager->width());

    CompanyListAction = CompanyManagerMenu->addAction(tr("单位列表"));
    CompanyManagerMenu->addSeparator();
    CompanyRegisterAction = CompanyManagerMenu->addAction(tr("单位注册"));
    CompanyEditAction = CompanyManagerMenu->addAction(tr("单位编辑"));

    connect(CompanyListAction,SIGNAL(triggered()),this,SLOT(slotShowCompanyList()));
//    connect(CompanyRegisterAction,SIGNAL(triggered()),this,SLOT(slotShowCompanyRegister()));
//    connect(CompanyEditAction,SIGNAL(triggered()),this,SLOT(slotCompanyEdit()));
}

void MainForm::CreatePersonManagerMenu()
{
    PersonManagerMenu = new QMenu(this);
    PersonManagerMenu->setFixedWidth(ui->btnPersonManager->width());

    PersonListAction = PersonManagerMenu->addAction(tr("人员列表"));
    PersonManagerMenu->addSeparator();
    PersonAddAction = PersonManagerMenu->addAction(tr("人员添加"));
    PersonEditAction = PersonManagerMenu->addAction(tr("人员编辑"));

    connect(PersonListAction,SIGNAL(triggered()),this,SLOT(slotShowPersonList()));
    connect(PersonAddAction,SIGNAL(triggered()),this,SLOT(slotShowPersonAdd()));
    connect(PersonEditAction,SIGNAL(triggered()),this,SLOT(slotPersonEdit()));
}

void MainForm::CreateFrontManagerMenu()
{
    FrontManagerMenu = new QMenu(this);
    FrontManagerMenu->setFixedWidth(ui->btnFrontManager->width());

    DeviceStatusAction = FrontManagerMenu->addAction(tr("设备列表"));
    FrontManagerMenu->addSeparator();
    DeviceAddAction = FrontManagerMenu->addAction(tr("设备添加"));
    DeviceEditAction = FrontManagerMenu->addAction(tr("设备编辑"));
    FrontManagerMenu->addSeparator();
    PersonBindAction = FrontManagerMenu->addAction(tr("人员绑定"));

    connect(DeviceStatusAction,SIGNAL(triggered()),this,SLOT(slotShowDeviceStatus()));
//    connect(DeviceAddAction,SIGNAL(triggered()),this,SLOT(slotShowDeviceAdd()));
//    connect(DeviceEditAction,SIGNAL(triggered()),this,SLOT(slotDeviceEdit()));
    connect(PersonBindAction,SIGNAL(triggered()),this,SLOT(slotPersonBind()));

}

void MainForm::slotShowCompanyList()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainForm::slotShowCompanyRegister()
{
    CompanyRegister *company_register = new CompanyRegister;
    company_register->SetWindowTitle(tr("单位注册"));
    company_register->show();
}

void MainForm::slotCompanyEdit()
{
    if(SelectedRowIndex == -1){
        MessageDialog message_dialog(MessageDialog::critical,tr("请选择要编辑的单位"));
        message_dialog.exec();
        return;
    }

    CompanyRegister *company_edit = new CompanyRegister;
    company_edit->SetWindowTitle(tr("单位编辑"));
    company_edit->show();

    SelectedRowIndex = -1;
}

void MainForm::slotShowPersonList()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainForm::slotShowPersonAdd()
{
    PersonAdd *person_add = new PersonAdd;
    connect(person_add,SIGNAL(signalControlStateEnable()),this,SLOT(slotControlStateEnable()));
    person_add->SetWindowTitle(tr("人员添加"));
    person_add->show();

    slotControlStateDisable();
}

void MainForm::slotPersonEdit()
{
    if(SelectedRowIndex == -1){
        MessageDialog message_dialog(MessageDialog::critical,tr("请选择要编辑的人员"));
        message_dialog.exec();
        return;
    }

    PersonAdd *person_edit = new PersonAdd;
    connect(person_edit,SIGNAL(signalControlStateEnable()),this,SLOT(slotControlStateEnable()));
    person_edit->SetWindowTitle(tr("人员编辑"));
    person_edit->SetData(ui->PersonMainInfoTableWidget->item(SelectedRowIndex,1)->text());
    person_edit->show();

    SelectedRowIndex = -1;
    slotControlStateDisable();
}

void MainForm::slotShowDeviceStatus()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainForm::slotShowDeviceAdd()
{
    DeviceAdd *device_add = new DeviceAdd;
    device_add->SetWindowTitle(tr("设备添加"));
    device_add->show();
}

void MainForm::slotDeviceEdit()
{
    if(SelectedRowIndex == -1){
        MessageDialog message_dialog(MessageDialog::critical,tr("请选择要编辑的设备"));
        message_dialog.exec();
        return;
    }

    DeviceAdd *device_edit = new DeviceAdd;
    device_edit->SetWindowTitle(tr("设备编辑"));
    device_edit->show();

    SelectedRowIndex = -1;
}

void MainForm::slotPersonBind()
{
    if(SelectedRowIndex == -1){
        MessageDialog message_dialog(MessageDialog::critical,tr("请选择要绑定的前端设备"));
        message_dialog.exec();
        return;
    }
    PersonBind *person_bind = new PersonBind;
    person_bind->SetWindowTitle(tr("人员绑定"));
    person_bind->setWindowState(Qt::WindowMaximized);

    QStringList HorizontalHeaderLabels;
    for(int i = 0; i < ui->DeviceMainInfoTableWidget->columnCount(); i++)
    {
        HorizontalHeaderLabels << ui->DeviceMainInfoTableWidget->horizontalHeaderItem(i)->text();
    }
    QStringList SelectedRowContent;
    for(int i = 0; i < ui->DeviceMainInfoTableWidget->columnCount(); i++)
    {
        if(ui->DeviceMainInfoTableWidget->item(SelectedRowIndex,i) != NULL)
        {
            SelectedRowContent << ui->DeviceMainInfoTableWidget->item(SelectedRowIndex,i)->text();
        }else{
            SelectedRowContent << QString("");
        }
    }

    person_bind->SetData(HorizontalHeaderLabels,SelectedRowContent);
    person_bind->show();

    SelectedRowIndex = -1;
}

void MainForm::getArea()
{
    QUrl getArea = QUrl(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getArea\":null}");
    reply = manager->get(QNetworkRequest(getArea));
    type = MainForm::GetOtherInfo;
    qDebug() << getArea;
}

void MainForm::getFunctionType()
{
    QUrl getFunctionType = QUrl(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getFunctionType\":null}");
    reply = manager->get(QNetworkRequest(getFunctionType));
    type = MainForm::GetOtherInfo;
    qDebug() << getFunctionType;
}

void MainForm::getDateTime()
{
    QUrl getDateTime = QUrl(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getDateTime\":null}");
    reply = manager->get(QNetworkRequest(getDateTime));
    type = MainForm::GetOtherInfo;
    qDebug() << getDateTime;
}

void MainForm::on_btnSearchCompanyInfo_clicked()
{
    QString SearchCondition;
    QString CompanyInfoFilter = ui->CompanyInfoFilterComboBox->currentText();
    if(CompanyInfoFilter == tr("企业名称")){
        SearchCondition = QString("CompanyName");
    }else if(CompanyInfoFilter == tr("营业执照")){
        SearchCondition = QString("CompanyCode");
    }else if(CompanyInfoFilter == tr("办公地址")){
        SearchCondition = QString("CompanyAddress");
    }else if(CompanyInfoFilter == tr("联系电话")){
        SearchCondition = QString("HandPhone");
    }

    QUrl getCompany;
    QString AreaFilter = ui->CompanyAreaFilterComboBox->currentText();
    if(AreaFilter == tr("全部")){
        getCompany = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getCompany\":[\"%1=%2\"]}").arg(SearchCondition)
                          .arg(ui->CompanyInfoFilterLineEdit->text()));
    }else{
        getCompany = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getCompany\":[\"%1=%2\",\"Area=%3\"]}").arg(SearchCondition).arg(ui->CompanyInfoFilterLineEdit->text()).arg(1000 + ui->CompanyAreaFilterComboBox->currentIndex()));
    }

    reply = manager->get(QNetworkRequest(getCompany));
    type = MainForm::GetOtherInfo;
    ui->CompanyMainInfoTableWidget->clearContents();
    ui->CompanyMainInfoTableWidget->setRowCount(0);
    qDebug() << getCompany;
}

void MainForm::on_btnSearchPersonInfo_clicked()
{
    QString SearchCondition;
    QString PersonInfoFilter = ui->PersonInfoFilterComboBox->currentText();
    if(PersonInfoFilter == tr("服务单位")){
        SearchCondition = QString("CompanyName");
    }else if(PersonInfoFilter == tr("姓名")){
        SearchCondition = QString("PersonName");
    }else if(PersonInfoFilter == tr("联系手机")){
        SearchCondition = QString("PersonHD");
    }else if(PersonInfoFilter == tr("身份证号")){
        SearchCondition = QString("");
    }else if(PersonInfoFilter == tr("卡号")){
        SearchCondition = QString("CardID");
    }

    QString FunctionTypeFilter =
            ui->FunctionTypeFilterComboBox->currentText();
    QUrl getPersonInfo;
    if(FunctionTypeFilter == tr("全部")){
        getPersonInfo = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getPersonInfo\":[\"%1=%2\"]}").arg(SearchCondition).arg(ui->PersonInfoFilterLineEdit->text()));
    }else{
        getPersonInfo = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getPersonInfo\":[\"%1=%2\",\"FunctionID=%3\"]}").arg(SearchCondition).arg(ui->PersonInfoFilterLineEdit->text()).arg(ui->FunctionTypeFilterComboBox->currentIndex()));
    }

    reply = manager->get(QNetworkRequest(getPersonInfo));
    type = MainForm::GetOtherInfo;
    ui->PersonMainInfoTableWidget->clearContents();
    ui->PersonMainInfoTableWidget->setRowCount(0);
    qDebug() << getPersonInfo;
}

void MainForm::on_btnSearchDeviceInfo_clicked()
{
    QString SortOrderBy;
    QString SortFilter = ui->DeviceSortFilterComboBox->currentText();
    if(SortFilter == tr("触发时间")){
        SortOrderBy = QString("TriggerTime");
    }else if(SortFilter == tr("触发时间倒序")){
        SortOrderBy = QString("TriggerTimeDesc");
    }else if(SortFilter == tr("启用时间排序")){
        SortOrderBy = QString("StartTime");
    }else if(SortFilter == tr("启用时间倒序")){
        SortOrderBy = QString("StartTimeDesc");
    }
    QUrl getDevice;
    QString AreaFilter = ui->DeviceAreaFilterComboBox->currentText();
    if(AreaFilter == tr("全部")){
        getDevice = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getDevice\":[\"DeviceName=%1\",\"OrderBy=%2\"]}").arg(ui->DeviceNumberLineEdit->text()).arg(SortOrderBy));
    }else{
        getDevice = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getDevice\":[\"DeviceName=%1\",\"AreaID=%2\",\"OrderBy=%3\"]}").arg(ui->DeviceNumberLineEdit->text()).arg(ui->DeviceAreaFilterComboBox->currentIndex() + 1000).arg(SortOrderBy));
    }

    reply = manager->get(QNetworkRequest(getDevice));
    type = MainForm::GetOtherInfo;
    ui->DeviceMainInfoTableWidget->clearContents();
    ui->DeviceMainInfoTableWidget->setRowCount(0);
    qDebug() << getDevice;
}

void MainForm::on_btnDeviceBind_clicked()
{
    if(SelectedRowIndex == -1){
        MessageDialog message_dialog(MessageDialog::critical,tr("请选择要绑定的卡号"));
        message_dialog.exec();
        return;
    }
#if 0
    DeviceBind *device_bind = new DeviceBind;
    device_bind->SetWindowTitle(tr("设备绑定"));
    device_bind->setWindowState(Qt::WindowMaximized);

    QStringList HorizontalHeaderLabels;
    for(int i = 0; i < ui->PersonMainInfoTableWidget->columnCount(); i++)
    {
        HorizontalHeaderLabels << ui->PersonMainInfoTableWidget->horizontalHeaderItem(i)->text();
    }
    QStringList SelectedRowContent;
    for(int i = 0; i < ui->PersonMainInfoTableWidget->columnCount(); i++)
    {
        if(ui->PersonMainInfoTableWidget->item(SelectedRowIndex,i) != NULL)
        {
            SelectedRowContent << ui->PersonMainInfoTableWidget->item(SelectedRowIndex,i)->text();
        }else{
            SelectedRowContent << QString("");
        }
    }

    device_bind->SetData(HorizontalHeaderLabels,SelectedRowContent);
    device_bind->show();
#endif

    QUrl getDevice = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getDevice\":[\"OrderBy=TriggerTime\"]}"));

    reply = manager->get(QNetworkRequest(getDevice));
    type = MainForm::GetAllDeviceNumber;
    qDebug() << getDevice;
}

void MainForm::on_btnDeviceUnbind_clicked()
{
    if(SelectedRowIndex == -1){
        MessageDialog message_dialog(
                    MessageDialog::critical,
                    tr("请选择要解除绑定的卡号"));
        message_dialog.exec();
        return;
    }
    QUrl delBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"delBinding\":[\"CardNumber=%1\"]}").arg(ui->PersonMainInfoTableWidget->item(SelectedRowIndex,1)->text()));
    reply = manager->get(QNetworkRequest(delBinding));
    qDebug() << delBinding;

    SelectedRowIndex = -1;
}

void MainForm::on_btnPersonBind_clicked()
{
    slotPersonBind();
}

void MainForm::on_btnRecordQuery_clicked()
{
    QString DeviceNumber = ui->RecordDeviceNumberLineEdit->text();
    QString StartDateTime = ui->StartDateEdit->text();
    QString EndDateTime = ui->EndDateEdit->text();
    QString RecordNumber = ui->RecordNumberComboBox->currentText();

    QUrl getMsgList = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getMsgList\":[\"DeviceNumber=%1\",\"DateTime=%2;%3\",\"Limit=%4\",\"Offset=0\"]}").arg(DeviceNumber).arg(StartDateTime).arg(EndDateTime).arg(RecordNumber));
    reply = manager->get(QNetworkRequest(getMsgList));
    type = MainForm::GetOtherInfo;
    ui->RecordMainInfoTableWidget->clearContents();
    ui->RecordPersonInfoTableWidget->clearContents();
    ui->RecordSubInfoTableWidget->clearContents();

    ui->RecordMainInfoTableWidget->setRowCount(0);
    ui->RecordPersonInfoTableWidget->setRowCount(0);
    ui->RecordSubInfoTableWidget->setRowCount(0);

    qDebug() << getMsgList;
}

void MainForm::slotReplyFinished(QNetworkReply *reply)
{
    //数组（用“[]”括起来的）
    //类（用"{}"括起来的）
    if(reply->error() == QNetworkReply::NoError){
        if(type == MainForm::GetPersonNewPic){
            QImage image;
            image.loadFromData(reply->readAll());
            QImage imgScaled = image.scaled(
                        ui->person_new_pic_label->width(),
                        ui->person_new_pic_label->height(),
                        Qt::IgnoreAspectRatio);
            ui->person_new_pic_label->setPixmap(
                        QPixmap::fromImage(imgScaled));

            reply->deleteLater();

            QUrl getPersonInfo = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getPersonInfo\":[\"CardID=%1\"]}").arg(ui->RecordSubInfoTableWidget->item(SelectedRowIndex,1)->text()));

            SelectedRowIndex = -1;
            reply = manager->get(QNetworkRequest(getPersonInfo));
            type = MainForm::GetPersonBasicInfo;
            qDebug() << getPersonInfo;
            return;
        }else if(type == MainForm::GetPersonOldPic){
            QImage image;
            image.loadFromData(reply->readAll());
            QImage imgScaled = image.scaled(
                        ui->person_old_pic_label->width(),
                        ui->person_old_pic_label->height(),
                        Qt::IgnoreAspectRatio);
            ui->person_old_pic_label->setPixmap(
                        QPixmap::fromImage(imgScaled));
            type = MainForm::GetOtherInfo;
        }else if(type == MainForm::GetAllDeviceNumber){
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            QString JsonData = codec->toUnicode(reply->readAll());
            qDebug() << JsonData;

            Json::Reader JsonReader;
            Json::Value JsonValue;
            QStringList DeviceNumberList;

            if(JsonReader.parse(JsonData.toStdString(),JsonValue)){
                const Json::Value DevcieArray = JsonValue["Devcie"];
                if(DevcieArray.type() == Json::arrayValue){
                    if(DevcieArray.size()){
                        for(unsigned int i = 1; i < DevcieArray.size(); i++){
                            if(DevcieArray[i][0u].type() ==
                                    Json::stringValue){
                                DeviceNumberList << QString(DevcieArray[i][0u].asString().data());
                            }
                        }
                    }
                }
            }
            reply->deleteLater();

            if(!DeviceNumberList.isEmpty()){
                QUrl addBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"addBinding\":[\"CardNumber=%1\",\"%2\"]}").arg(ui->PersonMainInfoTableWidget->item(SelectedRowIndex,1)->text()).arg(DeviceNumberList.join(",")));
                reply = manager->get(QNetworkRequest(addBinding));
                qDebug() << addBinding;
            }
            type = MainForm::GetOtherInfo;
            SelectedRowIndex = -1;
            return;
        }else{
            QTextCodec *codec = QTextCodec::codecForName("UTF-8");
            QString JsonData = codec->toUnicode(reply->readAll());
            qDebug() << JsonData;

            Json::Reader JsonReader;
            Json::Value JsonValue;

            if(JsonReader.parse(JsonData.toStdString(),JsonValue)){
                //解析数组对象
                const Json::Value AreaArray = JsonValue["Area"];
                if(AreaArray.type() == Json::arrayValue){
                    for(unsigned int i = 0; i < AreaArray.size(); i++){
                        ui->CompanyAreaFilterComboBox->addItem(QString(AreaArray[i][1].asString().data()) + QString("(") + QString::number(AreaArray[i][0u].asUInt()) + QString(")"));
                        ui->DeviceAreaFilterComboBox->addItem(QString(AreaArray[i][1].asString().data()) + QString("(") + QString::number(AreaArray[i][0u].asUInt()) + QString(")"));
                    }
                }

                const Json::Value FunctionTypeArray =
                        JsonValue["FunctionType"];
                if(FunctionTypeArray.type() == Json::arrayValue){
                    for(unsigned int i = 0; i < FunctionTypeArray.size(); i++)
                    {
                        ui->FunctionTypeFilterComboBox->addItem(QString(FunctionTypeArray[i][1].asString().data()) + QString("(") + QString::number(FunctionTypeArray[i][0u].asUInt()) + QString(")"));
                    }
                }

                const Json::Value DateTimeArray =
                        JsonValue["NowDateTime"];
                if(DateTimeArray.type() == Json::arrayValue){
                    for(unsigned int i = 0; i < DateTimeArray.size(); i++)
                    {
                        QString NowDateTime =
                                QString(DateTimeArray[0u].asString().data());
                        CommonSetting::SettingSystemDateTime(NowDateTime);
                    }
                }

                const Json::Value CompanyArray = JsonValue["Company"];
                if(CompanyArray.type() == Json::arrayValue){
                    ui->CompanyMainInfoTableWidget->clear();
                    ui->CompanyMainInfoTableWidget->setRowCount(0);
                    if(CompanyArray.size()){
                        QStringList HorizontalHeaderLabels;
                        for(unsigned int i = 0; i < CompanyArray[0u].size(); i++){
                            HorizontalHeaderLabels << QString(CompanyArray[0u][i].asString().data());
                        }

                        qint8 columnCount = HorizontalHeaderLabels.count() - 2;
                        ui->CompanyMainInfoTableWidget->setColumnCount(columnCount);
                        ui->CompanyMainInfoTableWidget->setHorizontalHeaderLabels(HorizontalHeaderLabels);
                        tableWidgetSetting(ui->CompanyMainInfoTableWidget,
                                           QHeaderView::Stretch);

                        qint32 CompanyMainInfoTableWidgetRowIndex = 0;
                        for(unsigned int i = 1,j = 0; i < CompanyArray.size(); i++,j = 0){
                            ui->CompanyMainInfoTableWidget->insertRow(
                                        CompanyMainInfoTableWidgetRowIndex);
                            for(unsigned int k = 0; k < CompanyArray[i].size() - 2; k++){
                                if(CompanyArray[i][k].type() == Json::stringValue)
                                {
                                    QTableWidgetItem *item = new QTableWidgetItem(CompanyArray[i][k].asString().data());
                                    ui->CompanyMainInfoTableWidget->setItem(
                                                CompanyMainInfoTableWidgetRowIndex,j++,item);
                                }else if(CompanyArray[i][k].type() == Json::intValue)
                                {
                                    QTableWidgetItem *item = new QTableWidgetItem(QString::number(CompanyArray[i][k].asInt()));
                                    ui->CompanyMainInfoTableWidget->setItem(
                                                CompanyMainInfoTableWidgetRowIndex,j++,item);
                                }
                            }
                            CompanyMainInfoTableWidgetRowIndex++;
                        }
                        tableSetAlignment(ui->CompanyMainInfoTableWidget);
                    }
                }

                const Json::Value PersonInfoArray =
                        JsonValue["PersonInfo"];
                if(PersonInfoArray.type() == Json::arrayValue){
                    if(type == MainForm::GetPersonBasicInfo){
                        ui->RecordPersonInfoTableWidget->clear();
                        ui->RecordPersonInfoTableWidget->setRowCount(0);
                    }else{
                        ui->PersonMainInfoTableWidget->clear();
                        ui->PersonMainInfoTableWidget->setRowCount(0);
                    }

                    if(PersonInfoArray.size()){
                        QStringList HorizontalHeaderLabels;
                        QStringList VerticalHeaderLabels;

                        for(unsigned int i = 0; i < PersonInfoArray[0u].size(); i++){
                            if(type == MainForm::GetPersonBasicInfo){
                                VerticalHeaderLabels << QString(PersonInfoArray[0u][i].asString().data());
                            }else{
                                HorizontalHeaderLabels << QString(PersonInfoArray[0u][i].asString().data());
                            }
                        }

                        if(type == MainForm::GetPersonBasicInfo){
                            qint8 rowCount = VerticalHeaderLabels.count() - 5;
                            ui->RecordPersonInfoTableWidget->setRowCount(rowCount);
                            ui->RecordPersonInfoTableWidget->setColumnCount(1);
                            ui->RecordPersonInfoTableWidget->setVerticalHeaderLabels(VerticalHeaderLabels);
                            tableWidgetSetting(ui->RecordPersonInfoTableWidget,QHeaderView::Stretch);
                            ui->RecordPersonInfoTableWidget->horizontalHeader()->setVisible(false);
                        }else{
                            qint8 columnCount = HorizontalHeaderLabels.count() - 15;
                            ui->PersonMainInfoTableWidget->setColumnCount(columnCount);
                            ui->PersonMainInfoTableWidget->setHorizontalHeaderLabels(HorizontalHeaderLabels);
                            tableWidgetSetting(ui->PersonMainInfoTableWidget,QHeaderView::Stretch);
                        }

                        qint32 PersonMainInfoTableWidgetRowIndex = 0;
                        for(unsigned int i = 1,j = 0; i < PersonInfoArray.size(); i++,j = 0){
                            if(type != MainForm::GetPersonBasicInfo){
                                ui->PersonMainInfoTableWidget->insertRow(PersonMainInfoTableWidgetRowIndex);
                            }

                            for(unsigned int k = 0; k < PersonInfoArray[i].size() - 15; k++){
                                if(PersonInfoArray[i][k].type() == Json::stringValue)
                                {
                                    QTableWidgetItem *item = new QTableWidgetItem(PersonInfoArray[i][k].asString().data());
                                    if(type == MainForm::GetPersonBasicInfo){
                                    ui->RecordPersonInfoTableWidget->setItem(k,0,item);
                                    }else{
                                    ui->PersonMainInfoTableWidget->setItem(PersonMainInfoTableWidgetRowIndex,j++,item);
                                    }
                                }else if(PersonInfoArray[i][k].type() == Json::intValue)
                                {
                                    QTableWidgetItem *item = new QTableWidgetItem(QString::number(PersonInfoArray[i][k].asInt()));
                                    if(type == MainForm::GetPersonBasicInfo){
                                    ui->RecordPersonInfoTableWidget->setItem(k,0,item);
                                    }else{
                                    ui->PersonMainInfoTableWidget->setItem(PersonMainInfoTableWidgetRowIndex,j++,item);
                                    }
                                }
                            }
                        }
                        PersonMainInfoTableWidgetRowIndex++;
                    }
                    if(type == MainForm::GetPersonBasicInfo){
                        tableSetAlignment(
                                    ui->RecordPersonInfoTableWidget);
                        reply->deleteLater();
                        QUrl PersonPicUrl = QUrl(QString(PersonInfoArray[1][17].asString().data()));
                        reply = manager->get(QNetworkRequest(PersonPicUrl));
                        type = MainForm::GetPersonOldPic;
                        return;
                    }else{
                        tableSetAlignment(
                                    ui->PersonMainInfoTableWidget);
                    }
                }

                const Json::Value DevcieArray = JsonValue["Devcie"];
                if(DevcieArray.type() == Json::arrayValue){
                    ui->DeviceMainInfoTableWidget->clear();
                    ui->DeviceMainInfoTableWidget->setRowCount(0);

                    if(DevcieArray.size()){
                        QStringList HorizontalHeaderLabels;
                        for(unsigned int i = 0; i < DevcieArray[0u].size(); i++)
                        {
                            HorizontalHeaderLabels << QString(DevcieArray[0u][i].asString().data());
                        }

                        qint8 columnCount = HorizontalHeaderLabels.count() - 2;
                        ui->DeviceMainInfoTableWidget->setColumnCount(columnCount);
                        ui->DeviceMainInfoTableWidget->setHorizontalHeaderLabels(HorizontalHeaderLabels);
                        tableWidgetSetting(ui->DeviceMainInfoTableWidget,
                                           QHeaderView::Stretch);

                        qint32 DeviceMainInfoTableWidgetRowIndex = 0;
                        for(unsigned int i = 1,j = 0; i < DevcieArray.size(); i++,j = 0){
                            ui->DeviceMainInfoTableWidget->insertRow(
                                        DeviceMainInfoTableWidgetRowIndex);
                            for(unsigned int k = 0; k < DevcieArray[i].size() - 2; k++){
                                if(DevcieArray[i][k].type() ==
                                        Json::stringValue)
                                {
                                    QTableWidgetItem *item = new QTableWidgetItem(DevcieArray[i][k].asString().data());
                                    ui->DeviceMainInfoTableWidget->setItem(
                                                DeviceMainInfoTableWidgetRowIndex,j++,item);
                                }else if(DevcieArray[i][k].type() ==
                                         Json::intValue)
                                {
                                    QTableWidgetItem *item = new QTableWidgetItem(QString::number(DevcieArray[i][k].asInt()));
                                    ui->DeviceMainInfoTableWidget->setItem(
                                                DeviceMainInfoTableWidgetRowIndex,j++,item);
                                }
                            }
                            DeviceMainInfoTableWidgetRowIndex++;
                        }
                        tableSetAlignment(ui->DeviceMainInfoTableWidget);
                    }
                }

                const Json::Value MsgArray = JsonValue["MsgList"];
                if(MsgArray.type() == Json::arrayValue){
                    ui->RecordMainInfoTableWidget->clear();
                    ui->RecordMainInfoTableWidget->setRowCount(0);

                    if(MsgArray.size()){
                        QStringList HorizontalHeaderLabels;
                        for(unsigned int i = 0; i < MsgArray[0u].size(); i++)
                        {
                            HorizontalHeaderLabels << QString(MsgArray[0u][i].asString().data());
                        }
                        qint8 columnCount = HorizontalHeaderLabels.count();
                        ui->RecordMainInfoTableWidget->setColumnCount(columnCount);
                        ui->RecordMainInfoTableWidget->setHorizontalHeaderLabels(HorizontalHeaderLabels);
                        ui->RecordMainInfoTableWidget->horizontalHeader()->setSectionHidden(MsgArray[0u].size() - 2,true);
                        ui->RecordMainInfoTableWidget->horizontalHeader()->setSectionHidden(MsgArray[0u].size() - 1,true);
                        tableWidgetSetting(ui->RecordMainInfoTableWidget,QHeaderView::Stretch);

                        qint32 RecordMainInfoTableWidgetRowIndex = 0;
                        for(unsigned int i = 1,j = 0; i < MsgArray.size(); i++,j = 0){
                            ui->RecordMainInfoTableWidget->insertRow(
                                        RecordMainInfoTableWidgetRowIndex);
                            for(unsigned int k = 0; k < MsgArray[i].size(); k++){
                                if(MsgArray[i][k].type() ==
                                        Json::stringValue)
                                {
                                    QTableWidgetItem *item = new QTableWidgetItem(MsgArray[i][k].asString().data());
                                    ui->RecordMainInfoTableWidget->setItem(
                                                RecordMainInfoTableWidgetRowIndex,j++,item);
                                }else if(MsgArray[i][k].type() ==
                                         Json::intValue)
                                {
                                    QTableWidgetItem *item = new QTableWidgetItem(QString::number(MsgArray[i][k].asInt()));
                                    ui->RecordMainInfoTableWidget->setItem(
                                                RecordMainInfoTableWidgetRowIndex,j++,item);
                                }
                            }
                            RecordMainInfoTableWidgetRowIndex++;
                        }
                        tableSetAlignment(ui->RecordMainInfoTableWidget);
                    }
                }

                const Json::Value MsgInfoArray = JsonValue["getMsgInfo"];
                if(MsgInfoArray.type() == Json::arrayValue){
                    ui->RecordSubInfoTableWidget->clear();
                    ui->RecordSubInfoTableWidget->setRowCount(0);

                    if(MsgInfoArray.size()){
                        QStringList HorizontalHeaderLabels;
                        for(unsigned int i = 0; i < MsgInfoArray[0u].size(); i++)
                        {
                            HorizontalHeaderLabels << QString(MsgInfoArray[0u][i].asString().data());
                        }
                        qint8 columnCount = HorizontalHeaderLabels.count();
                        ui->RecordSubInfoTableWidget->setColumnCount(columnCount);
                        ui->RecordSubInfoTableWidget->setHorizontalHeaderLabels(HorizontalHeaderLabels);
                        ui->RecordSubInfoTableWidget->horizontalHeader()->setSectionHidden(MsgInfoArray[0u].size() - 2,true);
                        ui->RecordSubInfoTableWidget->horizontalHeader()->setSectionHidden(MsgInfoArray[0u].size() - 1,true);
                        tableWidgetSetting(ui->RecordSubInfoTableWidget,
                                           QHeaderView::Stretch);

                        qint32 RecordSubInfoTableWidgetRowIndex = 0;
                        for(unsigned int i = 1,j = 0; i < MsgInfoArray.size(); i++,j = 0){
                            ui->RecordSubInfoTableWidget->insertRow(
                                        RecordSubInfoTableWidgetRowIndex);
                            for(unsigned int k = 0; k < MsgInfoArray[i].size(); k++){
                                if(MsgInfoArray[i][k].type() ==
                                        Json::stringValue)
                                {
                                    QTableWidgetItem *item = new QTableWidgetItem(MsgInfoArray[i][k].asString().data());
                                    ui->RecordSubInfoTableWidget->setItem(
                                                RecordSubInfoTableWidgetRowIndex,j++,item);
                                }else if(MsgInfoArray[i][k].type() ==
                                         Json::intValue)
                                {
                                    QTableWidgetItem *item = new QTableWidgetItem(QString::number(MsgInfoArray[i][k].asInt()));
                                    ui->RecordSubInfoTableWidget->setItem(
                                                RecordSubInfoTableWidgetRowIndex,j++,item);
                                }
                            }
                            RecordSubInfoTableWidgetRowIndex++;
                        }
                        tableSetAlignment(ui->RecordSubInfoTableWidget);
                    }
                }

                const Json::Value BindingInfo = JsonValue["BindingInfo"];
                if(BindingInfo.type() == Json::objectValue){
                    if(BindingInfo.size()){
                        if(BindingInfo.isMember("Msg")){
                            MessageDialog message_dialog(
                                        MessageDialog::information,
                                        BindingInfo["Msg"].asString().data());
                            message_dialog.exec();
                        }else{
                            Json::Value::Members members =
                                    BindingInfo.getMemberNames();
                            QStringList BindingInfoList;
                            for(Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it){
                                std::string key = *it;
                                BindingInfoList <<  QString(key.c_str()) << ":" << QString(BindingInfo[key].asString().data()) << "\n";
                            }
                            MessageDialog message_dialog(
                                        MessageDialog::information,
                                        BindingInfoList.join(""));
                            message_dialog.exec();
                        }
                    }
                }
            }
        }
    }

    reply->deleteLater();
}

void MainForm::tableWidgetSetting(QTableWidget *tableWidget, QHeaderView::ResizeMode mode)
{
    //设置单元格编辑模式
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //设置选择模式
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    //显示表格线
    tableWidget->setShowGrid(false);

    //去除表头的移动
    tableWidget->horizontalHeader()->setMovable(false);

    //设置交替行颜色选项
    tableWidget->setAlternatingRowColors(true);

    //使列完全填充并平分
    tableWidget->horizontalHeader()->setResizeMode(mode);
    //        tableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    //将行和列的大小设为与内容相匹配
    //    tableWidget->resizeRowsToContents();
    //    tableWidget->resizeColumnsToContents();

    //表格表头的显示与隐藏
    tableWidget->horizontalHeader()->setVisible(true);
    tableWidget->verticalHeader()->setVisible(true);

    //所有单元格设置字体和字体大小
    tableWidget->setFont(QFont(tr("WenQuanYi Micro Hei"),14));

    //设置水平表头所有列的对齐方式和字体
    if(type == MainForm::GetPersonBasicInfo){
        for(int i = 0; i < tableWidget->rowCount(); i++){
            QTableWidgetItem *verticalHeaderItem =
                    tableWidget->verticalHeaderItem(i);
            //        horizontalHeaderItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            verticalHeaderItem->setFont(QFont(tr("WenQuanYi Micro Hei"),14));
        }
    }else{
        for(int i = 0; i < tableWidget->columnCount(); i++){
            QTableWidgetItem *horizontalHeaderItem =
                    tableWidget->horizontalHeaderItem(i);
            //        horizontalHeaderItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
            horizontalHeaderItem->setFont(QFont(tr("WenQuanYi Micro Hei"),14));
        }
    }
}

void MainForm::tableSetAlignment(QTableWidget *tableWidget)
{
    //设置所有单元格的对齐方式
    for(int i = 0;i < tableWidget->rowCount();i++){
        for(int j = 0;j < tableWidget->columnCount();j++){
            QTableWidgetItem *CellItem = tableWidget->item(i,j);
            if(CellItem){
                CellItem->setTextAlignment(
                            Qt::AlignVCenter | Qt::AlignHCenter);
            }
        }
    }
}

void MainForm::on_DeviceMainInfoTableWidget_cellClicked(int row, int column)
{
    SelectedRowIndex = row;
}

void MainForm::on_RecordMainInfoTableWidget_cellClicked(int row, int column)
{
    QString UUID = ui->RecordMainInfoTableWidget->item(row,4)->text();
    QUrl getMsgInfo = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getMsgInfo\":[\"UUID=%1\"]}").arg(UUID));

    reply = manager->get(QNetworkRequest(getMsgInfo));
    type = MainForm::GetOtherInfo;
    qDebug() << getMsgInfo;
}

void MainForm::on_RecordSubInfoTableWidget_cellClicked(int row, int column)
{
    SelectedRowIndex = row;

    QUrl pic_new_url = ui->RecordSubInfoTableWidget->item(row,3)->text();
    reply = manager->get(QNetworkRequest(pic_new_url));
    type = MainForm::GetPersonNewPic;
    qDebug() << pic_new_url;
}

void MainForm::on_PersonMainInfoTableWidget_cellClicked(int row, int column)
{
    SelectedRowIndex = row;
}

void MainForm::slotUpdateSystemDate()
{
    QString Date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString Time = QTime::currentTime().toString("hh:mm:ss");
    QString Week = QDate::currentDate().toString("dddd");

    if(Week == tr("Monday")){
        Date = Date + tr(" 星期一");
    }else if(Week == tr("Tuesday")){
        Date = Date + tr(" 星期二");
    }if(Week == tr("Wednesday")){
        Date = Date + tr(" 星期三");
    }if(Week == tr("Thursday")){
        Date = Date + tr(" 星期四");
    }if(Week == tr("Friday")){
        Date = Date + tr(" 星期五");
    }if(Week == tr("Saturday")){
        Date = Date + tr(" 星期六");
    }if(Week == tr("Sunday")){
        Date = Date + tr(" 星期日");
    }

    ui->Date->setText(Date);
    ui->Time->setText(Time);
}

void MainForm::slotControlStateEnable()
{
    QList<QToolButton *> allPButtons =
            ui->TitleBarWidget->findChildren<QToolButton *>();
    for(qint8 i = 0; i < allPButtons.size(); i++){
        allPButtons.at(i)->setEnabled(true);
    }

    QList<QWidget *> allControl =
            ui->stackedWidget->findChildren<QWidget *>();
    for(qint8 i = 0; i < allControl.size(); i++){
        allControl.at(i)->setEnabled(true);
    }
}

void MainForm::slotControlStateDisable()
{
    QList<QToolButton *> allPButtons =
            ui->TitleBarWidget->findChildren<QToolButton *>();
    for(qint8 i = 0; i < allPButtons.size(); i++){
        allPButtons.at(i)->setDisabled(true);
    }

    QList<QWidget *> allControl =
            ui->stackedWidget->findChildren<QWidget *>();
    for(qint8 i = 0; i < allControl.size(); i++){
        allControl.at(i)->setDisabled(true);
    }
}
