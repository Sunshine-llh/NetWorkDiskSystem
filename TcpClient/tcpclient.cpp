#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QDebug>
TcpClient::TcpClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
    //连接服务器
    connect(&m_tcpScoket,SIGNAL(connected()),this,SLOT(showConnect()));
    m_tcpScoket.connectToHost(QHostAddress(m_strIP),m_usPort);
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

void TcpClient::on_send_clicked()
{
    QString strMsg = ui->linetext->text();
       if(!strMsg.isEmpty())
       {
           PDU *pdu = mkPDU(strMsg.size());
           pdu->uiMsgType = 8888;
           memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
           m_tcpScoket.write((char*)pdu,pdu->uiPDULen);
           qDebug() << "111";
           free(pdu);
           pdu = NULL;
       }
       else
       {
           QMessageBox::warning(this,"信息发送","发送的信息不能为空");
       }


}

