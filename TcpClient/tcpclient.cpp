#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
TcpClient::TcpClient(QWidget *parent) : QWidget(parent), ui(new Ui::TcpClient)
{
    //ui->setupUi();
    loadConfig();
    //连接服务器
    connect(&m_tcpScoket,SIGNAL(connected()),this,SLOT(showConnect()));
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        strData.replace("\r\n"," ");
        QStringList strList = strData.split(" ");
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << "IP地址为：" << m_strIP << "端口为：" << m_usPort;
        file.close();
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}
//获取服务器结果
void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","SUCESS!!!");
}
