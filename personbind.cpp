#include "personbind.h"
#include "ui_personbind.h"

using namespace std;
using namespace Json;

PersonBind::PersonBind(QWidget *parent) :
    QDialog(parent,Qt::Dialog),
    ui(new Ui::PersonBind)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);

    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(slotReplyFinished(QNetworkReply*)));

    ServerIP = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/IP");
    ServerListenPort = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/PORT");

    getFunctionType();

    SelectedRowIndex = -1;
}

PersonBind::~PersonBind()
{
    delete ui;
}

void PersonBind::on_btnClose_clicked()
{
    this->close();
}

void PersonBind::SetWindowTitle(QString title)
{
    ui->label_title->setText(title);
}

void PersonBind::getFunctionType()
{
    QUrl getFunctionType = QUrl(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getFunctionType\":null}");
    reply = manager->get(QNetworkRequest(getFunctionType));
    qDebug() << getFunctionType;
}

void PersonBind::getBinding()
{  
    QString FunctionType = ui->FunctionTypeFilterComboBox->currentText();
    QUrl getBinding;
    if(FunctionType == tr("全部")){
        getBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getBinding\":[\"DeviceNumber=%1\"]}").arg(ui->DeviceTableWidget->item(0,0)->text()));
    }else{
        getBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getBinding\":[\"DeviceNumber=%1\",\"FunctionTypeID=%2\"]}").arg(ui->DeviceTableWidget->item(0,0)->text()).arg(ui->FunctionTypeFilterComboBox->currentIndex()));
    }

    reply = manager->get(QNetworkRequest(getBinding));
    ui->BindTableWidget->clearContents();
    ui->BindTableWidget->setRowCount(0);
    qDebug() << getBinding;
}

void PersonBind::on_btnUnbind_clicked()
{
    if(SelectedRowIndex == -1){
        MessageDialog message_dialog(
                    MessageDialog::critical,
                    tr("请选择要解除绑定的卡号"));
        message_dialog.exec();

        return;
    }
    QUrl delBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"delBinding\":[\"CardNumber=%1\"]}").arg(ui->BindTableWidget->item(SelectedRowIndex,1)->text()));
    reply = manager->get(QNetworkRequest(delBinding));
    qDebug() << delBinding;

    SelectedRowIndex = -1;
}

void PersonBind::on_btnSearchUnbindPersonInfo_clicked()
{
    SearchUnbindPersonInfo *search_unbind_person =
            new SearchUnbindPersonInfo;
    connect(search_unbind_person,SIGNAL(signalSelectedContent(QList<QStringList>)),this,SLOT(slotSelectedContent(QList<QStringList>)));

    search_unbind_person->SetWindowTitle(tr("查找"));
    search_unbind_person->DeviceNumber =
            ui->DeviceTableWidget->item(0,0)->text();
    search_unbind_person->setWindowState(Qt::WindowMaximized);
    search_unbind_person->show();
}

void PersonBind::on_btnBind_clicked()
{
    if(ui->SearchTableWidget->rowCount() == 0){
        MessageDialog message_dialog(MessageDialog::critical,
                                     tr("请选择要绑定的卡号"));
        message_dialog.exec();
        return;
    }
    QStringList CardIDList;
    for(int i = 0; i < ui->SearchTableWidget->rowCount(); i++){
        CardIDList << ui->SearchTableWidget->item(i,1)->text();
    }
    QUrl addBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"addBinding\":[\"DeviceNumber=%1\",\"%2\"]}").arg(ui->DeviceTableWidget->item(0,0)->text()).arg(CardIDList.join(",")));
    reply = manager->get(QNetworkRequest(addBinding));
    ui->SearchTableWidget->clearContents();
    ui->SearchTableWidget->setRowCount(0);
    qDebug() << addBinding;
}

void PersonBind::slotReplyFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString JsonData = codec->toUnicode(reply->readAll());
        qDebug() << JsonData;

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
            }

            const Json::Value BindingStateArray =
                    JsonValue["BindingState"];
            if(BindingStateArray.type() == Json::arrayValue){
                ui->BindTableWidget->clearContents();
                ui->BindTableWidget->setRowCount(0);
                if(BindingStateArray.size()){
                    QStringList HorizontalHeaderLabels;
                    for(unsigned int i = 0; i < BindingStateArray[0u].size(); i++){
                        HorizontalHeaderLabels << QString(BindingStateArray[0u][i].asString().data());
                    }

                    qint8 columnCount =
                            HorizontalHeaderLabels.count();
                    tableWidgetSetting(ui->BindTableWidget,HorizontalHeaderLabels,columnCount - 1,QHeaderView::Stretch,false);

                    qint32 BindTableWidgetRowIndex = 0;
                    for(unsigned int i = 1,j = 0; i < BindingStateArray.size(); i++,j = 0){
                        ui->BindTableWidget->insertRow(
                                    BindTableWidgetRowIndex);
                        for(unsigned int k = 0; k < BindingStateArray[i].size() - 1; k++){
                            QTableWidgetItem *item = new QTableWidgetItem(BindingStateArray[i][k].asString().data());
                            ui->BindTableWidget->setItem(
                                        BindTableWidgetRowIndex,j++,item);
                        }
                        BindTableWidgetRowIndex++;
                    }
                    tableSetAlignment(ui->BindTableWidget);
                }
            }

            const Json::Value BindingInfo = JsonValue["BindingInfo"];
            if(BindingInfo.type() == Json::objectValue){
                if(BindingInfo.size()){
                    getBinding();
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

    reply->deleteLater();
}

void PersonBind::SetData(QStringList HorizontalHeaderLabels,
                         QStringList SelectedRowContent)
{
    tableWidgetSetting(ui->DeviceTableWidget,HorizontalHeaderLabels,HorizontalHeaderLabels.count(),QHeaderView::Stretch,false);
    ui->DeviceTableWidget->insertRow(0);

    for(int i = 0; i < SelectedRowContent.count(); i++){
        QTableWidgetItem *item =
                new QTableWidgetItem(SelectedRowContent.at(i));
        ui->DeviceTableWidget->setItem(0,i,item);
    }
    tableSetAlignment(ui->DeviceTableWidget);

    getBinding();//获取前端设备绑定了哪些卡号
}

void PersonBind::slotSelectedContent(QList<QStringList> SelectedContent)
{
    ui->SearchTableWidget->clearContents();
    ui->SearchTableWidget->setRowCount(0);

    if(!SelectedContent.isEmpty()){
        tableWidgetSetting(ui->SearchTableWidget,SelectedContent.at(0),SelectedContent.at(0).size(),QHeaderView::Stretch,true);

        qint32 SearchTableWidgetRowIndex = 0;
        for(int i = 1; i < SelectedContent.size(); i++){
            ui->SearchTableWidget->insertRow(SearchTableWidgetRowIndex);
            for(int j = 0; j < SelectedContent.at(i).size(); j++){
                QTableWidgetItem *item = new QTableWidgetItem(SelectedContent.at(i).at(j));
                ui->SearchTableWidget->setItem(
                            SearchTableWidgetRowIndex,j,item);
            }
            SearchTableWidgetRowIndex++;
        }
        tableSetAlignment(ui->SearchTableWidget);
    }
}

void PersonBind::tableWidgetSetting(QTableWidget *tableWidget, QStringList HorizontalHeaderLabels, qint8 columnCount, QHeaderView::ResizeMode mode, bool isVisibleVerticalHeader)
{
    //设置列数
    tableWidget->setColumnCount(columnCount);

    //设置水平表头
    tableWidget->setHorizontalHeaderLabels(HorizontalHeaderLabels);

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
    tableWidget->verticalHeader()->setVisible(isVisibleVerticalHeader);

    //所有单元格设置字体和字体大小
    tableWidget->setFont(QFont(tr("WenQuanYi Micro Hei"),14));

    //设置水平表头所有列的对齐方式和字体
    for(int i = 0; i < tableWidget->columnCount(); i++){
        QTableWidgetItem *horizontalHeaderItem =
                tableWidget->horizontalHeaderItem(i);
        horizontalHeaderItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        horizontalHeaderItem->setFont(QFont(tr("WenQuanYi Micro Hei"),14));
    }
}

void PersonBind::tableSetAlignment(QTableWidget *tableWidget)
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

void PersonBind::on_BindTableWidget_cellClicked(int row, int column)
{
    SelectedRowIndex = row;
}

void PersonBind::on_FunctionTypeFilterComboBox_currentIndexChanged(const QString &arg1)
{
    QString FunctionType = ui->FunctionTypeFilterComboBox->currentText();
    QUrl getBinding;
    if(FunctionType == tr("全部")){
        getBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getBinding\":[\"DeviceNumber=%1\"]}").arg(ui->DeviceTableWidget->item(0,0)->text()));
    }else{
        getBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getBinding\":[\"DeviceNumber=%1\",\"FunctionTypeID=%2\"]}").arg(ui->DeviceTableWidget->item(0,0)->text()).arg(ui->FunctionTypeFilterComboBox->currentIndex()));
    }

    reply = manager->get(QNetworkRequest(getBinding));
    ui->BindTableWidget->clearContents();
    ui->BindTableWidget->setRowCount(0);
    qDebug() << getBinding;
}
