#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "online.h"

class Friend : public QWidget
{
    Q_OBJECT

public:
    QString Search_name;
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUsr(PDU *pdu);
    void update_Online_FriendList(PDU *pdu);
    void update_Group_Msg(PDU *pdu);
    QListWidget *getfrind_list();
signals:

public slots:
    void showOnline();
    void showFriend();
    void Flush_friends();
    void delete_friend();
    void private_chat();
    void group_chat();

private:
    QTextEdit *m_pShowMsgTE;
    QListWidget *m_pFriendListWidget;
    QLineEdit *m_pInputMsgLE;
    QPushButton *m_pDelFriendPB;
    QPushButton *m_pFlushFriendPB;
    QPushButton *m_pShowOnlineUsrPB;
    QPushButton *m_pSearchUsrPB;
    QPushButton *m_pMsgSendPB;
    QPushButton *m_pPrivateChatPB;
    Online *m_pOnline;
};

#endif // FRIEND_H
