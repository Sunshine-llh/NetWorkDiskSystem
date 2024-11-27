#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H
#include <QTcpServer>
#include "mytcpsocket.h"
#include <QList>
class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();
    static MyTcpServer &getInstance();
    void incomingConnection(qintptr handle);
private:
    QList<MyTcpSocket*> mysocketlist;
};

#endif // MYTCPSERVER_H
