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
        qDebug()<< "数据库连接成功！";
        QSqlQuery query;
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

    qDebug() << username << password;
    QSqlQuery query;
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
bool Database:: offline(QString username)
{
    if(username.isEmpty())
        return false;
    else
    {
        qDebug() << username;
        QSqlQuery query;
        QString sql= QString("update usrInfo set online=0 where name=\'%1\' and online=1").arg(username);
        query.exec(sql);
        return true;
    }
}

//查询在线好友(得到name)
QStringList Database:: get_Online_friend()
{
    QStringList results;
    QSqlQuery query;
    QString sql = QString("select * from usrInfo where online=1");
    query.exec(sql);

    QString name;
    while(query.next())
    {
        name = query.value(1).toString();
        results.append(name);
    }
    qDebug() << results;
    return results;
}

//重写析构函数
Database::~Database()
{
    //关闭数据库
    db.close();
}

