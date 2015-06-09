#include "searchunbinddeviceinfo.h"
#include "ui_searchunbinddeviceinfo.h"

using namespace std;
using namespace Json;

SearchUnbindDeviceInfo::SearchUnbindDeviceInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchUnbindDeviceInfo)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);

    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(slotReplyFinished(QNetworkReply*)));

    ServerIP = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/IP");
    ServerListenPort = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/PORT");
}

SearchUnbindDeviceInfo::~SearchUnbindDeviceInfo()
{
    delete ui;
}

void SearchUnbindDeviceInfo::on_btnClose_clicked()
{
    this->close();
}

void SearchUnbindDeviceInfo::SetWindowTitle(QString title)
{
    ui->label_title->setText(title);
}

void SearchUnbindDeviceInfo::SetData(QString CardNumber)
{
    this->CardNumber = CardNumber;
}

void SearchUnbindDeviceInfo::on_btnAdd_clicked()
{
    QStringList index_list;
    QList<QTableWidgetSelectionRange> ranges = ui->DeviceMainInfoTableWidget->selectedRanges();
    for(int i = 0; i < ranges.count(); i++)
    {
        for(int j = ranges.at(i).topRow(); j <= ranges.at(i).bottomRow(); j++)
        {
            index_list << QString::number(j);
        }
    }

    QList<QStringList> SelectedContent;
    //添加表头
    QStringList HorizontalHeaderLabels;
    for(int i = 0; i < ui->DeviceMainInfoTableWidget->columnCount(); i++)
    {
            HorizontalHeaderLabels << ui->DeviceMainInfoTableWidget->horizontalHeaderItem(i)->text();
    }
    SelectedContent.append(HorizontalHeaderLabels);
    //添加选中行的内容
    for(int i = 0; i < index_list.size(); i++){
        QStringList SelectedRowContent;
        for(int j = 0; j < ui->DeviceMainInfoTableWidget->columnCount(); j++)
        {
            SelectedRowContent << ui->DeviceMainInfoTableWidget->item(index_list.at(i).toInt(),j)->text();
        }
        SelectedContent.append(SelectedRowContent);
    }

    emit signalSelectedContent(SelectedContent);
    this->close();
}

void SearchUnbindDeviceInfo::getBinding()
{
    QUrl getBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getBinding\":[\"CardNumber=%1\"]}").arg(CardNumber));

    reply = manager->get(QNetworkRequest(getBinding));
    ui->DeviceMainInfoTableWidget->clearContents();
    ui->DeviceMainInfoTableWidget->setRowCount(0);
    qDebug() << getBinding;
}

void SearchUnbindDeviceInfo::slotReplyFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString JsonData = codec->toUnicode(reply->readAll());
        qDebug() << JsonData;

        Json::Reader JsonReader;
        Json::Value JsonValue;

        if(JsonReader.parse(JsonData.toStdString(),JsonValue)){
            const Json::Value BindingStateArray = JsonValue["BindingState"];
            if(BindingStateArray.type() == Json::arrayValue){
                if(BindingStateArray.size()){
                    QStringList HorizontalHeaderLabels;
                    for(unsigned int i = 0; i < BindingStateArray[0u].size(); i++){
                        HorizontalHeaderLabels << QString(BindingStateArray[0u][i].asString().data());
                    }

                    qint8 columnCount = HorizontalHeaderLabels.count();
                    tableWidgetSetting(ui->DeviceMainInfoTableWidget,
                                       HorizontalHeaderLabels,columnCount,QHeaderView::Stretch,false);

                    qint32 DeviceMainInfoTableWidgetRowIndex = 0;
                    for(unsigned int i = 1,j = 0; i < BindingStateArray.size(); i++,j = 0){
                        ui->DeviceMainInfoTableWidget->insertRow(DeviceMainInfoTableWidgetRowIndex);
                        for(unsigned int k = 0; k < BindingStateArray[i].size(); k++){
                            if(BindingStateArray[i][k].type() ==
                                    Json::stringValue)
                            {
                                QTableWidgetItem *item = new QTableWidgetItem(BindingStateArray[i][k].asString().data());
                                ui->DeviceMainInfoTableWidget->setItem(
                                            DeviceMainInfoTableWidgetRowIndex,j++,item);
                            }else if(BindingStateArray[i][k].type() ==
                                     Json::intValue)
                            {
                                QTableWidgetItem *item = new QTableWidgetItem(QString::number(BindingStateArray[i][k].asInt()));
                                ui->DeviceMainInfoTableWidget->setItem(
                                            DeviceMainInfoTableWidgetRowIndex,j++,item);
                            }
                        }
                        DeviceMainInfoTableWidgetRowIndex++;
                    }
                    tableSetAlignment(ui->DeviceMainInfoTableWidget);
                }
            }
        }
    }

    reply->deleteLater();
}

void SearchUnbindDeviceInfo::tableWidgetSetting(QTableWidget *tableWidget, QStringList HorizontalHeaderLabels, qint8 columnCount, QHeaderView::ResizeMode mode, bool isVisibleVerticalHeader)
{
    //设置列数
    tableWidget->setColumnCount(columnCount);

    //设置水平表头
    tableWidget->setHorizontalHeaderLabels(HorizontalHeaderLabels);

    //设置单元格编辑模式
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //设置选择模式
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

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

void SearchUnbindDeviceInfo::tableSetAlignment(QTableWidget *tableWidget)
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
