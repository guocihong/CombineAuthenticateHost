#include "devicebind.h"
#include "ui_devicebind.h"

using namespace std;
using namespace Json;

DeviceBind::DeviceBind(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceBind)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);

    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(slotReplyFinished(QNetworkReply*)));

    ServerIP = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/IP");
    ServerListenPort = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/PORT");

    SelectedRowIndex = -1;
}

DeviceBind::~DeviceBind()
{
    delete ui;
}

void DeviceBind::SetData(QStringList HorizontalHeaderLabels,
                         QStringList SelectedRowContent)
{
    tableWidgetSetting(ui->PersonTableWidget,HorizontalHeaderLabels,HorizontalHeaderLabels.count(),QHeaderView::Stretch,false);
    ui->PersonTableWidget->insertRow(0);

    for(int i = 0; i < SelectedRowContent.count(); i++){
        QTableWidgetItem *item =
                new QTableWidgetItem(SelectedRowContent.at(i));
        ui->PersonTableWidget->setItem(0,i,item);
    }
    tableSetAlignment(ui->PersonTableWidget);

    getBinding();//获取卡号绑定了哪些设备
}

void DeviceBind::getBinding()
{
    QUrl getBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getBinding\":[\"CardNumber=%1\"]}").arg(ui->PersonTableWidget->item(0,1)->text()));

    reply = manager->get(QNetworkRequest(getBinding));
    ui->BindTableWidget->clearContents();
    ui->BindTableWidget->setRowCount(0);
    qDebug() << getBinding;
}

void DeviceBind::on_btnUnbind_clicked()
{
    QUrl delBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"delBinding\":[\"CardNumber=%1\"]}").arg(ui->PersonTableWidget->item(0,1)->text()));
    reply = manager->get(QNetworkRequest(delBinding));
    qDebug() << delBinding;
}

void DeviceBind::on_btnSearchUnbindDeviceInfo_clicked()
{
    SearchUnbindDeviceInfo *search_unbind_device =
            new SearchUnbindDeviceInfo;
    connect(search_unbind_device,SIGNAL(signalSelectedContent(QList<QStringList>)),this,SLOT(slotSelectedContent(QList<QStringList>)));

    search_unbind_device->SetWindowTitle(tr("查找"));
    search_unbind_device->CardNumber =
            ui->PersonTableWidget->item(0,1)->text();
    search_unbind_device->setWindowState(Qt::WindowMaximized);
    search_unbind_device->show();
    search_unbind_device->getBinding();
}

void DeviceBind::slotSelectedContent(QList<QStringList> SelectedContent)
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

void DeviceBind::on_btnBind_clicked()
{
    if(ui->SearchTableWidget->rowCount() == 0){
        MessageDialog message_dialog(MessageDialog::critical,
                                     tr("请选择要绑定的卡号"));
        message_dialog.exec();
        return;
    }
    QStringList DeviceNumberList;
    for(int i = 0; i < ui->SearchTableWidget->rowCount(); i++){
        DeviceNumberList << ui->SearchTableWidget->item(i,0)->text();
    }
    QUrl addBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"addBinding\":[\"CardNumber=%1\",\"%2\"]}").arg(ui->PersonTableWidget->item(0,1)->text()).arg(DeviceNumberList.join(",")));
    reply = manager->get(QNetworkRequest(addBinding));
    ui->SearchTableWidget->clearContents();
    ui->SearchTableWidget->setRowCount(0);
    qDebug() << addBinding;
}

void DeviceBind::on_btnClose_clicked()
{
    this->close();
}

void DeviceBind::SetWindowTitle(QString title)
{
    ui->label_title->setText(title);
}

void DeviceBind::slotReplyFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString JsonData = codec->toUnicode(reply->readAll());
        qDebug() << JsonData;

        Json::Reader JsonReader;
        Json::Value JsonValue;

        if(JsonReader.parse(JsonData.toStdString(),JsonValue)){
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
                    tableWidgetSetting(ui->BindTableWidget,HorizontalHeaderLabels,columnCount,QHeaderView::Stretch,false);

                    qint32 BindTableWidgetRowIndex = 0;
                    for(unsigned int i = 1,j = 0; i < BindingStateArray.size(); i++,j = 0){
                        ui->BindTableWidget->insertRow(
                                    BindTableWidgetRowIndex);
                        for(unsigned int k = 0; k < BindingStateArray[i].size(); k++){
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

void DeviceBind::tableWidgetSetting(QTableWidget *tableWidget, QStringList HorizontalHeaderLabels, qint8 columnCount, QHeaderView::ResizeMode mode, bool isVisibleVerticalHeader)
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

void DeviceBind::tableSetAlignment(QTableWidget *tableWidget)
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

void DeviceBind::on_BindTableWidget_cellClicked(int row, int column)
{
    SelectedRowIndex = row;
}
