#include "tcpserver.h"
#include <QTcpServer>
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

TcpServer::TcpServer(QWidget *parent) : QMainWindow(parent), ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QTcpServer *mTcpServer = new QTcpServer(this);//服务器对象
    QHostAddress mHostAddress;//本地IP地址
    quint16 _port;
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        strData.replace("\r\n"," ");
        QStringList strList = strData.split(" ");
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        mHostAddress.setAddress(m_strIP);
        _port = m_usPort;
        mTcpServer->listen(mHostAddress,_port);
        connect(mTcpServer,&QTcpServer::newConnection,this,[=]{
            qDebug() << "new client connected";;
        });
        file.close();
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}
void TcpServer::listening()
{
    //如果监听到client端的用户请求
   // QTcpServer *mTcpServer = new QTcpServer(this);
    //connect(mTcpServer,&QTcpServer::newConnection,)
}
