#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>
#include <QCheckBox>
#include <QListWidget>

class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);
    static ShareFile &getInstance();
    void update_friendslist(QListWidget *friend_list);

private:
    QPushButton *m_pSelectAllPB;
    QPushButton *m_pCancelSelectPB;
    QPushButton *m_pOKPB;
    QPushButton *m_pCancelPB;
    QScrollArea *m_pSA;
    QWidget *m_pFriendW;
    QVBoxLayout *m_pFriendWVBL;
    QButtonGroup *m_pButtonGroup;
public slots:
    void Selected_All();
    void Cancel_Selected();
    void Sure_Ok();
    void Cancel();

};

#endif // SHAREFILE_H
