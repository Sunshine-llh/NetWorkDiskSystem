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
bool regist(const char *username,const char *password)
{

    if(NULL==username && NULL==password)
    {
        return false;
    }
    QSqlQuery query;
    QString sql=QString("insert into usrInfo (name,pwd) values(\'%1\',\'%2\')").arg(username).arg(password);
    return query.exec(sql);


}
//重写析构函数
Database::~Database()
{
    //关闭数据库
    db.close();
}

