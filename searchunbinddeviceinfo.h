#ifndef SEARCHUNBINDDEVICEINFO_H
#define SEARCHUNBINDDEVICEINFO_H

#include "CommonSetting.h"
#include "json/json.h"

namespace Ui {
class SearchUnbindDeviceInfo;
}

class SearchUnbindDeviceInfo : public QWidget
{
    Q_OBJECT

public:
    explicit SearchUnbindDeviceInfo(QWidget *parent = 0);
    ~SearchUnbindDeviceInfo();
    void SetWindowTitle(QString title);
    void SetData(QString CardNumber);
    void getBinding();
    void tableWidgetSetting(QTableWidget *tableWidget,QStringList HorizontalHeaderLabels,qint8 columnCount,QHeaderView::ResizeMode mode,bool isVisibleVerticalHeader);
    void tableSetAlignment(QTableWidget *tableWidget);

private slots:
    void on_btnClose_clicked();
    void slotReplyFinished(QNetworkReply *);

    void on_btnAdd_clicked();

signals:
    void signalSelectedContent(QList<QStringList> SelectedContent);

public:
    Ui::SearchUnbindDeviceInfo *ui;

    QString CardNumber;

    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    QString ServerIP;
    QString ServerListenPort;
};

#endif // SEARCHUNBINDDEVICEINFO_H
