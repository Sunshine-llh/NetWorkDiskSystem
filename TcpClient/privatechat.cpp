#include "privatechat.h"
#include "ui_privatechat.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>
PrivateChat::PrivateChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}
void PrivateChat::save_chat_name(const char *chat_name)
{
    this->chat_name = chat_name;
    this->login_name = TcpClient::getInstance().get_login_name();
}

//私聊发送点击事件
void PrivateChat::on_sendMsg_pb_clicked()
{

    QString msg = ui->inputMsg_le->text();
    qDebug() << "私聊发送点击事件..." << msg;

    if(msg != NULL)
    {
        uint msg_size = msg.size();
        PDU *pdu = mkPDU(msg_size);
        pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;

        memcpy(pdu->caData, login_name.toStdString().c_str(), 32);
        memcpy(pdu->caData + 32, chat_name.toStdString().c_str(), 32);
        strcpy((char*)pdu->caMsg, msg.toStdString().c_str());

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    }
    else
    {
        QMessageBox::information(this, "消息发送", "发送消息不能为空！！！");
    }
}


//更新界面私聊信息
void PrivateChat::update_msg(const PDU *pdu)
{
    qDebug() << "更新界面私聊信息...";

    if(pdu == NULL)
        return;

    char friend_name[32] = {'\0'};
    memcpy(friend_name, pdu->caData, 32);
    QString msg = QString("%1: %2").arg(friend_name).arg((char*)pdu->caMsg);
    ui->showMsg_te->append(msg);
}
