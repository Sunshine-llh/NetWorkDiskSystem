#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include <database.h>
#include <QFile>
class MyTcpSocket : public QTcpSocket
{
private:
    QString login_name;
    QStringList results;

    QFile file;
    qint64 total_size;
    qint64 received_size;
    bool upload_flag;

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
