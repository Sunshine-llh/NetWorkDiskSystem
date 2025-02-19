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
public slots:
    void showConnect();
private slots:
    //void on_send_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();
    //定义处理监听服务器反馈信息的函数
    void show_information();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;
    //连接服务器
    QTcpSocket m_tcpScoket;
};
#endif // TCPCLIENT_H
