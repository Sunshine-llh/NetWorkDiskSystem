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

}

//返回ShareFile单例
ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

void ShareFile::update_friendslist(QListWidget *friend_list)
{
    qDebug() << friend_list;
}
