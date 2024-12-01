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
        //QSqlQuery query;
//        query.exec("select * from usrInfo");
//        while (query.next()) {
//            //QString data=query.value(1));
//             qDebug() << query.ValuesAsRows;
//        }
    }
}
//重写析构函数
Database::~Database()
{
    //关闭数据库
    db.close();
}
