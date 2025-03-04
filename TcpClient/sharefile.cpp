#include "sharefile.h"
#include <QtDebug>
#include "tcpclient.h"
#include "opewidget.h"

ShareFile::ShareFile(QWidget *parent) : QWidget(parent)
{
        m_pSelectAllPB = new QPushButton("全选");
        m_pCancelSelectPB = new QPushButton("取消选择");
        m_pOKPB = new QPushButton("确定");
        m_pCancelPB = new QPushButton("取消");
        m_pSA = new QScrollArea;
        m_pFriendW = new QWidget;
        m_pFriendWVBL = new QVBoxLayout(m_pFriendW);
        m_pButtonGroup = new QButtonGroup(m_pFriendW);
        m_pButtonGroup->setExclusive(false);

        QHBoxLayout *pTopHBL = new QHBoxLayout;
        pTopHBL->addWidget(m_pSelectAllPB);
        pTopHBL->addWidget(m_pCancelSelectPB);
        pTopHBL->addStretch();

        QHBoxLayout *pDownHBL = new QHBoxLayout;
        pDownHBL->addWidget(m_pOKPB);
        pDownHBL->addWidget(m_pCancelPB);

        QVBoxLayout *pMainVBL = new QVBoxLayout;
        pMainVBL->addLayout(pTopHBL);
        pMainVBL->addWidget(m_pSA);
        pMainVBL->addLayout(pDownHBL);
        setLayout(pMainVBL);

        connect(m_pSelectAllPB, SIGNAL(clicked(bool)), this, SLOT(Selected_All()));
        connect(m_pCancelSelectPB, SIGNAL(clicked(bool)), this, SLOT(Cancel_Selected()));
        connect(m_pOKPB, SIGNAL(clicked(bool)), this, SLOT(Sure_Ok()));
        connect(m_pCancelPB, SIGNAL(clicked(bool)), this, SLOT(Cancel()));

}


void ShareFile::Selected_All()
{

    QList<QAbstractButton*> button_list = m_pButtonGroup->buttons();
    qDebug() << "选择所有..." << "QList_Size:" << button_list.size();

    for (int i=0;i<button_list.size() ;i++ )
    {
          if(!button_list[i]->isChecked())
          {
              button_list[i]->setChecked(true);
          }
    }
}

void ShareFile::Cancel_Selected()
{
    QList<QAbstractButton*> button_list = m_pButtonGroup->buttons();
    qDebug() << "取消所有..." << "QList_Size:" << button_list.size();

    for (int i=0;i<button_list.size() ;i++ )
    {
          if(button_list[i]->isChecked())
          {
              button_list[i]->setChecked(false);
          }
    }
}

void ShareFile::Sure_Ok()
{

     qDebug() << "ok...";

     QString login_name = TcpClient::getInstance().get_login_name();
     QString Cur_path = TcpClient::getInstance().get_Cur_path();
     QString File_name = OpeWidget::getInstance().get_Book()->get_File_name();
     QString File_path = Cur_path + QString("/") + File_name;

     qDebug() << "login_name:" << login_name << "File_name:" << File_name << "File_path:" << File_path;

     QList<QAbstractButton*> button_list = m_pButtonGroup->buttons();
     int buttons_size = button_list.size();
     if(buttons_size == 0)
     {
         QMessageBox::warning(this, "文件分享", "请选择分享的好友！");
     }
     else
     {
         int share_num = 0;
         for(int i=0;i<buttons_size;i++)
         {
             if(button_list[i]->isChecked())
             share_num++;
         }

         uint msg_size = share_num * 32 +  File_path.size() + 1;
         PDU *pdu = mkPDU(msg_size);
         pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
         sprintf(pdu->caData, "%s %d", login_name.toStdString().c_str(), share_num);

         qDebug() << "share_num" << share_num;
         for (int i=0;i<buttons_size ;i++ )
         {
              if(button_list[i]->isChecked())
              {
                  qDebug() << "button_list_text:" << button_list[i]->text();
                  memcpy(pdu->caMsg + i *32, button_list[i]->text().toStdString().c_str(), 32);
              }
         }

         memcpy((char*)(pdu->caMsg) + share_num * 32, File_path.toStdString().c_str(), File_path.size());

         TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
         free(pdu);
         pdu = NULL;
     }

}

void ShareFile::Cancel()
{
    qDebug() << "取消...";
    hide();
}

//返回ShareFile单例
ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

//将在线好友name展示在shrefile界面上
void ShareFile::update_friendslist(QListWidget *friend_list)
{
    qDebug() << "分享文件接收到好友列表：" << friend_list;
    QList<QAbstractButton*> button_list = m_pButtonGroup->buttons();

    QAbstractButton* temp = NULL;
    for (int i=0;i<button_list.size() ;i++ )
    {
        temp = button_list[i];
        m_pFriendWVBL->removeWidget(temp);
        m_pButtonGroup->removeButton(temp);
        button_list.removeOne(temp);
        delete  temp;
        temp = NULL;
    }

    QCheckBox *pCB = NULL;
    for (int i=0;i<friend_list->count();i++ )
    {
        qDebug() << friend_list->item(i)->text();

        pCB = new QCheckBox(friend_list->item(i)->text());
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }

    m_pSA->setWidget(m_pFriendW);
}
