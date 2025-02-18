#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include <database.h>
class MyTcpSocket : public QTcpSocket
{
private:
    QString login_name;
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
public slots:
    void remsg();
};

#endif // MYTCPSOCKET_H
