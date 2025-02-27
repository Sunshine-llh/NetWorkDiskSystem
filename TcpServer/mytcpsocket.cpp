#include "mytcpsocket.h"
#include <QDebug>
#include "database.h"
#include "mytcpserver.h"
MyTcpSocket::MyTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this,&QTcpSocket::readyRead,this,&MyTcpSocket::remsg);
    connect(this,&QTcpSocket::disconnected,this,&MyTcpSocket::disconnected);
}

//用户返回请求用户的用户名
QString MyTcpSocket::get_login_name()
{
    return this->login_name;
}

//断开连接函数
void MyTcpSocket::disconnected()
{
    qDebug()<< login_name << "已断开连接...";
    Database::getInstance().update_online_state(login_name);
    emit offline(this);
}

void MyTcpSocket::offline(MyTcpSocket *mytcpsoket)
{

}

// 接收来自客户端的消息
void MyTcpSocket::remsg()
{
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen,sizeof(uint));
    qDebug() << uiPDULen;
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));

    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};

    strncpy(caName,pdu->caData,32);
    strncpy(caPwd,pdu->caData+32,32);

    switch (pdu->uiMsgType)
    {
    //注册请求
    case ENUM_MSG_TYPE_REGIST_REQUEST:
    {
        //向客户端响应
        bool rs=Database::getInstance().regist(caName,caPwd);
        if(rs)
        {
            free(pdu);
            pdu = mkPDU(0);
            pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
            strcpy(pdu->caData,REGIST_OK);
            qDebug() << "注册成功!";

        }
        else
        {
            pdu = mkPDU(0);
            qDebug() << "test";
            pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
            strcpy(pdu->caData,REGIST_FAILED);
            qDebug() << "注册失败!";
        }

        write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
        break;

    }
    //登录请求
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
    {
        //向客户端响应
        bool rs=Database::getInstance().login(caName,caPwd);
        free(pdu);
        pdu = mkPDU(0);
        if(rs)
        {
            pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
            login_name=caName;
            strcpy(pdu->caData,LOGIN_OK);
        }
        else
        {
            pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
            strcpy(pdu->caData,LOGIN_FAILED);
        }

        write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
        break;
    }

    //下线请求
    case ENUM_MSG_TYPE_OFFLINE_REQUEST:
    {
//        qDebug() << login_name;
//        Database::getInstance().offline(login_name);

//        if(rs)
//        {
//            free(pdu);
//            pdu = mkPDU(0);
//            pdu->uiMsgType=ENUM_MSG_TYPE_OFFLINE_RESPOND;
//            strcpy(pdu->caData,OFF_LINE_OK);
//        }
//        else
//        {
//            free(pdu);
//            pdu = mkPDU(0);
//            pdu->uiMsgType=ENUM_MSG_TYPE_OFFLINE_RESPOND;
//            strcpy(pdu->caData,OFF_LINE_FAILED);
//            login_name = "";
//        }
//        write((char *)pdu,pdu->uiPDULen);
//        free(pdu);
//        pdu=NULL;
//        break;
    }

     //响应客户端添加好友请求
     case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        qDebug() << "响应客户端添加好友请求...";
        char client_name[32] = {'\0'};
        char friend_name[32] = {'\0'};

        strncpy(client_name, pdu->caData, 32);
        strncpy(friend_name, pdu->caData+32, 32);
        int res = Database::getInstance().add_friend(client_name, friend_name);

        qDebug() << 'res:' << res << "..." << client_name << friend_name;
        PDU *respdu = NULL;

        if(res == -1)
        {   qDebug() << "-1 ...";
            respdu =mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, UNKNOW_ERROR);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        else if(res == 0)
        {
            qDebug() << "0 ...";
            respdu =mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, EXISTED_FRIEND);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        else if(res == 1)
        {
            qDebug() << "1 ...";
            MyTcpServer::getInstance().resend(friend_name, pdu);
        }
        else if(res == 2)
        {
            qDebug() << "2 ...";
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        else if(res == 3){
            qDebug() << "3 ...";
            PDU *respdu =mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData, ADD_FRIEND_NO_EXIST);
            write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        break;

    }

        //好友添加请求
        //1、同意
         case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
        {
            char login_name[32] = {'\0'};
            char friend_name[32] = {'\0'};
            qDebug() << friend_name << "同意............";

            strncpy(login_name, pdu->caData, 32);
            strncpy(friend_name, pdu->caData + 32, 32);

            Database::getInstance().handle_agree_friend(login_name, friend_name);

            MyTcpServer::getInstance().resend(login_name, pdu);
            break;
        }
        //2、拒绝
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {
            char login_name[32] = {'\0'};
            strncpy(login_name, pdu->caData, 32);
            qDebug() << "拒绝...";

            MyTcpServer::getInstance().resend(login_name, pdu);
            break;
        }


    //在线查询好友请求
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
    {
        qDebug() << "在线查询好友请求";
        results = Database:: getInstance().get_Online_friend();

        if(results.isEmpty())
        {
            return;
        }

        uint Msg_Len = results.size() * 32;
        PDU *rspdu = mkPDU(Msg_Len);
        rspdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;

        for(int i=0;i<results.size();i++)
        {
            memcpy((char *)rspdu->caMsg + i*32, results.at(i).toStdString().c_str(), results.at(i).size()+1);
            qDebug() << results.at(i);
        }

        write((char*)rspdu, rspdu->uiPDULen);
        free(rspdu);
        rspdu = NULL;
        break;
    }

    //搜索用户请求
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
    {

        qDebug() << "搜索用户请求";
        char search_name[32] = {'\0'};
        strncpy((char*)search_name, pdu->caData, 32);
        results = Database::getInstance().Search_friend(search_name);

        qDebug() << results;
        if(results.isEmpty())
        {
            PDU *rspdu = mkPDU(0);
            rspdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
            strcpy(rspdu->caData,SEARCH_USR_NO);
            write((char*)rspdu, rspdu->uiPDULen);
            return;
        }

        free(pdu);
        //定义Msg长度
        uint Msg_Len = results.size() * 32;

        PDU *rspdu = mkPDU(Msg_Len);
        rspdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        memcpy((char*)rspdu->caData,results.at(1).toStdString().c_str(), 32);

        memcpy((char*)rspdu->caMsg, results.at(0).toStdString().c_str(), 32);
        memcpy((char*)rspdu->caMsg + 32, results.at(1).toStdString().c_str(), 32);

        qDebug() << rspdu->caMsg;
        write((char*)rspdu, rspdu->uiPDULen);
        free(rspdu);
        pdu = NULL;

        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
    {
        qDebug() << "服务器响应刷新在线好友...";
        char login_name[32] = {'\0'};
        memcpy(login_name, pdu->caData, 32);
        QStringList results = Database::getInstance().get_online_friends(login_name);

        uint Msg_Len = results.size() * 32;

        PDU * respdu = mkPDU(Msg_Len);
        respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;

        for(int i=0; i<results.size();i++)
        {
            memcpy(respdu->caMsg + i * 32, results.at(i).toStdString().c_str(), 32);
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;

        break;

    }
     //接受删除好友请求
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
    {
        qDebug() << "服务器接受删除好友请求...";
        char login_name[32] = {'\0'};
        char friend_name[32] = {'\0'};
        memcpy(login_name, pdu->caData, 32);
        memcpy(friend_name, pdu->caData + 32, 32);

        bool res = Database::getInstance().delete_friend(login_name, friend_name);

        PDU *respdu = mkPDU(0);

        if(res)
        {
            strcpy(respdu->caData, DEL_FRIEND_OK);
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            write((char*)respdu, respdu->uiPDULen);
            MyTcpServer::getInstance().resend(friend_name, pdu);
        }
        else
        {
            strcpy(respdu->caData, DEL_FRIEND_FAILED);
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            write((char*)respdu, respdu->uiPDULen);
        }
        free(respdu);
        free(pdu);
        respdu = NULL;
        pdu = NULL;
        break;
    }

     //接受私聊请求
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
           qDebug() << "服务器接受私聊请求...";
           char friend_name[32] = {'\0'};
           memcpy(friend_name, pdu->caData + 32, 32);

           MyTcpServer::getInstance().resend(friend_name, pdu);
           break;
    }

    default: break;

    }

}

