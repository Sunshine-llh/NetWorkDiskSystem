#include "database.h"
#include<QMessageBox>
Database::Database(QObject *parent) : QObject(parent)
{
    db=QSqlDatabase::addDatabase("QSQLITE");
}
Database &Database::getInstance()
{
    static Database instance;
    return instance;
}

//重写析构函数
Database::~Database()
{
    //关闭数据库
    db.close();
}

void Database::initdatabase(){
    //设置数据库user
    db.setHostName("localhost");
    //设置数据库路径
    db.setDatabaseName("E:\\Projects\\C++\\NetWorkDiskSystem\\TcpServer\\cloud.db");
    //QSqlTableModel *model=new QSqlTableModel(this);
    //model->setTable("usrInfo");
    //model->select();
    //获取查询函数
    if(db.open()){
        QSqlQuery query;
        qDebug()<< "数据库连接成功！";
               query.exec("select * from usrInfo");
               while(query.next())
               {
                   QString data = QString("%1,%2,%3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
                   qDebug() << data;
               }
    }
    else
    {
        QMessageBox::critical(NULL,"连接","连接数据库失败！");
    }
}

//注册
bool Database::regist(const char *username,const char *password)
{

    if(username == NULL && password == NULL)
    {
        return false;
    }
    QSqlQuery query;
    qDebug() << username << password;
    QString sql=QString("insert into usrInfo values(null,\'%1\',\'%2\',0)").arg(username).arg(password);
    qDebug() << sql;
    return query.exec(sql);
}

//登录
bool Database::login(const char* username,const char* password)
{
    if(NULL==username && NULL==password)
    {
        return false;
    }
    QSqlQuery query;
    QString sql=QString("select * from usrInfo where name=\'%1\' and pwd=\'%2\'").arg(username).arg(password);
    query.exec(sql);
    qDebug() << "-------login_test------";

    if(query.next())
    {
        sql=QString("update usrInfo set online=1 where name=\'%1\'").arg(username);
        query.exec(sql);
        return true;
    }
    else
    {
        return false;
    }

}

//下线
void Database:: update_online_state(QString username)
{
    if(username.isEmpty())
        return ;
    else
    {
        QSqlQuery query;
        qDebug() << username;
        QString sql= QString("update usrInfo set online=0 where name=\'%1\' and online=1").arg(username);
        query.exec(sql);
    }
}

//查询在线好友(得到name)
QStringList Database:: get_Online_friend()
{
    QSqlQuery query;
    QStringList results;
    QString sql = QString("select name from usrInfo where online=1");
    query.exec(sql);
    results.clear();
    QString name;
    while(query.next())
    {
        results.append(query.value(0).toString());
    }
    qDebug() << results;
    return results;
}

//查询指定好友
QStringList Database::Search_friend(QString name)
{
    qDebug() << "查询指定好友";
    QStringList results;
    QString online_state=" ";
    QSqlQuery query;
    QString sql = QString("select * from usrInfo where name=\'%1\'").arg(name);
    query.exec(sql);
    if(query.next())
    {
        if(query.value(3)==1)
            online_state = "在线";

        else online_state = "离线";

        results.append(name);
        results.append(online_state);
    }

    return results;
}

//添加好友
int Database::add_friend(const char *login_name, const char *friend_name)
{
    qDebug() << "添加好友...";
    qDebug() << login_name << friend_name;

    if(friend_name == NULL && login_name == NULL)
    {
        return -1;
    }



    QString sql = QString("select * from friend where (id=(select id from usrInfo where name=\'%1\') and (friendId=(select id from usrInfo where name=\'%2\')) or (id=(select id from usrInfo where name=\'%3\') and (friendId=(select id from usrInfo where name=\'%4\'))").arg(friend_name).arg(login_name).arg(login_name).arg(friend_name);
    QSqlQuery query;
    query.exec(sql);

    qDebug() << sql;


    if(query.next())
    {
        return 0;
    }
    else
    {
        query.exec(QString("select online from usrInfo where name =\'%1\'").arg(friend_name));

        if(query.next())
        {
            int result = query.value(0).toInt();

            qDebug() << QString("select online from usrInfo where name =\'%1\'").arg(friend_name);

            if(result == 1)
                return 1;

            else if(result == 0)
                return 2;
        }
        else
            return 3;
        }
}

//处理添加好友成功的请求
void Database::handle_agree_friend(const char *login_name, const char *friend_name)
{
    if(login_name == NULL || friend_name == NULL) return ;

    qDebug() << "处理添加好友成功的请求" << login_name << friend_name;

    QSqlQuery query;
    QString sql = QString("insert into friend values((select id from usrInfo where name=\'%1\'),(select id from usrInfo where name=\'%2\'))").arg(login_name).arg(friend_name);
    qDebug() << sql;
    query.exec(sql);

    sql = QString("insert into friend values((select id from usrInfo where name=\'%1\'),(select id from usrInfo where name=\'%2\'))").arg(friend_name).arg(login_name);
    qDebug() << sql;
    query.exec(sql);
}

//返回login_name新的在线好友列表
QStringList Database::get_online_friends(const char *login_name)
{
    QStringList results;

    qDebug() << "返回login_name新的在线好友列表...";

    QSqlQuery query;
    QString sql = QString("select * from usrInfo where online=1 and id in (select friendId from friend where id=(select id from usrInfo where name =\'%1\'))").arg(login_name);
    qDebug() << sql;
    query.exec(sql);

    while(query.next())
    {
        results.append(query.value(1).toString());
    }
    return results;
}

//删除好友
bool Database::delete_friend(const char *login_name, const char *friend_name)
{
    if(login_name == NULL || friend_name == NULL) return false;
    qDebug() << "数据库删除好友...";

    QSqlQuery query;
    QString sql = QString("delete from friend where id=(select id from usrInfo where name =\'%1\') and friendId=(select id from usrInfo where name=\'%2\')").arg(login_name).arg(friend_name);
    qDebug() << sql;
    query.exec(sql);

    sql = QString("delete from friend where id=(select id from usrInfo where name =\'%1\') and friendId=(select id from usrInfo where name=\'%2\')").arg(friend_name).arg(login_name);
    qDebug() << sql;
    query.exec(sql);

    return true;

}
