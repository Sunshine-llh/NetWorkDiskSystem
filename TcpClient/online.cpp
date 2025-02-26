#include "online.h"
#include "ui_online.h"
#include "protocol.h"
#include <QDebug>
#include "tcpclient.h"
Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

//将服务器返回回来的用户显示到窗口页面
void Online::show_Online_Usr(PDU *pdu)
{
    if(pdu == NULL)
        return;
    int name_num = pdu->uiMsgLen / 32;

    qDebug() << name_num;
    char name[32];
    for(int i=0;i<name_num;i++)
    {
        memcpy(name, (char*)pdu->caMsg + i*32,32);
        qDebug() << name;
        ui->online_lw->addItem(name);
    }
}

//加好友请求
void Online::on_addFriend_pb_clicked()
{

    QListWidgetItem *Item = ui->online_lw->currentItem();
    QString friend_name = Item->text();

    //获取当前用户和所加好友的关系
    QString login_name = TcpClient::getInstance().get_login_name();

    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData, login_name.toStdString().c_str(), login_name.size());
    memcpy(pdu->caData + 32, friend_name.toStdString().c_str(), friend_name.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    qDebug() << "点击加好友按钮..." << login_name << friend_name;
    free(pdu);
    pdu = NULL;
}

