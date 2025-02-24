#include "mytcpserver.h"
#include <QDebug>

MyTcpServer::MyTcpServer()
{

}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)
{
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(handle);
    qDebug() << "连接成功！！！";
    getInstance().mysocketlist.append(pTcpSocket);

    qDebug() << getInstance().mysocketlist.size();

    connect(pTcpSocket,SIGNAL(offline(MyTcpSocket*)),this,SLOT(deleteSocket(MyTcpSocket*)));

}

void MyTcpServer::delete_offline(MyTcpSocket *mytcpsocket)
{
    QList<MyTcpSocket*>::iterator iter = mysocketlist.begin();
    for(;iter!=mysocketlist.end();iter++)
    {
        if(mytcpsocket == *iter)
        {
            delete *iter;
            *iter = NULL;
            mysocketlist.erase(iter);
            break;
        }
    }
}

//将用户添加好友请求返回给客户端
void MyTcpServer::resend(const char *client_name, PDU *pdu)
{
    qDebug() << client_name << pdu << "添加好友测试..." << mysocketlist.size();

    if(client_name == NULL || pdu == NULL)
        return;

    for(int i=0; i<mysocketlist.size();i++)
    {
        if(mysocketlist.at(i)->get_login_name() == client_name)
        {
            qDebug() << "resend...";
            mysocketlist.at(i)->write((char*)pdu, pdu->uiPDULen);
            break;
        }
    }
}
