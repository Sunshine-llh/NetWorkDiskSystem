#include "tcpclient.h"
#include <QApplication>
#include "friend.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //TcpClient w;
    Friend w;
    w.show();
    return a.exec();
}
