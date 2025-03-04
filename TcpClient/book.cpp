#include "book.h"
#include <QtDebug>
#include <QInputDialog>
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>
#include <QDir>
#include "opewidget.h"
#include <QFileDialog>
Book::Book(QWidget *parent) : QWidget(parent)
{
    m_pBookListW = new QListWidget;

     m_pReturnPB = new QPushButton("返回");
     m_pCreateDirPB = new QPushButton("创建文件夹");
     m_pDelDirPB = new QPushButton("删除文件夹");
     m_pRenamePB = new QPushButton("重命名文件夹");
     m_pFlushFilePB = new QPushButton("刷新文件");
     QVBoxLayout *pDirVBL = new  QVBoxLayout();
     pDirVBL->addWidget(m_pReturnPB);
     pDirVBL->addWidget(m_pCreateDirPB);
     pDirVBL->addWidget(m_pDelDirPB);
     pDirVBL->addWidget(m_pRenamePB);
     pDirVBL->addWidget(m_pFlushFilePB);

     QVBoxLayout *pFileVBL = new  QVBoxLayout();
     m_pUploadPB = new QPushButton("上传文件");
     m_DownLoadPB = new QPushButton("下载文件");
     m_pDelFilePB = new QPushButton("删除文件");
     m_pShareFilePB = new QPushButton("分享文件");
     pFileVBL->addWidget(m_pUploadPB);
     pFileVBL->addWidget(m_DownLoadPB);
     pFileVBL->addWidget(m_pDelFilePB);
     pFileVBL->addWidget(m_pShareFilePB);

     QHBoxLayout *pMain = new QHBoxLayout();
     pMain->addWidget(m_pBookListW);
     pMain->addItem(pDirVBL);
     pMain->addItem(pFileVBL);
     setLayout(pMain);

     CurDir_path.clear();
     connect(m_pCreateDirPB, SIGNAL(clicked(bool)), this, SLOT(Create_Dir()));
     connect(m_pFlushFilePB, SIGNAL(clicked(bool)), this, SLOT(Flush_File()));
     connect(m_pDelDirPB, SIGNAL(clicked(bool)), this, SLOT(Delete_Dir()));
     connect(m_pRenamePB, SIGNAL(clicked(bool)), this, SLOT(Rename_Dir_File()));
     connect(m_pBookListW, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(enterDir(QModelIndex)));

     connect(m_pReturnPB, SIGNAL(clicked(bool)), this, SLOT(Return()));

     p_timer = new QTimer;
     this->Uploead_statu = false;

     connect(m_pUploadPB, SIGNAL(clicked(bool)), this, SLOT(UploadFile()));
     connect(p_timer, SIGNAL(timeout()), this, SLOT(UploadFile_Data()));
     connect(m_pDelFilePB, SIGNAL(clicked(bool)), this, SLOT(Del_File()));
     connect(m_DownLoadPB, SIGNAL(clicked(bool)), this, SLOT(Download_File()));
     connect(m_pShareFilePB, SIGNAL(clicked(bool)), this, SLOT(Share_File()));
}

//点击创建文件夹按钮
void Book::Create_Dir()
{
    qDebug() << "点击创建文件夹按钮...";
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","新文件夹名字");

    if(!strNewDir.isEmpty())
    {
        if(strNewDir.size() < 32)
        {
            PDU *pdu = mkPDU(TcpClient::getInstance().get_Cur_path().size());

            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData, TcpClient::getInstance().get_login_name().toStdString().c_str(), TcpClient::getInstance().get_login_name().size());
            strncpy(pdu->caData + 32, strNewDir.toStdString().c_str(), strNewDir.size());
            memcpy(pdu->caMsg, TcpClient::getInstance().get_Cur_path().toStdString().c_str(), TcpClient::getInstance().get_Cur_path().size());

            TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
        else
        {
            QMessageBox::information(this, "新建文件夹", "文件名过长！");
        }
    }
    else
    {
        QMessageBox::warning(this, "新建文件夹", "新建文件夹名不能为空！");
    }
}

//点击刷新文件按钮
void Book::Flush_File()
{
    qDebug() << "点击刷新文件按钮...";

    QString Cur_path = TcpClient::getInstance().get_Cur_path();

    qDebug() << "Cur_path:" << Cur_path;

    PDU *pdu = mkPDU(Cur_path.size());
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg, Cur_path.toStdString().c_str(), Cur_path.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

//点击删除文件夹按钮
void Book::Delete_Dir()
{
    qDebug() << "点击删除文件夹按钮...";
    QDir dir;
    QString delete_name = m_pBookListW->currentItem()->text();
    QString Cur_path =  TcpClient::getInstance().get_Cur_path();
    qDebug() << delete_name << Cur_path + QString("/") + delete_name;

    if(delete_name.isNull())
    {
        QMessageBox::warning(this, "文件夹删除", "请选择文件夹!");
    }
    else
    {
        PDU * pdu = mkPDU(Cur_path.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;

        memcpy(pdu->caData, delete_name.toStdString().c_str(), delete_name.size());
        memcpy(pdu->caMsg, Cur_path.toStdString().c_str(), Cur_path.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    }
}

//点击修改目录文件名按钮
void Book::Rename_Dir_File()
{
    qDebug() << "点击修改目录文件名按钮...";

    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL)
    {
        QMessageBox::warning(this,"重命名文件","请选择要重命名的文件");
    }
    else
    {
        QString Old_name_text = pItem->text();
        QString rename_text = QInputDialog::getText(this, "重命名文件", "请输入新的文件名：");
        QString Cur_path =  TcpClient::getInstance().get_Cur_path();

        qDebug() << Old_name_text << rename_text << Cur_path;

        if(rename_text.isEmpty())
        {
            QMessageBox::warning(this, "重命名文件", "文件名不能为空！");
        }
        else
        {
            if(rename_text.size() > 32)
            {
                QMessageBox::warning(this, "重命名文件", "文件名长度过长！");
            }
            else
            {
                PDU *pdu = mkPDU(Cur_path.size());
                pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
                memcpy(pdu->caData, Old_name_text.toStdString().c_str(), 32);
                memcpy(pdu->caData + 32, rename_text.toStdString().c_str(), 32);
                memcpy(pdu->caMsg, Cur_path.toStdString().c_str(), Cur_path.size());

                TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
                free(pdu);
                pdu = NULL;
            }

        }
    }

}

//进入当前目录
void Book::enterDir(const QModelIndex &index)
{
    QString Dir_name = index.data().toString();
    QString Cur_path = TcpClient::getInstance().get_Cur_path();
    QString path = Cur_path + QString("/") + Dir_name;

    qDebug() << "进入当前目录..." << Dir_name << Cur_path << path;

    CurDir_path = Dir_name;

    qDebug() << get_CurDir_path();

    PDU *pdu = mkPDU(Cur_path.size());
    pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    memcpy(pdu->caData, Dir_name.toStdString().c_str(), Dir_name.size());
    memcpy(pdu->caMsg, Cur_path.toStdString().c_str(), Cur_path.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

//点击返回按钮
void Book::Return()
{
    qDebug() << "点击返回按钮...";
    QString Cur_path = TcpClient::getInstance().get_Cur_path();
    QString Root_path = QString("./") + TcpClient::getInstance().get_login_name();
    qDebug() << "Cur_path:" << Cur_path << "Root_path:" << Root_path;

    if(Cur_path == Root_path)
    {
        QMessageBox::information(this, "返回上一级", "当前已为根目录！");
    }
    else
    {
        int index = Cur_path.lastIndexOf("/");
        QString New_Cur_path = Cur_path.remove(index, Cur_path.size() - index);
        TcpClient::getInstance().set_Cur_path(New_Cur_path);
        CurDir_path.clear();
        Flush_File();

    }
}

//点击上传文件请求按钮
void Book::UploadFile()
{
    qDebug() << "点击上传文件按钮...";

    this->Upload_File_path =  QFileDialog::getOpenFileName();
    QString Cur_path = TcpClient::getInstance().get_Cur_path();

    qDebug() << "Upload_File_path:" << this->Upload_File_path;

    if(!Upload_File_path.isEmpty())
    {
        int index = Upload_File_path.lastIndexOf('/');
        QString FileName = Upload_File_path.right(Upload_File_path.size() - index - 1);
        qDebug() << "FileName:" << FileName;

        QFile file(this->Upload_File_path);
        qint64 file_size = file.size();

        PDU * pdu = mkPDU(Cur_path.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;

        memcpy(pdu->caMsg, Cur_path.toStdString().c_str(), Cur_path.size());
        sprintf(pdu->caData, "%s %lld", FileName.toStdString().c_str(), file_size);

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
        p_timer->start(1000);
    }
    else
    {
        QMessageBox::warning(this, "文件上传", "上传文件名不能为空！");
    }
}

//上传文件数据
void Book::UploadFile_Data()
{
    qDebug() << "正在上传数据...";
    p_timer->stop();

    QFile file(Upload_File_path);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "文件上传", "文件打开失败！");
        return ;
    }

    char *Buffer = new char[4096];
    qint64 res = 0;

    while(true)
    {
        res = file.read(Buffer, 4096);
        if(res > 0 && res <= 4096)
        {
            TcpClient::getInstance().getTcpSocket().write(Buffer, res);
        }
        else if(res == 0)
        {
            break;
        }
        else
        {
            QMessageBox::warning(this, "文件上传", "文件上传失败！");
            break;
        }
    }

    file.close();
    delete []Buffer;
    *Buffer = NULL;
}

//点击删除文件按钮
void Book::Del_File()
{
    qDebug() << "点击删除文件按钮...";
    QListWidgetItem *pItem = m_pBookListW->currentItem();

    if(pItem == NULL)
    {
        QMessageBox::warning(this, "删除文件", "请选择您要删除的文件！");
    }
    else
    {
        QString File_name = pItem->text();
        QString Cur_path = TcpClient::getInstance().get_Cur_path();
        PDU *pdu = mkPDU(Cur_path.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;
        memcpy(pdu->caMsg, Cur_path.toStdString().c_str(), Cur_path.size());
        memcpy(pdu->caData, File_name.toStdString().c_str(), File_name.size() + 1);

        qDebug()  << "File_name:" << File_name << "Cur_path:" << Cur_path;

        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

//点击文件下载按钮
void Book::Download_File()
{
    qDebug() << "点击文件下载按钮...";
    QListWidgetItem *pItem = m_pBookListW->currentItem();
    if(pItem == NULL)
    {
        QMessageBox::warning(this, "文件下载", "请选择要下载的文件！");
    }
    else
    {
        QString Save_path = QFileDialog::getSaveFileName();
        qDebug() << "Save_path:" << Save_path;

        if(Save_path.isEmpty())
        {
            QMessageBox::information(this, "文件下载", "请选择文件保存的路径！");
            this->Save_path.clear();
        }
        else
        {
            this->Save_path = Save_path;

            QString Cur_path = TcpClient::getInstance().get_Cur_path();
            QString File_name = pItem->text();

            qDebug() << "File_name:" << File_name << "Cur_path:" << Cur_path << "Save_path:" << Save_path;
            PDU *pdu = mkPDU(Save_path.size() + 1);
            pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;

            strcpy(pdu->caData, File_name.toStdString().c_str());
            memcpy(pdu->caMsg, Cur_path.toStdString().c_str(), Cur_path.size());

            TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
    }

}

//点击文件分享按钮
void Book:: Share_File()
{
    qDebug() << "点击文件分享按钮...";
    QListWidgetItem *Item = m_pBookListW->currentItem();

    if(Item == NULL)
    {
        QMessageBox::warning(this, "文件分享", "请选择分享文件！");
        return;
    }
    else
    {
        this->File_name = Item->text();
        qDebug() << "File_name:" << File_name;
    }

    QListWidget *friend_list = OpeWidget::getInstance().get_Friend()->getfrind_list();
    qDebug() << "friend_list_size:" << friend_list->size();

    ShareFile::getInstance().update_friendslist(friend_list);

    if(ShareFile::getInstance().isHidden())
    {
         ShareFile::getInstance().show();
    }

}

//展示服务器发送过来的目录文件列表
void Book::update_Booklist(const PDU *pdu)
{
    qDebug() << "展示服务器发送过来的目录文件列表...";
    if(pdu == NULL)
    {
        return;
    }

    m_pBookListW->clear();

    uint list_len = pdu->uiMsgLen / sizeof(FileInfo);
    FileInfo *fileinfo = NULL;
    for(int i=0; i< list_len ; i++)
    {
        fileinfo = (FileInfo*)(pdu->caMsg) + i;
        qDebug() << list_len << fileinfo->caFileName << fileinfo->iFileType;

        QListWidgetItem *pItem = new QListWidgetItem;
        if(fileinfo->iFileType == 1)
        {
            pItem->setIcon(QIcon(QPixmap(":/icon/dir.jpg")));
        }
        else if(fileinfo->iFileType == 0)
        {
            pItem->setIcon(QIcon(QPixmap(":/icon/reg.jpg")));
        }
        pItem->setText(fileinfo->caFileName);
        m_pBookListW->addItem(pItem);
    }

}

//清除当前路径
void Book::Clear_Dir_path()
{
    this->CurDir_path.clear();    
}

//返回当前路径
QString Book::get_CurDir_path()
{
    return this->CurDir_path;
}

//设置文件保存路径
void Book::set_Save_path(QString Save_path)
{
    this->Save_path = Save_path;
}

//返回文件保存路径
QString Book::get_Save_path()
{
    return this->Save_path;
}

//返回文件下载状态
bool Book::get_Uploead_statu()
{
    return this->Uploead_statu;
}

//设置文件下载状态
void Book::set_Upload_statu(bool Upload_statu)
{
    this->Uploead_statu =Upload_statu;
}
