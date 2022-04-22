#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 设置日志格式
    qSetMessagePattern("%{type} | %{time yyyy-MM-dd hh:mm:ss.zzz} | %{threadid} | %{message}");

    // 设置基本信息
    QCoreApplication::setOrganizationName("softsec");
    QCoreApplication::setOrganizationDomain("softsec.org");
    QCoreApplication::setApplicationName("ExileBot2");

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
