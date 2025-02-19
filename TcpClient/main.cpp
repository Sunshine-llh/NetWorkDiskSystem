#include "tcpclient.h"
#include <QApplication>
#include "opewidget.h"
#include "online.h"
#include "friend.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpClient::getInstance().show();
    return a.exec();
}
