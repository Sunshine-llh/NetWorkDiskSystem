#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QDebug>
#include <string.h>
TcpClient::TcpClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
    //连接服务器
    connect(&m_tcpScoket,SIGNAL(connected()),this,SLOT(showConnect()));
    //监听服务器
    connect(&m_tcpScoket,&QTcpSocket::readyRead,this,&TcpClient::show_information);
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

//void TcpClient::on_send_clicked()
//{
//    QString strMsg = ui->linetext->text();
//       if(!strMsg.isEmpty())
//       {
//           PDU *pdu = mkPDU(strMsg.size());
//           pdu->uiMsgType = 8888;
//           memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
//           m_tcpScoket.write((char*)pdu,pdu->uiPDULen);
//           qDebug() << "111";
//           free(pdu);
//           pdu = NULL;
//       }
//       else
//       {
//           QMessageBox::warning(this,"信息发送","发送的信息不能为空");
//       }
//}
//用于接受服务器的的响应函数
void TcpClient::show_information()
{
    //获取服务器端响应的数据
    uint uiPDULen=0;
    m_tcpScoket.read((char*)&uiPDULen,sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpScoket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    qDebug() << pdu->uiMsgType;
    qDebug() << pdu->caData;
    //判断返回的类型
    switch (pdu->uiMsgType) {

    case ENUM_MSG_TYPE_REGIST_RESPOND :
    {
        if(pdu->uiMsgType==1)
            QMessageBox::information(this,"注册",pdu->caData);
        else
            QMessageBox::information(this,"注册",pdu->caData);
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND  :
    {
        if(pdu->uiMsgType==3)
            QMessageBox::information(this,"登录",pdu->caData);
        else
            QMessageBox::information(this,"登录",pdu->caData);
    }
    case ENUM_MSG_TYPE_OFFLINE_RESPOND  :
    {
        if(pdu->uiMsgType==5)
            QMessageBox::information(this,"登录",pdu->caData);
        else
            QMessageBox::information(this,"登录",pdu->caData);
    }
    default:
    {
        break;
    }


    }
}
//登录按钮
void TcpClient::on_login_pb_clicked()
{
    //客户端提交登录信息
    //获取username文本信息
    QString username=ui->username->text();
    //获取passwo文本信息
    QString password=ui->password->text();
    if(username==NULL || password==NULL)
    {
        QMessageBox::warning(this,"提示","用户名密码不为空！");
    }
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
    strncpy(pdu->caData,username.toStdString().c_str(),32);
    strncpy(pdu->caData+32,password.toStdString().c_str(),32);
    m_tcpScoket.write((char*)pdu,pdu->uiPDULen);

}

//注册按钮
void TcpClient::on_regist_pb_clicked()
{
    //获取username文本信息
    QString username=ui->username->text();
    //获取passwo文本信息
    QString password=ui->password->text();
    qDebug() << username <<password;
    if(!username.isEmpty()&&!password.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,username.toStdString().c_str(),32);
        strncpy(pdu->caData+32,password.toStdString().c_str(),32);
        // pdu->uiMsgLen=sizeof(uint)+pdu->caData.s
        qDebug() << pdu->uiPDULen;
        //向服务器端发送用户信息
        m_tcpScoket.write((char*)pdu,pdu->uiPDULen);

    }
    else
    {
        QMessageBox::warning(this,"提示","用户名密码不为空！");
        return;
    }
}

//注销按钮
void TcpClient::on_cancel_pb_clicked()
{
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_OFFLINE_REQUEST;
    //向服务器端发送用户信息
    m_tcpScoket.write((char*)pdu,pdu->uiPDULen);
}
