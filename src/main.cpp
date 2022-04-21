#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 设置日志格式
    qSetMessagePattern("%{type} | %{time yyyy-MM-dd hh:mm:ss.zzz} | %{threadid} | %{message}");

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
