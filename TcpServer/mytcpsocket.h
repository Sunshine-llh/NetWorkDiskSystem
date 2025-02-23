#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include <database.h>
class MyTcpSocket : public QTcpSocket
{
private:
    QString login_name;
    QStringList results;
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString get_login_name();
public slots:
    void remsg();
    void disconnected();
signals:
    void offline(MyTcpSocket *mysocket);
};

#endif // MYTCPSOCKET_H
