#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QByteArray>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QDebug>
#include <string.h>
#include <opewidget.h>
#include "privatechat.h"
#include <QDir>
TcpClient::TcpClient(QWidget *parent) : QMainWindow(parent), ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
    //连接服务器
    connect(&m_tcpScoket,SIGNAL(connected()),this,SLOT(showConnect()));
    //监听服务器
    connect(&m_tcpScoket,&QTcpSocket::readyRead,this,&TcpClient::show_information);
    m_tcpScoket.connectToHost(QHostAddress(m_strIP),m_usPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{

    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        strData.replace("\r\n"," ");
        QStringList strList = strData.split(" ");
        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();
        qDebug() << "IP地址为：" << m_strIP << "端口为：" << m_usPort;
        file.close();
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}

//返回实例对象TcpClient
TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

//返回tcp_socket对象
QTcpSocket &TcpClient:: getTcpSocket()
{
    return m_tcpScoket;
}

//获取服务器结果
void TcpClient::showConnect()
{
    qDebug() << "连接服务器成功！";
}

//保存用户登录名
QString TcpClient:: get_login_name()
{
    return this->login_name;
}

//设置用户系统当前所在路径
void TcpClient::set_Cur_path(QString Cur_path)
{
    this->Cur_path = Cur_path;
}

//返回用户系统当前所在路径
QString TcpClient::get_Cur_path()
{
    return Cur_path;
}

//用于接受服务器的的响应函数
void TcpClient::show_information()
{

    if(!OpeWidget::getInstance().get_Book()->get_Uploead_statu())
   {
       //获取服务器端响应的数据
       qDebug() << "客户端响应...";
       uint uiPDULen=0;
       m_tcpScoket.read((char*)&uiPDULen,sizeof(uint));
       uint uiMsgLen = uiPDULen - sizeof(PDU);
       PDU *pdu = mkPDU(uiMsgLen);
       m_tcpScoket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));

       //判断返回的类型
       switch (pdu->uiMsgType) {

       case ENUM_MSG_TYPE_REGIST_RESPOND :
       {
           QMessageBox::information(this,"注册",pdu->caData);
           free(pdu);
           pdu = NULL;
           break;
       }

       case ENUM_MSG_TYPE_LOGIN_RESPOND  :
       {
           if(strcmp(pdu->caData,LOGIN_FAILED) == 0)
           {
               QMessageBox::information(this,"登录",pdu->caData);
           }
           else
           {
               Cur_path = QString("./%1").arg(login_name);

               QMessageBox::information(this,"登录",pdu->caData);
               OpeWidget::getInstance().show();
               this->hide();
           }

           free(pdu);
           pdu = NULL;
           break;
       }

       case ENUM_MSG_TYPE_OFFLINE_RESPOND  :
       {
           if(pdu->uiMsgType==5)
               QMessageBox::information(this,"退出",pdu->caData);
           else
               QMessageBox::information(this,"退出",pdu->caData);

           free(pdu);
           pdu = NULL;
           break;
       }

       //接收服务器返回的在线好友
       case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND :
       {
           qDebug() << "接收服务器返回的在线好友...";
           qDebug()<< *(pdu->caMsg);
           OpeWidget::getInstance().get_Friend()->showAllOnlineUsr(pdu);

           free(pdu);
           pdu = NULL;

           break;
       }

       //接收服务器返回的用户指定添加好友
        case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
       {

           qDebug() << "接受服务器返回的用户指定添加好友...";
           char name[32];
           char online_state[32];

           if(strcmp(pdu->caData, SEARCH_USR_NO)==0)
           {
               QMessageBox::information(this, "搜索好友", QString("%1 : not exits").arg(OpeWidget::getInstance().get_Friend()->Search_name));
           }
           else
           {
               memcpy((char*)name, (char*)pdu->caMsg, 32);
               memcpy((char*)online_state, (char*)pdu->caMsg + 32, 32);
               QMessageBox::information(this, "搜索好友", QString("\'%1\' :\'%2\'").arg(OpeWidget::getInstance().get_Friend()->Search_name).arg(online_state));
           }

           free(pdu);
           pdu = NULL;

           break;
       }

       //添加好友请求
       case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
       {
           qDebug() << "收到好友添加请求";
           char client_name[32] = {'\0'};
           strncpy(client_name, pdu->caData, 32);
           qDebug() << client_name;

           int res = QMessageBox::information(this, "添加好友", QString("\'%1\' want to add with you?").arg(client_name), QMessageBox::Yes, QMessageBox::No);

           qDebug() << res;

           PDU *respdu =mkPDU(0);
           memcpy(respdu->caData, pdu->caData, 64);

           if(res == QMessageBox::Yes)
           {
               respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
               qDebug() << "同意...";
           }
           else
           {
               respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
               qDebug() << "拒绝...";
           }

           m_tcpScoket.write((char*)respdu, respdu->uiPDULen);
           free(respdu);
           respdu =NULL;
           break;
       }

       case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
       {
           char friend_name[64] = {'\0'};
           strncpy(friend_name,pdu->caData, 64);
           QMessageBox::information(this,"好友请求",friend_name);
           qDebug() << friend_name << "friend_name...";
           break;
       }

       //接收服务器返回的添加好友请求
       case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
       {
           qDebug() << "接受服务器返回的添加好友成功请求...";
           char friend_name[32] = {'\0'};
           strncpy(friend_name, pdu->caData + 32, 32);
           QMessageBox::information(this, "添加好友", QString("添加%1好友成功！").arg(friend_name));
           break;

       }

       case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
       {
          qDebug() << "接受服务器返回的添加好友失败请求...";
          char friend_name[32] = {'\0'};
          strncpy(friend_name, pdu->caData + 32, 32);
          QMessageBox::information(this, "添加好友", QString("添加%1好友失败！").arg(friend_name));
          free(pdu);
          pdu = NULL;
          break;

       }

       //接收服务器刷新的在线好友列表
        case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
       {
           qDebug() << "接受服务器刷新的在线好友列表...";
           OpeWidget::getInstance().get_Friend()->update_Online_FriendList(pdu);
           break;
       }

       case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
       {
           char friend_name[32] = {'\0'};
           strncpy(friend_name, pdu->caData, 32);

           qDebug() << "客户端接收删除好友请求...";
           QMessageBox::information(this, "删除好友", QString("%1将你删除！").arg(friend_name));
           break;
       }

       case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
       {
           char msg[32] ={'\0'};
           memcpy(msg, pdu->caData, 32);
           qDebug() << msg;
           QMessageBox::information(this, "删除好友", msg);
           break;
       }

       //接收好友私聊信息
       case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
       {
          if(PrivateChat::getInstance().isHidden())
          {
              PrivateChat::getInstance().show();
          }
          char send_name[32] = {"\0"};
          memcpy(send_name, pdu->caData, 32);
          PrivateChat::getInstance().save_chat_name(send_name);
          PrivateChat::getInstance().update_msg(pdu);
          free(pdu);
          pdu = NULL;
          break;
       }

       //接收群聊信息
       case ENUM_MSG_TYPE_GROUP_CHAT_RESPOND:
       {
           qDebug() << "客户端接受群聊响应...";

           OpeWidget::getInstance().get_Friend()->update_Group_Msg(pdu);
           free(pdu);
           pdu = NULL;
           break;

       }

       //接收创建目录响应
       case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:
       {
           qDebug() << "客户端接受创建目录响应...";
           char msg[32] = {'\0'};
           strncpy(msg, pdu->caData, 32);
           qDebug() << msg;

           QMessageBox::information(this,"创建文件",pdu->caData);
           free(pdu);
           pdu = NULL;
           break;
       }

       //接收刷新文件响应
       case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
       {
           qDebug() << "接收刷新文件响应...";

           OpeWidget::getInstance().get_Book()->update_Booklist(pdu);
           break;
       }

       //接收删除文件目录响应
       case ENUM_MSG_TYPE_DEL_DIR_RESPOND:
       {
           qDebug() << "服务器接收删除文件目录响应...";

           QMessageBox::information(this, "目录删除",QString("%1").arg(pdu->caData));
           break;
       }

       //接收修改目录文件名响应
       case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:
       {
           qDebug() << "服务器接收修改目录文件名响应...";

           QMessageBox::information(this, "重命名文件", QString("%1").arg(pdu->caData));
           break;
       }

       //接收进入当前文件夹响应
       case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:
       {
           char msg[32] ={'\0'};
           memcpy(msg, pdu->caData, 32);
           qDebug() << "接收进入当前文件夹响应..." << msg;

           if(msg != ENTER_DIR_FAILURED)
           {
               OpeWidget::getInstance().get_Book()->update_Booklist(pdu);
               QString strEnterDir = OpeWidget::getInstance().get_Book()->get_CurDir_path();
               if(!strEnterDir.isEmpty())
                 {
                    this->Cur_path = this->Cur_path + "/" + strEnterDir;
                 }
               qDebug() << "strEnterDir :" << strEnterDir << "Curpath:" << Cur_path;
           }
           else
           {
               qDebug() << "双击为文件...";
           }
           break;
       }

       //接收上传文件响应
       case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
       {
           qDebug() << "客户端接收上传文件响应...";
           QMessageBox::information(this, "文件上传", pdu->caData);
           break;
       }

       //接收文件删除响应
       case ENUM_MSG_TYPE_DEL_FILE_RESPOND:
       {
           qDebug() << "客户端接收文件删除响应...";
           QMessageBox::information(this, "文件删除", pdu->caData);
       }

       //接收文件下载响应
       case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:
       {
           qDebug() << pdu->caData;
           qDebug() << "客户端接收文件下载响应...";

           char File_name[32] = {'\0'};
           sscanf(pdu->caData, "%s %lld", File_name, &(OpeWidget::getInstance().get_Book()->total_size));

           qDebug() << "File_name:" << File_name << "file_size:" << OpeWidget::getInstance().get_Book()->total_size;

           if(strlen(File_name) > 0 && OpeWidget::getInstance().get_Book()->total_size >0)
           {
               OpeWidget::getInstance().get_Book()->set_Upload_statu(true);

               QString Save_path = OpeWidget::getInstance().get_Book()->get_Save_path();
               qDebug() << "Save_path:" << Save_path;
               file.setFileName(Save_path);

               if(!file.open(QIODevice::WriteOnly))
               {
                    QMessageBox::warning(this, "文件下载", "文件写入失败！");
                    break;
               }

           }

       }

       //客户端接收好友文件共享
       case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
       {
           qDebug() << "客户端接收好友文件共享...";
           char friend_name[32] = {'\0'};
           char *File_path = new char[pdu->uiMsgLen];
           memcpy(File_path, pdu->caMsg, pdu->uiMsgLen);
           strcpy(friend_name, pdu->caData);

           qDebug() << "friend_name：" << friend_name << "File_path:" << File_path;

           QString msg = QString("friend:\'%1\'want to share File").arg(friend_name);

           int res = QMessageBox::question(this, "文件分享", msg);
           qDebug() << "msg:" << msg << "res:" << res;

           char *pos = strrchr(File_path, '/');
           int flag = 0;
           PDU *respdu= mkPDU(pdu->uiMsgLen);
           respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;

           if(QMessageBox::Yes == res)
           {
               if(pos != NULL)
               {
                   flag = 1;
                   qDebug() << pos + 1 << "flag:" << flag;

                   sprintf(respdu->caData, "%s %d", TcpClient::getInstance().get_login_name().toStdString().c_str(), flag);

                   strncpy((char*)respdu->caMsg, (char*)pdu->caMsg, pdu->uiMsgLen);
               }
           }

           else
           {
               sprintf(respdu->caData, "%s%d", TcpClient::getInstance().get_login_name().toStdString().c_str(), flag);
               strncpy((char*)respdu->caMsg, (char*)pdu->caMsg, pdu->uiMsgLen);
           }

           m_tcpScoket.write((char*)respdu, respdu->uiPDULen);
           free(respdu);
           respdu = NULL;
           break;
       }

       //客户端接收好友文件共享回复
       case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:
       {
           qDebug() << "客户端接收好友文件共享回复...";
           QMessageBox::information(this, "文件分享", pdu->caData);
       }

       //客户端接收移动文件回复
       case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:
       {
           qDebug() << "客户端接收移动文件回复...";
           QMessageBox::information(this, "文件移动", pdu->caData);
       }
       default:
       {
           break;
       }
           free(pdu);
           pdu = NULL;
       }
   }
   else
    {
        qDebug() << "客户端正在写入服务器发送的文件...";
        QByteArray Buffer = m_tcpScoket.readAll();

        file.write(Buffer);
        Book *book = OpeWidget::getInstance().get_Book();
        book->received_size += Buffer.size();

        qDebug() << "Buffer_size:" << Buffer.size() << "Book:" << book->total_size << book->received_size;

        if(book->total_size == book->received_size)
        {
             file.close();
             book->total_size =0;
             book->received_size = 0;
             book->set_Upload_statu(false);

             QMessageBox::information(this, "文件下载", "文件下载成功！");
        }
        else if(book->total_size < book->received_size)
        {
            file.close();
            book->total_size =0;
            book->received_size = 0;
            book->set_Upload_statu(false);

            QMessageBox::information(this, "文件下载", "文件下载失败！");
        }

    }
}

//登录按钮
void TcpClient::on_login_pb_clicked()
{

    //获取用户登录信息
    QString username=ui->username->text();
    QString password=ui->password->text();

    if(username==NULL || password==NULL)
    {
        QMessageBox::warning(this,"提示","用户名密码不为空！");
    }
    else
    {
        this->login_name = username;

        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        memcpy(pdu->caData,username.toStdString().c_str(),32);
        memcpy(pdu->caData+32,password.toStdString().c_str(),32);
        m_tcpScoket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

//注册按钮
void TcpClient::on_regist_pb_clicked()
{
    //获取用户登录信息
    QString username=ui->username->text();
    QString password=ui->password->text();

    if(!username.isEmpty()&&!password.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,username.toStdString().c_str(),32);
        strncpy(pdu->caData+32,password.toStdString().c_str(),32);
        m_tcpScoket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }
    else
    {
        QMessageBox::warning(this,"提示","用户名密码不为空！");
        return;
    }

}

//注销按钮
//void TcpClient::on_cancel_pb_clicked()
//{
//    PDU *pdu=mkPDU(0);
//    pdu->uiMsgType=ENUM_MSG_TYPE_OFFLINE_REQUEST;
//    //向服务器端发送用户信息
//    m_tcpScoket.write((char*)pdu,pdu->uiPDULen);
//    free(pdu);
//    pdu = NULL;
//}
