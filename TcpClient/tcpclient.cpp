#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QDebug>
#include <string.h>
#include <opewidget.h>
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

//返回实例对象TcpClient
TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

//返回tcp_socket对象
QTcpSocket &TcpClient:: getTcpSocket()
{
    return m_tcpScoket;
}

//获取服务器结果
void TcpClient::showConnect()
{
    QMessageBox::information(this,"连接服务器","SUCESS!!!");
}

//保存用户登录名
QString TcpClient:: get_login_name()
{
    return this->login_name;
}

//用于接受服务器的的响应函数
void TcpClient::show_information()
{
    //获取服务器端响应的数据
    qDebug() << "客户端响应...";
    uint uiPDULen=0;
    m_tcpScoket.read((char*)&uiPDULen,sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpScoket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    qDebug() << pdu->uiMsgType;
    qDebug() << pdu->caData << "test---";

    //判断返回的类型
    switch (pdu->uiMsgType) {

    case ENUM_MSG_TYPE_REGIST_RESPOND :
    {
        if(pdu->uiMsgType==1)
            QMessageBox::information(this,"注册",pdu->caData);
        else
            QMessageBox::information(this,"注册",pdu->caData);
        break;
    }

    case ENUM_MSG_TYPE_LOGIN_RESPOND  :
    {
        if(strcmp(pdu->caData,LOGIN_FAILED) == 0)
        {
            QMessageBox::information(this,"登录",pdu->caData);
        }
        else
        {
            QMessageBox::information(this,"登录",pdu->caData);
            OpeWidget::getInstance().show();
            this->hide();
        }
        break;
    }

    case ENUM_MSG_TYPE_OFFLINE_RESPOND  :
    {
        if(pdu->uiMsgType==5)
            QMessageBox::information(this,"退出",pdu->caData);
        else
            QMessageBox::information(this,"退出",pdu->caData);
        break;
    }

    //接收服务器返回的在线好友
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND :
    {
        qDebug() << "接收服务器返回的在线好友...";
        qDebug()<< pdu->caMsg;
        OpeWidget::getInstance().get_Friend()->showAllOnlineUsr(pdu);

        break;
    }

    //接受服务器返回的用户指定查询好友
     case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
    {

        qDebug() << "接受服务器返回的用户指定查询好友...";
        char name[32];
        char online_state[32];

        if(strcmp(pdu->caData, SEARCH_USR_NO)==0)
        {
            QMessageBox::information(this, "搜索好友", QString("%1 : not exits").arg(OpeWidget::getInstance().get_Friend()->Search_name));
        }
        else
        {
            memcpy((char*)name, (char*)pdu->caMsg, 32);
            memcpy((char*)online_state, (char*)pdu->caMsg + 32, 32);
            QMessageBox::information(this, "搜索好友", QString("\'%1\' :\'%2\'").arg(OpeWidget::getInstance().get_Friend()->Search_name).arg(online_state));
        }

        break;
    }
    //回复好友添加请求
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        qDebug() << "收到好友添加请求";
        char friend_name[32] = {'\0'};
        strncpy(friend_name, pdu->caData+32, 32);

        PDU *respdu =mkPDU(0);
        memcpy(respdu->caData, pdu->caData, 64);
        int res = QMessageBox::information(this, "好友添加回复", QString("\'%1\' want to add with you?").arg(friend_name));

        if(res == QMessageBox::Yes)
        {
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
        }
        else
        {
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
        }

        m_tcpScoket.write((char*)respdu, respdu->uiMsgLen);
        free(respdu);
        respdu =NULL;
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
    {
        QMessageBox::information(this,"好友请求",pdu->caData);
        break;
    }

    //接受服务器返回的添加好友请求
      case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
    {
        qDebug() << "接受服务器返回的添加好友成功请求...";
        char friend_name[32] = {'\0'};
        strncpy(friend_name, pdu->caData + 32, 32);
        QMessageBox::information(this, "添加好友", QString("添加好友:\'%1\'成功！").arg(friend_name));
        break;

    }
     case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
    {
       qDebug() << "接受服务器返回的添加好友失败请求...";
       char friend_name[32] = {'\0'};
       strncpy(friend_name, pdu->caData + 32, 32);
       QMessageBox::information(this, "添加好友", QString("添加好友:\'%1\'失败！").arg(friend_name));
       break;

    }
    default:
    {
        qDebug() << "default...";
        pdu = NULL;
        break;
    }
    }
}

//登录按钮
void TcpClient::on_login_pb_clicked()
{

    //获取用户登录信息
    QString username=ui->username->text();
    QString password=ui->password->text();

    if(username==NULL || password==NULL)
    {
        QMessageBox::warning(this,"提示","用户名密码不为空！");
    }

    this->login_name = username;

    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
    strncpy(pdu->caData,username.toStdString().c_str(),32);
    strncpy(pdu->caData+32,password.toStdString().c_str(),32);
    m_tcpScoket.write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

//注册按钮
void TcpClient::on_regist_pb_clicked()
{
    //获取用户登录信息
    QString username=ui->username->text();
    QString password=ui->password->text();

    if(!username.isEmpty()&&!password.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,username.toStdString().c_str(),32);
        strncpy(pdu->caData+32,password.toStdString().c_str(),32);
        m_tcpScoket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }
    else
    {
        QMessageBox::warning(this,"提示","用户名密码不为空！");
        return;
    }
}

//注销按钮
//void TcpClient::on_cancel_pb_clicked()
//{
//    PDU *pdu=mkPDU(0);
//    pdu->uiMsgType=ENUM_MSG_TYPE_OFFLINE_REQUEST;
//    //向服务器端发送用户信息
//    m_tcpScoket.write((char*)pdu,pdu->uiPDULen);
//    free(pdu);
//    pdu = NULL;
//}
