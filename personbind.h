#ifndef PERSONBIND_H
#define PERSONBIND_H

#include "searchunbindpersoninfo.h"
#include "MessageDialog/messagedialog.h"

namespace Ui {
class PersonBind;
}

class PersonBind : public QDialog
{
    Q_OBJECT

public:
    explicit PersonBind(QWidget *parent = 0);
    ~PersonBind();
    void SetWindowTitle(QString title);

    void getFunctionType();
    void getBinding();

    void SetData(QStringList HorizontalHeaderLabels,
                 QStringList SelectedRowContent);
    void tableWidgetSetting(QTableWidget *tableWidget,QStringList HorizontalHeaderLabels,qint8 columnCount,QHeaderView::ResizeMode mode,bool isVisibleVerticalHeader);
    void tableSetAlignment(QTableWidget *tableWidget);

private slots:
    void on_btnClose_clicked();
    void slotReplyFinished(QNetworkReply *);

    void on_BindTableWidget_cellClicked(int row, int column);

    void on_btnUnbind_clicked();
    void on_btnBind_clicked();

    void on_btnSearchUnbindPersonInfo_clicked();

    void slotSelectedContent(QList<QStringList> SelectedContent);

    void on_FunctionTypeFilterComboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::PersonBind *ui;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    int SelectedRowIndex;

    QString ServerIP;
    QString ServerListenPort;
};

#endif // PERSONBIND_H
