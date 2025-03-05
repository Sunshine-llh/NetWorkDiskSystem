#include "mytcpsocket.h"
#include <QDebug>
#include "database.h"
#include "mytcpserver.h"
#include <QDir>
#include <QFileInfoList>
MyTcpSocket::MyTcpSocket(QObject *parent) : QTcpSocket(parent)
{
    m_pTimer = new QTimer;
    connect(this,&QTcpSocket::readyRead,this,&MyTcpSocket::remsg);
    connect(this,&QTcpSocket::disconnected,this,&MyTcpSocket::disconnected);

    this->upload_flag = false;
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(send_File_to_Client()));
}

//用户返回请求用户的用户名
QString MyTcpSocket::get_login_name()
{
    return this->login_name;
}

//copy目录文件
void MyTcpSocket::Copy_Dir_File(QString strpath, QString despath)
{
    qDebug() << "正在复制文件..." << "str_path:" << strpath << "des_path:" << despath;
    QDir dir;
    dir.mkdir(despath);
    dir.setPath(strpath);
    QFileInfoList file_list = dir.entryInfoList();

    qDebug() << file_list << file_list.size();
    QString str_path;
    QString des_path;

        for(int i=0;i<file_list.size();i++)
        {
            qDebug() << file_list[i].fileName() << file_list[i].filePath();

            if(file_list[i].isFile())
            {
                str_path = strpath + QString("/") + file_list[i].fileName();
                des_path =  despath + QString("/") + file_list[i].fileName();
                qDebug() << str_path << des_path;
                QFile::copy(str_path, des_path);
            }
            else if(file_list[i].isDir())
            {
                if(file_list[i].fileName() == "." || file_list[i].fileName() == "..")
                {
                    continue;
                }
                str_path = strpath + QString("/") + file_list[i].fileName();
                des_path =  despath + QString("/") + file_list[i].fileName();

                qDebug() << str_path << des_path;
                Copy_Dir_File(str_path, des_path);
            }
        }

}

//断开连接函数
void MyTcpSocket::disconnected()
{
    qDebug()<< login_name << "已断开连接...";
    Database::getInstance().update_online_state(login_name);
    emit offline(this);
}

void MyTcpSocket::offline(MyTcpSocket *mytcpsoket)
{

}

//向客户端发送文件数据
void MyTcpSocket::send_File_to_Client()
{
    qDebug() << "向客户端发送文件数据...";

    char *Buffer =new char[4096];
    qint64 res = 0;

    while(true)
    {
        res = file.read(Buffer, 4096);

        if(res > 0 && res <= 4096)
        {
            write(Buffer, res);
        }
        else if(res == 0)
        {
            file.close();
            break;
        }
        else
        {
           qDebug() << "正在向客户端发送文件数据...";
           file.close();
           break;
        }
    }

    delete []Buffer;

}
// 接收来自客户端的消息
void MyTcpSocket::remsg()
{
   if(!upload_flag)
   {
       uint uiPDULen = 0;
       this->read((char*)&uiPDULen,sizeof(uint));
       qDebug() << uiPDULen;
       uint uiMsgLen = uiPDULen - sizeof(PDU);
       PDU *pdu = mkPDU(uiMsgLen);
       this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));

       char caName[32] = {'\0'};
       char caPwd[32] = {'\0'};

       strncpy(caName,pdu->caData,32);
       strncpy(caPwd,pdu->caData+32,32);

       switch (pdu->uiMsgType)
       {
       //注册请求
       case ENUM_MSG_TYPE_REGIST_REQUEST:
       {
           //向客户端响应
           bool rs=Database::getInstance().regist(caName,caPwd);
           if(rs)
           {
               free(pdu);
               pdu = mkPDU(0);
               pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
               strcpy(pdu->caData,REGIST_OK);

               QDir dir;
               qDebug() << QString("./%1").arg(caName);
               qDebug() << "注册成功！" << dir.mkdir(QString("./%1").arg(caName));
               qDebug() << "注册成功!";

           }
           else
           {
               pdu = mkPDU(0);
               qDebug() << "test";
               pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
               strcpy(pdu->caData,REGIST_FAILED);
               qDebug() << "注册失败!";
           }

           write((char *)pdu,pdu->uiPDULen);
           free(pdu);
           pdu=NULL;
           break;

       }
       //登录请求
       case ENUM_MSG_TYPE_LOGIN_REQUEST:
       {
           //向客户端响应
           bool rs=Database::getInstance().login(caName,caPwd);
           free(pdu);
           pdu = mkPDU(0);
           if(rs)
           {
               pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
               login_name=caName;
               strcpy(pdu->caData,LOGIN_OK);
           }
           else
           {
               pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
               strcpy(pdu->caData,LOGIN_FAILED);
           }

           write((char *)pdu,pdu->uiPDULen);
           free(pdu);
           pdu=NULL;
           break;
       }


        //响应客户端添加好友请求
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
       {
           qDebug() << "响应客户端添加好友请求...";
           char client_name[32] = {'\0'};
           char friend_name[32] = {'\0'};

           strncpy(client_name, pdu->caData, 32);
           strncpy(friend_name, pdu->caData+32, 32);
           int res = Database::getInstance().add_friend(client_name, friend_name);

           qDebug() << 'res:' << res << "..." << client_name << friend_name;
           PDU *respdu = NULL;

           if(res == -1)
           {   qDebug() << "-1 ...";
               respdu =mkPDU(0);
               respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
               strcpy(respdu->caData, UNKNOW_ERROR);
               write((char*)respdu, respdu->uiPDULen);
               free(respdu);
               respdu = NULL;
           }
           else if(res == 0)
           {
               qDebug() << "0 ...";
               respdu =mkPDU(0);
               respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
               strcpy(respdu->caData, EXISTED_FRIEND);
               write((char*)respdu, respdu->uiPDULen);
               free(respdu);
               respdu = NULL;
           }
           else if(res == 1)
           {
               qDebug() << "1 ...";
               MyTcpServer::getInstance().resend(friend_name, pdu);
           }
           else if(res == 2)
           {
               qDebug() << "2 ...";
               respdu = mkPDU(0);
               respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
               strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
               write((char*)respdu, respdu->uiPDULen);
               free(respdu);
               respdu = NULL;
           }
           else if(res == 3){
               qDebug() << "3 ...";
               PDU *respdu =mkPDU(0);
               respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
               strcpy(respdu->caData, ADD_FRIEND_NO_EXIST);
               write((char*)respdu, respdu->uiPDULen);
               free(respdu);
               respdu = NULL;
           }
           break;

       }

           //好友添加请求
           //1、同意
            case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
           {
               char login_name[32] = {'\0'};
               char friend_name[32] = {'\0'};
               qDebug() << friend_name << "同意............";

               strncpy(login_name, pdu->caData, 32);
               strncpy(friend_name, pdu->caData + 32, 32);

               Database::getInstance().handle_agree_friend(login_name, friend_name);

               MyTcpServer::getInstance().resend(login_name, pdu);
               break;
           }
           //2、拒绝
           case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
           {
               char login_name[32] = {'\0'};
               strncpy(login_name, pdu->caData, 32);
               qDebug() << "拒绝...";

               MyTcpServer::getInstance().resend(login_name, pdu);
               break;
           }


       //在线查询好友请求
       case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
       {
           qDebug() << "在线查询好友请求";
           results = Database:: getInstance().get_Online_friend();

           if(results.isEmpty())
           {
               return;
           }

           uint Msg_Len = results.size() * 32;
           PDU *rspdu = mkPDU(Msg_Len);
           rspdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;

           for(int i=0;i<results.size();i++)
           {
               memcpy((char *)rspdu->caMsg + i*32, results.at(i).toStdString().c_str(), results.at(i).size()+1);
               qDebug() << results.at(i);
           }

           write((char*)rspdu, rspdu->uiPDULen);
           free(rspdu);
           rspdu = NULL;
           break;
       }

       //搜索用户请求
       case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
       {

           qDebug() << "搜索用户请求";
           char search_name[32] = {'\0'};
           strncpy((char*)search_name, pdu->caData, 32);
           results = Database::getInstance().Search_friend(search_name);

           qDebug() << results;
           if(results.isEmpty())
           {
               PDU *rspdu = mkPDU(0);
               rspdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
               strcpy(rspdu->caData,SEARCH_USR_NO);
               write((char*)rspdu, rspdu->uiPDULen);
               return;
           }

           free(pdu);
           //定义Msg长度
           uint Msg_Len = results.size() * 32;

           PDU *rspdu = mkPDU(Msg_Len);
           rspdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
           memcpy((char*)rspdu->caData,results.at(1).toStdString().c_str(), 32);

           memcpy((char*)rspdu->caMsg, results.at(0).toStdString().c_str(), 32);
           memcpy((char*)rspdu->caMsg + 32, results.at(1).toStdString().c_str(), 32);

           qDebug() << rspdu->caMsg;
           write((char*)rspdu, rspdu->uiPDULen);
           free(rspdu);
           pdu = NULL;

           break;
       }

       case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
       {
           qDebug() << "服务器响应刷新在线好友...";
           char login_name[32] = {'\0'};
           memcpy(login_name, pdu->caData, 32);
           QStringList results = Database::getInstance().get_online_friends(login_name);

           uint Msg_Len = results.size() * 32;

           PDU * respdu = mkPDU(Msg_Len);
           respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;

           for(int i=0; i<results.size();i++)
           {
               memcpy(respdu->caMsg + i * 32, results.at(i).toStdString().c_str(), 32);
           }
           write((char*)respdu, respdu->uiPDULen);
           free(respdu);
           respdu = NULL;

           break;

       }
        //接收删除好友请求
       case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
       {
           qDebug() << "服务器接受删除好友请求...";
           char login_name[32] = {'\0'};
           char friend_name[32] = {'\0'};
           memcpy(login_name, pdu->caData, 32);
           memcpy(friend_name, pdu->caData + 32, 32);

           bool res = Database::getInstance().delete_friend(login_name, friend_name);

           PDU *respdu = mkPDU(0);

           if(res)
           {
               strcpy(respdu->caData, DEL_FRIEND_OK);
               respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
               write((char*)respdu, respdu->uiPDULen);
               MyTcpServer::getInstance().resend(friend_name, pdu);
           }
           else
           {
               strcpy(respdu->caData, DEL_FRIEND_FAILED);
               respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
               write((char*)respdu, respdu->uiPDULen);
           }
           free(respdu);
           free(pdu);
           respdu = NULL;
           pdu = NULL;
           break;
       }

        //接收私聊请求
       case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
       {
              qDebug() << "服务器接受私聊请求...";
              char friend_name[32] = {'\0'};
              memcpy(friend_name, pdu->caData + 32, 32);

              MyTcpServer::getInstance().resend(friend_name, pdu);
              break;
       }

        //接收群聊请求
       case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
       {
           qDebug() << "ENUM_MSG_TYPE_CREATE_DIR_REQUEST...";
           char login_name[32] = {'\0'};
           memcpy(login_name, pdu->caData, 32);
           qDebug() << login_name;

           pdu->uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_RESPOND;
           QStringList res = Database::getInstance().get_online_friends(login_name);

           if(!res.isEmpty())
           {
               for(int i=0; i<res.size(); i++)
               {
                   if(res.at(i) != login_name)
                   {
                       MyTcpServer::getInstance().resend(res.at(i).toStdString().c_str(), pdu);
                   }
               }
           }
           else
           {
               write((char*)pdu, pdu->uiPDULen);
           }

           free(pdu);
           pdu = NULL;
           break;
       }

       //接收文件夹创建请求
       case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
       {
           qDebug() << "服务器接受文件夹创建请求...";

           QDir dir;
           QString Cur_path = QString("%1").arg((char*)pdu->caMsg);
           qDebug() << "Cur_path:" << Cur_path;

           char create_dir_name[32] = {'\0'};
           memcpy(create_dir_name, pdu->caData + 32 , 32);

           qDebug() << "create_dir_name:" << create_dir_name;

           PDU *respdu = mkPDU(0);
           respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;

           bool res = dir.exists(Cur_path);

           qDebug() << "res" << res;

           if(res)
           {
                QString create_dir_path= Cur_path + QString("/") + create_dir_name;

                qDebug() << "新建文件路径：" << create_dir_path;

                res = dir.exists(create_dir_path);
                if(res)
                {
                     strcpy(respdu->caData, FILE_NAME_EXIST);
                }
                else
                {
                    dir.mkdir(create_dir_path);
                    strcpy(respdu->caData, CREAT_DIR_OK);
                }
           }
           else
           {
               strcpy(respdu->caData, DIR_NO_EXIST);
           }

           write((char*)respdu, respdu->uiPDULen);

           free(respdu);
           respdu = NULL;
           break;

       }

       //接收刷新文件请求
       case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
       {
           qDebug() << "服务器接受刷新文件请求...";

           QString Cur_path = QString("%1").arg((char*)pdu->caMsg);
           QDir dir(Cur_path);
           QFileInfoList FileInfoList = dir.entryInfoList();

           qDebug() << Cur_path << FileInfoList;
           PDU *respdu = mkPDU(FileInfoList.size() * sizeof(FileInfo));
           respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
           QString file_name;

           FileInfo *fileinfo = NULL;
           for(int i=0; i<FileInfoList.size(); i++)
           {
               qDebug() << FileInfoList.at(i).fileName();

               fileinfo = (FileInfo*)(respdu->caMsg) + i;
               file_name = FileInfoList.at(i).fileName();
               memcpy(fileinfo->caFileName, file_name.toStdString().c_str(), file_name.size()+1);

               if(FileInfoList.at(i).isDir())
               {
                   fileinfo->iFileType = 1;
               }
               else if(FileInfoList.at(i).isFile())
               {
                   fileinfo->iFileType = 0;
               }
           }

           write((char*)respdu, respdu->uiPDULen);
           free(respdu);
           respdu = NULL;
           break;
       }

       //接收删除文件夹请求
       case ENUM_MSG_TYPE_DEL_DIR_REQUEST:
       {
           qDebug() << "服务器接收删除文件夹请求...";

           QString File_path = QString("%1/%2").arg((char*)pdu->caMsg).arg((char*)pdu->caData);

           qDebug() << "文件路径：" << File_path;

           QDir dir;
           QFileInfo fileinfo(File_path);
           bool res = false;

           if(fileinfo.isDir()) res = true;
           else res = false;

           PDU *respdu = mkPDU(0);
           respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;

           if(res)
           {
               dir.setPath(File_path);
               dir.removeRecursively();
               strcpy(respdu->caData, DEL_DIR_OK);
           }
           else
           {
               strcpy(respdu->caData, DEL_DIR_FAILURED);
           }

           write((char*)respdu, respdu->uiPDULen);
           free(respdu);
           respdu = NULL;
       }

       //接收重命名文件夹请求
       case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
       {
           qDebug() << "服务器接收重命名文件夹请求...";

           char Old_name_text[32] = {'\0'};
           char rename_text[32] = {'\0'};
           memcpy(Old_name_text, pdu->caData, 32);
           memcpy(rename_text, pdu->caData + 32, 32);
           QString Old_path = QString("%1/%2").arg((char*)pdu->caMsg).arg(pdu->caData);
           QString New_path = QString("%1/%2").arg((char*)pdu->caMsg).arg(pdu->caData + 32);

           qDebug() << "重名文件名：" << Old_path << New_path << Old_name_text << rename_text;
           QDir dir;

           bool res = dir.rename(Old_path, New_path);

           PDU *respdu = mkPDU(0);
           respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
           if(res)
           {
               strcpy(respdu->caData, RENAME_FILE_OK);
           }
           else
           {
               strcpy(respdu->caData, RENAME_FILE_FAILURED);
           }

           write((char*)respdu, pdu->uiPDULen);
           free(respdu);
           free(pdu);
           respdu = NULL;
           pdu = NULL;
           break;

       }

       //接收进入文件夹的请求
       case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:
       {
           qDebug() << "接收进入文件夹的请求...";

           QString Cur_path = QString("%1").arg((char*)pdu->caMsg);
           QString Dir_name = QString("%1").arg(pdu->caData);

           QString Dir_path = Cur_path + QString("/") +Dir_name;

           qDebug() << "Dir_path" << Dir_path;
           QDir dir(Dir_path);

           QFileInfoList file_list = dir.entryInfoList();
           QFileInfo fileinfo(Dir_path);
           FileInfo *p_fileinfo =NULL;
           PDU *respdu =mkPDU(file_list.size() * sizeof(FileInfo));
           respdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
           if(fileinfo.isDir())
           {
               strncpy(respdu->caData, pdu->caData, 32);

               for(int i=0; i<file_list.size(); i++)
                   {
                       qDebug() << file_list.at(i);
                       p_fileinfo = (FileInfo*)respdu->caMsg + i;
                       memcpy(p_fileinfo->caFileName, file_list.at(i).fileName().toStdString().c_str(), file_list.at(i).fileName().size() + 1);

                       if(file_list.at(i).isDir())
                           p_fileinfo->iFileType = 1;
                       else if(file_list.at(i).isFile())
                           p_fileinfo->iFileType = 0;
                   }
           }
           else if(fileinfo.isFile())
           {
               strcpy(respdu->caData, ENTER_DIR_FAILURED);
           }

           write((char*)respdu, respdu->uiPDULen);
           free(respdu);
           respdu = NULL;
           break;
       }

       //接收上传文件的请求
       case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
       {
           qDebug() << "服务器接收上传文件的请求...";
           char File_name[32] = {'\0'};
           qint64 file_size = 0;

           sscanf(pdu->caData, "%s %lld", File_name, &file_size);

           qDebug() << "File_name:" << File_name << "file_size:" << file_size;

           QString File_path = QString("%1/%2").arg((char*)pdu->caMsg).arg(File_name);

           qDebug() << "File_path:" << File_path;

           this->file.setFileName(File_path);

           if(file.open(QIODevice::WriteOnly))
           {
                this->upload_flag = true;
                this->received_size = 0;
                this->total_size = file_size;
           }
           break;
       }

       //接收删除文件的请求
       case ENUM_MSG_TYPE_DEL_FILE_REQUEST:
       {
           qDebug() << "服务器接收删除文件请求...";
           QString File_path = QString("%1/%2").arg((char*)pdu->caMsg).arg(pdu->caData);
           qDebug() << "File_path:" << File_path;

           QFileInfo file(File_path);
           PDU *respdu = mkPDU(0);
           respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPOND;

           if(file.isDir())
           {
               strcpy(respdu->caData, DEL_FILE_FAILURED);
           }
           else if(file.isFile())
           {
               QDir dir;
               dir.remove(File_path);
               strcpy (respdu->caData, DEL_FILE_OK);
           }

           write((char*)respdu, respdu->uiPDULen);
           free(respdu);
           respdu = NULL;
       }

       //接收文件下载的请求
       case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
       {
           qDebug() << "服务器接收文件下载请求...";
           char File_name[32] = {'\0'};
           strcpy(File_name, pdu->caData);
           QString Upload_path = QString("%1/%2").arg((char*)pdu->caMsg).arg((char*)pdu->caData);

           qDebug() << "File_name:" << File_name << "Upload_path:" << Upload_path;

           QFileInfo fileinfo(Upload_path);

           PDU *respdu = mkPDU(0);
           respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
           if(fileinfo.isDir())
           {
               qDebug() << "目录存在！";
               strcpy(pdu->caData, UPLOAD_FILE_FAILURED);
           }
           else if(fileinfo.isFile())
           {
               qDebug() << "文件存在！";
               sprintf(respdu->caData, "%s %lld", File_name, fileinfo.size());

               qDebug() << respdu->caData;
               write((char*)respdu, respdu->uiPDULen);
               free(respdu);
               respdu = NULL;
               this->file.setFileName(Upload_path);
               this->file.open(QIODevice::ReadOnly);
               m_pTimer->start(1000);

               qDebug() << "File:" << file;
           }

           break;
       }

       //接收文件分享的请求
       case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
       {
           qDebug() << "服务器接收文件分享的请求...";
           char login_name[32] = {'\0'};
           int share_num;
           sscanf(pdu->caData, "%s %d", login_name, &share_num);
           qDebug() << "login_name1:" << login_name << "share_num:" << share_num;

           QString File_path = QString("%1").arg((char*)(pdu->caMsg) + share_num * 32);

           qDebug() << "File_path:" << File_path;

           char send_name[32] = {'\0'};
           PDU *respdu = mkPDU(pdu->uiPDULen - share_num * 32);
           respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
           strncpy(respdu->caData, login_name, 32);
           memcpy(respdu->caMsg, (char*)pdu->caMsg + share_num * 32, pdu->uiMsgLen - share_num * 32);

           for(int i=0;i<share_num;i++)
           {
                memcpy(send_name, (char*)(pdu->caMsg) + i * 32, 32);

                qDebug() << "send_name1:" << send_name;
                memcpy(send_name, (char*)pdu->caMsg + i * 32, 32);
                qDebug() << "send_name2:" << send_name;

                MyTcpServer::getInstance().resend(send_name, respdu);
           }
           free(respdu);
           respdu = NULL;
           break;

       }

       //接收文件分享的响应
       case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:
       {
           qDebug() << "接收客户端响应文件分享...";

           char send_name[32] = {'\0'};
           int flag;

           sscanf(pdu->caData, "%s%d", send_name, &flag);
           QString des_path = QString('/') + login_name;

           qDebug() << "login_name:" << login_name << "flag:" << flag;

           QString Share_file_path = QString("%1").arg((char*)pdu->caMsg);
           int index = Share_file_path.lastIndexOf("/");
           int frist_index = Share_file_path.indexOf('/');
           int second_index = Share_file_path.indexOf('/',frist_index + 1);
           QString login_name = Share_file_path.mid(frist_index + 1, second_index - (frist_index + 1));
           QString FileName = Share_file_path.right(Share_file_path.size() - index - 1);

           QString despath = QString(".") + des_path + QString("/") + FileName;
           qDebug() << "des_path:" << des_path << "Share_file_path:" << Share_file_path << "Filename:" << FileName << "despath:" << despath << "login_name:" << login_name;

           PDU * respdu = mkPDU(0);
           respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
           if(flag == 1)
           {
                qDebug() << "share ok...";

               QFileInfo file(Share_file_path);
               if(file.isFile())
               {
                   QFile::copy(Share_file_path, despath);

               }
               else if(file.isDir())
               {
                   Copy_Dir_File(Share_file_path, despath);
               }

               strcpy(respdu->caData, SHARE_FILE_OK);
               MyTcpServer::getInstance().resend(login_name.toStdString().c_str(), respdu);

           }
           else
           {
               qDebug() << "share failured...";
               strcpy(respdu->caData, SHARE_FILE_FAILURED);
               MyTcpServer::getInstance().resend(login_name.toStdString().c_str(), respdu);
           }

           free(respdu);
           respdu = NULL;
           break;

       }
       default:
       {
           break;
       }

       }

       free(pdu);
       pdu = NULL;
   }
   //文件读取
   else
   {
       qDebug() << "服务其正在上传文件...";

       PDU *respdu = NULL;
       respdu = mkPDU(0);
       respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
       QByteArray Buffer = readAll();
       file.write(Buffer);
       qDebug() << "传输文件大小:" << Buffer.size();

       this->received_size += Buffer.size();

       if(received_size == total_size)
       {
           file.close();
           upload_flag = false;

           strcpy(respdu->caData, UPLOAD_FILE_OK);
           write((char*)respdu, respdu->uiPDULen);
           free(respdu);
           respdu = NULL;
       }
       else if(received_size > total_size)
       {
           file.close();
           upload_flag = true;

           strcpy(respdu->caData, UPLOAD_FILE_FAILURED);
           write((char*)respdu, respdu->uiPDULen);
           free(respdu);
           respdu = NULL;
       }

   }

}

