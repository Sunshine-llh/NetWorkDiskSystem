#include "opewidget.h"

// 操作主界面UI
OpeWidget::OpeWidget(QWidget *parent) : QWidget(parent)
{
    m_pListW = new QListWidget(this);
    m_pListW->addItem("好友");
    m_pListW->addItem("图书");
    m_pFriend = new Friend;
    m_pBook = new Book;

    m_pSW = new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);
    setLayout(pMain);
    connect(m_pListW,SIGNAL(currentRowChanged(int)),m_pSW,SLOT(setCurrentIndex(int)));
}

// 生成OpeWidget实例对象
OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instance;
    return instance;
}

//返回m_pFriend
Friend *OpeWidget::get_Friend()
{
    return m_pFriend;
}

//返回m_pBook
Book *OpeWidget::get_Book()
{
    return m_pBook;
}
