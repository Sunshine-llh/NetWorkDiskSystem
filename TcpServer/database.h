#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtDebug>
#include <QSqlTableModel>
class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    static Database& getInstance();
    void initdatabase();
    bool regist(const char* username,const char* password);
    bool login(const char* username,const char* password);
    ~Database();

signals:
private:
    QSqlDatabase db;

};

#endif // DATABASE_H
