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
    void resend(const char* friend_name, PDU *pdu);
    void resend_(const char* friend_name, PDU *pdu);
private:
    QList<MyTcpSocket*> mysocketlist;
public slots:
    void delete_offline(MyTcpSocket *mytcpsocket);
};

#endif // MYTCPSERVER_H
