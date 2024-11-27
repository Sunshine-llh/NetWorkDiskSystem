#include "mytcpsocket.h"
#include <QDebug>
MyTcpSocket::MyTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    connect(this,SIGNAL(readyRead()),this,SLOT(remsg()));
}
 void MyTcpSocket::remsg()
{
    //读取消息
     qDebug() << this->bytesAvailable();
     uint uiPDULen = 0;
     this->read((char*)&uiPDULen,sizeof(uint));
     uint uiMsgLen = uiPDULen - sizeof(PDU);
     PDU *pdu = mkPDU(uiMsgLen);
     this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
     qDebug() << pdu->uiMsgType << pdu->caMsg;
}
