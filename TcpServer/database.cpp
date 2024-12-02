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
    QSqlTableModel *model=new QSqlTableModel(this);
    model->setTable("usrInfo");
    model->select();
    //获取查询函数
    if(db.open()){
        qDebug()<< "数据库连接成功！";
    }
    else
    {
        QMessageBox::critical(NULL,"连接","连接数据库失败！");
    }
}
//注册
bool Database::regist(const char *username,const char *password)
{

    if(NULL==username && NULL==password)
    {
        return false;
    }
    QSqlQuery query;
    QString sql=QString("insert into usrInfo (name,pwd) values(\'%1\',\'%2\')").arg(username).arg(password);
    //qDebug() << query.exec(sql);
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
    QString sql=QString("select * from usrInfo where name=\'%1\' and pwd=\'%2\' and online =0").arg(username).arg(password);
    query.exec(sql);
    qDebug() << "-------login_test------";
    if(query.next())
    {
        sql=QString("insert into usrInfo online=1 where name=\'%1\' and pwd=\'%2\'").arg(username).arg(password);
        query.exec(sql);
        return true;
    }
    else {
        return false;
    }

}
//重写析构函数
Database::~Database()
{
    //关闭数据库
    db.close();
}

