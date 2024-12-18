#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QMainWindow>
#include <QFile>
#include "ui_tcpserver.h"
QT_BEGIN_NAMESPACE
namespace Ui { class TcpServer; }
QT_END_NAMESPACE

class TcpServer : public QMainWindow
{
Q_OBJECT

public:
TcpServer(QWidget *parent = nullptr);
~TcpServer();
void loadConfig();
void listening();
private:
Ui::TcpServer *ui;
QString m_strIP;
quint16 m_usPort;
};
#endif // TCPSERVER_H
