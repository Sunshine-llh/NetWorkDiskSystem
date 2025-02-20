#include "friend.h"
#include <qdebug.h>
#include "protocol.h"
#include "tcpclient.h"

// 好友功能主体窗口
Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pShowMsgTE = new QTextEdit;
    m_pFriendListWidget = new QListWidget;
    m_pInputMsgLE = new QLineEdit;
    m_pDelFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友");
    m_pShowOnlineUsrPB = new QPushButton("显示在线用户");
    m_pSearchUsrPB = new QPushButton("查找用户");
    m_pMsgSendPB = new QPushButton("信息发送");
    m_pPrivateChatPB = new QPushButton("私聊");

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUsrPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);
    m_pOnline = new Online;
    QVBoxLayout *pMain = new QVBoxLayout;

    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();
    setLayout(pMain);
    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool)),this,SLOT(showOnline()));
    connect(m_pSearchUsrPB,SIGNAL(clicked(bool)),this,SLOT(showFriend()));
}

// 在线用户请求
void Friend::showOnline()
{
    qDebug() << "showOnline";
    if(m_pOnline->isHidden())
    {
       //向服务端发送请求
       PDU *pdu = mkPDU(0);
       pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
       TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
       free(pdu);
       pdu = NULL;
       m_pOnline->show();
    }
    else
    {
        m_pOnline->hide();
    }

}

//搜索用户请求
void Friend::showFriend()
{
    qDebug() << "showFriend";
    if(m_pOnline->isHidden())
    {
       //向服务端发送请求
       PDU *pdu = mkPDU(0);
       pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
       TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
       free(pdu);
       pdu = NULL;
       m_pOnline->show();
    }
    else
    {
        m_pOnline->hide();
    }
}

//展示服务器发送过来的在线用户
void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(pdu ==  NULL)
        return;
    m_pOnline->show_Online_Usr(pdu);
}
