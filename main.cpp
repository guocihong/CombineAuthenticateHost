#include "mainform.h"
#include "InputMethod/frminput.h"
#include "app.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CommonSetting::SetUTF8Code();
    CommonSetting::SetCustomStyle(":/image/blue.css");
    qApp->setFont(QFont(App::AppFontName,App::AppFontSize));

    QTranslator translator;
    translator.load(":/image/qt_zh_CN.qm");
    qApp->installTranslator(&translator);

    frmInput::Instance()->Init("bottom", "blue", 20);

    system("/bin/UdpMulticastClient -qws &");

    MainForm w;
    w.setGeometry(qApp->desktop()->availableGeometry());
    w.show();

    return a.exec();
}
