#include "mainform.h"
#include "InputMethod/frminput.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CommonSetting::SetUTF8Code();
    CommonSetting::SetCustomStyle();

    QTranslator translator;
    translator.load(":/qm/qt_zh_CN.qm");
    qApp->installTranslator(&translator);

    frmInput::Instance()->Init("bottom", "black", 25);

    system("/bin/UdpMulticastClient -qws &");

    MainForm w;
    w.setGeometry(qApp->desktop()->availableGeometry());
    w.show();

    return a.exec();
}
