#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include "protocol.h"
#include <database.h>
#include <QFile>
#include <QTimer>
class MyTcpSocket : public QTcpSocket
{
private:
    QString login_name;
    QStringList results;

    QFile file;
    qint64 total_size;
    qint64 received_size;
    bool upload_flag;
    QTimer *m_pTimer;
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString get_login_name();
    void Copy_Dir_File(QString strpath, QString despath);
public slots:
    void remsg();
    void disconnected();
    void send_File_to_Client();
signals:
    void offline(MyTcpSocket *mysocket);
};

#endif // MYTCPSOCKET_H
