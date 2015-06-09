#ifndef SEARCHUNBINDPERSONINFO_H
#define SEARCHUNBINDPERSONINFO_H

#include "CommonSetting.h"
#include "json/json.h"

namespace Ui {
class SearchUnbindPersonInfo;
}

class SearchUnbindPersonInfo : public QWidget
{
    Q_OBJECT

public:
    explicit SearchUnbindPersonInfo(QWidget *parent = 0);
    ~SearchUnbindPersonInfo();
    void SetWindowTitle(QString title);
    void SetData(QString DeviceNumber);

    void getFunctionType();
    void tableWidgetSetting(QTableWidget *tableWidget,QStringList HorizontalHeaderLabels,qint8 columnCount,QHeaderView::ResizeMode mode,bool isVisibleVerticalHeader);
    void tableSetAlignment(QTableWidget *tableWidget);

private slots:
    void on_btnClose_clicked();
    void on_btnSearchPersonInfo_clicked();
    void slotReplyFinished(QNetworkReply *);

    void on_btnAdd_clicked();

signals:
    void signalSelectedContent(QList<QStringList> SelectedContent);

public:
    Ui::SearchUnbindPersonInfo *ui;
    QString DeviceNumber;

    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    QString ServerIP;
    QString ServerListenPort;
};

#endif // SEARCHUNBINDPERSONINFO_H
