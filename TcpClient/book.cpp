#include "book.h"
#include <QtDebug>
#include <QInputDialog>
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>
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

}

//点击创建文件夹按钮
void::Book::Create_Dir()
{
    qDebug() << "点击创建文件夹按钮...";
    QString strNewDir = QInputDialog::getText(this,"新建文件夹","新文件夹名字");

    if(strNewDir != NULL)
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
        QMessageBox::information(this, "新建文件夹", "新建文件夹名不能为空！");
    }
}
