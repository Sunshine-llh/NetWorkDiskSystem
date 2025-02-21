#include "mytcpsocket.h"
#include <QDebug>
#include "database.h"
MyTcpSocket::MyTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this,&QTcpSocket::readyRead,this,&MyTcpSocket::remsg);
}
// 接收来自客户端的消息
void MyTcpSocket::remsg()
{
    //qDebug() << this->bytesAvailable();
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
        if(rs)
        {
            free(pdu);
            pdu = mkPDU(0);
            pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
            login_name=caName;
            strcpy(pdu->caData,LOGIN_OK);

        }
        else
        {
            free(pdu);
            pdu = mkPDU(0);
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
        qDebug() << login_name;
        bool rs = Database::getInstance().offline(login_name);

        if(rs)
        {
            free(pdu);
            pdu = mkPDU(0);
            pdu->uiMsgType=ENUM_MSG_TYPE_OFFLINE_RESPOND;
            strcpy(pdu->caData,OFF_LINE_OK);
        }
        else
        {
            free(pdu);
            pdu = mkPDU(0);
            pdu->uiMsgType=ENUM_MSG_TYPE_OFFLINE_RESPOND;
            strcpy(pdu->caData,OFF_LINE_FAILED);
            login_name = "";
        }
        write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
        break;
    }

    //在线查询好友请求
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
    {
        results = Database:: getInstance().get_Online_friend();
        if(results.isEmpty())
        {
            return;
        }
        free(pdu);

        //定义Msg长度
        uint Msg_Len = results.size() * 32;
        PDU *rspdu = mkPDU(Msg_Len);
        rspdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
        //strcpy(pdu->caData, SEARCH_USR_ONLINE);
        for(int i=0;i<results.size();i++)
        {
            memcpy((char *)rspdu->caMsg + i*32,results.at(i).toStdString().c_str(),results.at(i).size());
            qDebug() << results.at(i);
        }

        write((char*)rspdu, rspdu->uiPDULen);

        qDebug() << "在线查询好友请求";
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

        if(results.isEmpty())
        {
            return;
        }
        free(pdu);
        //定义Msg长度
        uint Msg_Len = results.size() * 32;

        PDU *pdu = mkPDU(Msg_Len);
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        memcpy((char*)pdu->caMsg, results.at(0).toStdString().c_str(), 32);
        memcpy((char*)pdu->caMsg + 32, results.at(1).toStdString().c_str(), 32);

        write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
        break;
    }

    default: break;

    }

}

