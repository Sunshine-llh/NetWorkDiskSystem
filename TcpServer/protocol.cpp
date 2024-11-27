#include "protocol.h"
PDU *mkPDU(uint uiMsgLen)
{
    //初始化
    uint uiPDULen=sizeof (PDU)+uiMsgLen;
    //分配内存
    PDU *pdu=(PDU*)malloc(uiPDULen);

    if(NULL==pdu)
    {
        exit(EXIT_FAILURE);
    }
    pdu->uiMsgLen=uiMsgLen;
    pdu->uiPDULen=uiPDULen;
    return pdu;
}
