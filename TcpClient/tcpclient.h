#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
QT_BEGIN_NAMESPACE
namespace Ui { class TcpClient; }
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig();
public slots:
    void showConnect();
private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;
    //连接服务器
    QTcpSocket m_tcpScoket;
};
#endif // TCPCLIENT_H