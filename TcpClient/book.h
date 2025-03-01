#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <protocol.h>
class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void update_Booklist(const PDU *pdu);
    void Clear_Dir_path();
    QString get_CurDir_path();


private:
    QListWidget *m_pBookListW;
    QPushButton *m_pReturnPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDelDirPB;
    QPushButton *m_pRenamePB;
    QPushButton *m_pFlushFilePB;
    QPushButton *m_pUploadPB;
    QPushButton *m_DownLoadPB;
    QPushButton *m_pDelFilePB;
    QPushButton *m_pShareFilePB;
    QString  CurDir_path;


signals:
public slots:
    void Create_Dir();
    void Flush_File();
    void Delete_Dir();
    void Rename_Dir_File();
    void enterDir(const QModelIndex &index);
};

#endif // BOOK_H
