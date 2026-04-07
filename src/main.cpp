#include <QApplication>
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 应用程序基础设置
    app.setApplicationName(QStringLiteral("留园食堂就餐仿真系统"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setOrganizationName(QStringLiteral("BJTU Group 15"));

    // 设置默认字体
    QFont appFont(QStringLiteral("Microsoft YaHei UI"), 10);
    app.setFont(appFont);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
