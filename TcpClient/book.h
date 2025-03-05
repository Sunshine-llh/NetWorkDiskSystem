#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <protocol.h>
#include <QTimer>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void update_Booklist(const PDU *pdu);
    void Clear_Dir_path();
    QString get_CurDir_path();
    void set_Save_path(QString Save_path);
    QString get_Save_path();
    bool get_Uploead_statu();
    void set_Upload_statu(bool Upload_statu);
    QString get_File_name();
    qint64 total_size;
    qint64 received_size;


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
    QPushButton *m_pMvFilePB;
    QPushButton *m_Mv_PathFilePB;

    QString  CurDir_path;
    QString Upload_File_path;
    QTimer *p_timer;
    QString Save_path;
    bool Uploead_statu;
    QString File_name;


signals:
public slots:
    void Create_Dir();
    void Flush_File();
    void Delete_Dir();
    void Rename_Dir_File();
    void enterDir(const QModelIndex &index);
    void  Return();
    void UploadFile();
    void UploadFile_Data();
    void Del_File();
    void Download_File();
    void Share_File();
    void Mv_File();
    void Selected_Dir_path();
};

#endif // BOOK_H
