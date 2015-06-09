#ifndef MAINFORM_H
#define MAINFORM_H

#include "companyregister.h"
#include "personadd.h"
#include "deviceadd.h"
//#include "devicebind.h"
#include "personbind.h"
#include "systemsetting.h"

namespace Ui {
class MainForm;
}

class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();
    void CreateCompanyManagerMenu();
    void CreatePersonManagerMenu();
    void CreateFrontManagerMenu();

    void getArea();
    void getFunctionType();
    void getDateTime();

    void tableWidgetSetting(QTableWidget *tableWidget, QHeaderView::ResizeMode mode);
    void tableSetAlignment(QTableWidget *tableWidget);

private slots:
    void slotControlStateEnable();
    void slotControlStateDisable();
    void slotReplyFinished(QNetworkReply *);

    void on_btnCompanyManager_clicked();
    void on_btnPersonManager_clicked();
    void on_btnFrontManager_clicked();
    void on_btnRecordQueryManager_clicked();
    void on_btnSystemSettingManager_clicked();

    void slotShowCompanyList();
    void slotShowCompanyRegister();
    void slotCompanyEdit();

    void slotShowPersonList();
    void slotShowPersonAdd();
    void slotPersonEdit();

    void slotShowDeviceStatus();
    void slotShowDeviceAdd();
    void slotDeviceEdit();
    void slotPersonBind();

    void on_btnSearchCompanyInfo_clicked();
    void on_btnSearchPersonInfo_clicked();
    void on_btnSearchDeviceInfo_clicked();
    void on_btnDeviceBind_clicked();
    void on_btnDeviceUnbind_clicked();
    void on_btnPersonBind_clicked();
    void on_btnRecordQuery_clicked();

    void on_DeviceMainInfoTableWidget_cellClicked(int row, int column);
    void on_RecordMainInfoTableWidget_cellClicked(int row, int column);
    void on_RecordSubInfoTableWidget_cellClicked(int row, int column);
    void on_PersonMainInfoTableWidget_cellClicked(int row, int column);
    void slotUpdateSystemDate();

private:
    Ui::MainForm *ui;
    enum OperatorType{
        GetOtherInfo,
        GetAllDeviceNumber,
        GetPersonBasicInfo,
        GetPersonNewPic,
        GetPersonOldPic
    };
    enum OperatorType type;

    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    QMenu *CompanyManagerMenu;
    QAction *CompanyListAction;
    QAction *CompanyRegisterAction;
    QAction *CompanyEditAction;

    QMenu *PersonManagerMenu;
    QAction *PersonListAction;
    QAction *PersonAddAction;
    QAction *PersonEditAction;

    QMenu *FrontManagerMenu;
    QAction *DeviceStatusAction;
    QAction *DeviceAddAction;
    QAction *DeviceEditAction;
    QAction *PersonBindAction;

    SystemSetting *system_setting;
    QTimer *SystemDateTimer;

    int SelectedRowIndex;

    QString ServerIP;
    QString ServerListenPort;
};

#endif // MAINFORM_H
