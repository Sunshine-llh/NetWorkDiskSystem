#include "friend.h"
#include <qdebug.h>
#include "protocol.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include "privatechat.h"
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
    connect(m_pFlushFriendPB, SIGNAL(clicked(bool)),this,SLOT(Flush_friends()));
    connect(m_pDelFriendPB, SIGNAL(clicked(bool)), this, SLOT(delete_friend()));
    connect(m_pPrivateChatPB, SIGNAL(clicked(bool)), this, SLOT(private_chat()));
    connect(m_pMsgSendPB, SIGNAL(clicked(bool)), this, SLOT(group_chat()));

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

    Search_name = QInputDialog::getText(this, "好友搜索", "用户名");

    if(!Search_name.isEmpty())
    {
        PDU *pdu =mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        memcpy((char*)pdu->caData, Search_name.toStdString().c_str(),Search_name.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }

}

//点击刷新在线好友请求按钮
void Friend::Flush_friends()
{
    qDebug() << "点击刷新在线好友请求按钮...";

    QString login_name = TcpClient::getInstance().get_login_name();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData, login_name.toStdString().c_str(),32);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);

    free(pdu);
    pdu = NULL;
}

//点击删除好友请求按钮
void Friend::delete_friend()
{
    QString Delete_name = m_pFriendListWidget->currentItem()->text();

    qDebug() << "点击删除好友请求...";

    QString login_name = TcpClient::getInstance().get_login_name();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    memcpy(pdu->caData, login_name.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, Delete_name.toStdString().c_str(), 32);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);

    free(pdu);
    pdu = NULL;

}

//点击私聊按钮
void Friend::private_chat()
{
    if(m_pFriendListWidget != NULL)
    {
        QString chat_name = m_pFriendListWidget->currentItem()->text();
        PrivateChat::getInstance().save_chat_name(chat_name.toStdString().c_str());
        if(PrivateChat::getInstance().isHidden())
        {
            PrivateChat::getInstance().show();
        }
    }
    else
    {
        QMessageBox::information(this, "私聊", "请选择私聊对象！");
    }
}

//点击发送信息按钮
void Friend::group_chat()
{
    QString msg = m_pInputMsgLE->text();

    if(m_pFriendListWidget != NULL)
    {
        QString login_name = TcpClient::getInstance().get_login_name();
        //unit msg_len = msg.size() * 32;
        PDU *pdu = mkPDU(0)
    }
    else
    {
        QMessageBox::information(this, "群聊消息", "无在线好友！");
    }
}
//展示服务器发送过来的在线用户
void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(pdu ==  NULL)
        return;
    m_pOnline->show_Online_Usr(pdu);

}

//更新当前用户的好友在线列表
void Friend::update_Online_FriendList(PDU *pdu)
{
    qDebug() << "更新当前用户的好友在线列表...";

    char caName[32] = {'\0'};
    uint ui_size = pdu->uiMsgLen / 32;

    for(int i=0;i<ui_size;i++)
    {
        memcpy(caName,pdu->caMsg + i * 32, 32);
        m_pFriendListWidget->addItem(caName);
    }
}

//更新群聊信息
void Friend::update_Group_Msg(PDU *pdu)
{
    qDebug() << "更新群聊信息...";
    char send_name[32] = {'\0'};
    memcpy(send_name, pdu->caData, 32);
    QString msg = QString("%1: %2").arg(send_name).arg((char*)pdu->caMsg);

    m_pShowMsgTE->append(msg);

}
