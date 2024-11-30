#include "tcpserver.h"
#include <QApplication>
#include "database.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Database::getInstance().initdatabase();
    TcpServer w;
    w.show();
    return a.exec();
}
