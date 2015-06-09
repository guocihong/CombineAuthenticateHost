#ifndef PERSONADD_H
#define PERSONADD_H

#include "CommonSetting.h"
#include "json/json.h"
#include "MessageDialog/messagedialog.h"

namespace Ui {
class PersonAdd;
}

class PersonAdd : public QDialog
{
    Q_OBJECT

public:
    explicit PersonAdd(QWidget *parent = 0);
    ~PersonAdd();
    void SetWindowTitle(QString title);
    void SetData(QString CardID);
    void getFunctionType();

private slots:
    void slotReplyFinished(QNetworkReply *);
    void on_FunctionTypeFilterComboBox_currentIndexChanged(int index);

    void on_btnClose_clicked();
    void on_btnQuery_clicked();
    void on_btnDownloadPic_clicked();
    void on_btnSave_clicked();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);

signals:
    void signalControlStateEnable();

private:
    Ui::PersonAdd *ui;

    enum OperatorType{
        GetBasicInfo,
        GetPersonPic
    };
    enum OperatorType type;

    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    QString ServerIP;
    QString ServerListenPort;

    QPoint mousePoint;
    bool mousePressed;

    QList<QCheckBox *> cb_list;

    qint8 FunctionTypeID;//已经分配的卡类型
    QStringList FunctionTypeList;
    QUrl PersonPicUrl;
};

#endif // PERSONADD_H
