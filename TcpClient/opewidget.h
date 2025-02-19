#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include <QWidget>
#include <QListWidget>
class OpeWidget : public QWidget
{
    Q_OBJECT
private:
    QListWidget *m_pListW;
public:
    explicit OpeWidget(QWidget *parent = nullptr);

signals:

};

#endif // OPEWIDGET_H
