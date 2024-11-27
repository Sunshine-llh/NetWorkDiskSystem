#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdio.h>
#include <QString>
struct PDU
{
    //总消息长度
    uint uiPDULen;
    //消息类型
    uint uiMsgType;
    //文件名
    char caData;
    //实际消息长度
    uint uiMsgLen;
    //实际消息
    int caMsg[];
};
PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
