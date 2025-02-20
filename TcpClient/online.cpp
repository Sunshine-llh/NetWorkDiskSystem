#include "online.h"
#include "ui_online.h"
#include "protocol.h"
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
    char name[32];
    for(int i=0;i<name_num;i++)
    {
        memcpy((char*)name, pdu->caMsg + i*32,32);
        ui->online_lw->addItem(name);
    }
}
