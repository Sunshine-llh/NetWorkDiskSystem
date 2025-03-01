#include "book.h"
#include <QtDebug>
#include <QInputDialog>
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>
#include <QDir>
#include "opewidget.h"
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
     connect(m_pCreateDirPB, SIGNAL(clicked(bool)), this, SLOT(Create_Dir()));
     connect(m_pFlushFilePB, SIGNAL(clicked(bool)), this, SLOT(Flush_File()));
     connect(m_pDelDirPB, SIGNAL(clicked(bool)), this, SLOT(Delete_Dir()));
     connect(m_pRenamePB, SIGNAL(clicked(bool)), this, SLOT(Rename_Dir_File()));
     connect(m_pBookListW, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(enterDir(QModelIndex)));

     CurDir_path.clear();
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
            PDU *pdu = mkPDU(strNewDir.size());
            pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
            strncpy(pdu->caData, TcpClient::getInstance().get_login_name().toStdString().c_str(), TcpClient::getInstance().get_login_name().size());
            strncpy(pdu->caData + 32, TcpClient::getInstance().get_Cur_path().toStdString().c_str(), TcpClient::getInstance().get_Cur_path().size());
            memcpy(pdu->caMsg, strNewDir.toStdString().c_str(), strNewDir.size());

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

    QString Cur_path = TcpClient::getInstance().get_Cur_path() + OpeWidget::getInstance().get_Book()->get_CurDir_path();

    qDebug() << Cur_path;
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
