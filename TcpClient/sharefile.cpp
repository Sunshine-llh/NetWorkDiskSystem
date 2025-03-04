#include "sharefile.h"
#include <QtDebug>
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
//    QFile
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
        pCB = new QCheckBox(friend_list->item(i)->text());
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }

    m_pSA->setWidget(m_pFriendW);
}
