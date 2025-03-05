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

    QString str_name = client_name;


    for(int i=0; i<mysocketlist.size();i++)
    {
        qDebug() << mysocketlist.at(i)->get_login_name();

        if(mysocketlist.at(i)->get_login_name() == str_name)
        {
            qDebug() << "resend..." << mysocketlist.at(i)->get_login_name();

            mysocketlist.at(i)->write((char*)pdu,pdu->uiPDULen);
            break;
        }
    }
}

//回复客户端分享者
void MyTcpServer::resend_(const char *friend_name, PDU *pdu)
{
     qDebug() << "friend_name:" << friend_name << "回复客户端分享者...";

     if(friend_name == NULL || pdu == NULL)
     {
         return;
     }

     QString str_name = friend_name;

     for(int i=0; i<mysocketlist.size();i++)
     {
         qDebug() << mysocketlist.at(i)->get_login_name();

         if(mysocketlist.at(i)->get_login_name() == str_name)
         {
             qDebug() << "resend..." << mysocketlist.at(i)->get_login_name();

             mysocketlist.at(i)->write((char*)pdu,pdu->uiPDULen);
             break;
         }
     }

}
