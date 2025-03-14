#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QMainWindow>
#include <QFile>
#include <QTcpSocket>
#include <protocol.h>
QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QMainWindow
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();
    void loadConfig();
    QString get_login_name();
    void set_Cur_path(QString Cur_path);
    QString get_Cur_path();
    QFile file;

public slots:
    void showConnect();

private slots:

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    //void on_cancel_pb_clicked();
    //定义处理监听服务器反馈信息的函数
    void show_information();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;
    //连接服务器
    QTcpSocket m_tcpScoket;
    //用户登录名
    QString login_name;
    QString Cur_path;
};
#endif // TCPCLIENT_H
