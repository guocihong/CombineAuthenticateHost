#ifndef DEVICEBIND_H
#define DEVICEBIND_H

#include "MessageDialog/messagedialog.h"
#include "json/json.h"
#include "CommonSetting.h"
#include "searchunbinddeviceinfo.h"

namespace Ui {
class DeviceBind;
}

class DeviceBind : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceBind(QWidget *parent = 0);
    ~DeviceBind();

    void SetWindowTitle(QString title);
    void getBinding();
    void SetData(QStringList HorizontalHeaderLabels,
                 QStringList SelectedRowContent);
    void tableWidgetSetting(QTableWidget *tableWidget,QStringList HorizontalHeaderLabels,qint8 columnCount,QHeaderView::ResizeMode mode,bool isVisibleVerticalHeader);
    void tableSetAlignment(QTableWidget *tableWidget);

private slots:
    void slotReplyFinished(QNetworkReply *reply);
    void on_btnUnbind_clicked();
    void on_btnSearchUnbindDeviceInfo_clicked();
    void on_btnBind_clicked();
    void on_btnClose_clicked();
    void slotSelectedContent(QList<QStringList> SelectedContent);

    void on_BindTableWidget_cellClicked(int row, int column);

private:
    Ui::DeviceBind *ui;

    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    int SelectedRowIndex;

    QString ServerIP;
    QString ServerListenPort;
};

#endif // DEVICEBIND_H
