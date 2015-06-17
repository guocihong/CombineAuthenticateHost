#include "searchunbindpersoninfo.h"
#include "ui_searchunbindpersoninfo.h"

using namespace std;
using namespace Json;

SearchUnbindPersonInfo::SearchUnbindPersonInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchUnbindPersonInfo)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    IconHelper::Instance()->SetIcon(ui->label_Ico, QChar(0xf1a5),14);

    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),
            this,SLOT(slotReplyFinished(QNetworkReply*)));

    ServerIP = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/IP");
    ServerListenPort = CommonSetting::ReadSettings("/bin/config.ini","ServerNetwork/PORT");

    getFunctionType();
}

SearchUnbindPersonInfo::~SearchUnbindPersonInfo()
{
    delete ui;
}

void SearchUnbindPersonInfo::on_btnClose_clicked()
{
    this->close();
}

void SearchUnbindPersonInfo::SetWindowTitle(QString title)
{
    ui->lab_Title->setText(title);
}

void SearchUnbindPersonInfo::SetData(QString DeviceNumber)
{
    this->DeviceNumber = DeviceNumber;
}

void SearchUnbindPersonInfo::getFunctionType()
{
    QUrl getFunctionType = QUrl(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getFunctionType\":null}");
    reply = manager->get(QNetworkRequest(getFunctionType));
}

void SearchUnbindPersonInfo::on_btnSearchPersonInfo_clicked()
{
    QString FunctionType = ui->FunctionTypeFilterComboBox->currentText();
    QUrl getBinding;
    if(FunctionType == tr("全部")){
        getBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getBinding\":[\"DeviceNumberNotIn=%1\"]}").arg(DeviceNumber));
    }else{
        getBinding = QUrl(QString(ServerIP + ":" + ServerListenPort + "/cgi-bin/authface/authface.cgi?{\"getBinding\":[\"DeviceNumberNotIn=%1\",\"FunctionTypeID=%2\"]}").arg(DeviceNumber).arg(ui->FunctionTypeFilterComboBox->currentIndex()));
    }

    reply = manager->get(QNetworkRequest(getBinding));
    ui->PersonMainInfoTableWidget->clearContents();
    ui->PersonMainInfoTableWidget->setRowCount(0);
//    qDebug() << getBinding;
}

void SearchUnbindPersonInfo::on_btnAdd_clicked()
{
    QStringList index_list;
    QList<QTableWidgetSelectionRange> ranges = ui->PersonMainInfoTableWidget->selectedRanges();
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
    for(int i = 0; i < ui->PersonMainInfoTableWidget->columnCount(); i++)
    {
            HorizontalHeaderLabels << ui->PersonMainInfoTableWidget->horizontalHeaderItem(i)->text();
    }
    SelectedContent.append(HorizontalHeaderLabels);
    //添加选中行的内容
    for(int i = 0; i < index_list.size(); i++){
        QStringList SelectedRowContent;
        for(int j = 0; j < ui->PersonMainInfoTableWidget->columnCount(); j++)
        {
            SelectedRowContent << ui->PersonMainInfoTableWidget->item(index_list.at(i).toInt(),j)->text();
        }
        SelectedContent.append(SelectedRowContent);
    }

    emit signalSelectedContent(SelectedContent);
    this->close();
}

void SearchUnbindPersonInfo::slotReplyFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString JsonData = codec->toUnicode(reply->readAll());
//        qDebug() << JsonData;

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
            const Json::Value BindingStateArray = JsonValue["BindingState"];
            if(BindingStateArray.type() == Json::arrayValue){
                if(BindingStateArray.size()){
                    QStringList HorizontalHeaderLabels;
                    for(unsigned int i = 0; i < BindingStateArray[0u].size(); i++){
                        HorizontalHeaderLabels << QString(BindingStateArray[0u][i].asString().data());
                    }

                    qint8 columnCount = HorizontalHeaderLabels.count() - 1;
                    tableWidgetSetting(ui->PersonMainInfoTableWidget,
                                       HorizontalHeaderLabels,columnCount,QHeaderView::Stretch,false);

                    qint32 PersonMainInfoTableWidgetRowIndex = 0;
                    for(unsigned int i = 1,j = 0; i < BindingStateArray.size(); i++,j = 0){
                        ui->PersonMainInfoTableWidget->insertRow(
                                    PersonMainInfoTableWidgetRowIndex);
                        for(unsigned int k = 0; k < BindingStateArray[i].size() - 1; k++){
                            if(BindingStateArray[i][k].type() ==
                                    Json::stringValue)
                            {
                                QTableWidgetItem *item = new QTableWidgetItem(BindingStateArray[i][k].asString().data());
                                ui->PersonMainInfoTableWidget->setItem(
                                            PersonMainInfoTableWidgetRowIndex,j++,item);
                            }else if(BindingStateArray[i][k].type() ==
                                     Json::intValue)
                            {
                                QTableWidgetItem *item = new QTableWidgetItem(QString::number(BindingStateArray[i][k].asInt()));
                                ui->PersonMainInfoTableWidget->setItem(
                                            PersonMainInfoTableWidgetRowIndex,j++,item);
                            }
                        }
                        PersonMainInfoTableWidgetRowIndex++;
                    }
                    tableSetAlignment(ui->PersonMainInfoTableWidget);
                }
            }
        }
    }

    reply->deleteLater();
}

void SearchUnbindPersonInfo::tableWidgetSetting(QTableWidget *tableWidget, QStringList HorizontalHeaderLabels, qint8 columnCount, QHeaderView::ResizeMode mode, bool isVisibleVerticalHeader)
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

    //使列完全填充并平分
    tableWidget->horizontalHeader()->setResizeMode(mode);
    //        tableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    //将行和列的大小设为与内容相匹配
    //    tableWidget->resizeRowsToContents();
    //    tableWidget->resizeColumnsToContents();

    //表格表头的显示与隐藏
    tableWidget->horizontalHeader()->setVisible(true);
    tableWidget->verticalHeader()->setVisible(isVisibleVerticalHeader);    

    //设置水平表头所有列的对齐方式和字体
    for(int i = 0; i < tableWidget->columnCount(); i++){
        QTableWidgetItem *horizontalHeaderItem =
                tableWidget->horizontalHeaderItem(i);
        horizontalHeaderItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);        
    }
}

void SearchUnbindPersonInfo::tableSetAlignment(QTableWidget *tableWidget)
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
