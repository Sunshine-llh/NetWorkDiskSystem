#include "mytcpsocket.h"
#include <QDebug>
MyTcpSocket::MyTcpSocket(QObject *parent) : QTcpSocket(parent)
{
   connect(this,&QTcpSocket::readyRead,this,&MyTcpSocket::remsg);
}
// 接收来自客户端的消息
void MyTcpSocket::remsg()
{
    qDebug() << this->bytesAvailable();
    uint uiPDULen = 0;
    this->read((char*)&uiPDULen,sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    strncpy(caName,pdu->caData,32);
    strncpy(caPwd,pdu->caData+32,32);
    qDebug() << caName << caPwd << pdu->uiMsgType;
}

