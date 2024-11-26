#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
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
    QString linemsg=ui->linetext->text();
    //调用MPD函数
    if(!linemsg.isEmpty()){
        PDU *pdu=mkPDU(linemsg.size());
        pdu->uiMsgType=8888;
        pdu->uiMsgLen=linemsg.size();
        memcpy(pdu->caMsg,linemsg.toStdString().c_str(),linemsg.size());
        m_tcpScoket.write((char *)pdu,pdu->uiPDULen);
        //释放pdu
        free(pdu);
        pdu=NULL;
    }else
    {
        QMessageBox::warning(this,"信息发送","发送的信息不能为空");
    }


}

