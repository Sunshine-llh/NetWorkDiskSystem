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
    void update_online_state(QString username);
    QStringList get_Online_friend();
    QStringList Search_friend(QString name);
    int add_friend(const char *friend_name, const char *login_name);
    void handle_agree_friend(const char *friend_name, const char * login_name);
    QStringList get_online_friends(const char * login_name);
    bool delete_friend(const char *login_name, const char *friend_name);
    ~Database();

signals:
private:
    QSqlDatabase db;

};

#endif // DATABASE_H
